#include "StringUtils.h"

//============================================================================
//----------------------------------------------------------------------------
//								  StringUtils.c
//----------------------------------------------------------------------------
//============================================================================


#include "Externs.h"
#include "Macintosh.h"
#include "ResourceIDs.h"


//==============================================================  Functions
//--------------------------------------------------------------  PasStringCopy
// Given a source string and storage for a second, this function…
// copies from one to the other.  It assumes Pascal style strings.

void PasStringCopy (StringPtr p1, StringPtr p2)
{
	register SInt16		stringLength;

	stringLength = *p2++ = *p1++;
	while (--stringLength >= 0)
		*p2++ = *p1++;
}

//--------------------------------------------------------------  PasStringCopyC

void PasStringCopyC (const char *s1, StringPtr p2)
{
	Byte		stringLength;

	stringLength = 0;
	while (*s1 != '\0' && stringLength < 255)
		p2[++stringLength] = *s1++;
	p2[0] = stringLength;
}

//--------------------------------------------------------------  WhichStringFirst

// This is a sorting function that handles two Pascal strings.  It…
// will return a 1 to indicate the 1st string is "greater", a 1 to…
// indicate the 2nd was greater and a 0 to indicate that the strings…
// are equal.

SInt16 WhichStringFirst (StringPtr p1, StringPtr p2)
{
	SInt16		smallestLength, seek, greater;
	Byte		char1, char2;
	Boolean		foundIt;

	smallestLength = p1[0];
	if (p2[0] < smallestLength)
		smallestLength = p2[0];

	greater = 0;					// neither are greater, they are equal
	seek = 1;						// start at character #1
	foundIt = false;
	do
	{
		char1 = p1[seek];			// make upper case (if applicable)
		if ((char1 > 0x60) && (char1 < 0x7B))
			char1 -= 0x20;
		char2 = p2[seek];			// make upper case (if applicable)
		if ((char2 > 0x60) && (char2 < 0x7B))
			char2 -= 0x20;

		if (char1 > char2)			// first string is greater
		{
			greater = 1;
			foundIt = true;
		}
		else if (char1 < char2)		// second string is greater
		{
			greater = 2;
			foundIt = true;
		}
		seek++;
		if (seek > smallestLength)	// we've reached the end of the line
		{
			if (!foundIt)
			{
				if (p1[0] < p2[0])	// shortest string wins
					greater = 1;
				else if (p1[0] > p2[0])
					greater = 2;
			}
			foundIt = true;
		}
	}
	while (!foundIt);

	return (greater);
}

//--------------------------------------------------------------  PasStringCopyNum

// This function copies a specified number of characters from one…
// Pascal string to another.

void PasStringCopyNum (StringPtr p1, StringPtr p2, Byte charsToCopy)
{
	SInt16		i;

	if (charsToCopy > *p1)		// if trying to copy more chars than there are
		charsToCopy = *p1;		// reduce the number of chars to copy to this size

	*p2 = charsToCopy;

	*p2++;
	*p1++;

	for (i = 0; i < charsToCopy; i++)
		*p2++ = *p1++;
}

//--------------------------------------------------------------  PasStringConcat
// This function concatenates the second Pascal string to the end of…
// the first Pascal string.

void PasStringConcat (StringPtr p1, StringPtr p2)
{
	SInt16		wasLength, addedLength, i;

	wasLength = *p1;
	if (wasLength > 255)
		wasLength = 255;

	addedLength = *p2;
	if ((wasLength + addedLength) > 255)
		addedLength = 255 - wasLength;

	*p1 = (Byte)(wasLength + addedLength);

	*p1++;
	*p2++;

	for (i = 0; i < wasLength; i++)
		*p1++;

	for (i = 0; i < addedLength; i++)
		*p1++ = *p2++;
}

//--------------------------------------------------------------  PasStringConcatC

void PasStringConcatC (StringPtr p1, const char *s2)
{
	Byte		stringLength;

	stringLength = p1[0];
	while (*s2 != '\0' && stringLength < 255)
		p1[++stringLength] = *s2++;
	p1[0] = stringLength;
}

//--------------------------------------------------------------  GetLineOfText

// This function walks through a source string and looks for an…
// entire line of text.  A "line" of text is assumed to be bounded…
// by carriage returns.  The index variable indicates which line…
// is sought.

