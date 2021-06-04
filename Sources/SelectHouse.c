//============================================================================
//----------------------------------------------------------------------------
//                               SelectHouse.c
//----------------------------------------------------------------------------
//============================================================================

#include "SelectHouse.h"

#include "DialogUtils.h"
#include "House.h"
#include "HouseIO.h"
#include "ResourceIDs.h"
#include "ResourceLoader.h"
#include "StringUtils.h"
#include "Utilities.h"

#include <commctrl.h>
#include <shlwapi.h>
#include <strsafe.h>

#define kMaxExtraHouses         8

BOOL InitLoadDialog (HWND hDlg);
INT_PTR CALLBACK LoadFilter (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SortHouseList (void);
void DoDirSearch (HWND ownerWindow);

houseSpecPtr g_theHousesSpecs;
SInt16 g_housesFound;
SInt16 g_thisHouseIndex;
SInt16 g_maxFiles;
SInt16 g_willMaxFiles;
SInt16 g_demoHouseIndex;

static houseSpec g_extraHouseSpecs[kMaxExtraHouses];
static SInt16 g_numExtraHouses = 0;
static HIMAGELIST g_houseIconImageList;

//==============================================================  Functions
//--------------------------------------------------------------  InitLoadDialog

BOOL InitLoadDialog (HWND hDlg)
{
	HWND houseListView;
	LVITEM lvItem;
	LVFINDINFO lvFindInfo;
	int i;

	CenterDialogOverOwner(hDlg);
	houseListView = GetDlgItem(hDlg, kHouseListItem);
	if (houseListView == NULL)
	{
		EndDialog(hDlg, IDCANCEL);
		return TRUE;
	}

	SendMessage(houseListView, WM_SETREDRAW, FALSE, 0);
	ListView_SetImageList(houseListView, g_houseIconImageList, LVSIL_NORMAL);
	for (i = 0; i < g_housesFound; i++)
	{
		lvItem.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
		lvItem.iItem = g_housesFound; // insert at the end
		lvItem.iSubItem = 0;
		lvItem.pszText = g_theHousesSpecs[i].houseName;
		lvItem.iImage = g_theHousesSpecs[i].iconIndex;
		lvItem.lParam = i;
		ListView_InsertItem(houseListView, &lvItem);
	}
	SendMessage(houseListView, WM_SETREDRAW, TRUE, 0);

	// automatically select and focus the currently loaded house
	lvFindInfo.flags = LVFI_PARAM;
	lvFindInfo.lParam = g_thisHouseIndex;
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

//--------------------------------------------------------------  LoadFilter

INT_PTR CALLBACK LoadFilter (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND houseListView;
	INT selectedIndex;
	LVITEM lvItem;
	SInt16 whoCares;

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
				if (lvItem.lParam != g_thisHouseIndex)
				{
					g_thisHouseIndex = (SInt16)lvItem.lParam;
					whoCares = CloseHouse(hDlg);
					PasStringCopy(g_theHousesSpecs[g_thisHouseIndex].name, g_thisHouseName);
					if (OpenHouse(hDlg))
						whoCares = ReadHouse(hDlg, true);
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

//--------------------------------------------------------------  DoLoadHouse

void DoLoadHouse (HWND ownerWindow)
{
	DialogBox(HINST_THISCOMPONENT,
			MAKEINTRESOURCE(kLoadHouseDialogID),
			ownerWindow, LoadFilter);
}

//--------------------------------------------------------------  SortHouseList

void SortHouseList (void)
{
	houseSpec tempSpec;
	SInt16 i, h, whosFirst;

	i = 0;  // remove exact duplicate houses
	while (i < g_housesFound)
	{
		h = i + 1;
		while (h < g_housesFound)
		{
			if (PasStringEqual(g_theHousesSpecs[i].name, g_theHousesSpecs[h].name, true))
			{
				g_theHousesSpecs[h] = g_theHousesSpecs[g_housesFound - 1];
				g_housesFound--;
			}
			h++;
		}
		i++;
	}

	for (i = 0; i < g_housesFound - 1; i++)
	{
		for (h = 0; h < (g_housesFound - i - 1); h++)
		{
			whosFirst = WhichStringFirst(g_theHousesSpecs[h].name,
					g_theHousesSpecs[h + 1].name);
			if (whosFirst == 1)
			{
				tempSpec = g_theHousesSpecs[h + 1];
				g_theHousesSpecs[h + 1] = g_theHousesSpecs[h];
				g_theHousesSpecs[h] = tempSpec;
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
	if (!PathAppend(pathBuffer, L"Houses"))
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
	#define kMaxDirectories  32
	WIN32_FIND_DATA ffd;
	WCHAR pathString[MAX_PATH];
	WCHAR newPathString[MAX_PATH];
	HANDLE findFileHandles[kMaxDirectories];
	HANDLE hff;
	SInt16 i, currentDir, numDirs;
	PCWSTR extPtr;
	LPCWSTR combineResult;
	HRESULT hr;
	int cxIcon, cyIcon;
	HICON houseIcon;
	Gp_HouseFile *houseFile;

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
	g_houseIconImageList = ImageList_Create(cxIcon, cyIcon, ILC_COLOR32 | ILC_MASK, 0, 0);
	ImageList_AddIcon(g_houseIconImageList, houseIcon);

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
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// handle a directory entry
			if ((numDirs < kMaxDirectories) &&
					(wcscmp(ffd.cFileName, L".") != 0) &&
					(wcscmp(ffd.cFileName, L"..") != 0))
			{
				hff = NULL;
				combineResult = PathCombine(newPathString, pathString, ffd.cFileName);
				if (combineResult != NULL)
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
			extPtr = PathFindExtension(ffd.cFileName);
			if ((g_housesFound < g_maxFiles) && (wcscmp(extPtr, L".glh") == 0))
			{
				combineResult = PathCombine(g_theHousesSpecs[g_housesFound].path,
						pathString, ffd.cFileName);
				if (combineResult != NULL)
				{
					PathRemoveExtension(ffd.cFileName);
					hr = StringCchCopy(g_theHousesSpecs[g_housesFound].houseName,
							ARRAYSIZE(g_theHousesSpecs[g_housesFound].houseName),
							ffd.cFileName);
					if (SUCCEEDED(hr) || hr == STRSAFE_E_INSUFFICIENT_BUFFER)
					{
						// STRSAFE_E_INSUFFICIENT_BUFFER is okay. Just means that the
						// house's name will be truncated on display.
						MacFromWinString(g_theHousesSpecs[g_housesFound].name,
								ARRAYSIZE(g_theHousesSpecs[g_housesFound].name),
								g_theHousesSpecs[g_housesFound].houseName);

						// Extract the house's icon.
						hr = Gp_LoadHouseFile(g_theHousesSpecs[g_housesFound].path, &houseFile);
						if (SUCCEEDED(hr))
						{
							houseIcon = Gp_LoadHouseIcon(houseFile, cxIcon, cyIcon);
							if (houseIcon != NULL)
							{
								g_theHousesSpecs[g_housesFound].iconIndex =
									ImageList_AddIcon(g_houseIconImageList, houseIcon);
								DestroyIcon(houseIcon);
							}
							else
							{
								g_theHousesSpecs[g_housesFound].iconIndex = 0;
							}
							Gp_UnloadHouseFile(houseFile);
							g_housesFound++;
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
			PathRemoveFileSpec(pathString); // remove final component in path
		}
	}

	if (g_housesFound < 1)
	{
		g_thisHouseIndex = -1;
		g_demoHouseIndex = -1;
		YellowAlert(ownerWindow, kYellowNoHouses, 0);
	}
	else
	{
		SortHouseList();
		g_thisHouseIndex = 0;
		for (i = 0; i < g_housesFound; i++)
		{
			if (PasStringEqual(g_theHousesSpecs[i].name, g_thisHouseName, false))
			{
				g_thisHouseIndex = i;
				break;
			}
		}
		PasStringCopy(g_theHousesSpecs[g_thisHouseIndex].name, g_thisHouseName);

		g_demoHouseIndex = -1;
		for (i = 0; i < g_housesFound; i++)
		{
			Str32 demoHouseName;

			PasStringCopyC("Demo House", demoHouseName);
			if (PasStringEqual(g_theHousesSpecs[i].name, demoHouseName, false))
			{
				g_demoHouseIndex = i;
				break;
			}
		}
	}
}

//--------------------------------------------------------------  BuildHouseList

void BuildHouseList (HWND ownerWindow)
{
	SInt16 i;

	// destroy icons from previous search
	if (g_houseIconImageList != NULL)
	{
		ImageList_Destroy(g_houseIconImageList);
		g_houseIconImageList = NULL;
	}
	// zero the number of houses found
	g_housesFound = 0;
	// 1st, insert extra houses into list
	for (i = 0; i < g_numExtraHouses; i++)
	{
		if (g_housesFound < g_maxFiles)
		{
			g_theHousesSpecs[g_housesFound] = g_extraHouseSpecs[i];
			g_housesFound++;
		}
	}
	// now, search folders for the rest
	DoDirSearch(ownerWindow);
}

//--------------------------------------------------------------  AddExtraHouse

void AddExtraHouse (const houseSpec *newHouse)
{
	if (g_numExtraHouses >= kMaxExtraHouses)
		return;

	g_extraHouseSpecs[g_numExtraHouses] = *newHouse;
	g_numExtraHouses++;
}
