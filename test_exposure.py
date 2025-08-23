import io
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
    time.sleep(2)
    print("ISO value:", cam.get_iso(0))
    print("aperture value:", cam.get_aperture(0))
    print("shutter speed value:", cam.get_shutter_speed(0))
    print("extended shutter speed value:", cam.get_extended_shutter_speed(0))
    time.sleep(2)
    cam.print_iso(0)
    cam.print_aperture(0)
    cam.print_shutter_speed(0)
    cam.print_extended_shutter_speed(0)
    time.sleep(2)
    cam.set_iso(0, 1)  # Example ISO value
    time.sleep(2)
    cam.set_iso(0, 2)  # Example ISO value
    time.sleep(2)
    cam.set_shutter_speed(0, 3)  # Example shutter speed value
    time.sleep(2)
    cam.set_shutter_speed(0, 4)  # Example shutter speed value
    time.sleep(2)
    cam.set_iso(0, 1000000)  # Error case
    time.sleep(2)
    cam.set_shutter_speed(0, 1000000)  # Error case
    cam.disconnect_camera(0)
    
# 6) SDK解放
cam.sdk_release()
