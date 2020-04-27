
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


#define	kPrefCreatorType	'ozm5'
#define	kPrefFileType		'gliP'
#define	kPrefFileName		L"Glider Prefs"
#define	kDefaultPrefFName	"\pPreferences"
#define kPrefsStringsID		160
#define kNewPrefsAlertID	160
#define	kPrefsFNameIndex	1


Boolean GetPrefsFilePath (LPWSTR, size_t);
Boolean WritePrefs (LPCWSTR, prefsInfo *);
HRESULT ReadPrefs (LPCWSTR, prefsInfo *);
Boolean DeletePrefs (LPCWSTR);
void BringUpDeletePrefsAlert (void);


//==============================================================  Functions
//--------------------------------------------------------------  GetPrefsFPath

Boolean GetPrefsFilePath (LPWSTR lpFilePath, size_t cchFilePath)
{
	WCHAR path[MAX_PATH];

	if (FAILED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path)))
		return false;
	if (FAILED(StringCchCat(path, ARRAYSIZE(path), L"\\glider-pro-windows")))
		return false;
	if (!CreateDirectory(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return false;
	if (FAILED(StringCchCat(path, ARRAYSIZE(path), L"\\")))
		return false;
	if (FAILED(StringCchCat(path, ARRAYSIZE(path), kPrefFileName)))
		return false;
	if (FAILED(StringCchCopy(lpFilePath, cchFilePath, path)))
		return false;

	return true;
}

//--------------------------------------------------------------  WritePrefs

Boolean WritePrefs (LPCWSTR prefsFilePath, prefsInfo *thePrefs)
{
	HANDLE		fileHandle;
	byteio		byteWriter;
	Str255		fileType;

	PasStringCopyC("Preferences", fileType);
	fileHandle = CreateFile(prefsFilePath, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		CheckFileError(GetLastError(), fileType);
		return false;
	}
	if (!byteio_init_handle_writer(&byteWriter, fileHandle))
	{
		CloseHandle(fileHandle);
		return false;
	}

	if (!WritePrefsInfo(&byteWriter, thePrefs))
	{
		CheckFileError(GetLastError(), fileType);
		byteio_close(&byteWriter);
		CloseHandle(fileHandle);
		return false;
	}

	if (!byteio_close(&byteWriter))
	{
		CheckFileError(GetLastError(), fileType);
		CloseHandle(fileHandle);
		return false;
	}
	if (!CloseHandle(fileHandle))
	{
		CheckFileError(GetLastError(), fileType);
		return false;
	}

	return true;
}

//--------------------------------------------------------------  SavePrefs

Boolean SavePrefs (prefsInfo *thePrefs, SInt16 versionNow)
{
	WCHAR prefsFilePath[MAX_PATH];

	thePrefs->prefVersion = versionNow;

	if (!GetPrefsFilePath(prefsFilePath, ARRAYSIZE(prefsFilePath)))
		return false;

	if (!WritePrefs(prefsFilePath, thePrefs))
		return false;

	return true;
}

//--------------------------------------------------------------  ReadPrefs

HRESULT ReadPrefs (LPCWSTR prefsFilePath, prefsInfo *thePrefs)
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
			CheckFileError(lastError, fileType);
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
			CheckFileError(lastError, fileType);
		byteio_close(&byteReader);
		CloseHandle(fileHandle);
		return HRESULT_FROM_WIN32(lastError);
	}

	if (!byteio_close(&byteReader))
	{
		lastError = GetLastError();
		CheckFileError(lastError, fileType);
		CloseHandle(fileHandle);
		return HRESULT_FROM_WIN32(lastError);
	}
	if (!CloseHandle(fileHandle))
	{
		lastError = GetLastError();
		CheckFileError(lastError, fileType);
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

Boolean LoadPrefs (prefsInfo *thePrefs, SInt16 versionNeed)
{
	WCHAR prefsFilePath[MAX_PATH];
	HRESULT hr;

	if (!GetPrefsFilePath(prefsFilePath, ARRAYSIZE(prefsFilePath)))
		return false;

	hr = ReadPrefs(prefsFilePath, thePrefs);
	if (hr == HRESULT_FROM_WIN32(ERROR_HANDLE_EOF))
	{
		BringUpDeletePrefsAlert();
		DeletePrefs(prefsFilePath);
		return false;
	}
	else if (FAILED(hr))
		return false;

	if (thePrefs->prefVersion != versionNeed)
	{
		BringUpDeletePrefsAlert();
		DeletePrefs(prefsFilePath);
		return false;
	}

	return true;
}

//--------------------------------------------------------------  BringUpDeletePrefsAlert

void BringUpDeletePrefsAlert (void)
{
	AlertData	alertData = { 0 };
	SInt16		whoCares;

	alertData.hwndParent = mainWindow;
	whoCares = Alert(kNewPrefsAlertID, &alertData);
}

