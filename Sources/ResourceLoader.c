#include "ResourceLoader.h"

#include "ByteIO.h"
#include "ImageIO.h"
#include "StructIO.h"
#include "Utilities.h" // TEMP TEMP TEMP

#include "miniz.h"

#include <mmsystem.h>
#include <shlwapi.h>
#include <strsafe.h>

#include <stdio.h>

//===========================================================================//

static FOURCC read_fourcc(const unsigned char *buf)
{
	return MAKEFOURCC(buf[0], buf[1], buf[2], buf[3]);
}

static WORD read_word(const unsigned char *buf)
{
	return (WORD)buf[0] | ((WORD)buf[1] << 8);
}

static DWORD read_dword(const unsigned char *buf)
{
	return (DWORD)buf[0] | ((DWORD)buf[1] << 8) |
		((DWORD)buf[2] << 16) | ((DWORD)buf[3] << 24);
}

typedef struct RiffChunk
{
	FOURCC ckID;
	DWORD ckSize;
	const unsigned char *ckData;
} RiffChunk;

// Read a RIFF chunk from the given buffer.
// The buffer pointer and length are updated to point just past the chunk
// that was read. If the function fails (there is not enough data in the
// buffer, or a parameter is NULL), then the return value is zero and
// the output parameter values are not modified. If the function succeeds,
// then the return value is nonzero and the output parameters are updated
// accordingly.
static int ReadRiffChunk(const void **ppBuffer, size_t *pLength, RiffChunk *pChunk)
{
	const unsigned char *dataPointer;
	size_t dataLength;
	RiffChunk riffChunk;
	DWORD skipSize;

	if (ppBuffer == NULL || pLength == NULL || pChunk == NULL)
		return 0;
	dataPointer = (const unsigned char *)*ppBuffer;
	dataLength = *pLength;
	if (dataPointer == NULL || dataLength < 8)
		return 0;
	riffChunk.ckID = read_fourcc(&dataPointer[0]);
	riffChunk.ckSize = read_dword(&dataPointer[4]);
	riffChunk.ckData = &dataPointer[8];
	skipSize = 8 + riffChunk.ckSize;
	// align to 2-byte word boundary
	if ((riffChunk.ckSize % 2) != 0)
		skipSize += 1;
	if (dataLength < skipSize)
		return 0;
	dataPointer += skipSize;
	dataLength -= skipSize;
	*ppBuffer = dataPointer;
	*pLength = dataLength;
	*pChunk = riffChunk;
	return 1;
}

#define CKID_RIFF MAKEFOURCC('R', 'I', 'F', 'F')
#define CKID_WAVE MAKEFOURCC('W', 'A', 'V', 'E')
#define CKID_fmt MAKEFOURCC('f', 'm', 't', ' ')
#define CKID_data MAKEFOURCC('d', 'a', 't', 'a')

// Parse a WAV file from an in-memory representation, and write the data
// into the waveData output parameter. Return nonzero on success and zero
// on failure.
//
// The pointer written into waveData->dataBytes is simply a pointer into the
// given buffer, and does not need to be freed separately.
static int ReadWAVFromMemory(const void *buffer, size_t length, WaveData *waveData)
{
	RiffChunk chunk;
	WAVEFORMATEX format;

	if (buffer == NULL || length == 0 || waveData == NULL)
		return 0;

	if (!ReadRiffChunk(&buffer, &length, &chunk))
		return 0;
	if (chunk.ckID != CKID_RIFF)
		return 0;
	if (chunk.ckSize < 4)
		return 0;
	if (read_fourcc(&chunk.ckData[0]) != CKID_WAVE)
		return 0;
	buffer = &chunk.ckData[4];
	length = chunk.ckSize - 4;

	// read format chunk (defined to be the first chunk in the list)
	if (!ReadRiffChunk(&buffer, &length, &chunk))
		return 0;
	if (chunk.ckID != CKID_fmt)
		return 0;
	if (chunk.ckSize < 16)
		return 0;
	format.wFormatTag = read_word(&chunk.ckData[0]);
	format.nChannels = read_word(&chunk.ckData[2]);
	format.nSamplesPerSec = read_dword(&chunk.ckData[4]);
	format.nAvgBytesPerSec = read_dword(&chunk.ckData[8]);
	format.nBlockAlign = read_word(&chunk.ckData[12]);
	format.wBitsPerSample = read_word(&chunk.ckData[14]);
	if (chunk.ckSize > 16)
		format.cbSize = read_word(&chunk.ckData[16]);
	else
		format.cbSize = 0;

	// check if format chunk is valid and supported
	if (format.wFormatTag != WAVE_FORMAT_PCM)
		return 0;
	if (format.nChannels != 1 && format.nChannels != 2)
		return 0;
	if (format.nSamplesPerSec == 0)
		return 0;
	if (format.nAvgBytesPerSec != (format.nSamplesPerSec * format.nBlockAlign))
		return 0;
	if (format.nBlockAlign != (format.nChannels * format.wBitsPerSample / 8))
		return 0;
	if (format.wBitsPerSample != 8 && format.wBitsPerSample != 16)
		return 0;
	if (format.cbSize != 0)
		return 0;

	// find and read data chunk
	do
	{
		if (!ReadRiffChunk(&buffer, &length, &chunk))
			return 0;
	} while (chunk.ckID != CKID_data);

	// give the wave data to the caller
	waveData->channels = format.nChannels;
	waveData->bitsPerSample = format.wBitsPerSample;
	waveData->samplesPerSec = format.nSamplesPerSec;
	waveData->dataLength = chunk.ckSize;
	waveData->dataBytes = chunk.ckData;
	return 1;
}

