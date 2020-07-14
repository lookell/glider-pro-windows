#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               StringUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include <wchar.h>

void PasStringCopy (ConstStringPtr p1, StringPtr p2);
void PasStringCopyC (const char *s1, StringPtr p2);
SInt16 WhichStringFirst (ConstStringPtr p1, ConstStringPtr p2);
void PasStringCopyNum (ConstStringPtr p1, StringPtr p2, Byte charsToCopy);
void PasStringConcat (StringPtr p1, ConstStringPtr p2);
void PasStringConcatC (StringPtr p1, const char *s2);
Boolean PasStringEqual (ConstStringPtr p1, ConstStringPtr p2, Boolean caseSens);
void GetLineOfText (ConstStringPtr srcStr, SInt16 index, StringPtr textLine);
void WrapText (StringPtr theText, SInt16 maxChars);
void GetFirstWordOfString (ConstStringPtr stringIn, StringPtr stringOut);
void CollapseStringToWidth (StringPtr theStr, SInt16 wide);
void GetChooserName (StringPtr thisName);
StringPtr GetLocalizedString (SInt16 index, StringPtr theString);
wchar_t *MacToWinLineEndings (const wchar_t *input);
wchar_t *WinToMacLineEndings (const wchar_t *input);
void NumToString(SInt32 theNum, StringPtr theString);
int WinFromMacString (wchar_t *winbuf, int winlen, ConstStringPtr macbuf);
int MacFromWinString (StringPtr macbuf, int maclen, const wchar_t *winbuf);

#endif
