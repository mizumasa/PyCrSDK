@echo off
mkdir build_py39
cd build_py39
cmake ..
cmake --build . --config Release
cd ..