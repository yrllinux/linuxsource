@echo 不再编译 BoardAgentLib 单元测试
@goto Stop
 cd project
@echo.
@echo      ==============================================
@echo      =  making board manager cppunit test - debug =
@echo      ==============================================
nmake /NOLOGO /A /B /f BoardAgentUnitTest.mak CFG="BoardAgentUnitTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f BoardAgentUnitTest.mak CFG="BoardAgentUnitTest - Win32 Debug" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\boardagenttest_d.txt

@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent"
copy /Y Debug\BrdMangerCppUnit.exe ..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent

@echo.
@echo      ===============================================
@echo      = making board manager cppunit test - release =
@echo      ===============================================
nmake /NOLOGO /A /B /f BoardAgentUnitTest.mak CFG="BoardAgentUnitTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f BoardAgentUnitTest.mak CFG="BoardAgentUnitTest - Win32 Release" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\boardagenttest_r.txt

@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent/" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent"
copy /Y Release\BrdMangerCppUnit.exe ..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent

@echo cd ..\..\..\mmpagent\mmpagttester

@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent\conf/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent\conf"
@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent\conf/" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent\conf"

copy /Y conf\brdcfg.ini ..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent\conf
copy /Y conf\brdcfg.ini ..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent\conf
copy /Y conf\brdcfgdebug.ini ..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent\conf
copy /Y conf\brdcfgdebug.ini ..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent\conf
copy /Y conf\mcucfg.ini ..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent\conf
copy /Y conf\mcucfg.ini ..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent\conf
@echo copy /Y ..\..\..\..\mcuagtlib\test\mcuagentunittest\McuAgentUnitTest\conf\mcucfg.ini ..\..\..\..\..\10-Common\version\debug\test\UnitTest\Service2\boardagent\conf
@echo copy /Y ..\..\..\..\mcuagtlib\test\mcuagentunittest\McuAgentUnitTest\conf\mcucfg.ini ..\..\..\..\..\10-Common\version\release\test\UnitTest\Service2\boardagent\conf

cd ..\..\UnitTest

:Stop