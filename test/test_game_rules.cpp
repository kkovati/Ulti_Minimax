#include <gtest/gtest.h>
#include "game_rules.hpp"

TEST(PartyStateTest, TestsetNextPlayer) {
    ulti_minimax::PartyState partyState();
	
    //EXPECT_EQ(add(1, 1), 2);
    //EXPECT_EQ(add(-1, -1), -2);
    //EXPECT_EQ(add(100, 200), 300);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}