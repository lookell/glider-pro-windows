//============================================================================
//----------------------------------------------------------------------------
//                               StringUtils.c
//----------------------------------------------------------------------------
//============================================================================

#include "StringUtils.h"

#include "ResourceIDs.h"
#include "Utilities.h"
#include "WinAPI.h"

#include <strsafe.h>

#include <limits.h>
#include <stdlib.h>

#define MAC_ROMAN_CODEPAGE          10000
#define kReturnKeyASCII             0x0D
#define kSpaceBarASCII              0x20

static Byte CopyPascalStringContents(
	void *dstPtr,
	size_t dstCapacity,
	const void *srcPtr,
	size_t srcLength);

//==============================================================  Functions
//--------------------------------------------------------------  CopyPascalStringContents

static Byte CopyPascalStringContents(
	void *dstPtr,
	size_t dstCapacity,
	const void *srcPtr,
	size_t srcLength)
{
	size_t fullLength;
	Byte copyLength;

	fullLength = (srcLength < dstCapacity) ? srcLength : dstCapacity;
	copyLength = (fullLength < 255) ? (Byte)fullLength : (Byte)255;
	memcpy(dstPtr, srcPtr, copyLength);
	return copyLength;
}

//--------------------------------------------------------------  PasStringCopy
// Given a source string and storage for a second, this function
// copies from one to the other.  It assumes Pascal style strings.

void PasStringCopy (ConstStringPtr src, StringPtr dst, size_t capacity)
{
	if (capacity == 0)
		return;

	dst[0] = CopyPascalStringContents(&dst[1], capacity - 1, &src[1], src[0]);
}

//--------------------------------------------------------------  PasStringCopyC

void PasStringCopyC (PCSTR src, StringPtr dst, size_t capacity)
{
	if (capacity == 0)
		return;

	dst[0] = CopyPascalStringContents(&dst[1], capacity - 1, src, strlen(src));
}

//--------------------------------------------------------------  PasStringEqual
// Compare two Pascal strings for equality. If the third parameter is
// nonzero (true), then the comparison is case-sensitive. If the third
// parameter is zero (false), then the comparison is case-insensitive.

Boolean PasStringEqual (ConstStringPtr p1, ConstStringPtr p2, Boolean caseSens)
{
	WCHAR firstString[256];
	WCHAR secondString[256];

	if (p1[0] != p2[0])
	{
		return false;
	}
	WinFromMacString(firstString, ARRAYSIZE(firstString), p1);
	WinFromMacString(secondString, ARRAYSIZE(secondString), p2);
	if (caseSens)
	{
		return (lstrcmp(firstString, secondString) == 0);
	}
	else
	{
		return (lstrcmpi(firstString, secondString) == 0);
	}
}

//--------------------------------------------------------------  GetLineOfText
// This function walks through a source string and looks for an
// entire line of text.  A "line" of text is assumed to be bounded
// by carriage returns.  The index variable indicates which line
// is sought.  The 'pOffset' and 'pLength' are filled in with the
// offset and length of the substring within 'srcStr'.  This function
// returns nonzero if the requested line is found, or zero if the
// line doesn't exist in the source text.

Boolean GetLineOfText (PCWSTR srcStr, SInt16 index, size_t *pOffset, size_t *pLength)
{
	SInt16 i;
	PCWSTR startPtr;
	PCWSTR carriagePtr;
	PCWSTR stopPtr;

	*pOffset = 0;
	*pLength = 0;

	if (index < 0)
	{
		return false;
	}

	startPtr = srcStr;
	for (i = 0; i < index; i++)
	{
		carriagePtr = wcschr(startPtr, L'\r');
		if (carriagePtr == NULL)
		{
			return false;
		}
		startPtr = carriagePtr + 1;
	}

	stopPtr = wcschr(startPtr, L'\r');
	if (stopPtr == NULL)
	{
		stopPtr = startPtr + wcslen(startPtr);
	}

	*pOffset = startPtr - srcStr;
	*pLength = stopPtr - startPtr;
	return true;
}

//--------------------------------------------------------------  WrapText
// Given a string and the maximum number of characters to put on
// one line, this function goes through and inserts carriage returns
// in order to ensure that no line of text exceeds maxChars.

