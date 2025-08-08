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
    cam.connect_camera(0,0)
    time.sleep(3)
    #zoom operation demo
    print("zoom operation demo")

    print("get_zoom_current_position", cam.get_zoom_current_position(0))
    print("get_zoom_max_position", cam.get_zoom_max_position(0))
    print("get_zoom_min_position", cam.get_zoom_min_position(0))
    print("get_zoom_position_step", cam.get_zoom_position_step(0))
    print("get_zoom_max_speed", cam.get_zoom_max_speed(0))
    print("get_zoom_min_speed", cam.get_zoom_min_speed(0))

    # Set zoom speed
    zoom_speed = 3  # Example speed value
    if cam.set_zoom_speed(0, zoom_speed):
        print(f"Zoom speed set to {zoom_speed}")
    else:
        print("Failed to set zoom speed")

    time.sleep(2)

    # Set zoom speed
    zoom_speed = -3  # Example speed value
    if cam.set_zoom_speed(0, zoom_speed):
        print(f"Zoom speed set to {zoom_speed}")
    else:
        print("Failed to set zoom speed")

    time.sleep(2)

    zoom_speed = 0  # Stop zooming
    if cam.set_zoom_speed(0, zoom_speed):
        print(f"Zoom speed set to {zoom_speed}")
    else:
        print("Failed to set zoom speed")

    time.sleep(2)
    cam.disconnect_camera(0)
    
# 6) SDK解放
cam.sdk_release()
