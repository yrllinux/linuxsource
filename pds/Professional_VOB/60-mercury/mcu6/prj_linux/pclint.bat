@echo off
rem +++++++++++++++++++++++++++++++++++++++++++++
rem             	 mcu8000_linux
rem +++++++++++++++++++++++++++++++++++++++++++++

set PclintFullName=%PCLINT_ROOT%\lint-nt.exe


rem ===================8000a_v6=====================
set Export_LintFile=export.lnt
set Output=mcu_8000a_v6_linux_pclint.log
%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output%

FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end1)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end1

rem ===================8000b_v6=====================
set Export_LintFile=export_8000b.lnt
set Output=mcu_8000b_v6_linux_pclint.log
%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output%
FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end2)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end2


rem ===================8000e_v6=====================
set Export_LintFile=export_8000e.lnt
set Output=mcu_8000e_v6_linux_pclint.log
%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output%
FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end
echo on