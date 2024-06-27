#include <optional>
#include <tuple>
#include <bitset>

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


class TreePathCoder1 {
private:
    uint64_t code1; // 16 digits stored in a 64-bit integer, 4 bits per digit
    uint64_t code2; // 16 digits stored in another 64-bit integer, 4 bits per digit

public:
    TreePathCoder1(uint64_t initialCode1 = 0, uint64_t initialCode2 = 0) : code1(initialCode1), code2(initialCode2) {}

    void setDigit(int position, uint8_t value) {
        assert(position >= 0 && position <= 31 && value <= 9);
        if (position < 16) {
            code1 &= ~((uint64_t)0xF << (position * 4)); // Clear the bits at the specified position in code1
            code1 |= ((uint64_t)value << (position * 4)); // Set the bits to the new value in code1
        }
        else {
            position -= 16; // Adjust the position for code2
            code2 &= ~((uint64_t)0xF << (position * 4)); // Clear the bits at the specified position in code2
            code2 |= ((uint64_t)value << (position * 4)); // Set the bits to the new value in code2
        }
    }

    uint8_t getDigit(int position) const {
        assert(position >= 0 && position <= 31);
        if (position < 16) {
            return (code1 >> (position * 4)) & 0xF; // Extract the bits at the specified position in code1
        }
        else {
            position -= 16; // Adjust the position for code2
            return (code2 >> (position * 4)) & 0xF; // Extract the bits at the specified position in code2
        }
    }

    void printCode() const {
        std::cout << "Iterative" << std::endl;
        for (int i = 31; i >= 0; --i) {
            std::cout << unsigned(getDigit(i));
        }
        std::cout << std::endl;

        std::cout << "Bitset" << std::endl;
        std::bitset<64> x(code1);
        std::cout << x << '\n';        
        std::bitset<64> y(code2);
        std::cout << y << '\n';
        std::cout << std::endl;

        //TODO: delete #include <bitset>
    }
};

}