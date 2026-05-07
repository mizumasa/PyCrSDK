"""
Absolute focus sample / 絶対フォーカス移動サンプル

This sample reads the supported absolute focus range, chooses a target position,
commands the camera to move focus to that position, and waits until the drive stops.
Use it to verify absolute focus positioning and focus driving status.

このサンプルは、絶対フォーカス位置の対応レンジを取得し、目標位置を決めて、
カメラにその位置への移動を指示し、駆動が停止するまで待機します。
絶対フォーカス位置指定とフォーカス駆動状態の確認に使います。
"""

import time

from focus_sample_common import align_to_step
from focus_sample_common import build_parser
from focus_sample_common import close_camera
from focus_sample_common import ensure_methods
from focus_sample_common import open_camera
from focus_sample_common import print_section
from focus_sample_common import wait_focus_stop


def main():
    parser = build_parser("Absolute focus sample", include_timeout=True)
    parser.add_argument("--target", type=int, default=None, help="absolute focus target")
    args = parser.parse_args()

    cam = None
    try:
        cam = open_camera(args.camera, args.mode, args.settle)
        if not ensure_methods(cam, [
            "get_focus_abs_position_current",
            "get_focus_abs_position_range",
            "set_focus_abs_position",
            "get_focus_driving_status",
        ]):
            return 2

        print_section("absolute focus range")
        min_pos, max_pos, step = cam.get_focus_abs_position_range(args.camera)
        cur_pos = cam.get_focus_abs_position_current(args.camera)
        print(f"current={cur_pos}, min={min_pos}, max={max_pos}, step={step}")

        if min_pos < 0 or max_pos < 0:
            print("absolute focus is unsupported on this camera")
            return 2

        if args.target is None:
            midpoint = min_pos + ((max_pos - min_pos) // 2)
            target = align_to_step(midpoint, min_pos, step)
            if target == cur_pos and step > 0:
                candidate = cur_pos + step
                if candidate <= max_pos:
                    target = candidate
        else:
            target = align_to_step(args.target, min_pos, step)

        print_section("move")
        print(f"moving focus to target={target}")
        if not cam.set_focus_abs_position(args.camera, int(target)):
            print("set_focus_abs_position failed")
            return 1

        completed = wait_focus_stop(cam, args.camera, timeout_sec=args.timeout)
        time.sleep(0.3)
        final_pos = cam.get_focus_abs_position_current(args.camera)
        print(f"completed={completed}, final_position={final_pos}")
        return 0 if completed else 1
    finally:
        close_camera(cam, args.camera)


if __name__ == "__main__":
    raise SystemExit(main())
