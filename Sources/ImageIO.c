#define GP_USE_WINAPI_H

#include "ImageIO.h"

#include <intsafe.h>

#include <stddef.h>
#include <stdlib.h>

#define LOAD_BMP_AS_COMPATIBLE FALSE
#define LOAD_BMP_AS_DIBSECTION TRUE

#define RETURN_IF_FAILED(hr) \
	do { HRESULT hr__ = (hr); if (FAILED(hr__)) { return hr__; } } while (0)

#define FAIL_IF_TRUE(cond) \
	do { if (cond) { return E_FAIL; } } while (0)

#define INVALIDARG_IF_TRUE(cond) \
	do { if (cond) { return E_INVALIDARG; } } while (0)

typedef struct ByteSlice
{
	const unsigned char *buffer;
	size_t length;
} ByteSlice;

static HRESULT GetDIBImageSize(const BITMAPINFOHEADER *bmiHeader, DWORD *pImageSize)
{
	DWORD width, height, bitCount;
	DWORD widthBits, scanlineSize;

	width = (DWORD)bmiHeader->biWidth;
	height = (DWORD)labs(bmiHeader->biHeight);
	bitCount = bmiHeader->biBitCount;
	RETURN_IF_FAILED(DWordMult(width, bitCount, &widthBits));
	RETURN_IF_FAILED(DWordAdd(widthBits, 31, &scanlineSize));
	scanlineSize = (scanlineSize & (~31)) / 8;
	RETURN_IF_FAILED(DWordMult(scanlineSize, height, pImageSize));
	return S_OK;
}

static HRESULT ReadBYTE(ByteSlice *slice, BYTE *pValue)
{
	FAIL_IF_TRUE(slice->length < 1);
	*pValue = (BYTE)slice->buffer[0];
	slice->buffer += 1;
	slice->length -= 1;
	return S_OK;
}

static HRESULT ReadWORD(ByteSlice *slice, WORD *pValue)
{
	FAIL_IF_TRUE(slice->length < 2);
	*pValue = ((WORD)slice->buffer[0])
		| ((WORD)slice->buffer[1] << 8);
	slice->buffer += 2;
	slice->length -= 2;
	return S_OK;
}

static HRESULT ReadDWORD(ByteSlice *slice, DWORD *pValue)
{
	FAIL_IF_TRUE(slice->length < 4);
	*pValue = ((DWORD)slice->buffer[0])
		| ((DWORD)slice->buffer[1] << 8)
		| ((DWORD)slice->buffer[2] << 16)
		| ((DWORD)slice->buffer[3] << 24);
	slice->buffer += 4;
	slice->length -= 4;
	return S_OK;
}

static HRESULT ReadLONG(ByteSlice *slice, LONG *pValue)
{
	FAIL_IF_TRUE(slice->length < 4);
	*pValue = (LONG)(((DWORD)slice->buffer[0])
		| ((DWORD)slice->buffer[1] << 8)
		| ((DWORD)slice->buffer[2] << 16)
		| ((DWORD)slice->buffer[3] << 24));
	slice->buffer += 4;
	slice->length -= 4;
	return S_OK;
}

static HRESULT ReadRGBTRIPLE(ByteSlice *slice, RGBTRIPLE *output)
{
	ByteSlice source;
	RGBTRIPLE value;

	source = *slice;
	RETURN_IF_FAILED(ReadBYTE(&source, &value.rgbtBlue));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.rgbtGreen));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.rgbtRed));
	*slice = source;
	*output = value;
	return S_OK;
}

static HRESULT ReadRGBQUAD(ByteSlice *slice, RGBQUAD *output)
{
	ByteSlice source;
	RGBQUAD value;

	source = *slice;
	RETURN_IF_FAILED(ReadBYTE(&source, &value.rgbBlue));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.rgbGreen));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.rgbRed));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.rgbReserved));
	*slice = source;
	*output = value;
	return S_OK;
}

