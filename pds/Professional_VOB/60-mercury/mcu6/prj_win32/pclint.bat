@echo off
rem +++++++++++++++++++++++++++++++++++++++++++++
rem             	 mcu8000a_v6
rem +++++++++++++++++++++++++++++++++++++++++++++

set Output=mcu8000a_v6_pclint.log
set Export_LintFile=export.lnt
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe

for %%c in (*.dsp ) do %PclintFullName% +linebuf %%c >%Export_LintFile%

%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std.lnt env-vc6.lnt %Export_LintFile% > %Output% 

FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end
echo on

rem @echo 按任一键退出
rem pause>nul

