#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

//============================================================================
//----------------------------------------------------------------------------
//                                MainWindow.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HDC		workSrcMap;

void RedrawSplashScreen (void);
void UpdateMainWindow (void);
void UpdateMenuBarWindow (void);
void OpenMainWindow (void);
void CloseMainWindow (void);
//void ZoomBetweenWindows (void);
void UpdateEditWindowTitle (void);
void HandleMainClick (HWND hwnd, Point wherePt, Boolean isDoubleClick);
HDC GetMainWindowDC (void);
void ReleaseMainWindowDC (HDC hdc);
//void ShowMenuBarOld (void);
//void HideMenuBarOld (void);
//void WashColorIn (void);
void InitMainWindowCursor (void);
HCURSOR GetMainWindowCursor (void);
void SetMainWindowCursor (HCURSOR hCursor);
LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