static HRESULT CheckBITMAPFILEHEADER(const BITMAPFILEHEADER *value)
{
	FAIL_IF_TRUE(value->bfType != MAKEWORD('B', 'M'));
	FAIL_IF_TRUE(value->bfReserved1 != 0);
	FAIL_IF_TRUE(value->bfReserved2 != 0);
	FAIL_IF_TRUE(value->bfSize == 0);
	FAIL_IF_TRUE(value->bfOffBits >= value->bfSize);
	return S_OK;
}

static HRESULT ReadBITMAPFILEHEADER(ByteSlice *slice, BITMAPFILEHEADER *output)
{
	ByteSlice source;
	// Avoid unaligned pointers by reading into local variables
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;

	source = *slice;
	RETURN_IF_FAILED(ReadWORD(&source, &bfType));
	RETURN_IF_FAILED(ReadDWORD(&source, &bfSize));
	RETURN_IF_FAILED(ReadWORD(&source, &bfReserved1));
	RETURN_IF_FAILED(ReadWORD(&source, &bfReserved2));
	RETURN_IF_FAILED(ReadDWORD(&source, &bfOffBits));
	output->bfType = bfType;
	output->bfSize = bfSize;
	output->bfReserved1 = bfReserved1;
	output->bfReserved2 = bfReserved2;
	output->bfOffBits = bfOffBits;
	RETURN_IF_FAILED(CheckBITMAPFILEHEADER(output));
	*slice = source;
	return S_OK;
}

static HRESULT CheckBITMAPCOREHEADER(const BITMAPCOREHEADER *value)
{
	FAIL_IF_TRUE(value->bcSize != sizeof(BITMAPCOREHEADER));
	FAIL_IF_TRUE(value->bcWidth == 0);
	FAIL_IF_TRUE(value->bcHeight == 0);
	FAIL_IF_TRUE(value->bcPlanes != 1);
	FAIL_IF_TRUE((value->bcBitCount != 1)
		&& (value->bcBitCount != 4)
		&& (value->bcBitCount != 8)
		&& (value->bcBitCount != 24));
	return S_OK;
}

static HRESULT ReadBITMAPCOREHEADER(ByteSlice *slice, BITMAPCOREHEADER *output)
{
	ByteSlice source;
	BITMAPCOREHEADER value;

	source = *slice;
	RETURN_IF_FAILED(ReadDWORD(&source, &value.bcSize));
	RETURN_IF_FAILED(ReadWORD(&source, &value.bcWidth));
	RETURN_IF_FAILED(ReadWORD(&source, &value.bcHeight));
	RETURN_IF_FAILED(ReadWORD(&source, &value.bcPlanes));
	RETURN_IF_FAILED(ReadWORD(&source, &value.bcBitCount));
	RETURN_IF_FAILED(CheckBITMAPCOREHEADER(&value));
	*slice = source;
	*output = value;
	return S_OK;
}

typedef struct BITMAPCOREINFO256
{
	BITMAPCOREHEADER bmciHeader;
	RGBTRIPLE bmciColors[256];
} BITMAPCOREINFO256;

static HRESULT ReadBITMAPCOREINFO(ByteSlice *slice, BITMAPCOREINFO256 *output)
{
	ByteSlice source;
	BITMAPCOREINFO256 value = { 0 };
	WORD idx, numColors;

	source = *slice;
	RETURN_IF_FAILED(ReadBITMAPCOREHEADER(&source, &value.bmciHeader));
	if (value.bmciHeader.bcBitCount <= 8)
	{
		numColors = 1 << value.bmciHeader.bcBitCount;
		for (idx = 0; idx < numColors; idx++)
		{
			RETURN_IF_FAILED(ReadRGBTRIPLE(&source, &value.bmciColors[idx]));
		}
	}
	*slice = source;
	*output = value;
	return S_OK;
}

