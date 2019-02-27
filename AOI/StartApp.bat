@ echo off
@ ping -n 5 127.1 >nul 2>nul
rem
cd/d %~dp0
start "" "MuraDetector.exe"
exit