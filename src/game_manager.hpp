#include <algorithm>
#include <iterator>
#include <optional>
#include <tuple>

#include "game_rules.hpp"


namespace ulti_minimax {

class TreePathCoder {
public:
	TreePathCoder() = default;
	TreePathCoder(uint8_t result, int index, uint8_t value) {
		setResult(result);
		setValue(index, value);
	};
	TreePathCoder(const TreePathCoder& other) {
		std::copy(std::begin(other.code), std::end(other.code), std::begin(code));
	};
	TreePathCoder& operator=(const TreePathCoder& other) {
		if (this != &other) {
			std::copy(std::begin(other.code), std::end(other.code), std::begin(code));
		}
		return *this;
	};
	TreePathCoder(TreePathCoder&& other) noexcept {
		std::move(std::begin(other.code), std::end(other.code), std::begin(code));
	};
	TreePathCoder& operator=(TreePathCoder&& other) noexcept {
		if (this != &other) {
			std::move(std::begin(other.code), std::end(other.code), std::begin(code));
		}
		return *this;
	};

	void setValue(int index, uint8_t value) {
		assert(0 <= index && index < size && value <= 9);
		code[index] = value;
	};
	uint8_t getValue(int index) const {
		assert(0 <= index && index < size);
		return code[index];
	};
	void setResult(uint8_t result) {
		assert(0 <= result && result <= 2);
		code[size] = result;
	};
	uint8_t getResult() const {
		return code[size];
	};

	void printCode() const;

private:
	static const int size = N_ACTION;
	uint8_t code[size + 1] = { 0 };
};


class GameManager {
public:
	GameManager() = default;
	void simulate();

private:
	PartyState partyState;

	TreePathCoder minimax(int);

	// Measurement
	int n_minimax_call = 0;
	int sum_time = 0;
	int n_meas = 0;
};

}