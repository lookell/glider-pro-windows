
//============================================================================
//----------------------------------------------------------------------------
//								 ObjectDraw2.c
//----------------------------------------------------------------------------
//============================================================================


//#include <Resources.h>
//#include <TextUtils.h>
#define _CRT_SECURE_NO_WARNINGS
#include <wchar.h>
#include "Macintosh.h"
#include "Externs.h"
#include "Environ.h"
#include "Objects.h"
#include "RectUtils.h"
#include "ResourceIDs.h"
#include "Room.h"
#include "Utilities.h"


#define k8WhiteColor			0
#define kIntenseYellowColor		5
#define kPaleVioletColor		42
#define kDarkFleshColor			58
#define k8TanColor				94
#define k8PissYellowColor		95
#define k8BrownColor			137
#define k8SkyColor				150
#define k8EarthBlueColor		170
#define k8DkRed2Color			223
#define kIntenseGreenColor		225
#define kIntenseBlueColor		235
#define k8LtstGrayColor			245
#define k8LtstGray4Color		247
#define k8LtstGray5Color		248
#define k8LtGrayColor			249
#define k8Gray2Color			251
#define k8DkGrayColor			252
#define k8DkGray2Color			253

#define kBBQMaskID				3900
#define kUpStairsMaskID			3901
#define kTrunkMaskID			3902
#define kMailboxRightMaskID		3903
#define kMailboxLeftMaskID		3904
#define kDoorInLeftMaskID		3905
#define kDoorInRightMaskID		3906
#define kWindowInLeftMaskID		3907
#define kWindowInRightMaskID	3908
#define kHipLampMaskID			3909
#define kDecoLampMaskID			3910
#define kGuitarMaskID			3911
#define kTVMaskID				3912
#define kVCRMaskID				3913
#define kStereoMaskID			3914
#define kMicrowaveMaskID		3915
#define kFireplaceMaskID		3916
#define kBearMaskID				3917
#define kVase1MaskID			3918
#define kVase2MaskID			3919
#define kManholeMaskID			3920
#define kBooksMaskID			3922
#define kCloudMaskID			3921
#define kRugMaskID				3923
#define kChimesMaskID			3924
#define kCinderMaskID			3925
#define kFlowerBoxMaskID		3926
#define kCobwebMaskID			3927
#define kCobwebPictID			3958
#define kFlowerBoxPictID		3959
#define kCinderPictID			3960
#define kChimesPictID			3961
#define kRugPictID				3962
#define kBooksPictID			3964
#define kCloudPictID			3965
#define kBulletinPictID			3966
#define kManholePictID			3967
#define kVase2PictID			3968
#define kVase1PictID			3969
#define kCalendarPictID			3970
#define kMicrowavePictID		3971
#define kBearPictID				3972
#define kFireplacePictID		3973
#define kOzmaPictID				3975
#define kWindowExRightPictID	3977
#define kWindowExLeftPictID		3978
#define kWindowInRightPictID	3979
#define kWindowInLeftPictID		3980
#define kDoorExLeftPictID		3981
#define kDoorExRightPictID		3982
#define kDoorInRightPictID		3983
#define kDoorInLeftPictID		3984
#define kMailboxRightPictID		3985
#define kMailboxLeftPictID		3986
#define kTrunkPictID			3987
#define kBBQPictID				3988
#define kStereoPictID			3989
#define kVCRPictID				3990
#define kGuitarPictID			3991
#define kTVPictID				3992
#define kDecoLampPictID			3993
#define kHipLampPictID			3994
#define kFilingCabinetPictID	3995
#define kDownStairsPictID		3996
#define kUpStairsPictID			3997

#define	kMailboxBase			296


//==============================================================  Functions
//--------------------------------------------------------------  DrawMailboxLeft

