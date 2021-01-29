//============================================================================
//----------------------------------------------------------------------------
//                                MainWindow.h
//----------------------------------------------------------------------------
//============================================================================

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include "MacTypes.h"
#include "WinAPI.h"

extern Rect g_workSrcRect;
extern HDC g_workSrcMap;
extern HWND g_mainWindow;
extern SInt16 g_isEditH;
extern SInt16 g_isEditV;
extern SInt16 g_playOriginH;
extern SInt16 g_playOriginV;
extern SInt16 g_splashOriginH;
extern SInt16 g_splashOriginV;
extern SInt16 g_theMode;
extern Boolean g_fadeGraysOut;
extern Boolean g_isDoColorFade;
extern Boolean g_splashDrawn;
extern HDC g_splashSrcMap;
extern Rect g_splashSrcRect;

void RegisterMainWindowClass (void);
void RedrawSplashScreen (SInt16 splashHouseIndex);
void OpenMainWindow (void);
void CloseMainWindow (void);
void UpdateEditWindowTitle (void);
HDC GetMainWindowDC (HWND hwnd);
void ReleaseMainWindowDC (HWND hwnd, HDC hdc);
void WashColorIn (void);

#endif