//===========================================================================//

#define HOUSE_DATA_NAME  "house.dat"
#define HOUSE_ICON_NAME  "house.ico"
#define BOUNDS_FORMAT_STRING  "bounds/%d.bin"
#define IMAGES_FORMAT_STRING  "images/%d.bmp"
#define SOUNDS_FORMAT_STRING  "sounds/%d.wav"

struct Gp_HouseFile
{
	WCHAR fileName[MAX_PATH];
	FILE *filePtr;
	mz_zip_archive archive;
};

static Gp_HouseFile *g_mermaidRes;

//--------------------------------------------------------------  Gp_FileSizeInHouseFile

static uint64_t
Gp_FileSizeInHouseFile (Gp_HouseFile *houseFile, const char *fileName)
{
	mz_uint32 index;
	mz_zip_archive_file_stat fileStat;

	if (houseFile == NULL)
	{
		return 0;
	}
	if (!mz_zip_reader_locate_file_v2(&houseFile->archive, fileName, NULL, 0, &index))
	{
		return 0;
	}
	if (!mz_zip_reader_file_stat(&houseFile->archive, index, &fileStat))
	{
		return 0;
	}
	return fileStat.m_uncomp_size;
}

//--------------------------------------------------------------  Gp_GetImageFileName

static HRESULT
Gp_GetImageFileName (PSTR pszDest, size_t cchDest, SInt16 resID)
{
	return StringCchPrintfA(pszDest, cchDest, IMAGES_FORMAT_STRING, (int)resID);
}

//--------------------------------------------------------------  Gp_GetSoundFileName

static HRESULT
Gp_GetSoundFileName (PSTR pszDest, size_t cchDest, SInt16 resID)
{
	return StringCchPrintfA(pszDest, cchDest, SOUNDS_FORMAT_STRING, (int)resID);
}

//--------------------------------------------------------------  Gp_GetBoundFileName

static HRESULT
Gp_GetBoundFileName (PSTR pszDest, size_t cchDest, SInt16 resID)
{
	return StringCchPrintfA(pszDest, cchDest, BOUNDS_FORMAT_STRING, (int)resID);
}

//--------------------------------------------------------------  Gp_ExtractFromHouseFile

static void *
Gp_ExtractFromHouseFile (Gp_HouseFile *houseFile, const char *fileName, size_t *pLength)
{
	return mz_zip_reader_extract_file_to_heap(&houseFile->archive, fileName, pLength, 0);
}

//--------------------------------------------------------------  MinizErrorToHResult

