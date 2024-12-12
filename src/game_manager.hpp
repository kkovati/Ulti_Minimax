#include <algorithm>
#include <iterator>
#include <optional>
#include <tuple>

#include "game_rules.hpp"


namespace ulti_minimax {

class TreePathCoder {
public:
	TreePathCoder() = default;
	TreePathCoder(Result result_, int index, uint8_t value) {
		setResult(result_);
		setValue(index, value);
	};
	TreePathCoder(const TreePathCoder& other) {
		result = other.result;
		std::copy(std::begin(other.code), std::end(other.code), std::begin(code));
	};
	TreePathCoder& operator=(const TreePathCoder& other) {
		if (this != &other) {
			result = other.result;
			std::copy(std::begin(other.code), std::end(other.code), std::begin(code));
		}
		return *this;
	};
	TreePathCoder(TreePathCoder&& other) noexcept {
		result = other.result;
		std::move(std::begin(other.code), std::end(other.code), std::begin(code));
	};
	TreePathCoder& operator=(TreePathCoder&& other) noexcept {
		if (this != &other) {
			result = other.result;
			std::move(std::begin(other.code), std::end(other.code), std::begin(code));
		}
		return *this;
	};

	void setValue(int index, uint8_t value) {
		assert(0 <= index && index < N_ACTION && value <= N_CARD_IN_HAND - 1);
		code[index] = value;
	};
	uint8_t getValue(int index) const {
		assert(0 <= index && index < N_ACTION);
		return code[index];
	};
	void setResult(Result result_) {
		result = result_;
	};
	Result getResult() const {
		return result;
	};

	void printCode() const;

private:
	Result result;
	uint8_t code[N_ACTION] = { 0 };
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