import sys
import time

try:
    import PyCrSDK
    cam = PyCrSDK.CameraManager()
except ImportError:
    print("Development environment detected.")
    sys.path.append("C:/masaru/CrSDK/PyCrSDK/build/Release")
    import pycrsdk as PyCrSDK
    cam = PyCrSDK.CameraManager()

# CrDevicePropertyCode (CrDeviceProperty.h より算出)
CrDeviceProperty_OSDImageMode = 0x02C5

# CrOSDImageMode 列挙値
CrOSDImageMode_Off = 0x00
CrOSDImageMode_On  = 0x01

OSD_MODE_NAMES = {
    CrOSDImageMode_Off: "Off",
    CrOSDImageMode_On:  "On",
}


def describe_osd_mode(value: int) -> str:
    return OSD_MODE_NAMES.get(value, f"Unknown(0x{value:02X})")


# 1) SDK 初期化
ok = cam.sdk_init()
print("sdk_init:", ok)

time.sleep(1)

# 2) カメラ列挙
cams = cam.enumerate_cameras()
print("Detected cameras:", cams)

if not cams:
    print("No camera found.")
else:
    # 3) 先頭のカメラに接続
    print("Connecting to the first camera...")
    cam.connect_camera(0, 0)
    time.sleep(3)

    # 4) OSDImageMode を取得
    result = cam.get_device_property(0, CrDeviceProperty_OSDImageMode)
    print("get_device_property result:", result)

    current = result.get("current_value")
    writable = result.get("can_set")
    possible = result.get("possible_values", [])

    print(f"OSDImageMode current : 0x{current:02X} ({describe_osd_mode(current)})")
    print(f"OSDImageMode writable: {writable}")
    print("OSDImageMode possible values:")
    for v in possible:
        print(f"  0x{v:02X} -> {describe_osd_mode(v)}")

    # 5) OSDImageMode を On (0x01) にセット
    target_value = CrOSDImageMode_On
    print(f"\nSetting OSDImageMode to 0x{target_value:02X} ({describe_osd_mode(target_value)})...")
    set_result = cam.set_device_property(0, CrDeviceProperty_OSDImageMode, target_value)
    print("set_device_property result:", set_result)

    time.sleep(1)

    # 5-1) セット後の値を確認
    result2 = cam.get_device_property(0, CrDeviceProperty_OSDImageMode)
    current2 = result2.get("current_value")
    print(f"OSDImageMode after set: 0x{current2:02X} ({describe_osd_mode(current2)})")

    # 6) カメラ切断
    cam.disconnect_camera(0)

# 7) SDK 解放
cam.sdk_release()
