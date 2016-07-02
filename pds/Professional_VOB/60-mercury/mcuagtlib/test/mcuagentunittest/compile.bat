
cd prj_win32

@echo.
@echo      =================================================
@echo      =  make win32 debug agent cppunit test - debug  =
@echo      =================================================
nmake /NOLOGO /A /B /f agenttest.mak CFG="AgentTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f agenttest.mak CFG="AgentTest - Win32 Debug" /x nmakeError.log

copy /Y Debug\McuAgentCppUnit.exe ..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcuagent

@if not exist "..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcuagent\conf/" mkdir "..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcuagent\conf"

copy /Y ..\conf\mcucfg.ini ..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcuagent\conf


@echo.
@echo      =================================================
@echo      = make win32 release agent cppunit test - debug =
@echo      =================================================
nmake /NOLOGO /A /B /f agenttest.mak CFG="AgentTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f agenttest.mak CFG="AgentTest - Win32 Release" /x nmakeError.log

copy /Y Release\McuAgentCppUnit.exe ..\..\..\..\..\10-common\version\release\test\unittest\service2\mcuagent

@if not exist "..\..\..\..\..\10-common\version\release\test\unittest\service2\mcuagent\conf/" mkdir "..\..\..\..\..\10-common\version\release\test\unittest\service2\mcuagent\conf"

copy /Y ..\conf\mcucfg.ini ..\..\..\..\..\10-common\version\release\test\unittest\service2\mcuagent\conf


cd ..\..
