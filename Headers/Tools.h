#ifndef TOOLS_H_
#define TOOLS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Tools.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#ifdef GP_USE_WINAPI_H
#include "WinAPI.h"
#endif

#ifdef GP_USE_WINAPI_H
extern HDC toolSrcMap;
extern HWND toolsWindow;
#endif
extern SInt16 isToolsH;
extern SInt16 isToolsV;
extern SInt16 toolSelected;
extern SInt16 toolMode;
extern Boolean isToolsOpen;

void SelectTool (SInt16 which);
void OpenToolsWindow (void);
void CloseToolsWindow (void);
void ToggleToolsWindow (void);
void NextToolMode (void);
void PrevToolMode (void);
void SetSpecificToolMode (SInt16 modeToSet);

#endif
