#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

//============================================================================
//----------------------------------------------------------------------------
//                                MainWindow.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#define WC_MAINWINDOW L"GliderMainWindow"

#ifdef GP_USE_WINAPI_H
extern HCURSOR handCursor;
extern HCURSOR vertCursor;
extern HCURSOR horiCursor;
extern HCURSOR diagBotCursor;
extern HCURSOR diagTopCursor;
#endif
extern Rect workSrcRect;
#ifdef GP_USE_WINAPI_H
extern HDC workSrcMap;
#endif
extern Rect mainWindowRect;
#ifdef GP_USE_WINAPI_H
extern HWND mainWindow;
#endif
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
void UpdateEditWindowTitle (void);
#ifdef GP_USE_WINAPI_H
void HandleMainClick (HWND hwnd, Point wherePt, Boolean isDoubleClick);
HDC GetMainWindowDC (void);
void ReleaseMainWindowDC (HDC hdc);
#endif
void InvalViewportRect (const Rect *theRect);
//void ShowMenuBarOld (void);
//void HideMenuBarOld (void);
void WashColorIn (void);
void InitMainWindowCursor (void);
#ifdef GP_USE_WINAPI_H
HCURSOR GetMainWindowCursor (void);
void SetMainWindowCursor (HCURSOR hCursor);
LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

#endif
