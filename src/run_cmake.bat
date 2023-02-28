call ../../../get_pico_root.bat
rem set
rem pause

set P=%PICO_ROOT%bin\python
set PATH=%P%;%PATH%
set P=%PICO_ROOT%bin\python\Scripts
set PATH=%P%;%PATH%


set P=%PICO_ROOT%bin\cmake\bin
set PATH=%P%;%PATH%

set P=%PICO_ROOT%bin\make
set PATH=%P%;%PATH%

set PICO_SDK_PATH=%PICO_ROOT%pico-sdk

set PICO_TOOLCHAIN_PATH=%PICO_ROOT%bin\gcc


cd ..

call cmake -S src -B . -G "Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug
pause


