#include <iostream>
#include <vector>
#include <chrono>

#include "game_manager.hpp"

int main()
{
	std::cout << "Start simulation\n";
	auto gameManager = ulti_minimax::GameManager();
	gameManager.simulate();
	return 0;
}
