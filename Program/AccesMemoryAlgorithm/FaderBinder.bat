@echo off

rem Check if psutil is installed
pip show psutil > nul 2>&1
if %errorlevel% neq 0 (
    echo Installing psutil...
    pip install psutil
)

rem Check if ctypes is installed
pip show ctypes > nul 2>&1
if %errorlevel% neq 0 (
    echo Installing ctypes...
    pip install ctypes
)

rem Check if pywin32 is installed
pip show pywin32 > nul 2>&1
if %errorlevel% neq 0 (
    echo Installing pywin32...
    pip install pywin32
)

rem Check if pygetwindow is installed
pip show pygetwindow > nul 2>&1
if %errorlevel% neq 0 (
    echo Installing pygetwindow...
    pip install pygetwindow
)

rem Check if rtmidi is installed
pip show python-rtmidi > nul 2>&1
if %errorlevel% neq 0 (
    echo Installing rtmidi...
    pip install python-rtmidi
)

rem Check if numpy is installed
pip show numpy > nul 2>&1
if %errorlevel% neq 0 (
    echo Installing numpy...
    pip install numpy
)

rem Check if os is installed (Note: os is a built-in module and doesn't need installation)
rem Check if sys is installed (Note: sys is a built-in module and doesn't need installation)

echo All required libraries are installed.
python FaderBinder.py
pause

