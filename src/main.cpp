#include <iostream>
#include <vector>
#include <chrono>

#include "game_manager.hpp"

int main()
{
	//std::cout << "Start simulation\n";
	//auto gameManager = ulti_minimax::GameManager();
	//gameManager.simulate();

	int n = 10000000;

	auto start = std::chrono::high_resolution_clock::now();
	auto tpc1 = ulti_minimax::TreePathCoder();
	auto tpc2 = ulti_minimax::TreePathCoder();
	for (int j = 0; j < n; ++j) {
		for (int i = 0; i < 32; ++i) {
			tpc1.setDigit(i, i % 10);
		}
		tpc2 = tpc1;
	}
	tpc1.printCode();
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	int time = duration.count();
	std::cout << "tpc Duration: " << time << " ms" << std::endl;

	start = std::chrono::high_resolution_clock::now();
	auto tpca1 = ulti_minimax::TreePathCoderArray();
	auto tpca2 = ulti_minimax::TreePathCoderArray();
	for (int j = 0; j < n; ++j) {
		for (int i = 0; i < 32; ++i) {
			tpca1.setDigit(i, i % 10);
		}
		tpca2 = tpca1;
	}
	tpca1.printCode();
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	time = duration.count();
	std::cout << "tpca Duration: " << time << " ms" << std::endl;


	return 0;
}



