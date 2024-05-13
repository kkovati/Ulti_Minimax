#include <iostream>
#include <vector>

#include "game_manager.hpp"

int main()
{
	std::cout << "Hello World!\n";
	auto gameManager = ulti_minimax::GameManager();
	gameManager.simulate();
	return 0;
}