void DrawMailboxLeft (Rect *theRect, SInt16 down)
{
	Rect		bounds;
	HDC			tempMap;
	HDC			tempMask;
	SInt32		darkGrayC, lightWoodC, darkWoodC;
	OSErr		theErr;

	if (thisMac.isDepth == 4)
	{
		darkGrayC = 13;
		lightWoodC = 9;
		darkWoodC = 11;
	}
	else
	{
		darkGrayC = k8DkGray2Color;
		lightWoodC = k8PissYellowColor;
		darkWoodC = k8BrownColor;
	}

	if (theRect->bottom < down + kMailboxBase)
	{
		ColorLine(backSrcMap, theRect->left + 49, theRect->bottom,
				theRect->left + 49, down + kMailboxBase, darkGrayC);
		ColorLine(backSrcMap, theRect->left + 50, theRect->bottom,
				theRect->left + 50, down + kMailboxBase + 1, lightWoodC);
		ColorLine(backSrcMap, theRect->left + 51, theRect->bottom,
				theRect->left + 51, down + kMailboxBase + 2, lightWoodC);
		ColorLine(backSrcMap, theRect->left + 52, theRect->bottom,
				theRect->left + 52, down + kMailboxBase + 3, lightWoodC);
		ColorLine(backSrcMap, theRect->left + 53, theRect->bottom,
				theRect->left + 53, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 54, theRect->bottom,
				theRect->left + 54, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 55, theRect->bottom,
				theRect->left + 55, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 56, theRect->bottom,
				theRect->left + 56, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 57, theRect->bottom,
				theRect->left + 57, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 58, theRect->bottom,
				theRect->left + 58, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 59, theRect->bottom,
				theRect->left + 59, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 60, theRect->bottom,
				theRect->left + 60, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 61, theRect->bottom,
				theRect->left + 61, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 62, theRect->bottom,
				theRect->left + 62, down + kMailboxBase + 3, darkGrayC);
	}

	bounds = srcRects[kMailboxLf];
	theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
	LoadGraphic(tempMap, kMailboxLeftPictID);

	theErr = CreateOffScreenGWorld(&tempMask, &bounds, 1);
	LoadGraphic(tempMask, kMailboxLeftMaskID);

	Mac_CopyMask(tempMap, tempMask, backSrcMap,
			&srcRects[kMailboxLf], &srcRects[kMailboxLf], theRect);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  DrawMailboxRight

void DrawMailboxRight (Rect *theRect, SInt16 down)
{
	Rect		bounds;
	HDC			tempMap;
	HDC			tempMask;
	SInt32		darkGrayC, lightWoodC, darkWoodC;
	OSErr		theErr;

	if (thisMac.isDepth == 4)
	{
		darkGrayC = 13;
		lightWoodC = 9;
		darkWoodC = 11;
	}
	else
	{
		darkGrayC = k8DkGray2Color;
		lightWoodC = k8PissYellowColor;
		darkWoodC = k8BrownColor;
	}

	if (theRect->bottom < down + kMailboxBase)
	{
		ColorLine(backSrcMap, theRect->left + 34, theRect->bottom,
				theRect->left + 34, down + kMailboxBase, darkGrayC);
		ColorLine(backSrcMap, theRect->left + 35, theRect->bottom,
				theRect->left + 35, down + kMailboxBase + 1, lightWoodC);
		ColorLine(backSrcMap, theRect->left + 36, theRect->bottom,
				theRect->left + 36, down + kMailboxBase + 2, lightWoodC);
		ColorLine(backSrcMap, theRect->left + 37, theRect->bottom,
				theRect->left + 37, down + kMailboxBase + 3, lightWoodC);
		ColorLine(backSrcMap, theRect->left + 38, theRect->bottom,
				theRect->left + 38, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 39, theRect->bottom,
				theRect->left + 39, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 40, theRect->bottom,
				theRect->left + 40, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 41, theRect->bottom,
				theRect->left + 41, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 42, theRect->bottom,
				theRect->left + 42, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 43, theRect->bottom,
				theRect->left + 43, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 44, theRect->bottom,
				theRect->left + 44, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 45, theRect->bottom,
				theRect->left + 45, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 46, theRect->bottom,
				theRect->left + 46, down + kMailboxBase + 3, darkWoodC);
		ColorLine(backSrcMap, theRect->left + 47, theRect->bottom,
				theRect->left + 47, down + kMailboxBase + 3, darkGrayC);
	}

	bounds = srcRects[kMailboxRt];
	theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
	LoadGraphic(tempMap, kMailboxRightPictID);

	theErr = CreateOffScreenGWorld(&tempMask, &bounds, 1);
	LoadGraphic(tempMask, kMailboxRightMaskID);

	Mac_CopyMask(tempMap, tempMask, backSrcMap,
			&srcRects[kMailboxRt], &srcRects[kMailboxRt], theRect);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  DrawSimpleTransport

void DrawSimpleTransport (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(transSrcMap, transMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawInvisTransport

void DrawInvisTransport (Rect *theRect)
{
	ColorFrameRect(backSrcMap, theRect, 32);
}

//--------------------------------------------------------------  DrawLightSwitch

void DrawLightSwitch (Rect *theRect, Boolean state)
{
	if (state)
	{
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&lightSwitchSrc[0], theRect, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&lightSwitchSrc[1], theRect, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawMachineSwitch

void DrawMachineSwitch (Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&machineSwitchSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&machineSwitchSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawThermostat

void DrawThermostat (Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&thermostatSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&thermostatSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawPowerSwitch

void DrawPowerSwitch (Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&powerSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&powerSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawKnifeSwitch

void DrawKnifeSwitch (Rect *theRect, Boolean state)
{
	if (state)
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&knifeSwitchSrc[0], theRect, srcCopy, nil);
	else
		Mac_CopyBits(switchSrcMap, backSrcMap,
				&knifeSwitchSrc[1], theRect, srcCopy, nil);
}

//--------------------------------------------------------------  DrawInvisibleSwitch

void DrawInvisibleSwitch (Rect *theRect)
{
	ColorFrameRect(backSrcMap, theRect, kIntenseGreenColor);
}

//--------------------------------------------------------------  DrawTrigger

void DrawTrigger (Rect *theRect)
{
	ColorFrameRect(backSrcMap, theRect, kIntenseBlueColor);
}

//--------------------------------------------------------------  DrawSoundTrigger

void DrawSoundTrigger (Rect *theRect)
{
	ColorFrameRect(backSrcMap, theRect, kIntenseYellowColor);
}

//--------------------------------------------------------------  DrawSimpleLight

void DrawSimpleLight (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(lightSrcMap, lightMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawFlourescent

void DrawFlourescent (Rect *theRect)
{
	Rect		partRect;
	SInt32		grayC, gray2C, gray3C, gray4C, violetC;

	if (thisMac.isDepth == 4)
	{
		grayC = 7L;
		gray2C = 5L;
		gray3C = 4L;
		gray4C = 1L;
		violetC = 3L;
	}
	else
	{
		grayC = k8LtGrayColor;
		gray2C = k8LtstGray5Color;
		gray3C = k8LtstGray4Color;
		gray4C = k8LtstGrayColor;
		violetC = kPaleVioletColor;
	}

	ColorLine(backSrcMap, theRect->left + 16, theRect->top,
			theRect->right - 17, theRect->top, grayC);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 1,
			theRect->right - 17, theRect->top + 1, gray2C);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 2,
			theRect->right - 17, theRect->top + 2, gray2C);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 3,
			theRect->right - 17, theRect->top + 3, gray3C);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 4,
			theRect->right - 17, theRect->top + 4, gray4C);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 5,
			theRect->right - 17, theRect->top + 5, violetC);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 6,
			theRect->right - 17, theRect->top + 6, k8WhiteColor);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 7,
			theRect->right - 17, theRect->top + 7, k8WhiteColor);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 8,
			theRect->right - 17, theRect->top + 8, k8WhiteColor);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 9,
			theRect->right - 17, theRect->top + 9, k8WhiteColor);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 10,
			theRect->right - 17, theRect->top + 10, k8WhiteColor);
	ColorLine(backSrcMap, theRect->left + 16, theRect->top + 11,
			theRect->right - 17, theRect->top + 11, violetC);

	partRect = flourescentSrc1;
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, theRect->left, theRect->top);

	Mac_CopyMask(lightSrcMap, lightMaskMap, backSrcMap,
			&flourescentSrc1, &flourescentSrc1, &partRect);

	partRect = flourescentSrc2;
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, -partRect.right, 0);
	QOffsetRect(&partRect, theRect->right, theRect->top);

	Mac_CopyMask(lightSrcMap, lightMaskMap, backSrcMap,
			&flourescentSrc2, &flourescentSrc2, &partRect);
}