static HRESULT MinizErrorToHResult (mz_zip_error errorCode)
{
	switch (errorCode)
	{
	case MZ_ZIP_NO_ERROR:
		return S_OK;

	case MZ_ZIP_UNDEFINED_ERROR:
		return E_FAIL;

	case MZ_ZIP_FAILED_FINDING_CENTRAL_DIR:
	case MZ_ZIP_NOT_AN_ARCHIVE:
	case MZ_ZIP_INVALID_HEADER_OR_CORRUPTED:
	case MZ_ZIP_CRC_CHECK_FAILED:
		return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

	case MZ_ZIP_ALLOC_FAILED:
		return E_OUTOFMEMORY;

	case MZ_ZIP_INVALID_PARAMETER:
		return E_INVALIDARG;

	case MZ_ZIP_FILE_NOT_FOUND:
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	default:
		return E_FAIL;
	}
}

//--------------------------------------------------------------  Gp_OpenZipReader

static HRESULT Gp_OpenZipReader (PCWSTR fileName, FILE **pFilePtr, mz_zip_archive *pArchive)
{
	errno_t err;
	unsigned long dosErrNum;
	mz_bool succeeded;
	mz_zip_error zipError;

	*pFilePtr = NULL;
	mz_zip_zero_struct(pArchive);

	_set_doserrno(ERROR_SUCCESS);
	err = _wfopen_s(pFilePtr, fileName, L"rb");
	if (err != 0)
	{
		*pFilePtr = NULL;
		_get_doserrno(&dosErrNum);
		if (dosErrNum == 0)
		{
			return E_FAIL;
		}
		return HRESULT_FROM_WIN32(dosErrNum);
	}
	succeeded = mz_zip_reader_init_cfile(pArchive, *pFilePtr, 0, 0);
	if (!succeeded)
	{
		zipError = mz_zip_get_last_error(pArchive);
		mz_zip_zero_struct(pArchive);
		fclose(*pFilePtr);
		*pFilePtr = NULL;
		return MinizErrorToHResult(zipError);
	}
	return S_OK;
}

//--------------------------------------------------------------  Gp_CloseZipReader

static void Gp_CloseZipReader (FILE **pFilePtr, mz_zip_archive *pArchive)
{
	mz_zip_end(pArchive);
	mz_zip_zero_struct(pArchive);
	fclose(*pFilePtr);
	*pFilePtr = NULL;
}

//--------------------------------------------------------------  Gp_LoadBuiltInAssets

HRESULT Gp_LoadBuiltInAssets (void)
{
	WCHAR mermaidFileName[MAX_PATH];
	DWORD numChars;
	HRESULT hr;

	if (Gp_BuiltInAssetsLoaded())
	{
		return S_OK;
	}
	numChars = GetModuleFileNameW(HINST_THISCOMPONENT,
		mermaidFileName, ARRAYSIZE(mermaidFileName));
	if (numChars >= ARRAYSIZE(mermaidFileName))
	{
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}
	else if (numChars == 0)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	PathRemoveFileSpecW(mermaidFileName);
	if (!PathAppendW(mermaidFileName, L"Mermaid.dat"))
	{
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}
	hr = Gp_LoadHouseFile(mermaidFileName, &g_mermaidRes);
	if (FAILED(hr))
	{
		g_mermaidRes = NULL;
		return hr;
	}
	return S_OK;
}

//--------------------------------------------------------------  Gp_UnloadBuiltInAssets

void Gp_UnloadBuiltInAssets (void)
{
	Gp_UnloadHouseFile(g_mermaidRes);
	g_mermaidRes = NULL;
}

//--------------------------------------------------------------  Gp_BuiltInAssetsLoaded

BOOLEAN Gp_BuiltInAssetsLoaded (void)
{
	return (g_mermaidRes != NULL);
}

//--------------------------------------------------------------  Gp_CreateHouseFile

HRESULT Gp_CreateHouseFile (PCWSTR fileName)
{
	FILE *newHouseFilePtr;
	mz_zip_archive newHouseArchive;

	if ((_wfopen_s(&newHouseFilePtr, fileName, L"wb") != 0) || (newHouseFilePtr == NULL))
	{
		return E_FAIL;
	}
	mz_zip_zero_struct(&newHouseArchive);
	if (!mz_zip_writer_init_cfile(&newHouseArchive, newHouseFilePtr, 0))
	{
		fclose(newHouseFilePtr);
		_wremove(fileName);
		return E_FAIL;
	}
	if (!mz_zip_writer_finalize_archive(&newHouseArchive))
	{
		mz_zip_end(&newHouseArchive);
		fclose(newHouseFilePtr);
		_wremove(fileName);
		return E_FAIL;
	}
	mz_zip_end(&newHouseArchive);
	fclose(newHouseFilePtr);
	return S_OK;
}

