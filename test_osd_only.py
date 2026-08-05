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
    import pycrsdk as PyCrSDK
    cam = PyCrSDK.CameraManager()

# CrDevicePropertyCode
CrDeviceProperty_OSDImageMode = 0x02C5

# CrOSDImageMode values
CrOSDImageMode_Off = 0x00
CrOSDImageMode_On  = 0x01

# CR_OSD_IMAGE_MAX_SIZE (SDK定義に合わせた最大サイズ)
OSD_BUF_SIZE = 1024 * 1024  # 1 MB

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
        time.sleep(3)  # カメラ準備待ち

        # 4) OSDImageMode を On にセット（リトライあり）
        print("Enabling OSD image mode...")
        osd_enabled = False
        for attempt in range(5):
            set_result = cam.set_device_property(0, CrDeviceProperty_OSDImageMode, CrOSDImageMode_On)
            print(f"  set_device_property attempt {attempt + 1}: {set_result}")
            time.sleep(1)
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

        buf = bytearray(OSD_BUF_SIZE)

        # 5) OSDプレーンのみをループ取得・描画
        print("Starting OSD-only display loop. Press ESC to exit.")
        while True:
            try:
                # get_osd: OSDプレーン画像のみをバッファに書き込む
                ok = cam.get_osd(0, buf)
                if ok:
                    arr = np.frombuffer(buf, dtype=np.uint8)
                    img = cv2.imdecode(arr, cv2.IMREAD_UNCHANGED)
                    if img is not None:
                        # アルファチャンネルがある場合はそのまま表示（BGRA）
                        cv2.imshow("OSD Only", img)
                    else:
                        print("Warning: Failed to decode OSD image.")
                else:
                    # OSD未更新または取得失敗時は前フレームを維持
                    pass

                key = cv2.waitKey(33)  # ~30 fps
                if key == 27:  # ESCで終了
                    break

            except Exception as e:
                print("Error during OSD capture:", e)
                break

        cv2.destroyAllWindows()

        # 6) OSDImageMode を Off に戻す
        print("Restoring OSDImageMode to Off...")
        cam.set_device_property(0, CrDeviceProperty_OSDImageMode, CrOSDImageMode_Off)

        # 7) カメラ切断
        cam.disconnect_camera(0)

    # 8) SDK解放
    cam.sdk_release()
