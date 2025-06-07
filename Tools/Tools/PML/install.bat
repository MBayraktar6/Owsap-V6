@copy /Y "%~dp0\PackMakerLite.exe" "c:\windows\system32\"
@copy /Y "%~dp0\PML_Pack.bat" "c:\windows\system32\"
@copy /Y "%~dp0\PML_Unpack.bat" "c:\windows\system32\"

@regedit.exe /S "%~dp0\MenuIntegration\PML_Pack.add.reg"
@regedit.exe /S "%~dp0\MenuIntegration\PML_Unpack.add.reg"

@echo PML installed
@pause
