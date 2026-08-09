# PyCrSDK

PyCrSDK builds a Windows wheel for each supported CPython version with CMake, pybind11, scikit-build-core, and cibuildwheel.

## Repository layout

```text
PyCrSDK/
├── .github/workflows/build-wheels.yml
├── build/
├── include/
├── installer/
│   └── PyCrSDK/
├── lib/
├── pybind_module.cpp
├── PyCrSDK.cpp
├── PyCrSDK.hpp
├── CMakeLists.txt
├── pyproject.toml
└── README.md
```

## Local wheel build

Prerequisites:

* Visual Studio 2022 with C++ tools
* CMake
* Python 3.9 to 3.13

Build a wheel for the current interpreter:

```powershell
python -m pip wheel . --no-deps -w wheelhouse
```

This avoids the local `build/` directory name colliding with Python's `build` package. The generated wheel is placed under `wheelhouse/` and should have a Python-specific tag such as `cp311-cp311-win_amd64`.

## GitHub Actions wheel build

The workflow in `.github/workflows/build-wheels.yml` builds Windows x64 wheels for:

* CPython 3.9
* CPython 3.10
* CPython 3.11
* CPython 3.12
* CPython 3.13

It runs on pushes to `main`, pull requests, manual dispatch, and version tags matching `v*`.

Artifacts:

* Every workflow run uploads all wheels as the `PyCrSDK-windows-wheels` artifact.
* Tag pushes such as `v0.1.0` also create or update the GitHub Release and attach the wheel files as release assets.

## Developer usage without install

For direct local testing from a build tree:

```python
import sys
sys.path.append("D:/PyCrSDK/build/Release")

import pycrsdk
```

If you run the extension directly from a build directory, keep the required runtime files available alongside the module, including the `CrAdapter` folder and bundled DLLs in `installer/PyCrSDK`.
