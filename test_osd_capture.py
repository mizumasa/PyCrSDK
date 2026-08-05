import cv2
import numpy as np
import sys
import time

try:
    import PyCrSDK
    cam = PyCrSDK.CameraManager()
except ImportError:
    print("Development environment detected.")
    sys.path.append("C:/masaru/CrSDK/PyCrSDK/build/Release")
    import pycrsdk
    cam = pycrsdk.CameraManager()

# CrDevicePropertyCode
CrDeviceProperty_OSDImageMode = 0x02C5

# CrOSDImageMode values
CrOSDImageMode_Off = 0x00
CrOSDImageMode_On  = 0x01

# get_live_view selected_index
LiveViewOnly    = 0
LiveViewAndOSD  = 1

# 1) SDK初期化
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
    if cam.connect_camera(0, 0):
        time.sleep(3)  # Wait for camera to be ready

        # 4) OSDImageMode を On にセット（リトライあり）
        print("Enabling OSD image mode...")
        osd_enabled = False
        for attempt in range(5):
            set_result = cam.set_device_property(0, CrDeviceProperty_OSDImageMode, CrOSDImageMode_On)
            print(f"  set_device_property attempt {attempt + 1}: {set_result}")
            time.sleep(1)  # 反映待ち
            prop = cam.get_device_property(0, CrDeviceProperty_OSDImageMode)
            current = prop.get("current_value")
            print(f"  OSDImageMode current: 0x{current:02X} ({'On' if current == CrOSDImageMode_On else 'Off'})")
            if current == CrOSDImageMode_On:
                osd_enabled = True
                print("OSD image mode is ON.")
                break
        else:
            print("ERROR: Could not enable OSD image mode. Aborting.")
            cam.disconnect_camera(0)
            cam.sdk_release()
            sys.exit(1)

        img_size = 1024 * 1024  # 最大1MB想定
        buf = bytearray(img_size)

        # 5) OSD合成ライブビューをループ表示
        print("Starting OSD live view loop. Press ESC to exit.")
        while True:
            try:
                # camera_index=0, selected_index=1 → LiveViewAndOSD (OSD合成済み画像)
                cam.get_live_view(0, LiveViewAndOSD, buf)

                arr = np.frombuffer(buf, dtype=np.uint8)
                img = cv2.imdecode(arr, cv2.IMREAD_COLOR)
                if img is not None:
                    cv2.imshow("OSD LiveView", img)
                else:
                    print("Warning: Failed to decode image.")

                key = cv2.waitKey(1)
                if key == 27:  # ESCキーで終了
                    break

            except Exception as e:
                print("Error during OSD live view:", e)
                break

        cv2.destroyAllWindows()

        # 6) OSDImageMode を Off に戻す
        print("Restoring OSDImageMode to Off...")
        cam.set_device_property(0, CrDeviceProperty_OSDImageMode, CrOSDImageMode_Off)

        # 7) カメラ切断
        cam.disconnect_camera(0)

    # 8) SDK解放
    cam.sdk_release()
