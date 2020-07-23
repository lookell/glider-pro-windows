#include "ResourceLoader.h"

#include "ImageIO.h"
#include "miniz.h"
#include "StructIO.h"

#include <stdio.h>

static wchar_t g_mermaidFileName[MAX_PATH];
static FILE *g_mermaidFilePtr = NULL;
static mz_zip_archive g_mermaidArchive;

static wchar_t g_houseFileName[MAX_PATH];
static FILE *g_houseFilePtr = NULL;
static mz_zip_archive g_houseArchive;

//--------------------------------------------------------------  Gp_FileSizeInZip

static uint64_t
Gp_FileSizeInZip (mz_zip_archive *archive, const char *filename)
{
	mz_uint32 file_index;
	mz_zip_archive_file_stat file_stat;

	if (!mz_zip_reader_locate_file_v2(archive, filename, NULL, 0, &file_index))
	{
		return 0;
	}
	if (!mz_zip_reader_file_stat(archive, file_index, &file_stat))
	{
		return 0;
	}
	return file_stat.m_uncomp_size;
}

//--------------------------------------------------------------  Gp_GetImageFilename

static HRESULT
Gp_GetImageFilename (PSTR pszDest, size_t cchDest, SInt16 imageID)
{
	return StringCchPrintfA(pszDest, cchDest, "images/%d.bmp", (int)imageID);
}

//--------------------------------------------------------------  Gp_EnumImagesInZip

static HRESULT
Gp_EnumImagesInZip (mz_zip_archive *archive, Gp_EnumResProc enumProc, void *userData)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	mz_uint idx, numImages;
	int resID;

	numImages = mz_zip_reader_get_num_files(archive);
	for (idx = 0; idx < numImages; idx++)
	{
		if (!mz_zip_reader_get_filename(archive, idx, filename, sizeof(filename)))
		{
			return E_FAIL;
		}
		if (sscanf_s(filename, "images/%d.bmp", &resID) == 1)
		{
			if (!enumProc((SInt16)resID, userData))
			{
				break;
			}
		}
	}
	return S_OK;
}

//--------------------------------------------------------------  Gp_ImageExistsInZip

static BOOLEAN
Gp_ImageExistsInZip (mz_zip_archive *archive, SInt16 imageID)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

	if (FAILED(Gp_GetImageFilename(filename, ARRAYSIZE(filename), imageID)))
	{
		return FALSE;
	}
	return (mz_zip_reader_locate_file(archive, filename, NULL, 0) >= 0);
}

//--------------------------------------------------------------  Gp_ImageSizeInZip

static uint64_t
Gp_ImageSizeInZip (mz_zip_archive *archive, SInt16 imageID)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

	if (FAILED(Gp_GetImageFilename(filename, ARRAYSIZE(filename), imageID)))
	{
		return 0;
	}
	return Gp_FileSizeInZip(archive, filename);
}

//--------------------------------------------------------------  Gp_LoadImageFromZip

static HRESULT
Gp_LoadImageFromZip (mz_zip_archive *archive, SInt16 imageID, HBITMAP *image)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HRESULT hr;

	hr = Gp_GetImageFilename(filename, ARRAYSIZE(filename), imageID);
	if (FAILED(hr))
	{
		return hr;
	}
	buffer = mz_zip_reader_extract_file_to_heap(archive, filename, &length, 0);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	hr = LoadMemoryBMP(image, buffer, length);
	free(buffer);
	return hr;
}

//--------------------------------------------------------------  Gp_LoadImageAsDIBFromZip

static HRESULT
Gp_LoadImageAsDIBFromZip (mz_zip_archive *archive, SInt16 imageID, HBITMAP *image)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HRESULT hr;

	hr = Gp_GetImageFilename(filename, ARRAYSIZE(filename), imageID);
	if (FAILED(hr))
	{
		return hr;
	}
	buffer = mz_zip_reader_extract_file_to_heap(archive, filename, &length, 0);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	hr = LoadMemoryBMPAsDIBSection(image, buffer, length);
	free(buffer);
	return hr;
}

//--------------------------------------------------------------  Gp_GetSoundFilename

static HRESULT
Gp_GetSoundFilename (PSTR pszDest, size_t cchDest, SInt16 soundID)
{
	return StringCchPrintfA(pszDest, cchDest, "sounds/%d.wav", (int)soundID);
}

//--------------------------------------------------------------  Gp_EnumSoundsInZip