void WrapText (StringPtr theText, SInt16 capacity, SInt16 maxChars)
{
	SInt16 lastChar, count, chars, spaceIs;
	Boolean foundEdge, foundSpace;

	if (capacity <= 0)
		return;

	lastChar = theText[0];
	count = 0;
	spaceIs = -1;

	do
	{
		chars = 0;
		foundEdge = false;
		foundSpace = false;
		do
		{
			count++;
			chars++;
			if (count < capacity)
			{
				if (theText[count] == kReturnKeyASCII)
				{
					foundEdge = true;
				}
				else if (theText[count] == kSpaceBarASCII)
				{
					foundSpace = true;
					spaceIs = count;
				}
			}
		}
		while ((count < lastChar) && (chars < maxChars) && (!foundEdge));

		if ((!foundEdge) && (count < lastChar) && (foundSpace) && (spaceIs >= 0))
		{
			if (spaceIs < capacity)
			{
				theText[spaceIs] = kReturnKeyASCII;
			}
			count = spaceIs + 1;
		}
	}
	while (count < lastChar);
}

//--------------------------------------------------------------  GetLocalizedString_Pascal

void GetLocalizedString_Pascal (UInt16 index, StringPtr theString, int stringCapacity)
{
	WCHAR buffer[256];

	GetLocalizedString(index, buffer, ARRAYSIZE(buffer));
	MacFromWinString(theString, stringCapacity, buffer);
}

//--------------------------------------------------------------  GetLocalizedString

void GetLocalizedString (UInt16 index, PWSTR pszDest, size_t cchDest)
{
	PWSTR pszStringBuffer;
	PCWSTR pszString;

	AllocLoadString(HINST_THISCOMPONENT, kLocalizedStringsBase + index, &pszStringBuffer);
	pszString = (pszStringBuffer != NULL) ? pszStringBuffer : L"";
	StringCchCopy(pszDest, cchDest, pszString);
	free(pszStringBuffer);
}

//--------------------------------------------------------------  MacToWinLineEndings
// This functions takes a wide string and replaces each instance of "\r" with "\r\n".
// The new string is returned if the function succeeds, or NULL is returned if the
// function fails (e.g., memory could not be allocated). The returned pointer must
// be deallocated with the standard 'free' function.

PWSTR MacToWinLineEndings (PCWSTR input)
{
	PWSTR output;
	size_t inputIndex, outputIndex, outputSize;

	if (input == NULL)
	{
		return NULL;
	}

	outputSize = 1; // 1 element for L'\0'
	for (inputIndex = 0; input[inputIndex] != L'\0'; inputIndex++)
	{
		outputSize++;
		if (input[inputIndex] == L'\r')
		{
			outputSize++;
		}
	}

	output = (PWSTR)calloc(outputSize, sizeof(*output));
	if (output == NULL)
	{
		return NULL;
	}

	outputIndex = 0;
	for (inputIndex = 0; (input[inputIndex] != L'\0') && (outputIndex < outputSize); inputIndex++)
	{
		output[outputIndex++] = input[inputIndex];
		if ((input[inputIndex] == L'\r') && (outputIndex < outputSize))
		{
			output[outputIndex++] = L'\n';
		}
	}
	output[outputSize - 1] = L'\0';

	return output;
}

//--------------------------------------------------------------  WinToMacLineEndings
// This functions takes a wide string and replaces each instance of "\r\n" with "\r".
// The new string is returned if the function succeeds. or NULL is returned if the
// function fails (e.g., memory could not be allocated for the new string). The
// returned pointer must be deallocated with the standard 'free' function.

PWSTR WinToMacLineEndings (PCWSTR input)
{
	PWSTR output;
	size_t inputIndex, outputIndex, outputSize;

	if (input == NULL)
	{
		return NULL;
	}

	outputSize = 1; // 1 element for L'\0'
	for (inputIndex = 0; input[inputIndex] != L'\0'; inputIndex++)
	{
		outputSize++;
		if (inputIndex != 0)
		{
			if (input[inputIndex - 1] == L'\r' && input[inputIndex] == L'\n')
			{
				--outputSize;
			}
		}
	}

	output = (PWSTR)calloc(outputSize, sizeof(*output));
	if (output == NULL)
	{
		return NULL;
	}

	outputIndex = 0;
	for (inputIndex = 0; (input[inputIndex] != L'\0') && (outputIndex < outputSize); inputIndex++)
	{
		output[outputIndex++] = input[inputIndex];
		if (inputIndex != 0)
		{
			if (input[inputIndex - 1] == L'\r' && input[inputIndex] == L'\n')
			{
				output[--outputIndex] = L'\0';
			}
		}
	}
	output[outputSize - 1] = L'\0';

	return output;
}

//--------------------------------------------------------------  NumToString
// Convert the given number to a decimal string representation.
// The string is written to the given output string.

void NumToString (SInt32 theNum, PWSTR theString, size_t capacity)
{
	StringCchPrintfW(theString, capacity, L"%ld", (long)theNum);
}

