
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
#include "Externs.h"
#include "Environ.h"


#define	kPrefCreatorType	'ozm5'
#define	kPrefFileType		'gliP'
#define	kPrefFileName		L"Glider Prefs"
#define	kDefaultPrefFName	"\pPreferences"
#define kPrefsStringsID		160
#define kNewPrefsAlertID	160
#define	kPrefsFNameIndex	1


Boolean GetPrefsFPath (LPWSTR, size_t);
Boolean WritePrefs (LPCWSTR, prefsInfo *);
OSErr ReadPrefs (SInt32 *, SInt16 *, prefsInfo *);
Boolean DeletePrefs (SInt32 *, SInt16 *);
void BringUpDeletePrefsAlert (void);


//==============================================================  Functions
//--------------------------------------------------------------  GetPrefsFPath

Boolean GetPrefsFPath (LPWSTR lpFolderPath, size_t cchFolderPath)
{
	WCHAR path[MAX_PATH];

	if (FAILED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path)))
		return false;
	if (FAILED(StringCchCat(path, ARRAYSIZE(path), L"\\glider-pro-windows")))
		return false;
	if (!CreateDirectory(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return false;
	if (FAILED(StringCchCopy(lpFolderPath, cchFolderPath, path)))
		return false;

	return true;
}

//--------------------------------------------------------------  WritePrefs

Boolean WritePrefs (LPCWSTR prefsDirPath, prefsInfo *thePrefs)
{
	HANDLE		fileHandle;
	byteio		byteWriter;
	WCHAR		prefsFilePath[MAX_PATH];
	Str255		fileType;

	PasStringCopyC("Preferences", fileType);
	if (FAILED(StringCchCopy(prefsFilePath, ARRAYSIZE(prefsFilePath), prefsDirPath)))
		return false;
	if (FAILED(StringCchCat(prefsFilePath, ARRAYSIZE(prefsFilePath), L"\\")))
		return false;
	if (FAILED(StringCchCat(prefsFilePath, ARRAYSIZE(prefsFilePath), kPrefFileName)))
		return false;

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
	WCHAR prefsDirPath[MAX_PATH];

	thePrefs->prefVersion = versionNow;

	if (!GetPrefsFPath(prefsDirPath, ARRAYSIZE(prefsDirPath)))
		return false;

	if (!WritePrefs(prefsDirPath, thePrefs))
		return false;

	return true;
}

//--------------------------------------------------------------  ReadPrefs

OSErr ReadPrefs (SInt32 *prefDirID, SInt16 *systemVolRef, prefsInfo *thePrefs)
{
	return (-1);
#if 0
	OSErr		theErr;
	short		fileRefNum;
	long		byteCount;
	FSSpec		theSpecs;
	Str255		fileName = kPrefFileName;

	theErr = FSMakeFSSpec(*systemVolRef, *prefDirID, fileName, &theSpecs);
	if (theErr != noErr)
	{
		if (theErr == fnfErr)
			return(theErr);
		else
		{
			CheckFileError(theErr, "\pPreferences");
			return(theErr);
		}
	}

	theErr = FSpOpenDF(&theSpecs, fsRdWrPerm, &fileRefNum);
	if (theErr != noErr)
	{
		CheckFileError(theErr, "\pPreferences");
		return(theErr);
	}

	byteCount = sizeof(*thePrefs);

	theErr = FSRead(fileRefNum, &byteCount, thePrefs);
	if (theErr != noErr)
	{
		if (theErr == eofErr)
			theErr = FSClose(fileRefNum);
		else
		{
			CheckFileError(theErr, "\pPreferences");
			theErr = FSClose(fileRefNum);
		}
		return(theErr);
	}

	theErr = FSClose(fileRefNum);
	if (theErr != noErr)
	{
		CheckFileError(theErr, "\pPreferences");
		return(theErr);
	}

	return(theErr);
#endif
}

//--------------------------------------------------------------  DeletePrefs

Boolean DeletePrefs (SInt32 *dirID, SInt16 *volRef)
{
	return false;
#if 0
	FSSpec		theSpecs;
	Str255		fileName = kPrefFileName;
	OSErr		theErr;

	theErr = FSMakeFSSpec(*volRef, *dirID, fileName, &theSpecs);
	if (theErr != noErr)
		return(false);
	else
		theErr = FSpDelete(&theSpecs);

	if (theErr != noErr)
		return(false);

	return(true);
#endif
}

//--------------------------------------------------------------  LoadPrefs

Boolean LoadPrefs (prefsInfo *thePrefs, SInt16 versionNeed)
{
	return false;
#if 0
	long		prefDirID;
	OSErr		theErr;
	short		systemVolRef;
	Boolean		noProblems;

	noProblems = GetPrefsFPath(&prefDirID, &systemVolRef);
	if (!noProblems)
		return(false);

	theErr = ReadPrefs(&prefDirID, &systemVolRef, thePrefs);
	if (theErr == eofErr)
	{
		BringUpDeletePrefsAlert();
		noProblems = DeletePrefs(&prefDirID, &systemVolRef);
		return (false);
	}
	else if (theErr != noErr)
		return (false);

	if (thePrefs->prefVersion != versionNeed)
	{
		BringUpDeletePrefsAlert();
		noProblems = DeletePrefs(&prefDirID, &systemVolRef);
		return(false);
	}

	return (true);
#endif
}

//--------------------------------------------------------------  BringUpDeletePrefsAlert

void BringUpDeletePrefsAlert (void)
{
	MessageBox(mainWindow, L"BringUpDeletePrefsAlert()", NULL, MB_ICONHAND);
	return;
#if 0
	short		whoCares;

	InitCursor();
//	CenterAlert(kNewPrefsAlertID);
	whoCares = Alert(kNewPrefsAlertID, nil);
#endif
}

