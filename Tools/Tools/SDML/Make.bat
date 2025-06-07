@echo off
title SDML
mkdir data
7za\7za.exe a data/monster.7z "%~dp0\ymir work\*" -r -x!*.gr2 -x!*.dds -x!*.jpg -x!*.png -x!*.mde -x!*.py -x!*.tga -x!*.bmp -x!*.msm -x!*.mse
pause
