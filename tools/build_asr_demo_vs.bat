@echo off
REM build_asr_demo_vs.bat
REM Compile the ASR demo using MSVC `cl` (Developer Command Prompt)

IF "%1"=="clean" GOTO :clean

IF NOT EXIST build (
  mkdir build
)

echo Compiling with MSVC cl...
cl /EHsc /O2 runtime\asr_lockfree_queue.cpp runtime\asr_supervisor.cpp tests\asr_queue_test.cpp /Fe:build\asr_test.exe
exit /b %errorlevel%

:clean
del build\asr_test.exe
exit /b 0