//--------------------------------------------------------------  DrawTrackLight

void DrawTrackLight (Rect *theRect)
{
	#define		kTrackLightSpacing	64
	Rect		partRect;
	SInt32		grayC, gray2C, gray3C, gray4C;
	SInt16		which, howMany, i, spread;

	if (thisMac.isDepth == 4)
	{
		grayC = 7L;
		gray2C = 8L;
		gray3C = 4L;
		gray4C = 11L;
	}
	else
	{
		grayC = k8LtGrayColor;
		gray2C = k8Gray2Color;
		gray3C = k8LtstGray4Color;
		gray4C = k8DkGrayColor;
	}

	ColorLine(backSrcMap, theRect->left, theRect->top - 3,
			theRect->right - 1, theRect->top - 3, gray2C);
	ColorLine(backSrcMap, theRect->left, theRect->top - 2,
			theRect->right - 1, theRect->top - 2, grayC);
	ColorLine(backSrcMap, theRect->left, theRect->top - 1,
			theRect->right - 1, theRect->top - 1, grayC);
	ColorLine(backSrcMap, theRect->left, theRect->top,
			theRect->right - 1, theRect->top, gray3C);
	ColorLine(backSrcMap, theRect->left, theRect->top + 1,
			theRect->right - 1, theRect->top + 1, gray4C);
	ColorLine(backSrcMap, theRect->left, theRect->top + 2,
			theRect->right - 1, theRect->top + 2, gray3C);

	partRect = trackLightSrc[0];			// left most track light
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, theRect->left, theRect->top);
	which = 0;
	Mac_CopyMask(lightSrcMap, lightMaskMap, backSrcMap,
			&trackLightSrc[which], &trackLightSrc[which], &partRect);

	partRect = trackLightSrc[0];			// right most track light
	ZeroRectCorner(&partRect);
	QOffsetRect(&partRect, -partRect.right, 0);
	QOffsetRect(&partRect, theRect->right, theRect->top);
	which = 2;
	Mac_CopyMask(lightSrcMap, lightMaskMap, backSrcMap,
			&trackLightSrc[which], &trackLightSrc[which], &partRect);

	howMany = ((RectWide(theRect) - RectWide(&trackLightSrc[0])) /
			kTrackLightSpacing) - 1;
	if (howMany > 0)
	{
		which = 0;
		spread = (RectWide(theRect) - RectWide(&trackLightSrc[0])) / (howMany + 1);
		for (i = 0; i < howMany; i++)
		{
			partRect = trackLightSrc[0];			// filler track lights
			ZeroRectCorner(&partRect);
			QOffsetRect(&partRect, theRect->left, theRect->top);
			QOffsetRect(&partRect, spread * (i + 1), 0);
			which++;
			if (which >= kNumTrackLights)
				which = 0;
			Mac_CopyMask(lightSrcMap, lightMaskMap, backSrcMap,
					&trackLightSrc[which], &trackLightSrc[which], &partRect);
		}
	}
}

