import sys
import time
import MyPath

try:
    import PyCrSDK as pycr
    print("installer environment detected.")
except ImportError:
    import pycrsdk as pycr
    print("development environment detected.")


CAM_NO = 0
CONNECT_MODE = 0
POLL_INTERVAL_SEC = 0.2
MOVE_TIMEOUT_SEC = 5.0


def wait_zoom_stop(cam, no, timeout_sec=MOVE_TIMEOUT_SEC):
    start = time.time()
    while True:
        status = cam.get_zoom_driving_status(no)
        if status == -1:
            # Driving status unsupported. Treat as best-effort completion.
            return True
        if status == 0:
            return True
        if (time.time() - start) > timeout_sec:
            return False
        time.sleep(POLL_INTERVAL_SEC)


def move_abs_and_wait(cam, no, target):
    if not cam.set_zoom_abs_position(no, int(target)):
        return False
    return wait_zoom_stop(cam, no)


def run_zoom_move_sequence(cam, no):
    abs_min, abs_max, abs_step = cam.get_zoom_abs_position_range(no)
    print(f"zoom abs range: min={abs_min}, max={abs_max}, step={abs_step}")

    if abs_min >= 0 and abs_max >= 0 and abs_step > 0:
        print("moving to tele end (absolute max)...")
        if not move_abs_and_wait(cam, no, abs_max):
            print("tele absolute move timeout/failure")

        cur = cam.get_zoom_abs_position_current(no)
        print(f"at tele end current={cur}")

        print("moving to wide end (absolute min)...")
        if not move_abs_and_wait(cam, no, abs_min):
            print("wide absolute move timeout/failure")
            return False

        cur = cam.get_zoom_abs_position_current(no)
        print(f"at wide end current={cur}")
        return True

    print("absolute zoom is unsupported, fallback to relative move")
    min_speed, max_speed, step = cam.get_zoom_speed_range(no)
    print(f"zoom speed range: min={min_speed}, max={max_speed}, step={step}")

    tele_speed = max_speed if max_speed > 0 else 1
    wide_speed = min_speed if min_speed < 0 else -1

    print(f"relative tele start: speed={tele_speed}")
    if not cam.zoom_start(no, tele_speed):
        print("zoom_start tele failed")
        return False
    time.sleep(3.0)
    cam.zoom_stop(no)
    time.sleep(0.5)

    print(f"relative wide start: speed={wide_speed}")
    if not cam.zoom_start(no, wide_speed):
        print("zoom_start wide failed")
        return False
    time.sleep(3.0)
    cam.zoom_stop(no)

    return True


def main():
    cam = pycr.CameraManager()

    ok = cam.sdk_init()
    print("sdk_init:", ok)
    if not ok:
        return 1

    try:
        time.sleep(1)
        cams = cam.enumerate_cameras()
        print("detected cameras:", cams)
        if not cams:
            print("no camera found")
            return 1

        print("connecting...")
        if not cam.connect_camera(CAM_NO, CONNECT_MODE):
            print("connect failed")
            return 1

        time.sleep(2)
        success = run_zoom_move_sequence(cam, CAM_NO)
        print("zoom move result:", success)

        print("disconnecting...")
        cam.disconnect_camera(CAM_NO)
        return 0 if success else 2

    finally:
        cam.sdk_release()


if __name__ == "__main__":
    raise SystemExit(main())
