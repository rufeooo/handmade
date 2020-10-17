@echo off
call run_constants.bat
set file_arg=%1
for /F "delims=" %%i in ("%file_arg%") do set filename="%%~ni"

cl %1 /Fmfoo /O2 /nologo /std:c++latest /fp:strict /I src\ /Fo:%BIN_DIR%\ /DUNICODE /link user32.lib opengl32.lib gdi32.lib /OUT:%BIN_DIR%/%filename%.exe
echo result %ERRORLEVEL%
