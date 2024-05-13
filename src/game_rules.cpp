#include "game_rules.h"

namespace ulti_minimax {

Card::Card(uint8_t suit, uint8_t value) : suit(suit), value(value) {}

Card::Card(const Card& other) : suit(other.suit), value(other.value) {}

bool Card::operator==(const Card& other) const {
	return suit == other.suit && value == other.value;
}

bool Card::operator>(const Card& other) const {
	return suit == other.suit ? value > other.value : true;
}

bool Card::operator<(const Card& other) const {
	return suit == other.suit ? value > other.value : false;
}

void Deck::deal() {
	// Assuming you have a vector named objects containing 32 objects

    std::vector<Object> objects; // Your list of 32 objects
    
    // Shuffle the objects vector with a fixed seed
    std::mt19937 g(42); // Use any integer value as the seed (e.g., 42)
    std::shuffle(objects.begin(), objects.end(), g);
    
    // Create three random sets each containing 10 objects
    std::vector<std::vector<Object>> randomSets(3, std::vector<Object>(10));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 10; ++j) {
            randomSets[i][j] = objects[i * 10 + j];
        }
    }
}

void PartyState::deal() {
	return;
}

std::vector<Card> PartyState::get_playable_cards() {
	return std::vector<Card>();
}

}