static HRESULT CheckBITMAPINFOHEADER(const BITMAPINFOHEADER *value)
{
	DWORD expectedSize;

	// Unsupported values
	FAIL_IF_TRUE(value->biSize < sizeof(BITMAPINFOHEADER));
	FAIL_IF_TRUE(value->biWidth <= 0);
	FAIL_IF_TRUE(value->biHeight == 0);
	FAIL_IF_TRUE(value->biPlanes != 1);
	FAIL_IF_TRUE((value->biBitCount != 1) &&
		(value->biBitCount != 4) &&
		(value->biBitCount != 8) &&
		(value->biBitCount != 16) &&
		(value->biBitCount != 24) &&
		(value->biBitCount != 32));
	FAIL_IF_TRUE((value->biCompression != BI_RGB) &&
		(value->biCompression != BI_RLE8) &&
		(value->biCompression != BI_RLE4));
	FAIL_IF_TRUE(value->biClrUsed > 256);
	FAIL_IF_TRUE(value->biClrUsed == 1);
	FAIL_IF_TRUE(value->biClrImportant > value->biClrUsed);

	// Unsupported combinations
	switch (value->biBitCount)
	{
	case 1:
		FAIL_IF_TRUE(value->biClrUsed > 2);
		break;
	case 4:
		FAIL_IF_TRUE(value->biClrUsed > 16);
		break;
	case 8:
		FAIL_IF_TRUE(value->biClrUsed > 256);
		break;
	case 16:
		FAIL_IF_TRUE(value->biClrUsed != 0);
		break;
	case 24:
		FAIL_IF_TRUE(value->biClrUsed != 0);
		break;
	case 32:
		FAIL_IF_TRUE(value->biClrUsed != 0);
		break;
	}
	switch (value->biCompression)
	{
	case BI_RGB:
		RETURN_IF_FAILED(GetDIBImageSize(value, &expectedSize));
		FAIL_IF_TRUE((value->biSizeImage != 0) && (value->biSizeImage != expectedSize));
		break;
	case BI_RLE8:
		FAIL_IF_TRUE(value->biBitCount != 8);
		FAIL_IF_TRUE(value->biSizeImage == 0);
		break;
	case BI_RLE4:
		FAIL_IF_TRUE(value->biBitCount != 4);
		FAIL_IF_TRUE(value->biSizeImage == 0);
		break;
	}

	return S_OK;
}

static HRESULT ReadBITMAPINFOHEADER(ByteSlice *slice, BITMAPINFOHEADER *output)
{
	ByteSlice source;
	BITMAPINFOHEADER value;

	source = *slice;
	RETURN_IF_FAILED(ReadDWORD(&source, &value.biSize));
	RETURN_IF_FAILED(ReadLONG(&source, &value.biWidth));
	RETURN_IF_FAILED(ReadLONG(&source, &value.biHeight));
	RETURN_IF_FAILED(ReadWORD(&source, &value.biPlanes));
	RETURN_IF_FAILED(ReadWORD(&source, &value.biBitCount));
	RETURN_IF_FAILED(ReadDWORD(&source, &value.biCompression));
	RETURN_IF_FAILED(ReadDWORD(&source, &value.biSizeImage));
	RETURN_IF_FAILED(ReadLONG(&source, &value.biXPelsPerMeter));
	RETURN_IF_FAILED(ReadLONG(&source, &value.biYPelsPerMeter));
	RETURN_IF_FAILED(ReadDWORD(&source, &value.biClrUsed));
	RETURN_IF_FAILED(ReadDWORD(&source, &value.biClrImportant));
	RETURN_IF_FAILED(CheckBITMAPINFOHEADER(&value));
	*slice = source;
	*output = value;
	return S_OK;
}

typedef struct BITMAPINFO256
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
} BITMAPINFO256;

