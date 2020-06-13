#ifndef TOOLS_H_
#define TOOLS_H_

//============================================================================
//----------------------------------------------------------------------------
//                                  Tools.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

extern HDC		toolSrcMap;
extern HWND		toolsWindow;

void EraseSelectedTool (void);
void SelectTool (SInt16 which);
void UpdateToolsWindow (void);
void OpenToolsWindow (void);
void CloseToolsWindow (void);
void ToggleToolsWindow (void);
void HandleToolsClick (Point wherePt);
void NextToolMode (void);
void PrevToolMode (void);
void SetSpecificToolMode (SInt16 modeToSet);

#endif
