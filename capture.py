import sys
sys.path.append("C:/masaru/CrSDK/PyCrSDK/build/Release")

import pycrsdk
import time
cam = pycrsdk.PyCrSDK()

# 1) SDK初期化
ok = cam.sdk_init()
print("sdk_init:", ok)

time.sleep(1)  # Wait for the SDK to initialize
# 2) カメラ列挙
cams = cam.enumerate_cameras()
print("Detected cameras:", cams)
if not cams:
    print("No camera found.")
else:
    # 3) 先頭のカメラに接続
    print("Connecting to the first camera...")
    if cam.connect_camera(0):
        time.sleep(3)  # Wait for the camera to connect
        # 4) シャッターを切る
        print("getting ISO value...")
        iso_value = cam.get_iso(0)
        print("ISO value:", iso_value)

        success = cam.capture_image(0)
        print("Capture result:", success)

        # 5) 切断
        cam.disconnect_camera()

# 6) SDK解放
cam.sdk_release()
