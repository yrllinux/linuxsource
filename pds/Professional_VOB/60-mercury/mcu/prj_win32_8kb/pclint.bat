@echo off
rem +++++++++++++++++++++++++++++++++++++++++++++
rem             	 mcu8000b
rem +++++++++++++++++++++++++++++++++++++++++++++

set Output=mcu8000b_pclint.log
set Export_LintFile=export.lnt
set PclintFullName=%PCLINT_ROOT%\lint-nt.exe

for %%c in (*.dsp ) do %PclintFullName% +linebuf %%c >%Export_LintFile% 

%PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std.lnt env-vc6.lnt %Export_LintFile% > %Output% 
rem %PclintFullName% +ffn -i%PCLINT_ROOT%\ -i.\ std_linux.lnt env-vc6.lnt %Export_LintFile% > %Output_linux%

FOR /F "tokens=1,2,7" %%i in  (';findstr /C:"error 900"  %Output%';) do if "%%k" == "0" (goto end)

copy /Y %Output% ..\..\..\10-common\version\pclintinfo\%Output%

:end
echo on




