#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               StringUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include <wchar.h>

void PasStringCopy (StringPtr p1, StringPtr p2);
void PasStringCopyC (const char *s1, StringPtr p2);
SInt16 WhichStringFirst (StringPtr p1, StringPtr p2);
void PasStringCopyNum (StringPtr p1, StringPtr p2, Byte charsToCopy);
void PasStringConcat (StringPtr p1, StringPtr p2);
void PasStringConcatC (StringPtr p1, const char *s2);
void GetLineOfText (StringPtr srcStr, SInt16 index, StringPtr textLine);
void WrapText (StringPtr theText, SInt16 maxChars);
void GetFirstWordOfString (StringPtr stringIn, StringPtr stringOut);
void CollapseStringToWidth (StringPtr theStr, SInt16 wide);
void GetChooserName (StringPtr thisName);
StringPtr GetLocalizedString (SInt16 index, StringPtr theString);
wchar_t *MacToWinLineEndings (const wchar_t *input);
wchar_t *WinToMacLineEndings (const wchar_t *input);

#endif
