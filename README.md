# BorionLauncher

Source code of the Borion "launcher".

To build this project, make sure the Vulkan SDK is installed on your system.

Compiling DirectX 9 shaders requires "d3dx9_43.dll" to be available on your system. Set "IGNORE_DXBC" in CMake to skip compiling the shaders. Pre-compiled shaders are included for this case.