//--------------------------------------------------------------  Gp_LoadHouseFile

HRESULT Gp_LoadHouseFile (PCWSTR fileName, Gp_HouseFile **pHouseFile)
{
	Gp_HouseFile *houseFile;
	HRESULT hr;

	*pHouseFile = NULL;

	houseFile = (Gp_HouseFile *)calloc(1, sizeof(*houseFile));
	if (houseFile == NULL)
	{
		return E_OUTOFMEMORY;
	}
	hr = StringCchCopyW(houseFile->fileName, ARRAYSIZE(houseFile->fileName), fileName);
	if (FAILED(hr))
	{
		free(houseFile);
		return hr;
	}
	hr = Gp_OpenZipReader(houseFile->fileName, &houseFile->filePtr, &houseFile->archive);
	if (FAILED(hr))
	{
		free(houseFile);
		return hr;
	}

	*pHouseFile = houseFile;
	return S_OK;
}

//--------------------------------------------------------------  Gp_UnloadHouseFile

void Gp_UnloadHouseFile (Gp_HouseFile *houseFile)
{
	if (houseFile == NULL)
	{
		return;
	}
	Gp_CloseZipReader(&houseFile->filePtr, &houseFile->archive);
	free(houseFile);
}

//--------------------------------------------------------------  Gp_HouseFileReadOnly

BOOLEAN Gp_HouseFileReadOnly (Gp_HouseFile *houseFile)
{
	DWORD fileAttributes;

	if (houseFile == NULL)
	{
		return TRUE;
	}
	fileAttributes = GetFileAttributesW(houseFile->fileName);
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		return TRUE;
	}
	return ((fileAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY);
}

//--------------------------------------------------------------  Gp_HouseFileHasMovie

BOOLEAN Gp_HouseFileHasMovie (Gp_HouseFile *houseFile)
{
	// TODO: QuickTime movie support (or equivalent) for house movies
	(void)houseFile;
	return FALSE;
}

//--------------------------------------------------------------  Gp_GetHouseFilePath

HRESULT Gp_GetHouseFilePath (Gp_HouseFile *houseFile, PWSTR *ppszFilePath)
{
	size_t cchResult;
	PWSTR pszResult;
	HRESULT hr;

	*ppszFilePath = NULL;
	if (houseFile == NULL)
	{
		return E_INVALIDARG;
	}
	cchResult = wcslen(houseFile->fileName) + 1;
	pszResult = (PWSTR)calloc(cchResult, sizeof(*pszResult));
	if (pszResult == NULL)
	{
		return E_OUTOFMEMORY;
	}
	hr = StringCchCopyW(pszResult, cchResult, houseFile->fileName);
	if (FAILED(hr))
	{
		free(pszResult);
		return hr;
	}
	*ppszFilePath = pszResult;
	return S_OK;
}

//--------------------------------------------------------------  Gp_GetHouseDisplayName

HRESULT Gp_GetHouseDisplayName (Gp_HouseFile *houseFile, PWSTR *ppszDisplayName)
{
	PCWSTR pFileName;
	PCWSTR pFileExt;
	size_t cchResult;
	PWSTR pszResult;

	*ppszDisplayName = NULL;
	if (houseFile == NULL)
	{
		return E_INVALIDARG;
	}
	pFileName = PathFindFileNameW(houseFile->fileName);
	pFileExt = PathFindExtensionW(houseFile->fileName);
	cchResult = pFileExt - pFileName + 1;
	pszResult = (PWSTR)calloc(cchResult, sizeof(*pszResult));
	if (pszResult == NULL)
	{
		return E_OUTOFMEMORY;
	}
	StringCchCopyNW(pszResult, cchResult, pFileName, pFileExt - pFileName);
	*ppszDisplayName = pszResult;
	return S_OK;
}

//--------------------------------------------------------------  Gp_LoadHouseIcon

HICON Gp_LoadHouseIcon (Gp_HouseFile *houseFile, UINT width, UINT height)
{
	void *buffer;
	size_t length;
	HICON houseIcon;

	if (houseFile == NULL)
	{
		return NULL;
	}
	buffer = Gp_ExtractFromHouseFile(houseFile, HOUSE_ICON_NAME, &length);
	if (buffer == NULL)
	{
		return NULL;
	}
	houseIcon = LoadMemoryICO(buffer, length, width, height);
	free(buffer);
	return houseIcon;
}

