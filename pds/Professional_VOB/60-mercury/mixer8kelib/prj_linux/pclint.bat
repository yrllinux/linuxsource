@echo off
rem ########################
rem    mixer8kelib_linux
rem ########################

set Output=mixer8kelib_linux_pclint.log

set Export_LintFile=export.lnt
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe
 
%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output%

FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end
echo on
