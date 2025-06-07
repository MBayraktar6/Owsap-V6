@echo off
if "%~1"=="" (
	echo "drag and drop the folder on this .bat"
) else (
	if exist "%~1" (
		PackMakerLite --nolog -p "%~n1"
	) else (
		echo "%~1 not a folder to pack"
	)
)
pause
