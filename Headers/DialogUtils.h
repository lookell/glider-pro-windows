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
	wchar_t *arg[4];
} DialogParams;

void ParamDialogText (HWND hDlg, const DialogParams *params);
SInt16 Alert (SInt16 dialogID, HWND ownerWindow, const DialogParams *params);
BOOL FocusDefaultButton (HWND hDlg);
void CenterOverOwner (HWND hwnd);
void BringUpDialog (DialogPtr *theDialog, SInt16 dialogID);
//void GetPutDialogCorner (Point *theCorner);
//void GetGetDialogCorner (Point *theCorner);
//void CenterDialog (SInt16 dialogID);
void GetDialogRect (Rect *bounds, SInt16 dialogID);
//void TrueCenterDialog (SInt16 dialogID);
//void CenterAlert (SInt16 alertID);
//void ZoomOutDialogRect (SInt16 dialogID);
//void ZoomOutAlertRect (SInt16 alertID);
void FlashDialogButton (DialogPtr theDialog, SInt16 itemNumber);
void DrawDefaultButton (DialogPtr theDialog);
void GetDialogString (HWND theDialog, int item, StringPtr theString, int stringCapacity);
void SetDialogString (HWND theDialog, int item, ConstStringPtr theString);
int GetDialogStringLen (HWND theDialog, int item);
void GetDialogItemValue (DialogPtr theDialog, SInt16 item, SInt16 *theState);
void SetDialogItemValue (DialogPtr theDialog, SInt16 item, SInt16 theState);
void ToggleDialogItemValue (DialogPtr theDialog, SInt16 item);
void SetDialogNumToStr (DialogPtr theDialog, SInt16 item, SInt32 theNumber);
void GetDialogNumFromStr (DialogPtr theDialog, SInt16 item, SInt32 *theNumber);
void GetDialogItemRect (DialogPtr theDialog, SInt16 item, Rect *theRect);
void SetDialogItemRect (DialogPtr theDialog, SInt16 item, Rect *theRect);
void OffsetDialogItemRect (DialogPtr theDialog, SInt16 item, SInt16 h, SInt16 v);
void SelectFromRadioGroup (DialogPtr dial, SInt16 which, SInt16 first, SInt16 last);
void AddMenuToComboBox (HWND theDialog, int whichItem, HMENU theMenu);
void GetComboBoxMenuValue (HWND theDialog, int whichItem, SInt16 *value);
void SetComboBoxMenuValue (HWND theDialog, int whichItem, SInt16 value);
void MyEnableControl (DialogPtr theDialog, SInt16 whichItem);
void MyDisableControl (DialogPtr theDialog, SInt16 whichItem);
void DrawDialogUserText (DialogPtr dial, SInt16 item, StringPtr text, Boolean invert);
void DrawDialogUserText2 (DialogPtr dial, SInt16 item, StringPtr text);
void LoadDialogPICT (DialogPtr theDialog, SInt16 item, SInt16 theID);
void FrameDialogItem (DialogPtr theDialog, SInt16 item);
void FrameDialogItemC (DialogPtr theDialog, SInt16 item, SInt32 color);
void FrameOvalDialogItem (DialogPtr theDialog, SInt16 item);
void BorderDialogItem (DialogPtr theDialog, SInt16 item, SInt16 sides);
void ShadowDialogItem (DialogPtr theDialog, SInt16 item, SInt16 thickness);
void EraseDialogItem (DialogPtr theDialog, SInt16 item);

#endif
