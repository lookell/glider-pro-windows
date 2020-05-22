
//============================================================================
//----------------------------------------------------------------------------
//									Prefs.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Folders.h>
//#include <Gestalt.h>
//#include <Script.h>
//#include <TextUtils.h>
//#include <ToolUtils.h>
#include "Macintosh.h"
#include "ByteIO.h"
#include "DialogUtils.h"
#include "Externs.h"
#include "Environ.h"
#include "ResourceIDs.h"


#define	kPrefCreatorType	'ozm5'
#define	kPrefFileType		'gliP'
#define	kPrefFileName		L"Glider Prefs"
#define	kDefaultPrefFName	"\pPreferences"
#define	kPrefsFNameIndex	1


Boolean GetPrefsFilePath (LPWSTR, size_t);
Boolean WritePrefs (HWND, LPCWSTR, prefsInfo *);
HRESULT ReadPrefs (HWND, LPCWSTR, prefsInfo *);
Boolean DeletePrefs (LPCWSTR);
void BringUpDeletePrefsAlert (HWND);


//==============================================================  Functions
//--------------------------------------------------------------  GetPrefsFPath

Boolean GetPrefsFilePath (LPWSTR lpFilePath, size_t cchFilePath)
{
	WCHAR pathBuffer[MAX_PATH];
	HRESULT hr;

	if (!GetDataFolderPath(pathBuffer, ARRAYSIZE(pathBuffer)))
		return false;
	hr = StringCchCat(pathBuffer, ARRAYSIZE(pathBuffer), L"\\" kPrefFileName);
	if (FAILED(hr))
		return false;
	hr = StringCchCopy(lpFilePath, cchFilePath, pathBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

//--------------------------------------------------------------  WritePrefs

Boolean WritePrefs (HWND ownerWindow, LPCWSTR prefsFilePath, prefsInfo *thePrefs)
{
	HANDLE		fileHandle;
	byteio		byteWriter;
	Str255		fileType;

	PasStringCopyC("Preferences", fileType);
	fileHandle = CreateFile(prefsFilePath, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		CheckFileError(ownerWindow, GetLastError(), fileType);
		return false;
	}
	if (!byteio_init_handle_writer(&byteWriter, fileHandle))
	{
		CloseHandle(fileHandle);
		return false;
	}

	if (!WritePrefsInfo(&byteWriter, thePrefs))
	{
		CheckFileError(ownerWindow, GetLastError(), fileType);
		byteio_close(&byteWriter);
		CloseHandle(fileHandle);
		return false;
	}

	if (!byteio_close(&byteWriter))
	{
		CheckFileError(ownerWindow, GetLastError(), fileType);
		CloseHandle(fileHandle);
		return false;
	}
	if (!CloseHandle(fileHandle))
	{
		CheckFileError(ownerWindow, GetLastError(), fileType);
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
	HANDLE		fileHandle;
	byteio		byteReader;
	Str255		fileType;
	DWORD		lastError;

	PasStringCopyC("Preferences", fileType);
	fileHandle = CreateFile(prefsFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		lastError = GetLastError();
		if (lastError != ERROR_FILE_NOT_FOUND)
			CheckFileError(ownerWindow, lastError, fileType);
		return HRESULT_FROM_WIN32(lastError);
	}
	if (!byteio_init_handle_reader(&byteReader, fileHandle))
	{
		CloseHandle(fileHandle);
		return E_OUTOFMEMORY;
	}

	if (!ReadPrefsInfo(&byteReader, thePrefs))
	{
		lastError = GetLastError();
		if (lastError != ERROR_HANDLE_EOF)
			CheckFileError(ownerWindow, lastError, fileType);
		byteio_close(&byteReader);
		CloseHandle(fileHandle);
		return HRESULT_FROM_WIN32(lastError);
	}

	if (!byteio_close(&byteReader))
	{
		lastError = GetLastError();
		CheckFileError(ownerWindow, lastError, fileType);
		CloseHandle(fileHandle);
		return HRESULT_FROM_WIN32(lastError);
	}
	if (!CloseHandle(fileHandle))
	{
		lastError = GetLastError();
		CheckFileError(ownerWindow, lastError, fileType);
		return HRESULT_FROM_WIN32(lastError);
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

