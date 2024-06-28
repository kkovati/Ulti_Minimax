#include <algorithm>
#include <iterator>
#include <optional>
#include <tuple>

#include "game_rules.hpp"


namespace ulti_minimax {

class GameManager {
public:
	GameManager() = default;
	void simulate();

private:
	PartyState partyState;

	int minimax(int);

	// Measurement
	int n_minimax_call = 0;
	int sum_time = 0;
	int n_meas = 0;
};


class TreePathCoder {
public:
	TreePathCoder() {}
	TreePathCoder(const TreePathCoder& other) {
		std::copy(std::begin(other.code), std::end(other.code), std::begin(code));
	};
	TreePathCoder& operator=(const TreePathCoder& other) {
		std::copy(std::begin(other.code), std::end(other.code), std::begin(code)); 
		return *this;
	};

	void setDigit(int index, uint8_t value) {
		assert(0 <= index && index <= 31 && value <= 9);
		code[index] = value;
	};
	uint8_t getDigit(int index) const {
		assert(0 <= index && index <= 31);
		return code[index];
	};

	void printCode() const;

private:
	uint8_t code[32] = { 0 };
};

}