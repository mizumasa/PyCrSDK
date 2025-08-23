import os, sys, shutil
from .pycrsdk import CameraManager

__all__ = ["CameraManager"]

def _install_dlls():
    src = os.path.join(os.path.dirname(__file__), "CrAdapter")
    dst = os.path.join(os.path.dirname(sys.executable), "CrAdapter")
    if not os.path.exists(dst):
        try:
            shutil.copytree(src, dst, dirs_exist_ok=True)
            print(f"Installed DLLs to {dst}")
        except Exception as e:
            print(f"Warning: could not install DLLs: {e}")

_install_dlls()