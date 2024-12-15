#include <iostream>
#include <vector>
#include <chrono>

#include "game_manager.hpp"


// Native entry point
int main()
{
	std::cout << "Start simulation\n";
	auto gameManager = ulti_minimax::GameManager();

	// Determined deal
	std::string deal = "071315172021303336370001030405111214223202061016232426273134";
	gameManager.simulate(deal);

	// Random deal
	//gameManager.simulate();

	return 0;
}


// WASM entry point
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// Main function for WASM
// EMSCRIPTEN_KEEPALIVE ensures this function is callable from JavaScript
//
extern "C" EMSCRIPTEN_KEEPALIVE const char* wasm_main(const char* deal_code)
{
	std::string deal(deal_code);
	std::cout << "WASM: Received string: " << deal << std::endl;
    std::cout << "WASM: Start simulation" << std::endl;
	auto gameManager = ulti_minimax::GameManager();
	gameManager.simulate(deal);

	// Create a response string
	static std::string response = "Processed: " + deal;
	return deal.c_str();
}
#endif