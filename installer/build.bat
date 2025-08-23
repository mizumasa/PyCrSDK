copy ..\build\Release\pycrsdk.*.pyd PyCrSDK\
copy ..\build\Release\pycrsdk.exp PyCrSDK\
copy ..\build\Release\pycrsdk.lib PyCrSDK\
python -m build --wheel