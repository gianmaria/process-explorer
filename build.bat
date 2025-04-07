@echo off

setlocal
set mode=%1

if "%mode%"=="debug" (
    set optimization=/Od /Zi /MDd
    set macro=/D "_DEBUG"
) else if "%mode%"=="release" (
    set optimization=/O2 /Zi /MD
    set macro=/D "_NDEBUG"
) else (
    echo Invalid mode specified. Use "debug" or "release".
    exit /b 1
)

echo Building in: %mode%

set base=%~dp0

if not exist build\%mode% mkdir build\%mode%
pushd build\%mode%

if '1' == '1' (
    cl /nologo /FC /std:c++20 ^
    /EHsc ^
    %macro% ^
    %optimization% ^
    /Wall /external:anglebrackets /external:W0 ^
    /wd4820 /wd5045 /wd4365 ^
    /utf-8 /Zc:__cplusplus ^
    /D _CONSOLE /D _UNICODE /D UNICODE ^
    %base%\main.cpp ^
    /link ^
    Psapi.lib Advapi32.lib ^
    /MACHINE:X64 /SUBSYSTEM:CONSOLE ^
    /DEBUG:full /OUT:main.exe
) else (
    @REM cl /nologo /FC /std:c++20 ^
    @REM /EHsc ^
    @REM %macro% ^
    @REM %optimization% ^
    @REM /wd4820 /wd5045 /wd4365 ^
    @REM /utf-8 /Zc:__cplusplus ^
    @REM /D _CONSOLE /D _UNICODE /D UNICODE ^
    @REM %base%\main.cpp ^
    @REM /link ^
    @REM Psapi.lib Advapi32.lib ^
    @REM /DEBUG:full

    echo fast
    cl /FC /std:c++20 ^
    /EHsc ^
    %base%\main.cpp ^
    /link ^
    Psapi.lib Advapi32.lib
)

@REM /MANIFEST:EMBED /MANIFESTUAC:"level='requireAdministrator' uiAccess='false'" ^

popd
endlocal


@REM cl /FC /std:c++20 /EHsc ..\..\main.cpp /link Psapi.lib Advapi32.lib