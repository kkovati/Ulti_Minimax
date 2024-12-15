emcmake cmake ..
cmake --build .
copy cpp\src\ulti_minimax.js ..\webapp
copy cpp\src\ulti_minimax.wasm ..\webapp
pause