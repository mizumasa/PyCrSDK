import argparse
import sys
import time

import MyPath

try:
    import PyCrSDK as pycr
    print("installer environment detected.")
except ImportError:
    import pycrsdk as pycr
    print("development environment detected.")


DEFAULT_CAMERA_NO = 0
DEFAULT_CONNECT_MODE = 0
DEFAULT_SETTLE_SEC = 2.0
DEFAULT_TIMEOUT_SEC = 8.0
DEFAULT_POLL_SEC = 0.2


def create_camera_manager():
    return pycr.CameraManager()


def add_common_arguments(parser, include_timeout=False):
    parser.add_argument("--camera", type=int, default=DEFAULT_CAMERA_NO, help="camera index")
    parser.add_argument("--mode", type=int, default=DEFAULT_CONNECT_MODE, help="connect mode")
    parser.add_argument("--settle", type=float, default=DEFAULT_SETTLE_SEC, help="wait after connect")
    if include_timeout:
        parser.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT_SEC, help="operation timeout")
    return parser


def open_camera(camera_no=DEFAULT_CAMERA_NO, connect_mode=DEFAULT_CONNECT_MODE, settle_sec=DEFAULT_SETTLE_SEC):
    cam = create_camera_manager()

    if not cam.sdk_init():
        raise RuntimeError("sdk_init failed")

    time.sleep(1.0)
    camera_count = cam.enumerate_cameras()
    print(f"detected cameras: {camera_count}")
    if not camera_count:
        cam.sdk_release()
        raise RuntimeError("no camera found")

    if not cam.connect_camera(camera_no, connect_mode):
        cam.sdk_release()
        raise RuntimeError(f"connect failed: camera={camera_no}, mode={connect_mode}")

    time.sleep(settle_sec)
    return cam


def close_camera(cam, camera_no=DEFAULT_CAMERA_NO):
    if cam is None:
        return

    try:
        try:
            cam.disconnect_camera(camera_no)
        except Exception as exc:
            print(f"disconnect warning: {exc}")
    finally:
        cam.sdk_release()


def ensure_methods(cam, methods):
    missing = [name for name in methods if not hasattr(cam, name)]
    if missing:
        print("missing methods in current build:")
        for name in missing:
            print(f"  - {name}")
        print("build the focus API implementation first, then rerun this sample.")
        return False
    return True


def wait_until(predicate, timeout_sec=DEFAULT_TIMEOUT_SEC, poll_sec=DEFAULT_POLL_SEC):
    started = time.time()
    while True:
        if predicate():
            return True
        if (time.time() - started) > timeout_sec:
            return False
        time.sleep(poll_sec)


def wait_focus_stop(cam, camera_no, timeout_sec=DEFAULT_TIMEOUT_SEC, poll_sec=DEFAULT_POLL_SEC):
    if not hasattr(cam, "get_focus_driving_status"):
        return True

    def stopped():
        status = cam.get_focus_driving_status(camera_no)
        print(f"focus driving status: {status}")
        return status in (-1, 0)

    return wait_until(stopped, timeout_sec=timeout_sec, poll_sec=poll_sec)


def align_to_step(value, minimum, step):
    if step <= 0:
        return int(value)
    offset = max(0, int(value) - int(minimum))
    snapped = int(minimum) + (offset // int(step)) * int(step)
    return snapped


def build_parser(description, include_timeout=False):
    parser = argparse.ArgumentParser(description=description)
    return add_common_arguments(parser, include_timeout=include_timeout)


def optional_int(value):
    if value is None:
        return None
    return int(value)


def print_section(title):
    print()
    print(f"=== {title} ===")
