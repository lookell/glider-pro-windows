#define GP_USE_WINAPI_H

#include "SelectHouse.h"

//============================================================================
//----------------------------------------------------------------------------
//                               SelectHouse.c
//----------------------------------------------------------------------------
//============================================================================


#include "AnimCursor.h"
#include "DialogUtils.h"
#include "House.h"
#include "HouseIO.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <commctrl.h>
#include <strsafe.h>


#define kHouseListItem			1000
#define kMaxExtraHouses			8


BOOL InitLoadDialog (HWND hDlg);
INT_PTR CALLBACK LoadFilter (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SortHouseList (void);
BOOL GetHouseFolderPath (LPWSTR buffer, DWORD cch);
void DoDirSearch (HWND ownerWindow);


houseSpecPtr theHousesSpecs;
SInt16 housesFound;
SInt16 thisHouseIndex;
SInt16 maxFiles;
SInt16 willMaxFiles;
SInt16 demoHouseIndex;

static houseSpec extraHouseSpecs[kMaxExtraHouses];
static SInt16 numExtraHouses = 0;
static HIMAGELIST houseIconImageList;


//==============================================================  Functions
//--------------------------------------------------------------  InitLoadDialog

#ifndef COMPILEDEMO
BOOL InitLoadDialog (HWND hDlg)
{
	HWND houseListView;
	LVITEM lvItem;
	LVFINDINFO lvFindInfo;
	int i;

	CenterOverOwner(hDlg);
	houseListView = GetDlgItem(hDlg, kHouseListItem);
	if (houseListView == NULL)
	{
		EndDialog(hDlg, IDCANCEL);
		return TRUE;
	}

	SendMessage(houseListView, WM_SETREDRAW, FALSE, 0);
	ListView_SetImageList(houseListView, houseIconImageList, LVSIL_NORMAL);
	for (i = 0; i < housesFound; i++)
	{
		lvItem.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
		lvItem.iItem = housesFound; // insert at the end
		lvItem.iSubItem = 0;
		lvItem.pszText = theHousesSpecs[i].houseName;
		lvItem.iImage = theHousesSpecs[i].iconIndex;
		lvItem.lParam = i;
		ListView_InsertItem(houseListView, &lvItem);
	}
	SendMessage(houseListView, WM_SETREDRAW, TRUE, 0);

	// automatically select and focus the currently loaded house
	lvFindInfo.flags = LVFI_PARAM;
	lvFindInfo.lParam = thisHouseIndex;
	i = ListView_FindItem(houseListView, -1, &lvFindInfo);
	if (i < 0)
	{
		EndDialog(hDlg, IDCANCEL); // this shouldn't ever happen
		return TRUE;
	}
	ListView_SetItemState(houseListView, i, LVIS_FOCUSED | LVIS_SELECTED, (UINT)-1);
	ListView_EnsureVisible(houseListView, i, FALSE);

	return TRUE;
}
#endif

//--------------------------------------------------------------  LoadFilter

#ifndef COMPILEDEMO
INT_PTR CALLBACK LoadFilter (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND		houseListView;
	INT			selectedIndex;
	LVITEM		lvItem;
	SInt16		whoCares;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		return InitLoadDialog(hDlg);

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			houseListView = GetDlgItem(hDlg, kHouseListItem);
			selectedIndex = ListView_GetNextItem(houseListView, -1, LVNI_SELECTED);
			if (selectedIndex != -1)
			{
				lvItem.mask = LVIF_PARAM;
				lvItem.iItem = selectedIndex;
				lvItem.iSubItem = 0;
				ListView_GetItem(houseListView, &lvItem);
				if (lvItem.lParam != thisHouseIndex)
				{
					thisHouseIndex = (SInt16)lvItem.lParam;
					whoCares = CloseHouse(hDlg);
					PasStringCopy(theHousesSpecs[thisHouseIndex].name, thisHouseName);
					if (OpenHouse(hDlg))
						whoCares = ReadHouse(hDlg);
				}
			}
			EndDialog(hDlg, IDOK);
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
			return TRUE;
		}
		break;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_DBLCLK:
			if (((LPNMHDR)lParam)->idFrom == kHouseListItem)
			{
				LVHITTESTINFO hitTestInfo;

				hitTestInfo.pt = ((LPNMITEMACTIVATE)lParam)->ptAction;
				ListView_HitTest(((LPNMHDR)lParam)->hwndFrom, &hitTestInfo);
				if ((hitTestInfo.flags & LVHT_ONITEM) != 0)
				{
					SendMessage(hDlg, WM_COMMAND, IDOK, 0);
				}
				SetWindowLongPtr(hDlg, DWLP_MSGRESULT, 0);
				return TRUE;
			}
			break;
		}
		break;
	}
	return FALSE;
}
#endif

