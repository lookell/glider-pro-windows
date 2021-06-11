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

//==============================================================  Functions
//--------------------------------------------------------------  PasStringCopy
// Given a source string and storage for a second, this function
// copies from one to the other.  It assumes Pascal style strings.

void PasStringCopy (ConstStringPtr p1, StringPtr p2)
{
	SInt16 stringLength;

	stringLength = *p2++ = *p1++;
	while (--stringLength >= 0)
		*p2++ = *p1++;
}

//--------------------------------------------------------------  PasStringCopyC

void PasStringCopyC (PCSTR s1, StringPtr p2)
{
	Byte stringLength;

	stringLength = 0;
	while (*s1 != '\0' && stringLength < 255)
		p2[++stringLength] = *s1++;
	p2[0] = stringLength;
}

//--------------------------------------------------------------  PasStringConcat
// This function concatenates the second Pascal string to the end of
// the first Pascal string.

void PasStringConcat (StringPtr p1, ConstStringPtr p2)
{
	SInt16 wasLength, addedLength, i;

	wasLength = *p1;
	if (wasLength > 255)
		wasLength = 255;

	addedLength = *p2;
	if ((wasLength + addedLength) > 255)
		addedLength = 255 - wasLength;

	*p1 = (Byte)(wasLength + addedLength);

	p1++;
	p2++;

	for (i = 0; i < wasLength; i++)
		p1++;

	for (i = 0; i < addedLength; i++)
		*p1++ = *p2++;
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
// is sought.

void GetLineOfText (ConstStringPtr srcStr, SInt16 index, StringPtr textLine)
{
	SInt16 i, srcLength, count, start, stop;
	Boolean foundIt;

	PasStringCopyC("", textLine);
	srcLength = srcStr[0];

	if (index == 0)  // walk through to "index"
		start = 1;
	else
	{
		start = 0;
		count = 0;
		i = 0;
		foundIt = false;
		do
		{
			i++;
			if (srcStr[i] == kReturnKeyASCII)
			{
				count++;
				if (count == index)
				{
					start = i + 1;
					foundIt = true;
				}
			}
		}
		while ((i < srcLength) && (!foundIt));
	}

	if (start != 0)
	{
		i = start;
		stop = -1;

		foundIt = false;
		do
		{
			if (srcStr[i] == kReturnKeyASCII)
			{
				stop = i;
				foundIt = true;
			}
			i++;
		}
		while ((i < srcLength) && (!foundIt));

		if (!foundIt)
		{
			if (start > srcLength)
			{
				start = srcLength;
				stop = srcLength - 1;
			}
			else
				stop = i;
		}

		count = 0;

		for (i = start; i <= stop; i++)
		{
			count++;
			textLine[count] = srcStr[i];
		}
		textLine[0] = (Byte)count;

		// blank out the trailing carriage return, if any
		if (textLine[count] == kReturnKeyASCII)
			textLine[count] = kSpaceBarASCII;
	}
}

//--------------------------------------------------------------  WrapText
// Given a string and the maximum number of characters to put on
// one line, this function goes through and inserts carriage returns
// in order to ensure that no line of text exceeds maxChars.

void WrapText (StringPtr theText, SInt16 maxChars)
{
	SInt16 lastChar, count, chars, spaceIs;
	Boolean foundEdge, foundSpace;

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
			if (theText[count] == kReturnKeyASCII)
				foundEdge = true;
			else if (theText[count] == kSpaceBarASCII)
			{
				foundSpace = true;
				spaceIs = count;
			}
		}
		while ((count < lastChar) && (chars < maxChars) && (!foundEdge));

		if ((!foundEdge) && (count < lastChar) && (foundSpace) && (spaceIs >= 0))
		{
			theText[spaceIs] = kReturnKeyASCII;
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

PWSTR MacToWinLineEndings(PCWSTR input)
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

PWSTR WinToMacLineEndings(PCWSTR input)
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

void NumToString (SInt32 theNum, StringPtr theString)
{
	char buffer[sizeof("-2147483648")];
	size_t length;
	HRESULT hr;

	if (theString == NULL)
	{
		return;
	}
	// Return an empty string if an error occurs.
	theString[0] = 0;

	hr = StringCchPrintfA(buffer, ARRAYSIZE(buffer), "%ld", (long)theNum);
	if (SUCCEEDED(hr))
	{
		hr = StringCchLengthA(buffer, ARRAYSIZE(buffer), &length);
		if (SUCCEEDED(hr))
		{
			theString[0] = (Byte)length;
			memcpy(&theString[1], buffer, theString[0]);
		}
	}
}

//--------------------------------------------------------------  WinFromMacString
// Convert a MacRoman Pascal-style string to a UTF-16 C-style string.
// If the length of the converted string exceeds the output buffer's capacity,
// then the converted string is truncated.
// This is a wrapper around the Windows API function 'MultiByteToWideChar'.
// The return value of this function is the return value of 'MultiByteToWideChar'.
// Return nonzero on success and zero on failure.

int WinFromMacString(PWSTR winbuf, int winlen, ConstStringPtr macbuf)
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

int MacFromWinString(StringPtr macbuf, int maclen, PCWSTR winbuf)
{
	size_t inputSize;
	int copySize, result;
	HRESULT hr;

	// Calculate the number of wide characters to copy
	hr = StringCchLength(winbuf, INT_MAX, &inputSize);
	if (FAILED(hr))
	{
		inputSize = 0;
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
