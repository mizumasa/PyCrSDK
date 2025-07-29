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
    if cam.connect_camera(0):
        time.sleep(3)  # Wait for the camera to connect
        # 4) シャッターを切る
        print("getting ISO value...")
        iso_value = cam.get_iso(0)
        print("ISO value:", iso_value)


        img_size = 1024*1024  # 例: 最大1MBと想定

        # Pythonでバッファを用意
        buf = bytearray(img_size)

        # C++関数を呼び出し、データを直接書き込む
        while 1:
            try:
                cam.get_live_view(0, buf)
                arr = np.frombuffer(buf, dtype=np.uint8)
                img = cv2.imdecode(arr, cv2.IMREAD_COLOR)  # BGR画像としてデコード
                cv2.imshow("LiveView", img)
                key = cv2.waitKey(1)
                if key == 27:  # ESCキーで終了
                    break
            except Exception as e:
                print("Error during live view:", e)
                break
        
        # バッファ内容をImageに渡す場合
        if 0:
            from PIL import Image
            img = Image.open(io.BytesIO(buf))
            img = img.convert("RGB")
            img.show()


        success = cam.capture_image(0)
        print("Capture result:", success)

        # 5) 切断
        cam.disconnect_camera()

# 6) SDK解放
cam.sdk_release()
