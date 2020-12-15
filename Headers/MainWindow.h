#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

//============================================================================
//----------------------------------------------------------------------------
//                                MainWindow.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

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
extern HDC splashSrcMap;
extern Rect splashSrcRect;

void RegisterMainWindowClass (void);
void RedrawSplashScreen (SInt16 splashHouseIndex);
void UpdateMainWindow (void);
void OpenMainWindow (void);
void CloseMainWindow (void);
void UpdateEditWindowTitle (void);
HDC GetMainWindowDC (void);
void ReleaseMainWindowDC (HDC hdc);
void WashColorIn (void);

#endif
