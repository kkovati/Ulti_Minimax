#include <algorithm>
#include <iterator>
#include <optional>
#include <tuple>

#include "rules.hpp"


namespace ulti_minimax {

// This is a lightweight class which codes the current traverse path
// through the simulation tree. This is the only class which is 
// passed back and forth between the tree's nodes during the simulation,
// so it must be minimal in size. At the end of the simulation an
// instance of this class will contain the path through the tree which 
// is used the replicate the gameplay progression of the optimal solution.
//
class TreePathCoder {
public:

	TreePathCoder() = default;

	TreePathCoder(uint8_t result, int index, uint8_t value) {
		std::fill(code.begin(), code.end(), N_CARD_IN_HAND);
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
		assert(0 <= index && index <= LAST_ACTION_INDEX && value <= N_CARD_IN_HAND - 1);
		code[index] = value;
	};

	uint8_t getValue(int index) const {
		assert(0 <= index && index <= LAST_ACTION_INDEX);
		return code[index];
	};

	void setResult(uint8_t result) {
		assert(result == RESULT_UNDEFINED || result == PLAYER_WIN || result == OPPONENT_WIN);
		code[N_ACTION] = result;
	};

	uint8_t getResult() const {
		return code[N_ACTION];
	};

	void printResult() const;
	void printCode() const;	

private:
	//uint8_t code[N_ACTION + 1] = { N_CARD_IN_HAND };  // TODO faster
	std::array<uint8_t, N_ACTION + 1> code;
};


// Top level manager of the minimax simulation
//
class GameManager {
public:
	GameManager() = default;

	// Convert between ace-king and ace-ten order
	std::string applyAceTenOrder(const std::string&);
	std::string applyAceKingOrder(const std::string&);

	// Main simulation function
	std::string simulate(const std::string& = "");

private:
	PartyState partyState; // Contains all action and info of the gameplay
	uint8_t card_order = 0;		

	// Minimax algorithm
	TreePathCoder minimax(int);

	// Visualization
	int setPartyState(TreePathCoder, int);
	int progressBar = 0;
	void updateProgressBar();	

	// Measurement
	int n_minimax_call = 0;
	int sum_time = 0;
	int n_meas = 0;
};

}