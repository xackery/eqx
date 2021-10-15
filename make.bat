git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install Zlib:x64-windows
.\vcpkg\vcpkg install bullet3:x64-windows
.\vcpkg\vcpkg install glew:x64-windows
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE=c:/code/vcpkg/scripts/buildsystems/vcpkg.cmake ..