@echo off
if "%~1"=="" (
	echo "drag and drop the .idx on this .bat"
) else (
	if not exist "%~1\nul" (
		PackMakerLite --nolog -u "%~n1"
	) else (
		echo "%~1 not a .idx/data to unpack"
	)
)
pause