//--------------------------------------------------------------  DrawInvisLight

void DrawInvisLight (Rect *theRect)
{
	ColorFrameOval(backSrcMap, theRect, 17);
}

//--------------------------------------------------------------  DrawSimpleAppliance

void DrawSimpleAppliance (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(applianceSrcMap, applianceMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawMacPlus

void DrawMacPlus (Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect		screen;

	if (isLit)
	{
		Mac_CopyMask(applianceSrcMap, applianceMaskMap, backSrcMap,
				&srcRects[kMacPlus], &srcRects[kMacPlus], theRect);
	}

	screen = plusScreen1;
	ZeroRectCorner(&screen);
	QOffsetRect(&screen, theRect->left + 10, theRect->top + 7);
	if (isOn)
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&plusScreen2, &screen, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&plusScreen1, &screen, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawTV

void DrawTV (Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect		bounds;
	HDC			tempMap;
	HDC			tempMask;
	OSErr		theErr;

	if (isLit)
	{
		bounds = srcRects[kTV];
		theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
		LoadGraphic(tempMap, kTVPictID);

		theErr = CreateOffScreenGWorld(&tempMask, &bounds, 1);
		LoadGraphic(tempMask, kTVMaskID);

		Mac_CopyMask(tempMap, tempMask, backSrcMap,
				&srcRects[kTV], &srcRects[kTV], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = tvScreen1;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 17, theRect->top + 10);
	if (isOn)
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&tvScreen2, &bounds, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&tvScreen1, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawCoffee

void DrawCoffee (Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect		light;

	if (isLit)
	{
		Mac_CopyMask(applianceSrcMap, applianceMaskMap, backSrcMap,
				&srcRects[kCoffee], &srcRects[kCoffee], theRect);
	}

	light = coffeeLight1;
	ZeroRectCorner(&light);
	QOffsetRect(&light, theRect->left + 32, theRect->top + 57);
	if (isOn)
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&coffeeLight2, &light, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&coffeeLight1, &light, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawOutlet

void DrawOutlet (Rect *theRect)
{
	Mac_CopyMask(applianceSrcMap, applianceMaskMap, backSrcMap,
			&srcRects[kOutlet], &srcRects[kOutlet], theRect);
}

//--------------------------------------------------------------  DrawVCR

void DrawVCR (Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect		bounds;
	HDC			tempMap;
	HDC			tempMask;
	OSErr		theErr;

	if (isLit)
	{
		bounds = srcRects[kVCR];
		theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
		LoadGraphic(tempMap, kVCRPictID);

		theErr = CreateOffScreenGWorld(&tempMask, &bounds, 1);
		LoadGraphic(tempMask, kVCRMaskID);

		Mac_CopyMask(tempMap, tempMask, backSrcMap,
				&srcRects[kVCR], &srcRects[kVCR], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = vcrTime1;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 64, theRect->top + 6);
	if (isOn)
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&vcrTime2, &bounds, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&vcrTime1, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawStereo

void DrawStereo (Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect		bounds;
	HDC			tempMap;
	HDC			tempMask;
	OSErr		theErr;

	if (isLit)
	{
		bounds = srcRects[kStereo];
		theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth); 
		LoadGraphic(tempMap, kStereoPictID);

		theErr = CreateOffScreenGWorld(&tempMask, &bounds, 1);
		LoadGraphic(tempMask, kStereoMaskID);

		Mac_CopyMask(tempMap, tempMask, backSrcMap,
				&srcRects[kStereo], &srcRects[kStereo], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = stereoLight1;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 56, theRect->top + 20);
	if (isOn)
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&stereoLight2, &bounds, srcCopy, nil);
	}
	else
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&stereoLight1, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawMicrowave

void DrawMicrowave (Rect *theRect, Boolean isOn, Boolean isLit)
{
	Rect		bounds;
	HDC			tempMap;
	HDC			tempMask;
	OSErr		theErr;

	if (isLit)
	{
		bounds = srcRects[kMicrowave];
		theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
		LoadGraphic(tempMap, kMicrowavePictID);

		theErr = CreateOffScreenGWorld(&tempMask, &bounds, 1);
		LoadGraphic(tempMask, kMicrowaveMaskID);

		Mac_CopyMask(tempMap, tempMask, backSrcMap,
				&srcRects[kMicrowave], &srcRects[kMicrowave], theRect);

		DisposeGWorld(tempMap);
		DisposeGWorld(tempMask);
	}

	bounds = microOn;
	ZeroRectCorner(&bounds);
	QOffsetRect(&bounds, theRect->left + 14, theRect->top + 13);
	if (isOn)
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&microOn, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&microOn, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&microOn, &bounds, srcCopy, nil);
	}
	else if (isLit)
	{
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&microOff, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&microOff, &bounds, srcCopy, nil);
		QOffsetRect(&bounds, 16, 0);
		Mac_CopyBits(applianceSrcMap, backSrcMap,
				&microOff, &bounds, srcCopy, nil);
	}
}

//--------------------------------------------------------------  DrawBalloon

void DrawBalloon (Rect *theRect)
{
	Mac_CopyMask(balloonSrcMap, balloonMaskMap, backSrcMap,
			&balloonSrc[1], &balloonSrc[1], theRect);
}

//--------------------------------------------------------------  DrawCopter

void DrawCopter (Rect *theRect)
{
	Mac_CopyMask(copterSrcMap, copterMaskMap, backSrcMap,
			&copterSrc[1], &copterSrc[1], theRect);
}

//--------------------------------------------------------------  DrawDart

void DrawDart (Rect *theRect, SInt16 which)
{
	if (which == kDartLf)
	{
		Mac_CopyMask(dartSrcMap, dartMaskMap, backSrcMap,
				&dartSrc[0], &dartSrc[0], theRect);
	}
	else
	{
		Mac_CopyMask(dartSrcMap, dartMaskMap, backSrcMap,
				&dartSrc[2], &dartSrc[2], theRect);
	}
}

//--------------------------------------------------------------  DrawBall

void DrawBall (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(ballSrcMap, ballMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawFish

void DrawFish (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(enemySrcMap, enemyMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawDrip

void DrawDrip (Rect *theRect)
{
	Mac_CopyMask(dripSrcMap, dripMaskMap, backSrcMap,
			&dripSrc[3], &dripSrc[3], theRect);
}

//--------------------------------------------------------------  DrawMirror

void DrawMirror (Rect *mirror)
{
	Rect		tempRect;
	SInt32		grayC;

	if (thisMac.isDepth == 4)
	{
		grayC = 13;
	}
	else
	{
		grayC = k8DkGray2Color;
	}

	tempRect = *mirror;
	ColorRect(backSrcMap, &tempRect, k8WhiteColor);
	ColorFrameRect(backSrcMap, &tempRect, grayC);
	Mac_InsetRect(&tempRect, 1, 1);
	ColorFrameRect(backSrcMap, &tempRect, k8EarthBlueColor);
	Mac_InsetRect(&tempRect, 1, 1);
	ColorFrameRect(backSrcMap, &tempRect, k8EarthBlueColor);
	Mac_InsetRect(&tempRect, 1, 1);
	ColorFrameRect(backSrcMap, &tempRect, grayC);
}

//--------------------------------------------------------------  DrawSimpleClutter

void DrawSimpleClutter (SInt16 what, Rect *theRect)
{
	Mac_CopyMask(clutterSrcMap, clutterMaskMap, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);
}

//--------------------------------------------------------------  DrawFlower

void DrawFlower (Rect *theRect, SInt16 which)
{
	Mac_CopyMask(clutterSrcMap, clutterMaskMap, backSrcMap,
			&flowerSrc[which], &flowerSrc[which], theRect);
}

//--------------------------------------------------------------  DrawWallWindow

void DrawWallWindow (Rect *window)
{
	#define		kWindowSillThick	7
	Rect		tempRect, tempRect2;
	SInt32		brownC, tanC, dkstRedC;
	SInt16		halfWay;

	if (thisMac.isDepth == 4)
	{
		brownC = 11;
		tanC = 9;
		dkstRedC = 15;
	}
	else
	{
		brownC = k8BrownColor;
		tanC = k8TanColor;
		dkstRedC = k8DkRed2Color;
	}

	tempRect = *window;
	Mac_InsetRect(&tempRect, 3, 0);
	ColorRect(backSrcMap, &tempRect, brownC);
	HiliteRect(backSrcMap, &tempRect, tanC, dkstRedC);

	tempRect = *window;			// top sill
	tempRect.bottom = tempRect.top + kWindowSillThick;
	tempRect.left++;
	tempRect.right--;
	ColorRect(backSrcMap, &tempRect, brownC);
	HiliteRect(backSrcMap, &tempRect, tanC, dkstRedC);
	tempRect.left--;
	tempRect.right++;
	tempRect.top += 2;
	tempRect.bottom -= 2;
	ColorRect(backSrcMap, &tempRect, brownC);
	HiliteRect(backSrcMap, &tempRect, tanC, dkstRedC);

	tempRect = *window;			// bottom sill
	tempRect.top = tempRect.bottom - kWindowSillThick;
	QOffsetRect(&tempRect, 0, -4);
	tempRect.left++;
	tempRect.right--;
	ColorRect(backSrcMap, &tempRect, brownC);
	HiliteRect(backSrcMap, &tempRect, tanC, dkstRedC);
	tempRect.left--;
	tempRect.right++;
	tempRect.top += 2;
	tempRect.bottom -= 2;
	ColorRect(backSrcMap, &tempRect, brownC);
	HiliteRect(backSrcMap, &tempRect, tanC, dkstRedC);

	tempRect = *window;			// inside frame
	tempRect.left += 8;
	tempRect.right -= 8;
	tempRect.top += 11;
	tempRect.bottom -= 15;
	HiliteRect(backSrcMap, &tempRect, dkstRedC, tanC);

	halfWay = (tempRect.top + tempRect.bottom) / 2;

	tempRect2 = tempRect;		// top pane
	tempRect2.bottom = halfWay + 2;
	Mac_InsetRect(&tempRect2, 5, 5);
	HiliteRect(backSrcMap, &tempRect2, dkstRedC, tanC);
	Mac_InsetRect(&tempRect2, 1, 1);
	if (thisMac.isDepth == 4)
		ColorRect(backSrcMap, &tempRect2, 5);
	else
		ColorRect(backSrcMap, &tempRect2, k8SkyColor);

	tempRect2 = tempRect;		// bottom pane
	tempRect2.top = halfWay - 3;
	Mac_InsetRect(&tempRect2, 5, 5);
	HiliteRect(backSrcMap, &tempRect2, dkstRedC, tanC);
	Mac_InsetRect(&tempRect2, 1, 1);
	if (thisMac.isDepth == 4)
		ColorRect(backSrcMap, &tempRect2, 5);
	else
		ColorRect(backSrcMap, &tempRect2, k8SkyColor);

	ColorLine(backSrcMap, tempRect2.left - 5, tempRect2.top - 7,
			tempRect2.right + 5, tempRect2.top - 7, tanC);
}

//--------------------------------------------------------------  DrawCalendar

void DrawCalendar (Rect *theRect)
{
	SYSTEMTIME	localTime;
	Rect		bounds;
	HBITMAP		thePicture;
	BITMAP		bmInfo;
	WCHAR		monthStr[256];
	INT			monthLen;
	LOGFONT		lfCalendar;
	HFONT		theFont;

	thePicture = GetPicture(kCalendarPictID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);

	GetObject(thePicture, sizeof(bmInfo), &bmInfo);
	QSetRect(&bounds, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
	QOffsetRect(&bounds, theRect->left, theRect->top);
	Mac_DrawPicture(backSrcMap, thePicture, &bounds);
	DeleteObject(thePicture);

	lfCalendar.lfHeight = -9;
	lfCalendar.lfWidth = 0;
	lfCalendar.lfEscapement = 0;
	lfCalendar.lfOrientation = 0;
	lfCalendar.lfWeight = FW_BOLD;
	lfCalendar.lfItalic = FALSE;
	lfCalendar.lfUnderline = FALSE;
	lfCalendar.lfStrikeOut = FALSE;
	lfCalendar.lfCharSet = DEFAULT_CHARSET;
	lfCalendar.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lfCalendar.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lfCalendar.lfQuality = DEFAULT_QUALITY;
	lfCalendar.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy(lfCalendar.lfFaceName, L"Tahoma");

	GetLocalTime(&localTime);
	monthLen = LoadString(HINST_THISCOMPONENT,
			kMonthStringBase + localTime.wMonth,
			monthStr, ARRAYSIZE(monthStr));

	SaveDC(backSrcMap);
	SetBkMode(backSrcMap, TRANSPARENT);
	SetTextAlign(backSrcMap, TA_CENTER | TA_BASELINE);
	SetTextColor(backSrcMap, Index2ColorRef(kDarkFleshColor));
	theFont = CreateFontIndirect(&lfCalendar);
	SelectObject(backSrcMap, theFont);
	TextOut(backSrcMap, theRect->left + 32, theRect->top + 55, monthStr, monthLen);
	RestoreDC(backSrcMap, -1);
	DeleteObject(theFont);
}

//--------------------------------------------------------------  DrawBulletin

void DrawBulletin (Rect *theRect)
{
	Rect		bounds;
	HBITMAP		thePicture;
	BITMAP		bmInfo;

	thePicture = GetPicture(kBulletinPictID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);

	GetObject(thePicture, sizeof(bmInfo), &bmInfo);
	QSetRect(&bounds, 0, 0, (SInt16)bmInfo.bmWidth, (SInt16)bmInfo.bmHeight);
	QOffsetRect(&bounds, theRect->left, theRect->top);
	Mac_DrawPicture(backSrcMap, thePicture, &bounds);
	DeleteObject(thePicture);
}

//--------------------------------------------------------------  DrawPictObject

void DrawPictObject (SInt16 what, Rect *theRect)
{
	Rect		bounds;
	HBITMAP		thePicture;
	SInt16		pictID;

	switch (what)
	{
		case kFilingCabinet:
		pictID = kFilingCabinetPictID;
		break;

		case kDownStairs:
		pictID = kDownStairsPictID;
		break;

		case kDoorExRt:
		pictID = kDoorExRightPictID;
		break;

		case kDoorExLf:
		pictID = kDoorExLeftPictID;
		break;

		case kWindowExRt:
		pictID = kWindowExRightPictID;
		break;

		case kWindowExLf:
		pictID = kWindowExLeftPictID;
		break;

		case kOzma:
		pictID = kOzmaPictID;
		break;
	}

	thePicture = GetPicture(pictID);
	if (thePicture == NULL)
		RedAlert(kErrFailedGraphicLoad);

	bounds = srcRects[what];
	QOffsetRect(&bounds, theRect->left, theRect->top);
	Mac_DrawPicture(backSrcMap, thePicture, &bounds);
	DeleteObject(thePicture);
}

//--------------------------------------------------------------  DrawPictWithMaskObject

void DrawPictWithMaskObject (SInt16 what, Rect *theRect)
{
	Rect		bounds;
	HDC			tempMap;
	HDC			tempMask;
	SInt16		pictID, maskID;
	OSErr		theErr;

	switch (what)
	{
		case kCobweb:
		pictID = kCobwebPictID;
		maskID = kCobwebMaskID;
		break;

		case kCloud:
		pictID = kCloudPictID;
		maskID = kCloudMaskID;
		break;
	}

	bounds = srcRects[what];
	theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
	LoadGraphic(tempMap, pictID);

	theErr = CreateOffScreenGWorld(&tempMask, &bounds, 1);
	LoadGraphic(tempMask, maskID);

	Mac_CopyMask(tempMap, tempMask, backSrcMap,
			&srcRects[what], &srcRects[what], theRect);

	DisposeGWorld(tempMap);
	DisposeGWorld(tempMask);
}

//--------------------------------------------------------------  DrawPictSansWhiteObject

void DrawPictSansWhiteObject (SInt16 what, Rect *theRect)
{
	Rect		bounds;
	HDC			tempMap;
	SInt16		pictID;
	OSErr		theErr;

	switch (what)
	{
		case kBBQ:
		pictID = kBBQPictID;
		break;

		case kTrunk:
		pictID = kTrunkPictID;
		break;

		case kManhole:
		pictID = kManholePictID;
		break;

		case kBooks:
		pictID = kBooksPictID;
		break;

		case kUpStairs:
		pictID = kUpStairsPictID;
		break;

		case kDoorInLf:
		pictID = kDoorInLeftPictID;
		break;

		case kDoorInRt:
		pictID = kDoorInRightPictID;
		break;

		case kWindowInLf:
		pictID = kWindowInLeftPictID;
		break;

		case kWindowInRt:
		pictID = kWindowInRightPictID;
		break;

		case kHipLamp:
		pictID = kHipLampPictID;
		break;

		case kDecoLamp:
		pictID = kDecoLampPictID;
		break;

		case kGuitar:
		pictID = kGuitarPictID;
		break;

		case kCinderBlock:
		pictID = kCinderPictID;
		break;

		case kFlowerBox:
		pictID = kFlowerBoxPictID;
		break;

		case kFireplace:
		pictID = kFireplacePictID;
		break;

		case kBear:
		pictID = kBearPictID;
		break;

		case kVase1:
		pictID = kVase1PictID;
		break;

		case kVase2:
		pictID = kVase2PictID;
		break;

		case kRug:
		pictID = kRugPictID;
		break;

		case kChimes:
		pictID = kChimesPictID;
		break;
	}

	bounds = srcRects[what];
	theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
	LoadGraphic(tempMap, pictID);

	Mac_CopyBits(tempMap, backSrcMap,
			&srcRects[what], theRect, transparent, nil);

	DisposeGWorld(tempMap);
}
//--------------------------------------------------------------  DrawCustPictSansWhite

void DrawCustPictSansWhite (SInt16 pictID, Rect *theRect)
{
	Rect		bounds;
	HDC			tempMap;
	OSErr		theErr;

	bounds = *theRect;
	ZeroRectCorner(&bounds);
	theErr = CreateOffScreenGWorld(&tempMap, &bounds, kPreferredDepth);
	LoadGraphic(tempMap, pictID);

	Mac_CopyBits(tempMap, backSrcMap,
			&bounds, theRect, transparent, nil);

	DisposeGWorld(tempMap);
}