//--------------------------------------------------------------  Gp_HouseFileDataSize

uint64_t Gp_HouseFileDataSize (Gp_HouseFile *houseFile)
{
	return Gp_FileSizeInHouseFile(houseFile, HOUSE_DATA_NAME);
}

//--------------------------------------------------------------  Gp_ReadHouseData

HRESULT Gp_ReadHouseData (Gp_HouseFile *houseFile, houseType *houseData)
{
	void *buffer;
	size_t length;
	byteio *reader;
	HRESULT hr;

	if ((houseFile == NULL) || (houseData == NULL))
	{
		return E_INVALIDARG;
	}
	buffer = Gp_ExtractFromHouseFile(houseFile, HOUSE_DATA_NAME, &length);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	reader = byteio_init_memory_reader(buffer, length);
	if (reader == NULL)
	{
		free(buffer);
		return E_FAIL;
	}
	hr = ReadHouseType(reader, houseData);
	byteio_close(reader);
	free(buffer);
	return hr;
}

//--------------------------------------------------------------  write_house_to_zip

static HRESULT
write_house_to_zip (mz_zip_archive *archive, const houseType *house)
{
	byteio *dataWriter;
	void *dataBuffer;
	size_t dataLength;
	HRESULT writeResult;
	HRESULT closeResult;
	mz_bool succeeded;

	dataWriter = byteio_init_memory_writer(0);
	if (dataWriter == NULL)
	{
		return E_OUTOFMEMORY;
	}
	writeResult = WriteHouseType(dataWriter, house);
	closeResult = byteio_close_and_get_buffer(dataWriter, &dataBuffer, &dataLength);
	if (FAILED(writeResult))
	{
		return writeResult;
	}
	if (FAILED(closeResult))
	{
		return closeResult;
	}
	if (dataBuffer == NULL)
	{
		return E_FAIL;
	}
	succeeded = mz_zip_writer_add_mem(archive, HOUSE_DATA_NAME, dataBuffer,
		dataLength, (mz_uint)MZ_DEFAULT_COMPRESSION);
	free(dataBuffer);
	if (!succeeded)
	{
		return E_FAIL;
	}
	return S_OK;
}

//--------------------------------------------------------------  Gp_WriteHouseData

HRESULT Gp_WriteHouseData (Gp_HouseFile *houseFile, const houseType *houseData)
{
	wchar_t outputFileName[MAX_PATH];
	FILE *outputFilePtr;
	mz_zip_archive outputArchive;
	mz_uint fileIndex;
	mz_uint houseDataIndex;
	mz_uint numFiles;
	BOOL succeeded;
	DWORD lastError;
	HRESULT hr;

	if (houseFile == NULL)
	{
		return E_FAIL;
	}
	if (houseData == NULL)
	{
		return E_INVALIDARG;
	}

	hr = StringCchPrintfW(outputFileName, ARRAYSIZE(outputFileName),
		L"%s.tmp", houseFile->fileName);
	if (FAILED(hr))
	{
		return hr;
	}

	if ((_wfopen_s(&outputFilePtr, outputFileName, L"wb") != 0)
		|| (outputFilePtr == NULL))
	{
		return E_FAIL;
	}
	mz_zip_zero_struct(&outputArchive);
	if (!mz_zip_writer_init_cfile(&outputArchive, outputFilePtr, 0))
	{
		fclose(outputFilePtr);
		_wremove(outputFileName);
		return E_FAIL;
	}

	numFiles = mz_zip_reader_get_num_files(&houseFile->archive);
	if (!mz_zip_reader_locate_file_v2(&houseFile->archive, HOUSE_DATA_NAME, NULL, 0, &houseDataIndex))
	{
		houseDataIndex = numFiles;
	}
	hr = S_OK;
	for (fileIndex = 0; fileIndex < numFiles; fileIndex++)
	{
		if (fileIndex != houseDataIndex)
		{
			if (!mz_zip_writer_add_from_zip_reader(&outputArchive, &houseFile->archive, fileIndex))
			{
				hr = E_FAIL;
				break;
			}
		}
	}
	if (SUCCEEDED(hr))
	{
		hr = write_house_to_zip(&outputArchive, houseData);
	}
	if (SUCCEEDED(hr))
	{
		hr = mz_zip_writer_finalize_archive(&outputArchive) ? S_OK : E_FAIL;
	}
	mz_zip_end(&outputArchive);
	fclose(outputFilePtr);
	if (FAILED(hr))
	{
		_wremove(outputFileName);
		return hr;
	}

	//
	// Temporarily close the zip archive and file stream.
	//
	Gp_CloseZipReader(&houseFile->filePtr, &houseFile->archive);

	lastError = ERROR_SUCCESS;
	succeeded = ReplaceFileW(houseFile->fileName, outputFileName, NULL, 0, NULL, NULL);
	if (!succeeded)
	{
		succeeded = MoveFileExW(outputFileName, houseFile->fileName,
			MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED);
		if (!succeeded)
		{
			lastError = GetLastError();
		}
	}

	//
	// Restore the zip archive and file stream.
	//
	hr = Gp_OpenZipReader(houseFile->fileName, &houseFile->filePtr, &houseFile->archive);
	if (FAILED(hr))
	{
		RedAlert(kErrUnnaccounted); // FIXME FIXME FIXME
	}
	if (SUCCEEDED(hr) && (!succeeded))
	{
		hr = (lastError != ERROR_SUCCESS) ? HRESULT_FROM_WIN32(lastError) : E_FAIL;
	}
	return hr;
}

