# Ulti_Minimax


## Build for Native (e.g. Visual Studio)

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

## Build for WASM 

```console
cd build_wasm
emcmake cmake ..
cmake --build .
cd ..
```

After the build copy the generated JS and WASM files to the ```webapp/```.
```console
cp build_wasm/cpp/src/ulti_minimax.js webapp
cp build_wasm/cpp/src/ulti_minimax.wasm webapp
```

Use Python’s Built-in HTTP Server to host.
```console
cd webapp
python -m http.server 8000
cd ..
```

Open http://localhost:8000/index.html

## Misc