//--------------------------------------------------------------  DoLoadHouse

#ifndef COMPILEDEMO
void DoLoadHouse (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kLoadHouseDialogID),
			ownerWindow, LoadFilter);
}
#endif

//--------------------------------------------------------------  SortHouseList

void SortHouseList (void)
{
	houseSpec	tempSpec;
	SInt16		i, h, whosFirst;

	i = 0;			// remove exact duplicate houses
	while (i < housesFound)
	{
		h = i + 1;
		while (h < housesFound)
		{
			if (PasStringEqual(theHousesSpecs[i].name, theHousesSpecs[h].name, true))
			{
				theHousesSpecs[h] = theHousesSpecs[housesFound - 1];
				housesFound--;
			}
			h++;
		}
		i++;
	}

	for (i = 0; i < housesFound - 1; i++)
	{
		for (h = 0; h < (housesFound - i - 1); h++)
		{
			whosFirst = WhichStringFirst(theHousesSpecs[h].name,
					theHousesSpecs[h + 1].name);
			if (whosFirst == 1)
			{
				tempSpec = theHousesSpecs[h + 1];
				theHousesSpecs[h + 1] = theHousesSpecs[h];
				theHousesSpecs[h] = tempSpec;
			}
		}
	}
}

//--------------------------------------------------------------  GetHouseFolderPath

