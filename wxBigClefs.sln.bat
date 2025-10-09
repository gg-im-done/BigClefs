@echo off
msbuild C:\Users\asket\source\repos\BigClefs\wxBigClefs.sln -p:Configuration=Release -noLogo -verbosity:m
set BUILD_RESULT=%ERRORLEVEL%
if %BUILD_RESULT%==0 (
echo ---Start---
C:\Users\asket\source\repos\BigClefs\x64\Release\wxBigClefs.exe
echo ----End----
)

pause