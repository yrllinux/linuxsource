echo off
cd prj_win32
call pclint.bat

cd ..
cd prj_win32_8kb
call pclint.bat

cd ..
cd prj_win32_8ke
call pclint.bat

cd ..
cd prj_linux
call pclint.bat

echo on