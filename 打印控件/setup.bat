@echo off
set dir=system32
if exist "C:\WINDOWS\SysWOW64\" (
	set  dir=SysWOW64
)

set disk=c:

if exist d: (set disk=d:)
else if exit c: (set disk=c:)
else (set disk=%SYSTEMDRIVE%)



cd /%disk% %~d0

md %disk%\zyczs

copy dcrf32.dll C:\WINDOWS\%dir%\
copy MFC71.dll C:\WINDOWS\%dir%\MFC71.dll
copy msvcp71.dll C:\WINDOWS\%dir%\msvcp71.dll
copy msvcr71.dll C:\WINDOWS\%dir%\msvcr71.dll
move MyTestOcx.ocx C:\WINDOWS\%dir%\MyTestOcx.ocx
move TSCLIB.dll C:\WINDOWS\%dir%\TSCLIB.dll
move JZRF_API.dll C:\WINDOWS\%dir%\JZRF_API.dll
move function.dll C:\WINDOWS\%dir%\function.dll
move USB.dll C:\WINDOWS\%dir%\USB.dll
move USBKeyDLL.dll C:\WINDOWS\%dir%\USBKeyDLL.dll
move WINPSK.dll C:\WINDOWS\%dir%\WINPSK.dll
move ukey.ini %disk%\zyczs\ukey.ini

Regsvr32.exe C:\WINDOWS\%dir%\MyTestOcx.ocx

del MFC71.dll
del msvcp71.dll
del msvcr71.dll
del dcrf32.dll
del setup.bat