@echo off

if not exist "bin" mkdir "bin"

set CFLAGS=-std:c++17 -Wall -Wextra ^
    -Wno-c++98-compat -Wno-c++98-compat-pedantic ^
    -Wno-documentation -Wno-documentation-unknown-command -Wno-old-style-cast ^
    -Wno-nonportable-system-include-path ^
    -Wno-missing-prototypes -Wno-unused-parameter ^
    -Wno-implicit-int-float-conversion -Wno-sign-conversion -Wno-shorten-64-to-32 ^
    -Wno-unsafe-buffer-usage -Wno-zero-as-null-pointer-constant ^
    -Wno-reserved-macro-identifier -Wno-reserved-identifier ^
    -Wno-unused-macros -Wno-extra-semi-stmt

if not exist "bin\imgui.obj" (
    clang-cl %CFLAGS% -Os -c ^
        -Ivendor\imgui -Ivendor\glfw-win32\include ^
        -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS ^
        vendor\imgui\imgui.cpp ^
        vendor\imgui\imgui_draw.cpp ^
        vendor\imgui\imgui_tables.cpp ^
        vendor\imgui\imgui_widgets.cpp ^
        vendor\imgui\backends\imgui_impl_glfw.cpp ^
        vendor\imgui\backends\imgui_impl_opengl3.cpp ^
        -Fo"bin\\"
    if errorlevel 1 goto error
)

if "%1"=="release" (
    clang-cl %CFLAGS% -Os -c ^
        -I. -Ivendor\imgui -Ivendor\glfw-win32\include ^
        -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS ^
        physics_engine.cpp -Fo"bin\\"
) else (
    clang-cl %CFLAGS% -g -c ^
        -I. -Ivendor\imgui -Ivendor\glfw-win32\include ^
        -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DDEBUG ^
        physics_engine.cpp -Fo"bin\\"
)
if errorlevel 1 goto error

radlink ^
    bin\physics_engine.obj ^
    bin\imgui.obj bin\imgui_draw.obj bin\imgui_tables.obj bin\imgui_widgets.obj ^
    bin\imgui_impl_glfw.obj bin\imgui_impl_opengl3.obj ^
    vendor\glfw-win32\glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib ^
    -out:"bin\physics-engine.exe"
if errorlevel 1 goto error

goto end
:error
exit /b 1
:end
