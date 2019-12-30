@ECHO OFF
SET "TARGET_DIR=%1"


CD "%~dp0"

mkdir "%TARGET_DIR%\GD3D11\shaders"
Xcopy "D3D11Engine\Shaders\*" "%TARGET_DIR%\GD3D11\shaders" /y /s

mkdir "%TARGET_DIR%\GD3D11\shaders\CSFFT"
COPY /y "D3D11Engine\CSFFT\*.hlsl" "%TARGET_DIR%\GD3D11\shaders\CSFFT"

Xcopy "blobs\data" "%TARGET_DIR%\GD3D11\data\" /y /s
Xcopy "blobs\Meshes" "%TARGET_DIR%\GD3D11\Meshes\" /y /s
Xcopy "blobs\Textures" "%TARGET_DIR%\GD3D11\Textures\" /y /s


COPY /y "D3D11Engine\AntTweakBar.dll" "%TARGET_DIR%\AntTweakBar.dll"
COPY /y "D3D11Engine\Assimp32.dll" "%TARGET_DIR%\Assimp32.dll"
Xcopy "blobs\libs\*" "%TARGET_DIR%\" /y /s
