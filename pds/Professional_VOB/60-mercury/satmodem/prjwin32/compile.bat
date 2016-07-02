del /F /Q Debug\*.*
del ..\..\..\10-common\lib\debug\win32\ModemCmdProxy.lib

del /F /Q release\*.*
del ..\..\..\10-common\lib\release\win32\ModemCmdProxy.lib

rem call VCVARS32.bat

nmake /NOLOGO /a /s /f MdcomLib.mak CFG="MdcomLib - Win32 Debug" 

nmake /NOLOGO /a /s /f MdcomLib.mak CFG="MdcomLib - Win32 Release" 

