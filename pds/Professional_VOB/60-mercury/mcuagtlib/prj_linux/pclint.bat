@echo off
rem +++++++++++++++++++++++++++++++++++++++++++++
rem             	 mcuagtlib_linux
rem +++++++++++++++++++++++++++++++++++++++++++++
rem +-------------------------------------------+

rem *******************8000a*************************
set Export_LintFile=export.lnt
set Output=mcuagtlib_linux_pclint.log
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe

%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output%

FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end1)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end1


rem *******************8000b*************************
set Export_LintFile=export_8000b.lnt
set Output=mcuagtlib8000b_linux_pclint.log
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe

%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output%
FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end2)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end2


rem *******************8000e*************************
set Export_LintFile=export_8000e.lnt
set Output=mcuagtlib8000e_linux_pclint.log
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe

%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output%
FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end
echo on
