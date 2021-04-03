#!/bin/sh
cd "${0%/*}"

if [ $(uname) = "Darwin" ]; then
  clang++ borderless.mm -o libborderless.dylib -std=c++17 -shared -ObjC++ -framework Cocoa -m64
elif [ $(uname) = "Linux" ]; then
  g++ borderless.cpp -o libborderless.so -std=c++17 -shared -static-libgcc -static-libstdc++ -lX11 -m64
elif [ $(uname) = "FreeBSD" ]; then
  clang++ borderless.cpp -o libborderless.so -std=c++17 -shared -I/usr/local/include -L/usr/local/lib -lX11 -m64
else
  g++ borderless.cpp -o libborderless.dll -std=c++17 -shared -static-libgcc -static-libstdc++ -static -m64
fi
