#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

//============================================================================
//----------------------------------------------------------------------------
//                                MainWindow.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

#define WC_MAINWINDOW L"GliderMainWindow"

extern HCURSOR handCursor;
extern HCURSOR beamCursor;
extern HCURSOR vertCursor;
extern HCURSOR horiCursor;
extern HCURSOR diagCursor;
extern Rect workSrcRect;
extern HDC workSrcMap;
extern Rect mainWindowRect;
extern HWND mainWindow;
extern SInt16 isEditH;
extern SInt16 isEditV;
extern SInt16 playOriginH;
extern SInt16 playOriginV;
extern SInt16 splashOriginH;
extern SInt16 splashOriginV;
extern SInt16 theMode;
extern Boolean fadeGraysOut;
extern Boolean isDoColorFade;
extern Boolean splashDrawn;

void RedrawSplashScreen (void);
void UpdateMainWindow (void);
void OpenMainWindow (void);
void CloseMainWindow (void);
//void ZoomBetweenWindows (void);
void UpdateEditWindowTitle (void);
void HandleMainClick (HWND hwnd, Point wherePt, Boolean isDoubleClick);
HDC GetMainWindowDC (void);
void ReleaseMainWindowDC (HDC hdc);
//void ShowMenuBarOld (void);
//void HideMenuBarOld (void);
void WashColorIn (void);
void InitMainWindowCursor (void);
HCURSOR GetMainWindowCursor (void);
void SetMainWindowCursor (HCURSOR hCursor);
LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
