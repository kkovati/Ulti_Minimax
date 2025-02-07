#include <iostream>
#include <vector>
#include <chrono>

#include "game_manager.hpp"


// Native entry point
int main()
{

#ifdef __EMSCRIPTEN__
	// Workaround for WASM compilation
	return 0;
#endif

	std::cout << "Start simulation\n";
	auto gameManager = ulti_minimax::GameManager();

	// Deal code structure:
	// deal[0]		= game type
	// deal[1]		= trump [0-3]
	// deal[2...61]	= cards [suit, value]
	std::string deal = "00071315172021303336370001030405111214223202061016232426273134";
	// Start simulation
	// Result : PLAYER_WIN
	// No.calls: 27497887
	// Duration : 5752 ms
	// TreePathCoder : 7 0 0 7 0 0 6 0 0 5 0 0 4 0 0 0 2 0 0 0 0 10 10 10 10 10 10 10 10 10

	// Generate game progression string from set up PartyState
	// gameProgression[0] = result 
	// gameProgression[1] = round starting player index 
	// gameProgression[2] = card suit 
	// gameProgression[3] = card value
	// gameProgression[4] = next player index in the round
	std::string gameProgression = gameManager.simulate(deal);

	// Random deal
	//std::string gameProgression = gameManager.simulate();
		
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
	std::string gameProgression = gameManager.simulate(deal);

	// Create a response string
	std::cout << "WASM: Response string: " << gameProgression << std::endl;

	// Allocate memory in the WASM heap for the response string
	char* response_cstr = (char*)malloc(gameProgression.size() + 1);
	std::strcpy(response_cstr, gameProgression.c_str());
	return response_cstr;
}

#endif