static HRESULT
Gp_EnumSoundsInZip (mz_zip_archive *archive, Gp_EnumResProc enumProc, void *userData)
{
	mz_uint idx;
	mz_uint numImages;
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	int resID;

	numImages = mz_zip_reader_get_num_files(archive);
	for (idx = 0; idx < numImages; idx++)
	{
		if (!mz_zip_reader_get_filename(archive, idx, filename, sizeof(filename)))
		{
			return E_FAIL;
		}
		if (sscanf_s(filename, "sounds/%d.wav", &resID) == 1)
		{
			if (!enumProc((SInt16)resID, userData))
			{
				break;
			}
		}
	}
	return S_OK;
}

//--------------------------------------------------------------  Gp_SoundExistsInZip

static BOOLEAN
Gp_SoundExistsInZip (mz_zip_archive *archive, SInt16 soundID)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

	if (FAILED(Gp_GetSoundFilename(filename, ARRAYSIZE(filename), soundID)))
	{
		return FALSE;
	}
	return (mz_zip_reader_locate_file(archive, filename, NULL, 0) >= 0);
}

//--------------------------------------------------------------  Gp_ImageSizeInZip

static uint64_t
Gp_SoundSizeInZip (mz_zip_archive *archive, SInt16 soundID)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

	if (FAILED(Gp_GetSoundFilename(filename, ARRAYSIZE(filename), soundID)))
	{
		return 0;
	}
	return Gp_FileSizeInZip(archive, filename);
}

//--------------------------------------------------------------  Gp_LoadSoundFromZip