static HRESULT ReadBITMAPINFO(ByteSlice *slice, BITMAPINFO256 *output)
{
	ByteSlice source;
	BITMAPINFO256 value = { 0 };
	DWORD infoHeaderSize;
	DWORD idx, numColors;

	source = *slice;
	RETURN_IF_FAILED(ReadDWORD(&source, &infoHeaderSize));
	if (slice->length < infoHeaderSize)
	{
		return E_FAIL;
	}
	source = *slice;
	if (infoHeaderSize == sizeof(BITMAPCOREHEADER))
	{
		BITMAPCOREINFO256 coreInfo;
		RETURN_IF_FAILED(ReadBITMAPCOREINFO(&source, &coreInfo));
		// Convert the BITMAPCOREINFO structure into a BITMAPINFO structure.
		value.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		value.bmiHeader.biWidth = coreInfo.bmciHeader.bcWidth;
		value.bmiHeader.biHeight = coreInfo.bmciHeader.bcHeight;
		value.bmiHeader.biPlanes = coreInfo.bmciHeader.bcPlanes;
		value.bmiHeader.biBitCount = coreInfo.bmciHeader.bcBitCount;
		value.bmiHeader.biCompression = BI_RGB;
		value.bmiHeader.biSizeImage = 0;
		value.bmiHeader.biXPelsPerMeter = 0;
		value.bmiHeader.biYPelsPerMeter = 0;
		value.bmiHeader.biClrUsed = 0;
		value.bmiHeader.biClrImportant = 0;
		if (value.bmiHeader.biBitCount <= 8)
		{
			numColors = 1 << value.bmiHeader.biBitCount;
			for (idx = 0; idx < numColors; idx++)
			{
				value.bmiColors[idx].rgbBlue = coreInfo.bmciColors[idx].rgbtBlue;
				value.bmiColors[idx].rgbGreen = coreInfo.bmciColors[idx].rgbtGreen;
				value.bmiColors[idx].rgbRed = coreInfo.bmciColors[idx].rgbtRed;
				value.bmiColors[idx].rgbReserved = 0;
			}
		}
	}
	else if (infoHeaderSize < sizeof(BITMAPINFOHEADER))
	{
		return E_FAIL;
	}
	else // (infoHeaderSize >= sizeof(BITMAPINFOHEADER))
	{
		RETURN_IF_FAILED(ReadBITMAPINFOHEADER(&source, &value.bmiHeader));
		// Ignore the extra information beyond what's in the BITMAPINFOHEADER
		value.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		// Skip ahead by the actual header size to find the color table
		source = *slice;
		source.buffer += infoHeaderSize;
		source.length -= infoHeaderSize;
		if (value.bmiHeader.biBitCount <= 8)
		{
			numColors = value.bmiHeader.biClrUsed;
			if (numColors == 0)
			{
				numColors = 1 << value.bmiHeader.biBitCount;
			}
			for (idx = 0; idx < numColors; idx++)
			{
				RETURN_IF_FAILED(ReadRGBQUAD(&source, &value.bmiColors[idx]));
			}
		}
	}
	*slice = source;
	*output = value;
	return S_OK;
}