BOOL GetHouseFolderPath(LPWSTR buffer, DWORD cch)
{
	WCHAR pathBuffer[MAX_PATH];
	HRESULT hr;

	if (!GetDataFolderPath(pathBuffer, ARRAYSIZE(pathBuffer)))
		return FALSE;
	hr = StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L"\\Houses");
	if (FAILED(hr))
		return FALSE;
	if (!CreateDirectory(pathBuffer, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return FALSE;

	hr = StringCchCopy(buffer, cch, pathBuffer);
	return SUCCEEDED(hr);
}

//--------------------------------------------------------------  DoDirSearch

static HANDLE OpenFindFile(LPCWSTR lpPath, LPWIN32_FIND_DATA lpFindFileData)
{
	WCHAR pattern[MAX_PATH];
	if (FAILED(StringCchCopy(pattern, ARRAYSIZE(pattern), lpPath)))
		return INVALID_HANDLE_VALUE;
	if (FAILED(StringCchCat(pattern, ARRAYSIZE(pattern), L"\\*")))
		return INVALID_HANDLE_VALUE;
	return FindFirstFile(pattern, lpFindFileData);
}

void DoDirSearch (HWND ownerWindow)
{
	#define			kMaxDirectories		32
	WIN32_FIND_DATA	ffd;
	WCHAR			pathString[MAX_PATH];
	WCHAR			newPathString[MAX_PATH];
	HANDLE			findFileHandles[kMaxDirectories];
	HANDLE			hff;
	SInt16			i, currentDir, numDirs;
	PWCH			extPtr, sepPtr;
	HRESULT			hr;
	int				cxIcon, cyIcon;
	HICON			houseIcon;

	cxIcon = GetSystemMetrics(SM_CXICON);
	cyIcon = GetSystemMetrics(SM_CYICON);
	houseIcon = (HICON)LoadImage(
		HINST_THISCOMPONENT,
		MAKEINTRESOURCE(IDI_HOUSE),
		IMAGE_ICON,
		cxIcon,
		cyIcon,
		LR_DEFAULTCOLOR | LR_SHARED
	);
	if (houseIcon == NULL)
	{
		RedAlert(kErrFailedResourceLoad);
	}
	houseIconImageList = ImageList_Create(cxIcon, cyIcon, ILC_COLOR32 | ILC_MASK, 0, 0);
	ImageList_AddIcon(houseIconImageList, houseIcon);

	currentDir = 0;
	numDirs = 1;
	if (!GetHouseFolderPath(pathString, ARRAYSIZE(pathString)))
		RedAlert(kErrFailedCatSearch);
	hff = OpenFindFile(pathString, &ffd);
	if (hff == INVALID_HANDLE_VALUE || hff == NULL)
		RedAlert(kErrFailedCatSearch);
	findFileHandles[currentDir] = hff;

	while (numDirs > 0)
	{
		SpinCursor(1);
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// handle a directory entry
			if ((numDirs < kMaxDirectories) &&
					(wcscmp(ffd.cFileName, L".") != 0) &&
					(wcscmp(ffd.cFileName, L"..") != 0))
			{
				hff = NULL;
				hr = StringCchPrintf(newPathString, ARRAYSIZE(newPathString),
						L"%s\\%s", pathString, ffd.cFileName);
				if (SUCCEEDED(hr))
				{
					hff = OpenFindFile(newPathString, &ffd);
				}
				if (hff != INVALID_HANDLE_VALUE && hff != NULL)
				{
					memcpy(pathString, newPathString, sizeof(pathString));
					currentDir++;
					numDirs++;
					findFileHandles[currentDir] = hff;
					continue; // restart the loop to use the file found just now
				}
			}
		}
		else
		{
			// handle a file entry
			extPtr = wcsrchr(ffd.cFileName, L'.');
			if (extPtr == NULL)
				extPtr = ffd.cFileName;
			if ((housesFound < maxFiles) && (wcscmp(extPtr, L".glh") == 0))
			{
				hr = StringCchPrintf(theHousesSpecs[housesFound].path,
						ARRAYSIZE(theHousesSpecs[housesFound].path),
						L"%s\\%s", pathString, ffd.cFileName);
				if (SUCCEEDED(hr))
				{
					*extPtr = L'\0';
					hr = StringCchCopy(theHousesSpecs[housesFound].houseName,
							ARRAYSIZE(theHousesSpecs[housesFound].houseName),
							ffd.cFileName);
					if (SUCCEEDED(hr) || hr == STRSAFE_E_INSUFFICIENT_BUFFER)
					{
						// STRSAFE_E_INSUFFICIENT_BUFFER is okay. Just means that the
						// house's name will be truncated on display.
						MacFromWinString(theHousesSpecs[housesFound].name,
								ARRAYSIZE(theHousesSpecs[housesFound].name),
								theHousesSpecs[housesFound].houseName);

						// Extract the house's icon.
						// TODO: Upgrade the ResourceLoader interface to handle multiple
						// houses. (Perhaps return the mz_zip_archive* as a void*.)
						hr = Gp_LoadHouseFile(theHousesSpecs[housesFound].path);
						if (SUCCEEDED(hr))
						{
							houseIcon = Gp_LoadHouseIcon(0, 0);
							if (houseIcon != NULL)
							{
								theHousesSpecs[housesFound].iconIndex =
									ImageList_AddIcon(houseIconImageList, houseIcon);
								DestroyIcon(houseIcon);
							}
							else
							{
								theHousesSpecs[housesFound].iconIndex = 0;
							}
							Gp_UnloadHouseFile();
							housesFound++;
						}
					}
				}
			}
		}
		// advance to the next entry
		while (!FindNextFile(findFileHandles[currentDir], &ffd))
		{
			FindClose(findFileHandles[currentDir]);
			findFileHandles[currentDir] = INVALID_HANDLE_VALUE;
			currentDir--;
			numDirs--;
			if (currentDir < 0)
				break;
			sepPtr = wcsrchr(pathString, L'\\');
			if (sepPtr)
				*sepPtr = L'\0';
		}
	}

	if (housesFound < 1)
	{
		thisHouseIndex = -1;
		demoHouseIndex = -1;
		YellowAlert(ownerWindow, kYellowNoHouses, 0);
	}
	else
	{
		SortHouseList();
		thisHouseIndex = 0;
		for (i = 0; i < housesFound; i++)
		{
			if (PasStringEqual(theHousesSpecs[i].name, thisHouseName, false))
			{
				thisHouseIndex = i;
				break;
			}
		}
		PasStringCopy(theHousesSpecs[thisHouseIndex].name, thisHouseName);

		demoHouseIndex = -1;
		for (i = 0; i < housesFound; i++)
		{
			Str32 demoHouseName;

			PasStringCopyC("Demo House", demoHouseName);
			if (PasStringEqual(theHousesSpecs[i].name, demoHouseName, false))
			{
				demoHouseIndex = i;
				break;
			}
		}
	}
}

//--------------------------------------------------------------  BuildHouseList

void BuildHouseList (HWND ownerWindow)
{
	SInt16 i;

	if (houseIconImageList != NULL)			// destroy icons from previous search
	{
		ImageList_Destroy(houseIconImageList);
		houseIconImageList = NULL;
	}
	housesFound = 0;						// zero the number of houses found
	for (i = 0; i < numExtraHouses; i++)	// 1st, insert extra houses into list
	{
		if (housesFound < maxFiles)
		{
			theHousesSpecs[housesFound] = extraHouseSpecs[i];
			housesFound++;
		}
	}
	DoDirSearch(ownerWindow);				// now, search folders for the rest
}

//--------------------------------------------------------------  AddExtraHouse

void AddExtraHouse (const houseSpec *newHouse)
{
	if (numExtraHouses >= kMaxExtraHouses)
		return;

	extraHouseSpecs[numExtraHouses] = *newHouse;
	numExtraHouses++;
}

