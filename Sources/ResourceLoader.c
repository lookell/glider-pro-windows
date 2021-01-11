#include "ResourceLoader.h"

#include "ByteIO.h"
#include "ImageIO.h"
#include "StructIO.h"
#include "Utilities.h" // TEMP TEMP TEMP

#include "miniz.h"

#include <strsafe.h>

#include <stdio.h>

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

//--------------------------------------------------------------  Gp_OpenHouseFileImpl

static HRESULT
Gp_OpenHouseFileImpl (Gp_HouseFile *houseFile, PCWSTR fileName)
{
	HRESULT hr;
	errno_t err;
	mz_bool succeeded;

	if (fileName != NULL)
	{
		hr = StringCchCopyW(houseFile->fileName, ARRAYSIZE(houseFile->fileName), fileName);
		if (FAILED(hr))
		{
			return hr;
		}
	}
	err = _wfopen_s(&houseFile->filePtr, houseFile->fileName, L"rb");
	if ((err != 0) || (houseFile->filePtr == NULL))
	{
		return E_FAIL;
	}
	mz_zip_zero_struct(&houseFile->archive);
	succeeded = mz_zip_reader_init_cfile(&houseFile->archive, houseFile->filePtr, 0, 0);
	if (!succeeded)
	{
		fclose(houseFile->filePtr);
		houseFile->filePtr = NULL;
		return E_FAIL;
	}

	return S_OK;
}

//--------------------------------------------------------------  Gp_CloseHouseFileImpl

static void
Gp_CloseHouseFileImpl (Gp_HouseFile *houseFile)
{
	mz_zip_end(&houseFile->archive);
	fclose(houseFile->filePtr);
	houseFile->filePtr = NULL;
}

//--------------------------------------------------------------  Gp_LoadBuiltInAssets

HRESULT Gp_LoadBuiltInAssets (void)
{
	WCHAR mermaidFileName[MAX_PATH];
	DWORD numChars;
	wchar_t *lastSlash;
	HRESULT hr;
	Gp_HouseFile *houseFile;

	if (Gp_BuiltInAssetsLoaded())
	{
		return S_OK;
	}
	numChars = GetModuleFileNameW(HINST_THISCOMPONENT,
		mermaidFileName, ARRAYSIZE(mermaidFileName));
	if (numChars == ARRAYSIZE(mermaidFileName))
	{
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}
	else if (numChars == 0)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	lastSlash = wcsrchr(mermaidFileName, L'\\');
	if (lastSlash == NULL)
	{
		lastSlash = &mermaidFileName[0];
	}
	*lastSlash = L'\0';
	hr = StringCchCatW(mermaidFileName, ARRAYSIZE(mermaidFileName), L"\\Mermaid.dat");
	if (FAILED(hr))
	{
		return hr;
	}
	houseFile = Gp_LoadHouseFile(mermaidFileName);
	if (houseFile == NULL)
	{
		return E_FAIL;
	}
	g_mermaidRes = houseFile;
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

Gp_HouseFile *Gp_LoadHouseFile (PCWSTR fileName)
{
	Gp_HouseFile *houseFile;
	HRESULT hr;

	if (fileName == NULL)
	{
		return NULL;
	}
	houseFile = (Gp_HouseFile *)calloc(1, sizeof(*houseFile));
	if (houseFile == NULL)
	{
		return NULL;
	}
	hr = Gp_OpenHouseFileImpl(houseFile, fileName);
	if (FAILED(hr))
	{
		free(houseFile);
		return NULL;
	}
	return houseFile;
}

//--------------------------------------------------------------  Gp_UnloadHouseFile

void Gp_UnloadHouseFile (Gp_HouseFile *houseFile)
{
	if (houseFile == NULL)
	{
		return;
	}
	Gp_CloseHouseFileImpl(houseFile);
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
	return (fileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
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
	Gp_CloseHouseFileImpl(houseFile);

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
	hr = Gp_OpenHouseFileImpl(houseFile, NULL);
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
