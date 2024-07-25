@echo off
setlocal enabledelayedexpansion

:: 定义注册表键路径
set "RegistryKey=HKEY_CLASSES_ROOT\DesktopBackground\Shell"

reg add "%RegistryKey%\HzDesktop"

:: 创建 HzDesktop 键
reg add "%RegistryKey%\HzDesktop" /ve /d "开启HzDesktop" /f

:: 添加 Icon 值
reg add "%RegistryKey%\HzDesktop" /v Icon /t REG_SZ /d "D:\Documents\Codes\CppDemos\HzDesktop\build\Release\HzDesktopMgr.exe" /f

:: 创建 command 子键
reg add "%RegistryKey%\HzDesktop\command" /ve /d "D:\Documents\Codes\CppDemos\HzDesktop\build\Release\HzDesktopMgr.exe" /f

echo 完成添加注册表项。
pause