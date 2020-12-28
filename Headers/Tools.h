#ifndef TOOLS_H_
#define TOOLS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Tools.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HWND g_toolsWindow;
extern SInt16 g_isToolsH;
extern SInt16 g_isToolsV;
extern SInt16 g_toolSelected;
extern SInt16 g_toolMode;
extern Boolean g_isToolsOpen;

void SelectTool (SInt16 which);
void OpenToolsWindow (void);
void CloseToolsWindow (void);
void ToggleToolsWindow (void);
void NextToolMode (void);
void PrevToolMode (void);
void SetSpecificToolMode (SInt16 modeToSet);

#endif
