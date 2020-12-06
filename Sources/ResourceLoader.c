#define GP_USE_WINAPI_H

#include "ResourceLoader.h"

#include "ByteIO.h"
#include "ImageIO.h"
#include "miniz.h"
#include "StructIO.h"

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
static Gp_HouseFile *g_houseRes;

//--------------------------------------------------------------  Gp_FileSizeInResFile

static uint64_t
Gp_FileSizeInResFile (Gp_HouseFile *resFile, const char *filename)
{
	mz_uint32 file_index;
	mz_zip_archive_file_stat file_stat;

	if (resFile == NULL)
	{
		return 0;
	}
	if (!mz_zip_reader_locate_file_v2(&resFile->archive, filename, NULL, 0, &file_index))
	{
		return 0;
	}
	if (!mz_zip_reader_file_stat(&resFile->archive, file_index, &file_stat))
	{
		return 0;
	}
	return file_stat.m_uncomp_size;
}

//--------------------------------------------------------------  Gp_GetImageFilename

static HRESULT
Gp_GetImageFilename (PSTR pszDest, size_t cchDest, SInt16 resID)
{
	return StringCchPrintfA(pszDest, cchDest, IMAGES_FORMAT_STRING, (int)resID);
}

//--------------------------------------------------------------  Gp_GetSoundFilename

static HRESULT
Gp_GetSoundFilename (PSTR pszDest, size_t cchDest, SInt16 resID)
{
	return StringCchPrintfA(pszDest, cchDest, SOUNDS_FORMAT_STRING, (int)resID);
}

//--------------------------------------------------------------  Gp_GetBoundFilename

static HRESULT
Gp_GetBoundFilename (PSTR pszDest, size_t cchDest, SInt16 resID)
{
	return StringCchPrintfA(pszDest, cchDest, BOUNDS_FORMAT_STRING, (int)resID);
}

//--------------------------------------------------------------  Gp_ImageExistsInResFile

static BOOLEAN
Gp_ImageExistsInResFile (Gp_HouseFile *resFile, SInt16 imageID)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

	if (resFile == NULL)
	{
		return FALSE;
	}
	if (FAILED(Gp_GetImageFilename(filename, ARRAYSIZE(filename), imageID)))
	{
		return FALSE;
	}
	return (mz_zip_reader_locate_file(&resFile->archive, filename, NULL, 0) >= 0);
}

//--------------------------------------------------------------  Gp_ExtractFromResFile

static void *
Gp_ExtractFromResFile (Gp_HouseFile *resFile, const char *filename, size_t *pLength)
{
	if ((resFile == NULL) || (filename == NULL) || (pLength == NULL))
	{
		return NULL;
	}
	return mz_zip_reader_extract_file_to_heap(&resFile->archive, filename, pLength, 0);
}

//--------------------------------------------------------------  Gp_LoadImageFromResFile

static HBITMAP
Gp_LoadImageFromResFile (Gp_HouseFile *resFile, SInt16 imageID)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HBITMAP image;

	if (resFile == NULL)
	{
		return NULL;
	}
	if (FAILED(Gp_GetImageFilename(filename, ARRAYSIZE(filename), imageID)))
	{
		return NULL;
	}
	buffer = Gp_ExtractFromResFile(resFile, filename, &length);
	if (buffer == NULL)
	{
		return NULL;
	}
	image = LoadMemoryBMP(buffer, length);
	free(buffer);
	return image;
}

//--------------------------------------------------------------  Gp_LoadImageAsDIBFromResFile

static HBITMAP
Gp_LoadImageAsDIBFromResFile (Gp_HouseFile *resFile, SInt16 imageID)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HBITMAP image;

	if (resFile == NULL)
	{
		return NULL;
	}
	if (FAILED(Gp_GetImageFilename(filename, ARRAYSIZE(filename), imageID)))
	{
		return NULL;
	}
	buffer = Gp_ExtractFromResFile(resFile, filename, &length);
	if (buffer == NULL)
	{
		return NULL;
	}
	image = LoadMemoryBMPAsDIBSection(buffer, length);
	free(buffer);
	return image;
}

//--------------------------------------------------------------  Gp_LoadSoundFromResFile

static HRESULT
Gp_LoadSoundFromResFile (Gp_HouseFile *resFile, SInt16 soundID, WaveData *sound)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HRESULT hr;

	if ((resFile == NULL) || (sound == NULL))
	{
		return E_INVALIDARG;
	}
	hr = Gp_GetSoundFilename(filename, ARRAYSIZE(filename), soundID);
	if (FAILED(hr))
	{
		return hr;
	}
	buffer = Gp_ExtractFromResFile(resFile, filename, &length);
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

