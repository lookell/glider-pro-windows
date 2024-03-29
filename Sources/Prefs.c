//============================================================================
//----------------------------------------------------------------------------
//                                  Prefs.c
//----------------------------------------------------------------------------
//============================================================================

#include "Prefs.h"

#include "ByteIO.h"
#include "DialogUtils.h"
#include "FileError.h"
#include "ResourceIDs.h"
#include "StructIO.h"
#include "Utilities.h"

#include <shlwapi.h>
#include <strsafe.h>

#define kPrefFileName       L"Glider Prefs"

Boolean GetPrefsFilePath (LPWSTR lpFilePath, size_t cchFilePath);
Boolean WritePrefs (HWND ownerWindow, LPCWSTR prefsFilePath, const prefsInfo *thePrefs);
HRESULT ReadPrefs (HWND ownerWindow, LPCWSTR prefsFilePath, prefsInfo *thePrefs);
Boolean DeletePrefs (LPCWSTR prefsFilePath);
void BringUpDeletePrefsAlert (HWND ownerWindow);

//==============================================================  Functions
//--------------------------------------------------------------  GetPrefsFPath

Boolean GetPrefsFilePath (LPWSTR lpFilePath, size_t cchFilePath)
{
	WCHAR pathBuffer[MAX_PATH];
	HRESULT hr;

	if (!GetDataFolderPath(pathBuffer, ARRAYSIZE(pathBuffer)))
		return false;
	if (!PathAppend(pathBuffer, kPrefFileName))
		return false;
	hr = StringCchCopy(lpFilePath, cchFilePath, pathBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

//--------------------------------------------------------------  WritePrefs

Boolean WritePrefs (HWND ownerWindow, LPCWSTR prefsFilePath, const prefsInfo *thePrefs)
{
	HANDLE fileHandle;
	byteio *byteWriter;
	HRESULT writeResult;
	HRESULT closeResult;

	fileHandle = CreateFile(prefsFilePath, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		CheckFileError(ownerWindow, HRESULT_FROM_WIN32(GetLastError()), L"Preferences");
		return false;
	}
	byteWriter = byteio_init_handle_writer(fileHandle);
	if (byteWriter == NULL)
	{
		CloseHandle(fileHandle);
		return false;
	}
	writeResult = WritePrefsInfo(byteWriter, thePrefs);
	closeResult = byteio_close(byteWriter);
	CloseHandle(fileHandle);
	if (FAILED(writeResult))
	{
		CheckFileError(ownerWindow, writeResult, L"Preferences");
		return false;
	}
	if (FAILED(closeResult))
	{
		CheckFileError(ownerWindow, closeResult, L"Preferences");
		return false;
	}
	return true;
}

//--------------------------------------------------------------  SavePrefs

Boolean SavePrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNow)
{
	WCHAR prefsFilePath[MAX_PATH];

	thePrefs->prefVersion = versionNow;

	if (!GetPrefsFilePath(prefsFilePath, ARRAYSIZE(prefsFilePath)))
		return false;

	if (!WritePrefs(ownerWindow, prefsFilePath, thePrefs))
		return false;

	return true;
}

//--------------------------------------------------------------  ReadPrefs

HRESULT ReadPrefs (HWND ownerWindow, LPCWSTR prefsFilePath, prefsInfo *thePrefs)
{
	HANDLE fileHandle;
	byteio *byteReader;
	DWORD lastError;
	HRESULT readResult;

	fileHandle = CreateFile(prefsFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		lastError = GetLastError();
		if (lastError != ERROR_FILE_NOT_FOUND)
		{
			CheckFileError(ownerWindow, HRESULT_FROM_WIN32(lastError), L"Preferences");
		}
		return HRESULT_FROM_WIN32(lastError);
	}
	byteReader = byteio_init_handle_reader(fileHandle);
	if (byteReader == NULL)
	{
		CloseHandle(fileHandle);
		return E_OUTOFMEMORY;
	}
	readResult = ReadPrefsInfo(byteReader, thePrefs);
	byteio_close(byteReader);
	CloseHandle(fileHandle);
	if (FAILED(readResult))
	{
		if (readResult != HRESULT_FROM_WIN32(ERROR_HANDLE_EOF))
		{
			CheckFileError(ownerWindow, readResult, L"Preferences");
		}
		return readResult;
	}
	return S_OK;
}

//--------------------------------------------------------------  DeletePrefs

Boolean DeletePrefs (LPCWSTR prefsFilePath)
{
	if (!DeleteFile(prefsFilePath))
		return false;

	return true;
}

//--------------------------------------------------------------  LoadPrefs

Boolean LoadPrefs (HWND ownerWindow, prefsInfo *thePrefs, SInt16 versionNeed)
{
	WCHAR prefsFilePath[MAX_PATH];
	HRESULT hr;

	if (!GetPrefsFilePath(prefsFilePath, ARRAYSIZE(prefsFilePath)))
		return false;

	hr = ReadPrefs(ownerWindow, prefsFilePath, thePrefs);
	if (hr == HRESULT_FROM_WIN32(ERROR_HANDLE_EOF))
	{
		BringUpDeletePrefsAlert(ownerWindow);
		DeletePrefs(prefsFilePath);
		return false;
	}
	else if (FAILED(hr))
		return false;

	if (thePrefs->prefVersion != versionNeed)
	{
		BringUpDeletePrefsAlert(ownerWindow);
		DeletePrefs(prefsFilePath);
		return false;
	}

	return true;
}

//--------------------------------------------------------------  BringUpDeletePrefsAlert

void BringUpDeletePrefsAlert (HWND ownerWindow)
{
	Alert(kNewPrefsAlertID, ownerWindow, NULL);
}
