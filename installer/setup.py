from setuptools import setup, find_packages

setup(
    name="PyCrSDK",
    version="0.1.0",
    packages=find_packages(),
    include_package_data=True,  # MANIFEST.in を有効にする
    package_data={
        'PyCrSDK': [
            'pycrsdk.cp311-win_amd64.pyd',
            'pycrsdk.exp',
            'pycrsdk.lib',
            'Cr_Core.dll'
            ],  # ホイールに含めるバイナリ
    },
)