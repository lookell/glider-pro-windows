#ifndef DIALOG_UTILS_H_
#define DIALOG_UTILS_H_

//============================================================================
//----------------------------------------------------------------------------
//                               DialogUtils.h
//----------------------------------------------------------------------------
//============================================================================

#include "MacTypes.h"
#include "WinAPI.h"

typedef struct DialogParams
{
	PCWSTR arg[4];
} DialogParams;

void ParamDialogText (HWND hDlg, const DialogParams *params);
SInt16 Alert (SInt16 dialogID, HWND ownerWindow, const DialogParams *params);
BOOL FocusDefaultButton (HWND hDlg);
void CenterOverOwner (HWND hwnd);
//void BringUpDialog (DialogPtr *theDialog, SInt16 dialogID);
//void CenterDialog (SInt16 dialogID);
//void DrawDefaultButton (DialogPtr theDialog);
void GetDialogString (HWND theDialog, int item, StringPtr theString, int stringCapacity);
void SetDialogString (HWND theDialog, int item, ConstStringPtr theString);
int GetDialogStringLen (HWND theDialog, int item);
void GetDialogItemRect (HWND theDialog, int item, Rect *theRect);
//void SetDialogItemRect (DialogPtr theDialog, SInt16 item, Rect *theRect);
void AddMenuToComboBox (HWND theDialog, int whichItem, HMENU theMenu);
void GetComboBoxMenuValue (HWND theDialog, int whichItem, SInt16 *value);
void SetComboBoxMenuValue (HWND theDialog, int whichItem, SInt16 value);

#endif