//--------------------------------------------------------------  Gp_LoadImage

HBITMAP Gp_LoadImage (Gp_HouseFile *houseFile, SInt16 imageID)
{
	HBITMAP image;

	image = Gp_LoadHouseImage(houseFile, imageID);
	if (image == NULL)
	{
		image = Gp_LoadBuiltInImage(imageID);
	}
	return image;
}

//--------------------------------------------------------------  Gp_LoadImageAsDIB

HBITMAP Gp_LoadImageAsDIB (Gp_HouseFile *houseFile, SInt16 imageID)
{
	HBITMAP image;

	image = Gp_LoadHouseImageAsDIB(houseFile, imageID);
	if (image == NULL)
	{
		image = Gp_LoadBuiltInImageAsDIB(imageID);
	}
	return image;
}

//--------------------------------------------------------------  Gp_LoadSound

HRESULT Gp_LoadSound (Gp_HouseFile *houseFile, SInt16 soundID, WaveData *sound)
{
	HRESULT hr;

	hr = Gp_LoadHouseSound(houseFile, soundID, sound);
	if (FAILED(hr))
	{
		hr = Gp_LoadBuiltInSound(soundID, sound);
	}
	return hr;
}

//--------------------------------------------------------------  Gp_LoadBuiltInImage

HBITMAP Gp_LoadBuiltInImage (SInt16 imageID)
{
	return Gp_LoadHouseImage(g_mermaidRes, imageID);
}

//--------------------------------------------------------------  Gp_LoadBuiltInImageAsDIB

HBITMAP Gp_LoadBuiltInImageAsDIB (SInt16 imageID)
{
	return Gp_LoadHouseImageAsDIB(g_mermaidRes, imageID);
}

//--------------------------------------------------------------  Gp_LoadBuiltInSound

HRESULT Gp_LoadBuiltInSound (SInt16 soundID, WaveData *sound)
{
	return Gp_LoadHouseSound(g_mermaidRes, soundID, sound);
}

//--------------------------------------------------------------  Gp_GetFirstImageID

