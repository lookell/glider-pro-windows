//============================================================================
//----------------------------------------------------------------------------
//                                   Menu.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef MENU_H_
#define MENU_H_

#include "GliderDefines.h"
#include "MacTypes.h"
#include "WinAPI.h"

extern HMENU g_theMenuBar;
extern HMENU g_appleMenu;
extern HMENU g_gameMenu;
extern HMENU g_optionsMenu;
extern HMENU g_houseMenu;
extern LPWSTR g_appleMenuTitle;
extern LPWSTR g_gameMenuTitle;
extern LPWSTR g_optionsMenuTitle;
extern LPWSTR g_houseMenuTitle;
extern Boolean g_menusUp;
extern Boolean g_resumedSavedGame;

void UpdateClipboardMenus (void);
void UpdateMenus (Boolean newMode);
void DoAppleMenu (HWND hwnd, SInt16 theItem);
void DoGameMenu (HWND hwnd, SInt16 theItem);
void DoOptionsMenu (HWND hwnd, SInt16 theItem);
void DoHouseMenu (HWND hwnd, SInt16 theItem);
void DoMenuChoice (HWND hwnd, WORD menuChoice);
void UpdateMapCheckmark (Boolean checkIt);
void UpdateToolsCheckmark (Boolean checkIt);
void UpdateCoordinateCheckmark (Boolean checkIt);
void EnableMenuBar (void);
void DisableMenuBar (void);

#endif
