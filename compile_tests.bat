@echo off
echo [ProXPL] Building and running tests via CMake...
if not exist build mkdir build
cmake -S . -B build -DBUILD_TESTS=ON
if %errorlevel% neq 0 (
    echo [ProXPL] CMake configuration failed.
    exit /b %errorlevel%
)

cmake --build build --config Release
if %errorlevel% neq 0 (
    echo [ProXPL] CMake build failed.
    exit /b %errorlevel%
)

echo [ProXPL] Running test suite...
cd build
ctest -C Release --output-on-failure
cd ..
