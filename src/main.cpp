#include <iostream>
#include <vector>

#include "game_manager.hpp"

int main()
{
	//std::cout << "Start simulation\n";
	//auto gameManager = ulti_minimax::GameManager();
	//gameManager.simulate();
	//return 0;

	// TODO: tests needed
	auto c1 = ulti_minimax::TreePathCoder1();
	for (int i = 0; i < 32; ++i) {
		c1.setDigit(i, i % 10);
		std::cout << i << std::endl;
		c1.printCode();
	}
	
	return 0;
}