static HRESULT LoadMemoryBMPImpl(HBITMAP *phBitmap, const ByteSlice *slice, BOOL loadAsDIB)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFO256 bitmapInfo;
	DWORD biSizeImage;
	ByteSlice fileSlice;
	ByteSlice readerSlice;
	ByteSlice bitsSlice;
	const BITMAPINFO *lpbmi;
	int bmWidth;
	int bmHeight;
	HDC referenceDC;
	void *sectionBits;
	HBITMAP hbm;

	fileSlice = *slice;

	readerSlice = fileSlice;
	RETURN_IF_FAILED(ReadBITMAPFILEHEADER(&readerSlice, &fileHeader));
	FAIL_IF_TRUE(fileHeader.bfSize != fileSlice.length);

	RETURN_IF_FAILED(ReadBITMAPINFO(&readerSlice, &bitmapInfo));
	biSizeImage = bitmapInfo.bmiHeader.biSizeImage;
	if (biSizeImage == 0)
	{
		RETURN_IF_FAILED(GetDIBImageSize(&bitmapInfo.bmiHeader, &biSizeImage));
		FAIL_IF_TRUE(biSizeImage == 0);
	}

	bitsSlice = fileSlice;
	bitsSlice.buffer += fileHeader.bfOffBits;
	bitsSlice.length -= fileHeader.bfOffBits;
	FAIL_IF_TRUE(bitsSlice.length < biSizeImage);
	bitsSlice.length = biSizeImage;

	lpbmi = (const BITMAPINFO *)&bitmapInfo;
	if (loadAsDIB)
	{
		hbm = CreateDIBSection(NULL, lpbmi, DIB_RGB_COLORS, &sectionBits, NULL, 0);
		FAIL_IF_TRUE(hbm == NULL);
		memcpy(sectionBits, bitsSlice.buffer, bitsSlice.length);
		*phBitmap = hbm;
	}
	else
	{
		hbm = NULL;
		bmWidth = (int)lpbmi->bmiHeader.biWidth;
		bmHeight = abs((int)lpbmi->bmiHeader.biHeight);
		if (lpbmi->bmiHeader.biBitCount != 1)
		{
			referenceDC = GetDC(NULL);
			if (referenceDC != NULL)
			{
				hbm = CreateCompatibleBitmap(referenceDC, bmWidth, bmHeight);
				ReleaseDC(NULL, referenceDC);
			}
		}
		else
		{
			hbm = CreateBitmap(bmWidth, bmHeight, 1, 1, NULL);
		}
		FAIL_IF_TRUE(hbm == NULL);
		if (!SetDIBits(NULL, hbm, 0, bmHeight, bitsSlice.buffer, lpbmi, DIB_RGB_COLORS))
		{
			DeleteObject(hbm);
			return E_FAIL;
		}
		*phBitmap = hbm;
	}
	return S_OK;
}

HRESULT LoadMemoryBMP(HBITMAP *phBitmap, const void *buffer, size_t length)
{
	ByteSlice fileSlice;

	INVALIDARG_IF_TRUE(phBitmap == NULL);
	INVALIDARG_IF_TRUE(buffer == NULL);

	fileSlice.buffer = (const unsigned char *)buffer;
	fileSlice.length = length;
	return LoadMemoryBMPImpl(phBitmap, &fileSlice, LOAD_BMP_AS_COMPATIBLE);
}

HRESULT LoadMemoryBMPAsDIBSection(HBITMAP *phBitmap, const void *buffer, size_t length)
{
	ByteSlice fileSlice;

	INVALIDARG_IF_TRUE(phBitmap == NULL);
	INVALIDARG_IF_TRUE(buffer == NULL);

	fileSlice.buffer = (const unsigned char *)buffer;
	fileSlice.length = length;
	return LoadMemoryBMPImpl(phBitmap, &fileSlice, LOAD_BMP_AS_DIBSECTION);
}

// To load .ICO files, the directory data and icon image data are converted
// from the .ICO file format to the RT_GROUP_ICON and RT_ICON resource formats.
// The converted directory data and icon image can then be passed to the
// functions LookupIconIdFromDirectoryEx and CreateIconFromResourceEx. This way,
// this icon loader should act similar to Windows, because Windows does most of
// the work.

// The icon directory structures are WORD-aligned
#pragma pack(push, 2)

typedef struct IconFileDirEntry
{
	BYTE Width;
	BYTE Height;
	BYTE ColorCount;
	BYTE Reserved;
	WORD Planes;
	WORD BitCount;
	DWORD BytesInRes;
	DWORD ImageOffset;
} IconFileDirEntry;

C_ASSERT(sizeof(IconFileDirEntry) == 16);

typedef struct IconFileDirectory
{
	WORD Reserved; // Must be 0
	WORD Type; // Must be 1 (RES_ICON)
	WORD Count;
	IconFileDirEntry Entries[ANYSIZE_ARRAY];
} IconFileDirectory;

C_ASSERT(sizeof(IconFileDirectory) == 6 + ANYSIZE_ARRAY * sizeof(IconFileDirEntry));

typedef struct GroupIconDirEntry
{
	BYTE Width;
	BYTE Height;
	BYTE ColorCount;
	BYTE Reserved;
	WORD Planes;
	WORD BitCount;
	DWORD BytesInRes;
	WORD IconID;
} GroupIconDirEntry;