//--------------------------------------------------------------  WinFromMacString
// Convert a MacRoman Pascal-style string to a UTF-16 C-style string.
// If the length of the converted string exceeds the output buffer's capacity,
// then the converted string is truncated.
// This is a wrapper around the Windows API function 'MultiByteToWideChar'.
// The return value of this function is the return value of 'MultiByteToWideChar'.
// Return nonzero on success and zero on failure.

int WinFromMacString (PWSTR winbuf, int winlen, ConstStringPtr macbuf)
{
	int copySize, result;

	// Calculate the number of Mac characters to copy
	copySize = macbuf[0];
	if (copySize > winlen - 1)
	{
		copySize = winlen - 1;
	}

	// Do the conversion and terminate the output string
	result = MultiByteToWideChar(
		MAC_ROMAN_CODEPAGE, 0x00000000,
		(LPCCH)&macbuf[1], copySize,
		winbuf, winlen - 1);
	winbuf[result] = L'\0';
	return result;
}

//--------------------------------------------------------------  MacFromWinString
// Convert a UTF-16 C-style string to a MacRoman Pascal-style string.
// (This may result in data-loss!)
// If the length of the converted string exceeds the output buffer's capacity,
// then the converted string is truncated.
// This is a wrapper around the Windows API function 'WideCharToMultiByte'.
// The return value of this function is the return value of 'WideCharToMultiByte'.
// Return nonzero on success and zero on failure.

int MacFromWinString (StringPtr macbuf, int maclen, PCWSTR winbuf)
{
	size_t inputSize;
	int copySize, result;

	// Calculate the number of wide characters to copy
	inputSize = wcslen(winbuf);
	if (inputSize > (size_t)INT_MAX)
	{
		inputSize = (size_t)INT_MAX;
	}
	copySize = (int)inputSize;
	if (copySize > maclen - 1)
	{
		copySize = maclen - 1;
	}

	// Do the conversion and save the length.
	// If WideCharToMultiByte fails, it returns zero, which is
	// handy as a length for a failed conversion.
	result = WideCharToMultiByte(
		MAC_ROMAN_CODEPAGE, 0x00000000,
		winbuf, copySize,
		(LPSTR)&macbuf[1], maclen - 1,
		NULL, NULL);
	macbuf[0] = (Byte)result;
	return result;
}

//--------------------------------------------------------------  AllocStringVPrintfA

PSTR AllocStringVPrintfA (PCSTR format, va_list args)
{
	int resultLength;
	va_list argsCopy;
	size_t cchResult;
	PSTR pszResult;
	HRESULT hr;

	va_copy(argsCopy, args);
	resultLength = _vscprintf(format, argsCopy);
	va_end(argsCopy);
	if (resultLength < 0)
	{
		return NULL;
	}
	cchResult = (size_t)resultLength + 1;
	pszResult = (PSTR)calloc(cchResult, sizeof(*pszResult));
	if (pszResult == NULL)
	{
		return NULL;
	}
	hr = StringCchVPrintfA(pszResult, cchResult, format, args);
	if (FAILED(hr))
	{
		free(pszResult);
		return NULL;
	}
	return pszResult;
}

//--------------------------------------------------------------  AllocStringPrintfA

PSTR AllocStringPrintfA (PCSTR format, ...)
{
	va_list args;
	PSTR result;

	va_start(args, format);
	result = AllocStringVPrintfA(format, args);
	va_end(args);
	return result;
}

//--------------------------------------------------------------  AllocStringVPrintfW

PWSTR AllocStringVPrintfW (PCWSTR format, va_list args)
{
	int resultLength;
	va_list argsCopy;
	size_t cchResult;
	PWSTR pszResult;
	HRESULT hr;

	va_copy(argsCopy, args);
	resultLength = _vscwprintf(format, argsCopy);
	va_end(argsCopy);
	if (resultLength < 0)
	{
		return NULL;
	}
	cchResult = (size_t)resultLength + 1;
	pszResult = (PWSTR)calloc(cchResult, sizeof(*pszResult));
	if (pszResult == NULL)
	{
		return NULL;
	}
	hr = StringCchVPrintfW(pszResult, cchResult, format, args);
	if (FAILED(hr))
	{
		free(pszResult);
		return NULL;
	}
	return pszResult;
}

//--------------------------------------------------------------  AllocStringPrintfW

PWSTR AllocStringPrintfW (PCWSTR format, ...)
{
	va_list args;
	PWSTR result;

	va_start(args, format);
	result = AllocStringVPrintfW(format, args);
	va_end(args);
	return result;
}
