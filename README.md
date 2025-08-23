# PyCrSDK  

PyCrSDK/  
├── build/  
├── installer/  
|    └── build.bat  
├── include/  
├── lib/  
├── pybind_module.cpp  
├── PyCrSDK.cpp  
├── PyCrSDK.hpp  
├── external/  
|    └── pybind11/    ← clone  https://github.com/pybind/pybind11.git  
├── CMakeLists.txt  
├── make.bat  
└── README.md  

# How to build

1. Make library

install Visual Studio（exam：VS2022） + CMake

```
>> cd PyCrSDK
>> make.bat
```

```
-- Building for: Visual Studio 17 2022
-- The C compiler identification is MSVC 19.44.35213.0
-- The CXX compiler identification is MSVC 19.44.35213.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- pybind11 v3.0.1

*************
*************

  Generating code
  Finished generating code
  pycrsdk.vcxproj -> ******\build\Release\pycrsdk.cp311-win_amd64.pyd
  Building Custom Rule ******\CMakeLists.txt


```

2. Make installer

```
>> cd installer
>> pip install build
>> build.bat
```

```
copy ..\build\Release\pycrsdk.*.pyd PyCrSDK\
..\build\Release\pycrsdk.cp311-win_amd64.pyd
        1 file(s) copied.

copy ..\build\Release\pycrsdk.exp PyCrSDK\
        1 file(s) copied.

copy ..\build\Release\pycrsdk.lib PyCrSDK\
        1 file(s) copied.

python -m build --wheel
* Creating isolated environment: venv+pip...
* Installing packages in isolated environment:
  - setuptools >= 40.8.0
* Getting build dependencies for wheel...
running egg_info
writing PyCrSDK.egg-info\PKG-INFO
writing dependency_links to PyCrSDK.egg-info\dependency_links.txt
writing top-level names to PyCrSDK.egg-info\top_level.txt
reading manifest file 'PyCrSDK.egg-info\SOURCES.txt'
reading manifest template 'MANIFEST.in'
writing manifest file 'PyCrSDK.egg-info\SOURCES.txt'
* Building wheel...
*************
*************
adding 'PyCrSDK/CrAdapter/libusb-1.0.dll'
adding 'pycrsdk-0.1.0.dist-info/METADATA'
adding 'pycrsdk-0.1.0.dist-info/WHEEL'
adding 'pycrsdk-0.1.0.dist-info/top_level.txt'
adding 'pycrsdk-0.1.0.dist-info/RECORD'
removing build\bdist.win-amd64\wheel
Successfully built pycrsdk-0.1.0-py3-none-any.whl

```
