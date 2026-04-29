import sys
import time
import MyPath

try:
    import PyCrSDK
    cam = PyCrSDK.CameraManager()
except ImportError:
    print("Development environment detected.")
    import pycrsdk
    cam = pycrsdk.CameraManager()


ok = cam.sdk_init()
print("sdk_init:", ok)
if not ok:
    raise SystemExit(1)

try:
    time.sleep(1)
    cams = cam.enumerate_cameras()
    print("Detected cameras:", cams)

    if not cams:
        print("No camera found.")
        raise SystemExit(0)

    camera_no = 0
    print("Connecting to the first camera...")
    if not cam.connect_camera(camera_no, 0):
        print("Failed to connect camera.")
        raise SystemExit(1)

    time.sleep(2)

    print("Set Camera Eframing: ON")
    result_on = cam.set_camera_eframing(camera_no, True)
    print("set_camera_eframing(True):", result_on)

    time.sleep(2)

    print("Set Camera Eframing: OFF")
    result_off = cam.set_camera_eframing(camera_no, False)
    print("set_camera_eframing(False):", result_off)

    time.sleep(1)
    cam.disconnect_camera(camera_no)
finally:
    cam.sdk_release()