void GetLineOfText (StringPtr srcStr, SInt16 index, StringPtr textLine)
{
	SInt16		i, srcLength, count, start, stop;
	Boolean		foundIt;

	PasStringCopyC("", textLine);
	srcLength = srcStr[0];

	if (index == 0)						// walk through to "index"
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

// Given a string and the maximum number of characters to put on…
// one line, this function goes through and inserts carriage returns…
// in order to ensure that no line of text exceeds maxChars.

void WrapText (StringPtr theText, SInt16 maxChars)
{
	SInt16		lastChar, count, chars, spaceIs;
	Boolean		foundEdge, foundSpace;

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

//--------------------------------------------------------------  GetFirstWordOfString

// Walks a string looking for a space (denoting first word of string).

void GetFirstWordOfString (StringPtr stringIn, StringPtr stringOut)
{
	SInt16		isLong, spaceAt, i;

	isLong = stringIn[0];
	spaceAt = isLong;

	for (i = 1; i < isLong; i++)
	{
		if ((stringIn[i] == ' ') && (spaceAt == isLong))
			spaceAt = i - 1;
	}

	if (spaceAt <= 0)
		PasStringCopyC("", stringOut);
	else
		PasStringCopyNum(stringIn, stringOut, (Byte)spaceAt);
}

//--------------------------------------------------------------  CollapseStringToWidth

// Given a string and a maximum width (in pixels), this function…
// calculates how wide the text would be drawn with the current…
// font.  If the text would exceed our width limit, characters…
// are dropped off the end of the string and "…" appended.

void CollapseStringToWidth (StringPtr theStr, SInt16 wide)
{
	return;
#if 0
	short		dotsWide;
	Boolean 	tooWide;

	dotsWide = StringWidth("\p…");
	tooWide = StringWidth(theStr) > wide;
	while (tooWide)
	{
		theStr[0]--;
		tooWide = ((StringWidth(theStr) + dotsWide) > wide);
		if (!tooWide)
			PasStringConcat(theStr, "\p…");
	}
#endif
}

//--------------------------------------------------------------  GetChooserName

// This function extracts the user name stored away by the Chooser.

void GetChooserName (StringPtr thisName)
{
	PasStringCopyC("", thisName);
	return;
#if 0
	#define		kChooserStringID	-16096
	Handle		theNameHandle;
	Byte		oldState;

	theNameHandle = (Handle)GetString(kChooserStringID);
	if (theNameHandle != nil) {
		oldState = HGetState(theNameHandle);
		HLock(theNameHandle);
		PasStringCopy((StringPtr)*theNameHandle, thisName);
		HSetState(theNameHandle, oldState);
		}
	else
		thisName[0] = 0;
#endif
}

//--------------------------------------------------------------  GetLocalizedString

StringPtr GetLocalizedString (SInt16 index, StringPtr theString)
{
	WCHAR buffer[256];
	int length;

	length = LoadString(HINST_THISCOMPONENT, kLocalizedStringsBase + index,
			buffer, ARRAYSIZE(buffer));
	if (length <= 0)
		buffer[0] = L'\0';
	MacFromWinString(theString, sizeof(Str255), buffer);
	return (theString);
}

//--------------------------------------------------------------  MacToWinLineEndings

// This functions takes a wide string and replaces each instance of "\r" with "\r\n".
// The new string is returned if the function succeeds, or NULL is returned if the
// function fails (e.g., memory could not be allocated). The returned pointer must
// be deallocated with the standard 'free' function.

wchar_t *MacToWinLineEndings(const wchar_t *input)
{
	wchar_t *output;
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

	output = calloc(outputSize, sizeof(*output));
	if (output == NULL)
	{
		return NULL;
	}

	outputIndex = 0;
	for (inputIndex = 0; input[inputIndex] != L'\0'; inputIndex++)
	{
		output[outputIndex++] = input[inputIndex];
		if (input[inputIndex] == L'\r')
		{
			output[outputIndex++] = L'\n';
		}
	}

	return output;
}

//--------------------------------------------------------------  WinToMacLineEndings

// This functions takes a wide string and replaces each instance of "\r\n" with "\r".
// The new string is returned if the function succeeds. or NULL is returned if the
// function fails (e.g., memory could not be allocated for the new string). The
// returned pointer must be deallocated with the standard 'free' function.

wchar_t *WinToMacLineEndings(const wchar_t *input)
{
	wchar_t *output;
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

	output = calloc(outputSize, sizeof(*output));
	if (output == NULL)
	{
		return NULL;
	}

	outputIndex = 0;
	for (inputIndex = 0; input[inputIndex] != L'\0'; inputIndex++)
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

	return output;
}
