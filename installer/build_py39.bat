copy ..\build_py39\Release\pycrsdk.*.pyd PyCrSDK\
copy ..\build_py39\Release\pycrsdk.exp PyCrSDK\
copy ..\build_py39\Release\pycrsdk.lib PyCrSDK\
python -m build --wheel