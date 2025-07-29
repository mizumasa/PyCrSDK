@echo off
cd build
cmake ..
cmake --build . --config Release
cd ..