C_ASSERT(sizeof(GroupIconDirEntry) == 14);

typedef struct GroupIconDirectory
{
	WORD Reserved;
	WORD Type;
	WORD Count;
	GroupIconDirEntry Entries[ANYSIZE_ARRAY];
} GroupIconDirectory;

C_ASSERT(sizeof(GroupIconDirectory) == 6 + ANYSIZE_ARRAY * sizeof(GroupIconDirEntry));

#pragma pack(pop)

// The icon image format is the same in both .ICO files and in RT_ICON resources.
// It is the same to a packed DIB (bitmap info followed by pixel bits), except that
// the monochrome mask bits follow the pixel bits and the height field has double the
// icon height to account for the mask. N.B.: the mask bits are always in monochrome
// format, even if the icon itself is not monochrome.

static HRESULT CheckIconFileDirEntry(const IconFileDirEntry *value)
{
	FAIL_IF_TRUE(value->Reserved != 0);
	FAIL_IF_TRUE(value->BytesInRes == 0);
	return S_OK;
}

static HRESULT ReadIconFileDirEntry(ByteSlice *slice, IconFileDirEntry *output)
{
	ByteSlice source;
	IconFileDirEntry value;

	source = *slice;
	RETURN_IF_FAILED(ReadBYTE(&source, &value.Width));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.Height));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.ColorCount));
	RETURN_IF_FAILED(ReadBYTE(&source, &value.Reserved));
	RETURN_IF_FAILED(ReadWORD(&source, &value.Planes));
	RETURN_IF_FAILED(ReadWORD(&source, &value.BitCount));
	RETURN_IF_FAILED(ReadDWORD(&source, &value.BytesInRes));
	RETURN_IF_FAILED(ReadDWORD(&source, &value.ImageOffset));
	RETURN_IF_FAILED(CheckIconFileDirEntry(&value));
	*slice = source;
	*output = value;
	return S_OK;
}

static HRESULT CheckIconFileDirectory(const IconFileDirectory *value)
{
	WORD idx;

	FAIL_IF_TRUE(value->Reserved != 0);
	FAIL_IF_TRUE(value->Type != RES_ICON);
	// If true, it would break the conversion to the RT_GROUP_ICON format
	FAIL_IF_TRUE(value->Count == MAXWORD);
	for (idx = 0; idx < value->Count; idx++)
	{
		CheckIconFileDirEntry(&value->Entries[idx]);
	}
	return S_OK;
}

static HRESULT ReadIconFileDirectory(ByteSlice *slice, IconFileDirectory **output)
{
	ByteSlice source;
	IconFileDirectory *value;
	WORD iconReserved;
	WORD iconType;
	WORD iconCount;
	WORD idx;
	size_t valueSize;

	source = *slice;
	RETURN_IF_FAILED(ReadWORD(&source, &iconReserved));
	RETURN_IF_FAILED(ReadWORD(&source, &iconType));
	RETURN_IF_FAILED(ReadWORD(&source, &iconCount));
	valueSize = offsetof(IconFileDirectory, Entries) + (iconCount * sizeof(IconFileDirEntry));
	value = (IconFileDirectory *)malloc(valueSize);
	if (value == NULL)
	{
		return E_OUTOFMEMORY;
	}
	value->Reserved = iconReserved;
	value->Type = iconType;
	value->Count = iconCount;
	for (idx = 0; idx < iconCount; idx++)
	{
		RETURN_IF_FAILED(ReadIconFileDirEntry(&source, &value->Entries[idx]));
	}
	RETURN_IF_FAILED(CheckIconFileDirectory(value));
	*slice = source;
	*output = value;
	return S_OK;
}

