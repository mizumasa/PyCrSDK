"""
Live absolute focus position polling sample / 絶対フォーカス位置リアルタイム取得サンプル

This sample keeps reading the current absolute focus position from the camera at a
fixed interval and prints the value continuously. Use it while AF or MF is active to
observe how the reported absolute focus position changes over time.

このサンプルは、一定間隔でカメラの現在の絶対フォーカス位置を読み続け、
連続的に表示します。AF 中や MF 中に、報告される絶対フォーカス位置が時間とともに
どのように変化するかを観察する用途です。
"""

import time

from focus_sample_common import build_parser
from focus_sample_common import close_camera
from focus_sample_common import ensure_methods
from focus_sample_common import open_camera
from focus_sample_common import print_section


def main():
    parser = build_parser("Live absolute focus position polling sample")
    parser.add_argument("--interval", type=float, default=0.2, help="poll interval in seconds")
    parser.add_argument("--duration", type=float, default=10.0, help="total polling duration in seconds, <=0 means infinite")
    args = parser.parse_args()

    cam = None
    try:
        cam = open_camera(args.camera, args.mode, args.settle)
        if not ensure_methods(cam, ["get_focus_abs_position_current"]):
            return 2

        print_section("live absolute focus position")
        print("press Ctrl+C to stop")

        started = time.time()
        sample_index = 0
        while True:
            current_pos = cam.get_focus_abs_position_current(args.camera)
            elapsed = time.time() - started
            print(f"[{sample_index:04d}] elapsed={elapsed:6.2f}s, focus_abs_position={current_pos}")
            sample_index += 1

            if args.duration > 0 and elapsed >= args.duration:
                break
            time.sleep(max(0.01, args.interval))

        return 0
    except KeyboardInterrupt:
        print("stopped by user")
        return 0
    finally:
        close_camera(cam, args.camera)


if __name__ == "__main__":
    raise SystemExit(main())