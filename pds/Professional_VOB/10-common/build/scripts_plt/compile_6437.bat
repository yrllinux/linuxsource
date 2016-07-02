call C:\CCStudio_v3.3\DosRun.bat

echo "===================== compiling lib [dm6437] ================"

cd ..\..\..\..\platform\20-cbb\platform\dm6437\driver

call compile.bat

cd ..\..\..\..\..\

rem del /F /Q .\kdv40-kdv7000_vob\10-common\lib\release\ti_d6437\*.lib
copy /Y .\platform\10-common\lib\cbb\platform\release\dm6437\*.lib .\Professional_VOB\10-common\lib\release\ti_d6437\

cd .\Professional_VOB\10-common\build\scripts_plt