static HRESULT ConvertIconFileDirToGroupDir(
	const IconFileDirectory *fileDir,
	GroupIconDirectory **outputDir)
{
	size_t outputSize;
	GroupIconDirectory *groupDir;
	WORD idx;

	outputSize = offsetof(GroupIconDirectory, Entries)
		+ fileDir->Count * sizeof(GroupIconDirEntry);
	groupDir = (GroupIconDirectory *)malloc(outputSize);
	if (groupDir == NULL)
	{
		return E_OUTOFMEMORY;
	}
	groupDir->Reserved = 0;
	groupDir->Type = RES_ICON;
	groupDir->Count = fileDir->Count;
	for (idx = 0; idx < fileDir->Count; idx++)
	{
		groupDir->Entries[idx].Width = fileDir->Entries[idx].Width;
		groupDir->Entries[idx].Height = fileDir->Entries[idx].Height;
		// Fix up ColorCount, since it is often set incorrectly.
		if (fileDir->Entries[idx].BitCount >= 8)
		{
			groupDir->Entries[idx].ColorCount = 0;
		}
		else
		{
			groupDir->Entries[idx].ColorCount = 1 << fileDir->Entries[idx].BitCount;
		}
		groupDir->Entries[idx].Reserved = 0;
		groupDir->Entries[idx].Planes = 1;
		groupDir->Entries[idx].BitCount = fileDir->Entries[idx].BitCount;
		groupDir->Entries[idx].BytesInRes = fileDir->Entries[idx].BytesInRes;
		groupDir->Entries[idx].IconID = idx + 1; // resource IDs must not be zero
	}
	*outputDir = groupDir;
	return S_OK;
}

HRESULT LoadMemoryICO(HICON *phIcon, const void *buffer, size_t length, int width, int height)
{
	IconFileDirectory *iconFileDir;
	GroupIconDirectory *groupIconDir;
	ByteSlice fileSlice;
	ByteSlice readerSlice;
	int iconID;
	WORD iconIndex;
	DWORD imageOffset, imageLength, imageEnd;
	PBYTE resBits;
	DWORD lastError;

	INVALIDARG_IF_TRUE(phIcon == NULL);
	INVALIDARG_IF_TRUE(buffer == NULL);

	fileSlice.buffer = (const unsigned char *)buffer;
	fileSlice.length = length;

	readerSlice = fileSlice;
	RETURN_IF_FAILED(ReadIconFileDirectory(&readerSlice, &iconFileDir));
	RETURN_IF_FAILED(ConvertIconFileDirToGroupDir(iconFileDir, &groupIconDir));
	// If `width` or `height` are zero here, they are interpreted as the values
	// of the SM_CXICON and SM_CYICON system metrics instead.
	iconID = LookupIconIdFromDirectoryEx((PBYTE)groupIconDir, TRUE, width, height, LR_DEFAULTCOLOR);
	free(groupIconDir);
	groupIconDir = NULL;
	if (iconID <= 0)
	{
		free(iconFileDir);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	else if (iconID > iconFileDir->Count)
	{
		// LookupIconIdFromDirectoryEx should never return an invalid value,
		// given valid data.
		free(iconFileDir);
		return E_FAIL;
	}

	iconIndex = (WORD)iconID - 1;
	imageOffset = iconFileDir->Entries[iconIndex].ImageOffset;
	imageLength = iconFileDir->Entries[iconIndex].BytesInRes;
	free(iconFileDir);
	iconFileDir = NULL;
	RETURN_IF_FAILED(DWordAdd(imageOffset, imageLength, &imageEnd));
	if (imageEnd > fileSlice.length)
	{
		return E_FAIL;
	}
	resBits = (PBYTE)malloc(imageLength);
	if (resBits == NULL)
	{
		return E_OUTOFMEMORY;
	}
	memcpy(resBits, &fileSlice.buffer[imageOffset], imageLength);
	*phIcon = CreateIconFromResourceEx(resBits, imageLength, TRUE,
		0x00030000, width, height, LR_DEFAULTCOLOR);
	lastError = GetLastError();
	free(resBits);
	resBits = NULL;
	if (*phIcon == NULL)
	{
		return HRESULT_FROM_WIN32(lastError);
	}
	return S_OK;
}
