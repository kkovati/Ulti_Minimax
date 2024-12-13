#include <iostream>
#include <vector>
#include <chrono>

#include "game_manager.hpp"


// Native entry point
int main()
{
	std::cout << "Start simulation\n";
	auto gameManager = ulti_minimax::GameManager();
	gameManager.simulate();
	return 0;
}


// WASM entry point
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// Wrapper function to simulate game logic
extern "C" void wasm_simulate()
{
    std::cout << "WASM: Start simulation\n";
    auto gameManager = ulti_minimax::GameManager();
    gameManager.simulate();
}

// EMSCRIPTEN_KEEPALIVE ensures this function is callable from JavaScript
extern "C" EMSCRIPTEN_KEEPALIVE void wasm_entrypoint()
{
    std::cout << "WASM: Entrypoint called\n";
    wasm_simulate();
}
#endif