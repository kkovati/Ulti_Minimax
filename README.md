# 🎴 Ulti Minimax

## 💡 Introduction

Ulti Minimax is a simulator tool to find perfect strategies for the Hungarian card game, 
[Ulti](https://en.wikipedia.org/wiki/Ulti), 
using the 
[minimax algorithm](https://en.wikipedia.org/wiki/Minimax).<br>
Analyze possible moves, explore decision-making strategies, and improve your gameplay with brute-force simulation!

The simulator core is written in C++ and designed for performance. 
The project also features a user interface that runs in the browser and 
accesses the WebAssembly (WASM)-compiled simulator core, 
making it usable without requiring manual compilation and build.
Additionally, the simulator is accessible as a 
[webapp](https://kkovati.github.io/Ulti_Minimax/) 
hosted on GitHub Pages.

## 🖥️ Use the webapp!

The primary way to use this tool is through the web application: 
[kkovati.github.io/Ulti_Minimax](https://kkovati.github.io/Ulti_Minimax/)

How to use:
- Deal cards to the Player and Opponents
- Select a trump suit
- Start the simulation

The simulator evaluates all possible plays based on the given deal and 
determines whether a perfect strategy exists for the Player.<br>
If a perfect strategy was found, a possible winning game for the Player is displayed.

## 📟 Native Build (e.g. Visual Studio)

This is for building the project as a native C++ project to use in Visual Studio for example.
Use CMake to build the project with the following commands with a specified the generator:

```console
cd build_native
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
cd ..
```

Now you can open the ```build_native/Ulti_Minimax.sln``` with Visual Studio.<br>
In Visual Studio ```Solution Explorer -> ulti_minimax -> Set as Startup Project``` 
and now you can build and run the project.

## 📦 WASM Build

[WebAssembly](https://en.wikipedia.org/wiki/WebAssembly) 
(WASM) allows running high-performance code in the browser. 
To compile the project to WASM, we use 
[Emscripten](https://emscripten.org/), 
a toolchain for compiling C++ code to WebAssembly.

If you want to make any change in the simulator core or in any C++ code,
use the following steps to build it into WASM:

1) Download and install Emscripten: https://emscripten.org/docs/getting_started/downloads.html

2) Build the project with Emscripten.

```console
cd build_wasm
emcmake cmake ..
cmake --build .
cd ..
```

3) After the build copy the generated JS and WASM files to the ```webapp/```  directory.

```console
cp build_wasm/cpp/src/ulti_minimax.js webapp
cp build_wasm/cpp/src/ulti_minimax.wasm webapp
```

## 💻 Run in localhost

1) Use Python’s Built-in HTTP Server to host the application from the root folder with this command:

```console
python -m http.server 8000
```

2) Open http://localhost:8000/index.html in browser.

## Misc
