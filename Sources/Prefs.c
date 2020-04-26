
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
#include "Externs.h"
#include "Environ.h"


#define	kPrefCreatorType	'ozm5'
#define	kPrefFileType		'gliP'
#define	kPrefFileName		"\pGlider Prefs"
#define	kDefaultPrefFName	"\pPreferences"
#define kPrefsStringsID		160
#define kNewPrefsAlertID	160
#define	kPrefsFNameIndex	1


Boolean GetPrefsFPath (LPWSTR, size_t);
Boolean CreatePrefsFolder (SInt16 *);
Boolean WritePrefs (SInt32 *, SInt16 *, prefsInfo *);
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

//--------------------------------------------------------------  CreatePrefsFolder

Boolean CreatePrefsFolder (SInt16 *systemVolRef)
{
	return false;
#if 0
	HFileParam	fileParamBlock;
	Str255		folderName;
	OSErr		theErr;

	GetIndString(folderName, kPrefsStringsID, kPrefsFNameIndex);

	fileParamBlock.ioVRefNum = *systemVolRef;
	fileParamBlock.ioDirID = 0;
	fileParamBlock.ioNamePtr = folderName;
	fileParamBlock.ioCompletion = nil;

	theErr = PBDirCreate((HParmBlkPtr)&fileParamBlock, false);
	if (theErr != noErr)
	{
		CheckFileError(theErr, "\pPreferences");
		return(false);
	}
	return(true);
#endif
}

//--------------------------------------------------------------  WritePrefs

Boolean WritePrefs (SInt32 *prefDirID, SInt16 *systemVolRef, prefsInfo *thePrefs)
{
	return false;
#if 0
	OSErr		theErr;
	short		fileRefNum;
	long		byteCount;
	FSSpec		theSpecs;
	Str255		fileName = kPrefFileName;

	theErr = FSMakeFSSpec(*systemVolRef, *prefDirID, fileName, &theSpecs);
	if (theErr != noErr)
	{
		if (theErr != fnfErr)
		{
			CheckFileError(theErr, "\pPreferences");
			return(false);
		}
		theErr = FSpCreate(&theSpecs, kPrefCreatorType, kPrefFileType, smSystemScript);
		if (theErr != noErr)
		{
			CheckFileError(theErr, "\pPreferences");
			return(false);
		}
	}
	theErr = FSpOpenDF(&theSpecs, fsRdWrPerm, &fileRefNum);
	if (theErr != noErr)
	{
		CheckFileError(theErr, "\pPreferences");
		return(false);
	}

	byteCount = sizeof(*thePrefs);

	theErr = FSWrite(fileRefNum, &byteCount, thePrefs);
	if (theErr != noErr)
	{
		CheckFileError(theErr, "\pPreferences");
		return(false);
	}

	theErr = FSClose(fileRefNum);
	if (theErr != noErr)
	{
		CheckFileError(theErr, "\pPreferences");
		return(false);
	}

	return(true);
#endif
}

//--------------------------------------------------------------  SavePrefs

Boolean SavePrefs (prefsInfo *thePrefs, SInt16 versionNow)
{
	return false;
#if 0
	long		prefDirID;
	short		systemVolRef;

	thePrefs->prefVersion = versionNow;

	if (!GetPrefsFPath(&prefDirID, &systemVolRef))
		return(false);

	if (!WritePrefs(&prefDirID, &systemVolRef, thePrefs))
		return(false);

	return(true);
#endif
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

