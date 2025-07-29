cmake_minimum_required(VERSION 3.15)
project(PyCrSDK)

add_subdirectory(external/pybind11)

# 必要であれば明示的に設定 (例: C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SDKのヘッダ・ライブラリをインクルード
include_directories(
  ./include
  ./include/CRSDK
  ./lib/opencv/include
)
link_directories(./lib/crsdk)

# includeディレクトリ内のすべての.cppファイルを取得
file(GLOB INCLUDE_SOURCES "include/*.cpp")

# ソースファイル
set(SOURCES
    PyCrSDK.cpp
    pybind_module.cpp
    ${INCLUDE_SOURCES}
)

# pybind11_add_moduleでPython拡張モジュールを作る
pybind11_add_module(pycrsdk  MODULE ${SOURCES})

# Sony Camera Remote SDK のライブラリ名: macOS/linuxなら "CameraRemote"
# Windowsなら "CameraRemote.lib" など。
# - macOSでは libCameraRemote.dylib が実際にはリンクされます
# target_link_libraries(pycam PRIVATE CameraRemote)
target_link_libraries(pycrsdk PRIVATE Cr_Core)

if(WIN32)
    target_compile_definitions(pycrsdk PRIVATE UNICODE _UNICODE)
endif()

# macOS向けに RPATH を設定する例 (SDK を一緒に配布するなどの場合)
# set_target_properties(pycam PROPERTIES
#     INSTALL_RPATH "@loader_path"
# )

set(OpenCV_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv/Windows/x86_64/Release/lib")
set(OpenCV_LIBS
    ${OpenCV_LIB_DIR}/opencv_core480.lib
    ${OpenCV_LIB_DIR}/opencv_highgui480.lib
    ${OpenCV_LIB_DIR}/opencv_imgcodecs480.lib
    ${OpenCV_LIB_DIR}/opencv_imgproc480.lib
)

# OpenCV ライブラリをリンク
target_link_libraries(pycrsdk PRIVATE ${OpenCV_LIBS})

# DLL をビルドディレクトリにコピー
add_custom_command(TARGET pycrsdk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv/Windows/x86_64/Release/bin/opencv_core480.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv/Windows/x86_64/Release/bin/opencv_highgui480.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv/Windows/x86_64/Release/bin/opencv_imgcodecs480.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv/Windows/x86_64/Release/bin/opencv_imgproc480.dll"
        $<TARGET_FILE_DIR:pycrsdk>
)