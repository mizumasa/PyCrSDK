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


def main():
    ok = cam.sdk_init()
    print("sdk_init:", ok)
    if not ok:
        return

    try:
        count = cam.enumerate_cameras()
        print("detected cameras:", count)
        if count <= 0:
            print("no camera found")
            return

        if not cam.connect_camera(0, 0):
            print("connect failed")
            return

        time.sleep(1.0)

        if not cam.set_camera_eframing(0, True):
            print("failed to enable camera eframing")
            return

        # Instant framing move to the specified rectangle (Single).
        ok = cam.execute_eframing(
            0,
            10, 10, 40, 40,
            10, 10, 40, 40,
            100, 100,
            "Single",
            1,
            -1,
        )
        print("execute_eframing instant:", ok)
        time.sleep(2.0)

        # Smooth framing move to another rectangle (PTZ + speed).
        ok = cam.execute_eframing(
            0,
            25, 20, 35, 35,
            25, 20, 35, 35,
            100, 100,
            "Single",
            1,
            10,
        )
        print("execute_eframing smooth:", ok)
        time.sleep(2.0)

        # Relative move/resize after ExecuteEframing succeeded.
        ok = cam.update_eframing_area(0, 1, "out", 2, 0, -2, -2)
        print("update_eframing_area:", ok)
        time.sleep(1.0)

        cam.disconnect_camera(0)
    finally:
        cam.sdk_release()


if __name__ == "__main__":
    main()
