@echo off

set "BASEDIR=%~dp0"
set "DFU_UTIL=%BASEDIR%dfu-util.exe"
set "FIRMWARE=%BASEDIR%firmware.bin"
set "VIDPID=0483:a51a"
set "ALT=0"
set "ADDR=0x98000000"
set "NB_ITERATIONS=50"

for /L %%i in (1,1,%NB_ITERATIONS%) do (
  "%DFU_UTIL%" -d %VIDPID% -a %ALT% -s %ADDR% -D "%FIRMWARE%"
)

exit /b %ERRORLEVEL%
