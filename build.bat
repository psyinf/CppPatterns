cmake -G "Visual Studio 15 2017 Win64" src  -B"build/vc15_64" -DCMAKE_INSTALL_PREFIX:PATH=../../install/vc15_64 -DDEVELOPER_BUILD=ON
cmake --build build/vc15_64 --target INSTALL --config RelWithDebInfo 

pause