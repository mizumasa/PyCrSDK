import io
import cv2
import numpy as np
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
    cam.connect_camera(0,2)
    time.sleep(2)
    iso_value = cam.get_iso(0)
    print("ISO value:", iso_value)
    time.sleep(2)
    cam.execute_movie_rec(0, True)  # 開始
    time.sleep(4)
    cam.execute_movie_rec(0, False)  # 停止
    time.sleep(4)
    #cam.download_latest_files(0, 0, 1, 0)
    cam.disconnect_camera(0)
    time.sleep(5)
    iso_value = cam.get_iso(0)
    print("ISO value:", iso_value)
    time.sleep(5)
    print("Connecting to the first camera again...")
    cam.connect_camera(0,2)
    time.sleep(5)
    iso_value = cam.get_iso(0)
    print("ISO value:", iso_value)
    time.sleep(5)
    cam.disconnect_camera(0)
    
# 6) SDK解放
cam.sdk_release()
