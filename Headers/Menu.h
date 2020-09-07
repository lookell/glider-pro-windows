#ifndef MENU_H_
#define MENU_H_

//============================================================================
//----------------------------------------------------------------------------
//                                   Menu.h
//----------------------------------------------------------------------------
//============================================================================

#include "GliderDefines.h"
#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#ifdef GP_USE_WINAPI_H
extern HMENU theMenuBar;
extern HMENU appleMenu;
extern HMENU gameMenu;
extern HMENU optionsMenu;
extern HMENU houseMenu;
extern LPWSTR appleMenuTitle;
extern LPWSTR gameMenuTitle;
extern LPWSTR optionsMenuTitle;
extern LPWSTR houseMenuTitle;
#endif
extern Boolean menusUp;
extern Boolean resumedSavedGame;

void UpdateClipboardMenus (void);
void UpdateMenus (Boolean newMode);
#ifdef GP_USE_WINAPI_H
void DoAppleMenu (HWND hwnd, SInt16 theItem);
void DoGameMenu (HWND hwnd, SInt16 theItem);
void DoOptionsMenu (HWND hwnd, SInt16 theItem);
void DoHouseMenu (HWND hwnd, SInt16 theItem);
void DoMenuChoice (HWND hwnd, WORD menuChoice);
#endif
void UpdateMapCheckmark (Boolean checkIt);
void UpdateToolsCheckmark (Boolean checkIt);
void UpdateCoordinateCheckmark (Boolean checkIt);
void EnableMenuBar (void);
void DisableMenuBar (void);

#endif
