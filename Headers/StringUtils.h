//============================================================================
//----------------------------------------------------------------------------
//                               StringUtils.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include "MacTypes.h"
#include "WinAPI.h"

void PasStringCopy (ConstStringPtr p1, StringPtr p2);
void PasStringCopyC (PCSTR s1, StringPtr p2);
void PasStringConcat (StringPtr p1, ConstStringPtr p2);
Boolean PasStringEqual (ConstStringPtr p1, ConstStringPtr p2, Boolean caseSens);
void GetLineOfText (ConstStringPtr srcStr, SInt16 index, StringPtr textLine);
void WrapText (StringPtr theText, SInt16 maxChars);
void GetLocalizedString (UInt16 index, PWSTR pszDest, size_t cchDest);
void GetLocalizedString_Pascal (UInt16 index, StringPtr theString, int stringCapacity);
PWSTR MacToWinLineEndings (PCWSTR input);
PWSTR WinToMacLineEndings (PCWSTR input);
void NumToString(SInt32 theNum, StringPtr theString);
int WinFromMacString (PWSTR winbuf, int winlen, ConstStringPtr macbuf);
int MacFromWinString (StringPtr macbuf, int maclen, PCWSTR winbuf);
PSTR AllocStringVPrintfA (PCSTR format, va_list args);
PSTR AllocStringPrintfA (PCSTR format, ...);
PWSTR AllocStringVPrintfW (PCWSTR format, va_list args);
PWSTR AllocStringPrintfW (PCWSTR format, ...);

#endif