//--------------------------------------------------------------  Gp_OpenResFile

static Gp_HouseFile *
Gp_OpenResFile (PCWSTR fileName)
{
	Gp_HouseFile *resFile;
	HRESULT hr;
	errno_t err;
	mz_bool mz_succeeded;

	if (fileName == NULL)
	{
		return NULL;
	}
	resFile = (Gp_HouseFile *)calloc(1, sizeof(*resFile));
	if (resFile == NULL)
	{
		return NULL;
	}
	hr = StringCchCopyW(resFile->fileName, ARRAYSIZE(resFile->fileName), fileName);
	if (FAILED(hr))
	{
		free(resFile);
		return NULL;
	}
	err = _wfopen_s(&resFile->filePtr, fileName, L"rb");
	if ((err != 0) || (resFile->filePtr == NULL))
	{
		free(resFile);
		return NULL;
	}
	mz_zip_zero_struct(&resFile->archive);
	mz_succeeded = mz_zip_reader_init_cfile(&resFile->archive, resFile->filePtr, 0, 0);
	if (!mz_succeeded)
	{
		fclose(resFile->filePtr);
		free(resFile);
		return NULL;
	}
	return resFile;
}

//--------------------------------------------------------------  Gp_CloseResFile

static void
Gp_CloseResFile (Gp_HouseFile *resFile)
{
	if (resFile == NULL)
	{
		return;
	}
	mz_zip_end(&resFile->archive);
	fclose(resFile->filePtr);
	free(resFile);
}

//--------------------------------------------------------------  Gp_LoadBuiltInAssets

HRESULT Gp_LoadBuiltInAssets (void)
{
	WCHAR mermaidFileName[MAX_PATH];
	DWORD numChars;
	wchar_t *lastSlash;
	HRESULT hr;
	Gp_HouseFile *resFile;

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
	resFile = Gp_OpenResFile(mermaidFileName);
	if (resFile == NULL)
	{
		return E_FAIL;
	}
	g_mermaidRes = resFile;
	return S_OK;
}

//--------------------------------------------------------------  Gp_UnloadBuiltInAssets

