@echo off
REM =============================================================================
REM  build-windows.bat — Compilation + packaging de CourseOrientation (Windows)
REM
REM  Prérequis :
REM    - Qt 6.x installé et qmake dans PATH
REM    - MSVC 2022 (ou MinGW) dans PATH  (ex: appel via "x64 Native Tools")
REM    - windeployqt dans PATH
REM =============================================================================

setlocal

set BUILD_DIR=build-windows
set DIST_DIR=dist-windows
set EXE_NAME=CourseOrientation.exe

echo.
echo ======================================================
echo   CourseOrientation — Build Windows Release
echo ======================================================
echo.

REM ── Créer le répertoire de build ─────────────────────────────────────────────
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM ── qmake ────────────────────────────────────────────────────────────────────
echo [1/4] Generation du Makefile...
qmake ..\CourseOrientation.pro CONFIG+=release
if errorlevel 1 ( echo ERREUR: qmake a echoue & cd .. & exit /b 1 )

REM ── nmake / mingw32-make ──────────────────────────────────────────────────────
echo [2/4] Compilation...
where nmake >nul 2>&1 && (
    nmake release
) || (
    mingw32-make -j%NUMBER_OF_PROCESSORS% release
)
if errorlevel 1 ( echo ERREUR: compilation echouee & cd .. & exit /b 1 )

cd ..

REM ── Copier l'exe + windeployqt ────────────────────────────────────────────────
echo [3/4] Packaging avec windeployqt...
if exist %DIST_DIR% rmdir /s /q %DIST_DIR%
mkdir %DIST_DIR%
copy %BUILD_DIR%\release\%EXE_NAME% %DIST_DIR%\

windeployqt ^
    --release ^
    --no-translations ^
    --no-system-d3d-compiler ^
    %DIST_DIR%\%EXE_NAME%

if errorlevel 1 ( echo ERREUR: windeployqt a echoue & exit /b 1 )

REM ── Créer le ZIP ──────────────────────────────────────────────────────────────
echo [4/4] Creation de l'archive ZIP...
powershell -Command "Compress-Archive -Path '%DIST_DIR%\*' -DestinationPath 'CourseOrientation-windows-x64.zip' -Force"

echo.
echo ======================================================
echo   Build termine : CourseOrientation-windows-x64.zip
echo ======================================================
echo.
endlocal
