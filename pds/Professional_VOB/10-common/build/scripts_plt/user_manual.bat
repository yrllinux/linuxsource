echo "===================copy user manual=========="
if not exist "..\..\version\release\win32\user_manual" (
    mkdir "..\..\version\release\win32\user_manual"
)
del /F /Q ..\..\..\10-common\version\release\win32\user_manual\*.*
copy /Y "..\..\..\00-public\user manual\chinese user manual\*.chm" ..\..\version\release\win32\user_manual\
copy /Y "..\..\..\00-public\user manual\chinese user manual\*.pdf" ..\..\version\release\win32\user_manual\