void Gp_UnloadBuiltInAssets (void)
{
	Gp_CloseResFile(g_mermaidRes);
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

	if (_wfopen_s(&newHouseFilePtr, fileName, L"wb") != 0)
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

HRESULT Gp_LoadHouseFile (PCWSTR fileName)
{
	Gp_HouseFile *resFile;

	Gp_UnloadHouseFile();
	resFile = Gp_OpenResFile(fileName);
	if (resFile == NULL)
	{
		return E_FAIL;
	}
	g_houseRes = resFile;
	return S_OK;
}

//--------------------------------------------------------------  Gp_UnloadHouseFile

void Gp_UnloadHouseFile (void)
{
	Gp_CloseResFile(g_houseRes);
	g_houseRes = NULL;
}

//--------------------------------------------------------------  Gp_HouseFileLoaded

BOOLEAN Gp_HouseFileLoaded (void)
{
	return (g_houseRes != NULL);
}

//--------------------------------------------------------------  Gp_HouseFileReadOnly

BOOLEAN Gp_HouseFileReadOnly (void)
{
	DWORD fileAttributes;

	if (!Gp_HouseFileLoaded())
	{
		return TRUE;
	}
	fileAttributes = GetFileAttributesW(g_houseRes->fileName);
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		return TRUE;
	}
	return (fileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
}

//--------------------------------------------------------------  Gp_LoadHouseIcon

HICON Gp_LoadHouseIcon (UINT width, UINT height)
{
	void *buffer;
	size_t length;
	HICON houseIcon;

	buffer = Gp_ExtractFromResFile(g_houseRes, HOUSE_ICON_NAME, &length);
	if (buffer == NULL)
	{
		return NULL;
	}
	houseIcon = LoadMemoryICO(buffer, length, width, height);
	free(buffer);
	return houseIcon;
}

//--------------------------------------------------------------  Gp_HouseFileDataSize

uint64_t Gp_HouseFileDataSize (void)
{
	return Gp_FileSizeInResFile(g_houseRes, HOUSE_DATA_NAME);
}

//--------------------------------------------------------------  Gp_ReadHouseData

HRESULT Gp_ReadHouseData (houseType *house)
{
	void *buffer;
	size_t length;
	byteio reader;
	int succeeded;

	if (house == NULL)
	{
		return E_INVALIDARG;
	}
	buffer = Gp_ExtractFromResFile(g_houseRes, HOUSE_DATA_NAME, &length);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	if (!byteio_init_memory_reader(&reader, buffer, length))
	{
		free(buffer);
		return E_FAIL;
	}
	succeeded = ReadHouseType(&reader, house);
	byteio_close(&reader);
	free(buffer);
	return succeeded ? S_OK : E_FAIL;
}

//--------------------------------------------------------------  concat_strings

static HRESULT
concat_strings (PWSTR lpOutput, size_t cchOutput, PCWSTR stringA, PCWSTR stringB)
{
	HRESULT hr;

	hr = StringCchCopyW(lpOutput, cchOutput, stringA);
	if (SUCCEEDED(hr))
	{
		hr = StringCchCatW(lpOutput, cchOutput, stringB);
	}
	return hr;
}

//--------------------------------------------------------------  write_house_to_zip

static HRESULT
write_house_to_zip (mz_zip_archive *archive, const houseType *house)
{
	byteio dataWriter;
	void *dataBuffer;
	size_t dataLength;
	mz_bool succeeded;

	if (!byteio_init_memory_writer(&dataWriter, 0))
	{
		return E_OUTOFMEMORY;
	}
	if (!WriteHouseType(&dataWriter, house))
	{
		byteio_close(&dataWriter);
		return E_OUTOFMEMORY;
	}
	if (!byteio_close_and_get_buffer(&dataWriter, &dataBuffer, &dataLength))
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

HRESULT Gp_WriteHouseData (const houseType *house)
{
	wchar_t backUpFileName[MAX_PATH];
	wchar_t outputFileName[MAX_PATH];
	wchar_t originalFileName[MAX_PATH];
	FILE *outputFilePtr;
	mz_zip_archive outputArchive;
	mz_uint fileIndex;
	mz_uint houseDataIndex;
	mz_uint numFiles;
	DWORD lastError;
	HRESULT hr;

	if (house == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return E_FAIL;
	}

	hr = StringCchCopyW(originalFileName, ARRAYSIZE(originalFileName), g_houseRes->fileName);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = concat_strings(backUpFileName, ARRAYSIZE(backUpFileName), originalFileName, L".bak");
	if (FAILED(hr))
	{
		return hr;
	}
	hr = concat_strings(outputFileName, ARRAYSIZE(outputFileName), originalFileName, L".tmp");
	if (FAILED(hr))
	{
		return hr;
	}

	if (_wfopen_s(&outputFilePtr, outputFileName, L"wb") != 0)
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

	numFiles = mz_zip_reader_get_num_files(&g_houseRes->archive);
	if (!mz_zip_reader_locate_file_v2(&g_houseRes->archive, HOUSE_DATA_NAME, NULL, 0, &houseDataIndex))
	{
		houseDataIndex = numFiles;
	}
	hr = S_OK;
	for (fileIndex = 0; fileIndex < numFiles; fileIndex++)
	{
		if (fileIndex != houseDataIndex)
		{
			if (!mz_zip_writer_add_from_zip_reader(&outputArchive, &g_houseRes->archive, fileIndex))
			{
				hr = E_FAIL;
				break;
			}
		}
	}
	if (SUCCEEDED(hr))
	{
		hr = write_house_to_zip(&outputArchive, house);
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

	Gp_UnloadHouseFile();
	lastError = ERROR_SUCCESS;
	if (!ReplaceFileW(originalFileName, outputFileName, backUpFileName, 0, NULL, NULL))
	{
		_wremove(outputFileName);
		lastError = GetLastError();
		if (lastError == ERROR_UNABLE_TO_MOVE_REPLACEMENT_2)
		{
			if (!MoveFileExW(backUpFileName, originalFileName, MOVEFILE_REPLACE_EXISTING))
			{
				lastError = GetLastError();
			}
		}
	}
	hr = Gp_LoadHouseFile(originalFileName);
	_wremove(backUpFileName);
	if (SUCCEEDED(hr) && lastError != ERROR_SUCCESS)
	{
		hr = HRESULT_FROM_WIN32(lastError);
	}
	return hr;
}

//--------------------------------------------------------------  Gp_LoadImage

HBITMAP Gp_LoadImage (SInt16 imageID)
{
	HBITMAP image;

	image = Gp_LoadHouseImage(imageID);
	if (image != NULL)
	{
		return image;
	}
	image = Gp_LoadBuiltInImage(imageID);
	if (image != NULL)
	{
		return image;
	}
	return NULL;
}

//--------------------------------------------------------------  Gp_LoadImageAsDIB

HBITMAP Gp_LoadImageAsDIB (SInt16 imageID)
{
	HBITMAP image;

	image = Gp_LoadHouseImageAsDIB(imageID);
	if (image != NULL)
	{
		return image;
	}
	image = Gp_LoadBuiltInImageAsDIB(imageID);
	if (image != NULL)
	{
		return image;
	}
	return NULL;
}

//--------------------------------------------------------------  Gp_LoadSound

HRESULT Gp_LoadSound (SInt16 soundID, WaveData *sound)
{
	HRESULT hr;

	if (sound == NULL)
	{
		return E_INVALIDARG;
	}
	hr = Gp_LoadHouseSound(soundID, sound);
	if (FAILED(hr))
	{
		hr = Gp_LoadBuiltInSound(soundID, sound);
	}
	return hr;
}

//--------------------------------------------------------------  Gp_LoadBuiltInImage

HBITMAP Gp_LoadBuiltInImage (SInt16 imageID)
{
	return Gp_LoadImageFromResFile(g_mermaidRes, imageID);
}

//--------------------------------------------------------------  Gp_LoadBuiltInImageAsDIB

HBITMAP Gp_LoadBuiltInImageAsDIB (SInt16 imageID)
{
	return Gp_LoadImageAsDIBFromResFile(g_mermaidRes, imageID);
}

//--------------------------------------------------------------  Gp_LoadBuiltInSound

HRESULT Gp_LoadBuiltInSound (SInt16 soundID, WaveData *sound)
{
	return Gp_LoadSoundFromResFile(g_mermaidRes, soundID, sound);
}

//--------------------------------------------------------------  Gp_GetFirstImageID

SInt16 Gp_GetFirstHouseImageID (SInt16 minimum, SInt16 maximum, SInt16 def)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	mz_uint index;
	mz_uint numFiles;
	Boolean foundImage;
	SInt16 firstID;
	int thisID;

	if (!Gp_HouseFileLoaded())
	{
		return def;
	}
	foundImage = false;
	firstID = def;
	numFiles = mz_zip_reader_get_num_files(&g_houseRes->archive);
	for (index = 0; index < numFiles; ++index)
	{
		if (!mz_zip_reader_get_filename(&g_houseRes->archive, index, filename, sizeof(filename)))
		{
			continue;
		}
		if (sscanf_s(filename, IMAGES_FORMAT_STRING, &thisID) == 1)
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

BOOLEAN Gp_HouseImageExists (SInt16 imageID)
{
	return Gp_ImageExistsInResFile(g_houseRes, imageID);
}

//--------------------------------------------------------------  Gp_LoadHouseImage

HBITMAP Gp_LoadHouseImage (SInt16 imageID)
{
	return Gp_LoadImageFromResFile(g_houseRes, imageID);
}

//--------------------------------------------------------------  Gp_LoadHouseImageAsDIB

HBITMAP Gp_LoadHouseImageAsDIB (SInt16 imageID)
{
	return Gp_LoadImageAsDIBFromResFile(g_houseRes, imageID);
}

//--------------------------------------------------------------  Gp_LoadHouseSound

HRESULT Gp_LoadHouseSound (SInt16 soundID, WaveData *sound)
{
	return Gp_LoadSoundFromResFile(g_houseRes, soundID, sound);
}

//--------------------------------------------------------------  Gp_LoadHouseBounding

HRESULT Gp_LoadHouseBounding (SInt16 imageID, boundsType *bounds)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	byteio byteReader;
	int succeeded;
	HRESULT hr;

	if (bounds == NULL)
	{
		return E_INVALIDARG;
	}
	hr = Gp_GetBoundFilename(filename, ARRAYSIZE(filename), imageID);
	if (FAILED(hr))
	{
		return hr;
	}
	buffer = Gp_ExtractFromResFile(g_houseRes, filename, &length);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	if (!byteio_init_memory_reader(&byteReader, buffer, length))
	{
		free(buffer);
		return E_OUTOFMEMORY;
	}
	hr = S_OK;
	succeeded = ReadBoundsType(&byteReader, bounds);
	byteio_close(&byteReader);
	free(buffer);
	if (!succeeded)
	{
		hr = E_FAIL;
	}
	return hr;
}

