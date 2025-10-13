@echo off
REM Test build script for Windows-only packet sniffer

echo ===============================================
echo Packet Sniffer - Windows Build Test
echo ===============================================

REM Check if WinPcap or Npcap is installed
echo Checking WinPcap/Npcap installation...
if not exist "C:\Windows\System32\wpcap.dll" (
    if not exist "C:\Windows\System32\Npcap\wpcap.dll" (
        echo WARNING: WinPcap or Npcap may not be installed
        echo Download Npcap from: https://npcap.com/
        echo Download WinPcap from: https://www.winpcap.org/install/ ^(legacy^)
    )
)

REM Clean previous build
echo Cleaning previous build...
make clean

REM Build the project
echo Building packet sniffer...
make
if %ERRORLEVEL% EQU 0 (
    echo ✅ Build successful!
    echo Binary created: packet_sniffer.exe
    echo.
    echo Usage:
    echo   packet_sniffer.exe          # Basic usage
    echo   packet_sniffer.exe -h       # Show help
    echo   packet_sniffer.exe --list   # List devices
    echo.
    echo Note: Requires administrator privileges
    echo       ^(Run Command Prompt as Administrator^)
) else (
    echo ❌ Build failed!
    exit /b 1
)