SInt16 Gp_GetFirstHouseImageID (Gp_HouseFile *houseFile,
	SInt16 minimum, SInt16 maximum, SInt16 def)
{
	char fileName[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	mz_uint index;
	mz_uint numFiles;
	Boolean foundImage;
	SInt16 firstID;
	int thisID;

	if (houseFile == NULL)
	{
		return def;
	}
	foundImage = false;
	firstID = def;
	numFiles = mz_zip_reader_get_num_files(&houseFile->archive);
	for (index = 0; index < numFiles; ++index)
	{
		if (!mz_zip_reader_get_filename(&houseFile->archive, index, fileName, sizeof(fileName)))
		{
			continue;
		}
		if (sscanf_s(fileName, IMAGES_FORMAT_STRING, &thisID) == 1)
		{
			if ((int)minimum <= thisID && thisID <= (int)maximum)
			{
				if (!foundImage)
				{
					foundImage = true;
					firstID = (SInt16)thisID;
				}
				if (firstID > (SInt16)thisID)
				{
					firstID = (SInt16)thisID;
				}
			}
		}
	}
	return firstID;
}

//--------------------------------------------------------------  Gp_HouseImageExists

BOOLEAN Gp_HouseImageExists (Gp_HouseFile *houseFile, SInt16 imageID)
{
	char fileName[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

	if (houseFile == NULL)
	{
		return FALSE;
	}
	if (FAILED(Gp_GetImageFileName(fileName, ARRAYSIZE(fileName), imageID)))
	{
		return FALSE;
	}
	return (mz_zip_reader_locate_file(&houseFile->archive, fileName, NULL, 0) >= 0);
}

//--------------------------------------------------------------  Gp_LoadHouseImage

HBITMAP Gp_LoadHouseImage (Gp_HouseFile *houseFile, SInt16 imageID)
{
	char fileName[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HBITMAP image;

	if (houseFile == NULL)
	{
		return NULL;
	}
	if (FAILED(Gp_GetImageFileName(fileName, ARRAYSIZE(fileName), imageID)))
	{
		return NULL;
	}
	buffer = Gp_ExtractFromHouseFile(houseFile, fileName, &length);
	if (buffer == NULL)
	{
		return NULL;
	}
	image = LoadMemoryBMP(buffer, length);
	free(buffer);
	return image;
}

//--------------------------------------------------------------  Gp_LoadHouseImageAsDIB

HBITMAP Gp_LoadHouseImageAsDIB (Gp_HouseFile *houseFile, SInt16 imageID)
{
	char fileName[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HBITMAP image;

	if (houseFile == NULL)
	{
		return NULL;
	}
	if (FAILED(Gp_GetImageFileName(fileName, ARRAYSIZE(fileName), imageID)))
	{
		return NULL;
	}
	buffer = Gp_ExtractFromHouseFile(houseFile, fileName, &length);
	if (buffer == NULL)
	{
		return NULL;
	}
	image = LoadMemoryBMPAsDIBSection(buffer, length);
	free(buffer);
	return image;
}

//--------------------------------------------------------------  Gp_LoadHouseSound

HRESULT Gp_LoadHouseSound (Gp_HouseFile *houseFile, SInt16 soundID, WaveData *sound)
{
	char fileName[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HRESULT hr;

	if ((houseFile == NULL) || (sound == NULL))
	{
		return E_INVALIDARG;
	}
	hr = Gp_GetSoundFileName(fileName, ARRAYSIZE(fileName), soundID);
	if (FAILED(hr))
	{
		return hr;
	}
	buffer = Gp_ExtractFromHouseFile(houseFile, fileName, &length);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	if (!ReadWAVFromMemory(buffer, length, sound))
	{
		free(buffer);
		return E_FAIL;
	}
	// ReadWAVFromMemory returns a pointer to within the buffer. Slide the
	// dataBytes over to the beginning of the buffer so that the dataBytes
	// can be freed normally.
	memmove(buffer, sound->dataBytes, sound->dataLength);
	sound->dataBytes = (unsigned char *)buffer;
	return S_OK;
}

//--------------------------------------------------------------  Gp_LoadHouseBounding

HRESULT Gp_LoadHouseBounding (Gp_HouseFile *houseFile, SInt16 imageID, boundsType *bounds)
{
	char fileName[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	byteio *byteReader;
	HRESULT hr;

	if ((houseFile == NULL) || (bounds == NULL))
	{
		return E_INVALIDARG;
	}
	hr = Gp_GetBoundFileName(fileName, ARRAYSIZE(fileName), imageID);
	if (FAILED(hr))
	{
		return hr;
	}
	buffer = Gp_ExtractFromHouseFile(houseFile, fileName, &length);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	byteReader = byteio_init_memory_reader(buffer, length);
	if (byteReader == NULL)
	{
		free(buffer);
		return E_OUTOFMEMORY;
	}
	hr = ReadBoundsType(byteReader, bounds);
	byteio_close(byteReader);
	free(buffer);
	return hr;
}
