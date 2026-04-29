#ifndef CRCONTROLCODE_H
#define CRCONTROLCODE_H

#include "CrTypes.h"

namespace SCRSDK
{

enum CrControlCode : CrInt32u
{
	CrControlCode_Undefined                         = 0x00000000,
	CrControlCode_S1AndS2Button                     = 0x0000D2E6,
	CrControlCode_Release                           = 0x00010001,
	CrControlCode_MovieRecButton                    = 0x0000D2C8,
	CrControlCode_MovieRecButtonToggle              = 0x0000F001,
	CrControlCode_MovieRecButtonToggle2             = 0x0000D2FE,
	CrControlCode_SelectedMediaFormat               = 0x0000D2E2,
	CrControlCode_CancelMediaFormat                 = 0x0000D2E7,
	CrControlCode_RECSettingsReset                  = 0x0000D2F3,
	CrControlCode_APS_C_or_Full_Switching           = 0x0000D2EB,
	CrControlCode_CancelRemoteTouchOperation        = 0x0000D2E5,
	CrControlCode_PixelMapping                      = 0x0000D300,
	CrControlCode_TimeCodePresetReset               = 0x0000D302,
	CrControlCode_UserBitPresetReset                = 0x0000D303,
	CrControlCode_SensorCleaning                    = 0x0000D304,
	CrControlCode_ResetPictureProfile               = 0x0000D305,
	CrControlCode_ResetCreativeLook                 = 0x0000D306,
	CrControlCode_StreamButton                      = 0x0000D307,
	CrControlCode_FlickerScan                       = 0x0000D2F1,
	CrControlCode_ContinuousShootingSpotBoostButton = 0x0000D2F6,
	CrControlCode_TrackingOnAndAFOnButton           = 0x0000D30D,
	CrControlCode_ForcedFileNumberReset             = 0x0000D30E,
	CrControlCode_CameraStandBy                     = 0x0000D315,
	CrControlCode_PowerOff                          = 0x0000D301,
	CrControlCode_PowerOn                           = 0x0000D316,
	CrControlCode_RemoteKeyUp                       = 0x0000D2CD,
	CrControlCode_RemoteKeyDown                     = 0x0000D2CE,
	CrControlCode_RemoteKeyLeft                     = 0x0000D2CF,
	CrControlCode_RemoteKeyRight                    = 0x0000D2D0,
	CrControlCode_RemoteKeyCancelBackButton         = 0x0000D2F7,
	CrControlCode_RemoteKeyDisplayButton            = 0x0000D2F8,
	CrControlCode_RemoteKeySet                      = 0x0000D2F9,
	CrControlCode_RemoteKeyRightUp                  = 0x0000D2FA,
	CrControlCode_RemoteKeyRightDown                = 0x0000D2FB,
	CrControlCode_RemoteKeyLeftUp                   = 0x0000D2FC,
	CrControlCode_RemoteKeyLeftDown                 = 0x0000D2FD,
	CrControlCode_RemoteKeyMenuButton               = 0x0000D2FF,
	CrControlCode_ResetMultiMatrix                  = 0x0000D2EE,
	CrControlCode_CancelFocusPosition               = 0x0000F002,
	CrControlCode_CancelZoomPosition                = 0x0000F00C,
	CrControlCode_CancelContentsTransfer            = 0x00020002,
	CrControlCode_S1Button                          = 0x0000D2C1,
	CrControlCode_S2Button                          = 0x0000D2C2,
	CrControlCode_AELButton                         = 0x0000D2C3,
	CrControlCode_FELButton                         = 0x0000D2C9,
	CrControlCode_AWBLButton                        = 0x0000D2D9,
	CrControlCode_NearFar                           = 0x0000D2D1,
	CrControlCode_AFAreaPosition                    = 0x0000D2DC,
	CrControlCode_ZoomOperation                     = 0x0000D2DD,
	CrControlCode_CustomWBCaptureStandby            = 0x0000D2DF,
	CrControlCode_CustomWBCaptureStandbyCancel      = 0x0000D2E0,
	CrControlCode_CustomWBCapture                   = 0x0000D2E1,
	CrControlCode_HighResolutionShutterSpeedAdjust  = 0x0000D2E3,
	CrControlCode_HighResolutionShutterSpeedAdjustInIntegralMultiples = 0x0000D2F0,
	CrControlCode_FocusOperation                    = 0x0000D2EF,
	CrControlCode_RemoteTouchOperation              = 0x0000D2E4,
	CrControlCode_SaveZoomAndFocusPosition          = 0x0000D2E9,
	CrControlCode_LoadZoomAndFocusPosition          = 0x0000D2EA,
	CrControlCode_ColorTemperatureStep              = 0x0000D2EC,
	CrControlCode_WhiteBalanceTintStep              = 0x0000D2ED,
	CrControlCode_SetPresetInfoZoomOnlyValue        = 0x0000D2F2,
	CrControlCode_CameraButtonFunction              = 0x0000D309,
	CrControlCode_CameraButtonFunctionMulti         = 0x0000D30A,
	CrControlCode_CameraDialFunction                = 0x0000D30B,
	CrControlCode_CameraLeverFunction               = 0x0000D30C,
	CrControlCode_CreateNewFolder                   = 0x0000D314,
	CrControlCode_ShutterECSNumberStep              = 0x0000F000,
	CrControlCode_ZoomOperationWithInt16            = 0x0000F003,
	CrControlCode_FocusOperationWithInt16           = 0x0000F004,
	CrControlCode_PresetPTZFRecall                  = 0x0000F015,
	CrControlCode_USBConnectionModeRequest          = 0x0000F012,
};

// CrDataType_Button
enum CrControlButton : CrInt16u
{
	CrControlButton_Up = 0x0001,
	CrControlButton_Down = 0x0002,
};

// Media Format
//   Multiple items cannot be selected.
enum CrMediaFormatValue : CrInt16u
{
	CrMediaFormat_FullFormatSlot1 = 0x0001,
	CrMediaFormat_FullFormatSlot2 = 0x0002,
	CrMediaFormat_QuickFormatSlot1 = 0x0011,
	CrMediaFormat_QuickFormatSlot2 = 0x0012,
};

// USB Connection Mode Request
enum CrUSBConnectionModeRequest : CrInt16u
{
	CrUSBConnectionModeRequest_MSC = 0x0001,
};

enum CrStreamButton : CrInt16u
{
	CrStreamButton_StopStreaming = 0x0001,
	CrStreamButton_StartStreaming = 0x0002,
};

class SCRSDK_API CrControlCodeInfo
{
public:
	CrControlCodeInfo();
	~CrControlCodeInfo();
	CrControlCodeInfo(const CrControlCodeInfo& ref);
	CrControlCodeInfo& operator =(const CrControlCodeInfo& ref);
	CrControlCode GetCode() const;
	CrDataType GetValueType() const;
	CrInt32u GetValueSize() const;
	CrInt8u* GetValues() const;

private:
	CrControlCode           code;
	CrInt32u                reserved;
	CrDataType              valueType;
	CrInt32u                valuesSize;
	CrInt8u*                values;
};

}

#endif // CRCONTROLCODE_H
