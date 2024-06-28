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
    TreePathCoder() : code0(0), code1(0) {}

    void setDigit(int, uint64_t);
    uint8_t getDigit(int) const;

	void printCode() const;

private:
    uint64_t code0; // 16 digits stored in a 64-bit integer, 4 bits per digit
    uint64_t code1; // 16 digits stored in another 64-bit integer, 4 bits per digit

};

}