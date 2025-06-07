@echo off

REM ## Create directories
if not exist "RootLib\" mkdir RootLib\
if not exist "RootLib\cyTemp" mkdir RootLib\cyTemp\

REM ## Delete files
del RootLib\*.py
del RootLib\cyTemp\*.c
del RootLib\cyTemp\*.py