static HRESULT
Gp_LoadSoundFromZip (mz_zip_archive *archive, SInt16 soundID, WaveData *sound)
{
	char filename[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
	void *buffer;
	size_t length;
	HRESULT hr;

	hr = Gp_GetSoundFilename(filename, ARRAYSIZE(filename), soundID);
	if (FAILED(hr))
	{
		return hr;
	}
	buffer = mz_zip_reader_extract_file_to_heap(archive, filename, &length, 0);
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
	sound->dataBytes = buffer;
	return S_OK;
}

//--------------------------------------------------------------  Gp_LoadBuiltInAssets

HRESULT Gp_LoadBuiltInAssets (void)
{
	DWORD numChars;
	wchar_t *lastSlash;
	HRESULT hr;

	if (g_mermaidFilePtr != NULL)
	{
		return S_OK;
	}
	numChars = GetModuleFileNameW(HINST_THISCOMPONENT,
		g_mermaidFileName, ARRAYSIZE(g_mermaidFileName));
	if (numChars == ARRAYSIZE(g_mermaidFileName))
	{
		return E_NOT_SUFFICIENT_BUFFER;
	}
	else if (numChars == 0)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	lastSlash = wcsrchr(g_mermaidFileName, L'\\');
	if (lastSlash == NULL)
	{
		lastSlash = &g_mermaidFileName[0];
	}
	*lastSlash = L'\0';
	hr = StringCchCatW(g_mermaidFileName, ARRAYSIZE(g_mermaidFileName), L"\\Mermaid.dat");
	if (FAILED(hr))
	{
		return hr;
	}
	if (_wfopen_s(&g_mermaidFilePtr, g_mermaidFileName, L"rb") != 0)
	{
		g_mermaidFilePtr = NULL;
		return E_FAIL;
	}
	// NOTE: The zip archive structure must be passed directly for initialization,
	// because it sets up pointers to itself. Do not initialize it separately and
	// then move the result into g_mermaidArchive!
	mz_zip_zero_struct(&g_mermaidArchive);
	if (!mz_zip_reader_init_cfile(&g_mermaidArchive, g_mermaidFilePtr, 0, 0))
	{
		fclose(g_mermaidFilePtr);
		g_mermaidFilePtr = NULL;
		return E_FAIL;
	}
	return S_OK;
}

//--------------------------------------------------------------  Gp_UnloadBuiltInAssets

void Gp_UnloadBuiltInAssets (void)
{
	if (Gp_BuiltInAssetsLoaded())
	{
		mz_zip_reader_end(&g_mermaidArchive);
		fclose(g_mermaidFilePtr);
		g_mermaidFilePtr = NULL;
	}
}

//--------------------------------------------------------------  Gp_BuiltInAssetsLoaded

BOOLEAN Gp_BuiltInAssetsLoaded (void)
{
	return (g_mermaidFilePtr != NULL);
}

//--------------------------------------------------------------  Gp_LoadHouseFile

HRESULT Gp_LoadHouseFile (PCWSTR fileName)
{
	UNREFERENCED_PARAMETER(fileName);

	return E_NOTIMPL;
}

//--------------------------------------------------------------  Gp_UnloadHouseFile

void Gp_UnloadHouseFile (void)
{
	if (g_houseFilePtr != NULL)
	{
		mz_zip_reader_end(&g_houseArchive);
		fclose(g_houseFilePtr);
		g_houseFilePtr = NULL;
	}
}

//--------------------------------------------------------------  Gp_HouseFileLoaded

BOOLEAN Gp_HouseFileLoaded (void)
{
	return (g_houseFilePtr != NULL);
}

//--------------------------------------------------------------  Gp_HouseFileReadOnly

BOOLEAN Gp_HouseFileReadOnly (void)
{
	DWORD fileAttributes;

	if (!Gp_HouseFileLoaded())
	{
		return TRUE;
	}
	fileAttributes = GetFileAttributesW(g_houseFileName);
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		return TRUE;
	}
	return (fileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
}

//--------------------------------------------------------------  Gp_HouseHasIcon

BOOLEAN Gp_HouseHasIcon (void)
{
	if (!Gp_HouseFileLoaded())
	{
		return FALSE;
	}
	return (mz_zip_reader_locate_file(&g_houseArchive, "house.ico", NULL, 0) >= 0);
}

//--------------------------------------------------------------  Gp_LoadHouseIcon

HRESULT Gp_LoadHouseIcon (HICON *houseIcon, UINT width, UINT height)
{
	void *buffer;
	size_t length;
	HRESULT hr;

	if (houseIcon == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	buffer = mz_zip_reader_extract_file_to_heap(&g_houseArchive, "house.ico", &length, 0);
	if (buffer == NULL)
	{
		return E_FAIL;
	}
	hr = LoadMemoryICO(houseIcon, buffer, length, width, height);
	free(buffer);
	return hr;
}

//--------------------------------------------------------------  Gp_ReadHouseData

HRESULT Gp_ReadHouseData (houseType *house)
{
	void *buffer;
	size_t length;
	byteio reader;
	int succeeded;
	HRESULT hr;

	if (house == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	buffer = mz_zip_reader_extract_file_to_heap(&g_houseArchive, "house.dat", &length, 0);
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

//--------------------------------------------------------------  Gp_WriteHouseData

HRESULT Gp_WriteHouseData (const houseType *house)
{
	UNREFERENCED_PARAMETER(house);

	return E_NOTIMPL;
}

//--------------------------------------------------------------  Gp_ImageExists

BOOLEAN Gp_ImageExists (SInt16 imageID)
{
	return Gp_HouseImageExists(imageID) || Gp_BuiltInImageExists(imageID);
}

//--------------------------------------------------------------  Gp_LoadImage

HRESULT Gp_LoadImage (SInt16 imageID, HBITMAP *image)
{
	HRESULT hr;

	if (image == NULL)
	{
		return E_INVALIDARG;
	}
	hr = Gp_LoadHouseImage(imageID, image);
	if (FAILED(hr))
	{
		hr = Gp_LoadBuiltInImage(imageID, image);
	}
	return hr;
}

//--------------------------------------------------------------  Gp_LoadImageAsDIB

HRESULT Gp_LoadImageAsDIB (SInt16 imageID, HBITMAP *image)
{
	HRESULT hr;

	if (image == NULL)
	{
		return E_INVALIDARG;
	}
	hr = Gp_LoadHouseImageAsDIB(imageID, image);
	if (FAILED(hr))
	{
		hr = Gp_LoadBuiltInImageAsDIB(imageID, image);
	}
	return hr;
}

//--------------------------------------------------------------  Gp_SoundExists

BOOLEAN Gp_SoundExists (SInt16 soundID)
{
	return Gp_HouseSoundExists(soundID) || Gp_BuiltInSoundExists(soundID);
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

//--------------------------------------------------------------  Gp_EnumBuiltInImages

HRESULT Gp_EnumBuiltInImages (Gp_EnumResProc enumProc, void *userData)
{
	if (enumProc == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_BuiltInAssetsLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_EnumImagesInZip(&g_mermaidArchive, enumProc, userData);
}

//--------------------------------------------------------------  Gp_BuiltInImageExists

BOOLEAN Gp_BuiltInImageExists (SInt16 imageID)
{
	if (!Gp_BuiltInAssetsLoaded())
	{
		return FALSE;
	}
	return Gp_ImageExistsInZip(&g_mermaidArchive, imageID);
}

//--------------------------------------------------------------  Gp_BuiltInImageSize

uint64_t Gp_BuiltInImageSize (SInt16 imageID)
{
	if (!Gp_BuiltInAssetsLoaded())
	{
		return 0;
	}
	return Gp_ImageSizeInZip(&g_mermaidArchive, imageID);
}

//--------------------------------------------------------------  Gp_LoadBuiltInImage

HRESULT Gp_LoadBuiltInImage (SInt16 imageID, HBITMAP *image)
{
	if (image == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_BuiltInAssetsLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_LoadImageFromZip(&g_mermaidArchive, imageID, image);
}

//--------------------------------------------------------------  Gp_LoadBuiltInImageAsDIB

HRESULT Gp_LoadBuiltInImageAsDIB (SInt16 imageID, HBITMAP *image)
{
	if (image == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_BuiltInAssetsLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_LoadImageAsDIBFromZip(&g_mermaidArchive, imageID, image);
}

//--------------------------------------------------------------  Gp_EnumBuiltInSounds

HRESULT Gp_EnumBuiltInSounds (Gp_EnumResProc enumProc, void *userData)
{
	if (enumProc == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_BuiltInAssetsLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_EnumSoundsInZip(&g_mermaidArchive, enumProc, userData);
}

//--------------------------------------------------------------  Gp_BuiltInSoundExists

BOOLEAN Gp_BuiltInSoundExists (SInt16 soundID)
{
	if (!Gp_BuiltInAssetsLoaded())
	{
		return FALSE;
	}
	return Gp_SoundExistsInZip(&g_mermaidArchive, soundID);
}

//--------------------------------------------------------------  Gp_BuiltInSoundSize

uint64_t Gp_BuiltInSoundSize (SInt16 soundID)
{
	if (!Gp_BuiltInAssetsLoaded())
	{
		return 0;
	}
	return Gp_SoundSizeInZip(&g_mermaidArchive, soundID);
}

//--------------------------------------------------------------  Gp_LoadBuiltInSound

HRESULT Gp_LoadBuiltInSound (SInt16 soundID, WaveData *sound)
{
	if (sound == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_BuiltInAssetsLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_LoadSoundFromZip(&g_mermaidArchive, soundID, sound);
}

//--------------------------------------------------------------  Gp_EnumHouseImages

HRESULT Gp_EnumHouseImages (Gp_EnumResProc enumProc, void *userData)
{
	if (enumProc == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_EnumImagesInZip(&g_houseArchive, enumProc, userData);
}

//--------------------------------------------------------------  Gp_HouseImageExists

BOOLEAN Gp_HouseImageExists (SInt16 imageID)
{
	if (!Gp_HouseFileLoaded())
	{
		return FALSE;
	}
	return Gp_ImageExistsInZip(&g_houseArchive, imageID);
}

//--------------------------------------------------------------  Gp_HouseImageSize

uint64_t Gp_HouseImageSize (SInt16 imageID)
{
	if (!Gp_HouseFileLoaded())
	{
		return 0;
	}
	return Gp_ImageSizeInZip(&g_houseArchive, imageID);
}

//--------------------------------------------------------------  Gp_LoadHouseImage

HRESULT Gp_LoadHouseImage (SInt16 imageID, HBITMAP *image)
{
	if (image == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_LoadImageFromZip(&g_houseArchive, imageID, image);
}

//--------------------------------------------------------------  Gp_LoadHouseImageAsDIB

HRESULT Gp_LoadHouseImageAsDIB (SInt16 imageID, HBITMAP *image)
{
	if (image == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_LoadImageAsDIBFromZip(&g_houseArchive, imageID, image);
}

//--------------------------------------------------------------  Gp_EnumHouseSounds

HRESULT Gp_EnumHouseSounds (Gp_EnumResProc enumProc, void *userData)
{
	if (enumProc == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return E_ILLEGAL_METHOD_CALL;
	}
	return Gp_EnumSoundsInZip(&g_houseArchive, enumProc, userData);
}

//--------------------------------------------------------------  Gp_HouseSoundExists

BOOLEAN Gp_HouseSoundExists (SInt16 soundID)
{
	if (!Gp_HouseFileLoaded())
	{
		return FALSE;
	}
	return Gp_SoundExistsInZip(&g_houseArchive, soundID);
}

//--------------------------------------------------------------  Gp_HouseSoundSize

uint64_t Gp_HouseSoundSize (SInt16 soundID)
{
	if (!Gp_HouseFileLoaded())
	{
		return 0;
	}
	return Gp_SoundSizeInZip(&g_houseArchive, soundID);
}

//--------------------------------------------------------------  Gp_LoadHouseSound

HRESULT Gp_LoadHouseSound (SInt16 soundID, WaveData *sound)
{
	if (sound == NULL)
	{
		return E_INVALIDARG;
	}
	if (!Gp_HouseFileLoaded())
	{
		return FALSE;
	}
	return Gp_LoadSoundFromZip(&g_houseArchive, soundID, sound);
}

