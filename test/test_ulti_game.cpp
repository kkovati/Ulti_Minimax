#include <iostream>
#include <vector>
#include <gtest/gtest.h>

#include "game_manager.hpp"
#include "game_rules.hpp"

TEST(PartyStateTest, TestChooseWinnerCard) {
    ulti_minimax::PartyState partyState;

    std::vector<std::vector<int>> testCases = {
        {0, 2, 0, 0, 0, 1, 0},
        {0, 2, 0, 4, 0, 3, 1},
        {0, 2, 0, 0, 0, 3, 2},

        {0, 2, 1, 4, 2, 4, 0},
        {0, 2, 1, 1, 2, 1, 0},

        {0, 2, 0, 4, 1, 3, 1},
        {0, 2, 0, 0, 1, 3, 0},
        {0, 2, 0, 0, 1, 3, 0},
        {0, 2, 1, 4, 0, 3, 2},
        {0, 2, 1, 4, 0, 1, 0}
    };

    for (const auto& testCase : testCases) {

        ulti_minimax::Card c0(testCase[0], testCase[1]);
        ulti_minimax::Card c1(testCase[2], testCase[3]);
        ulti_minimax::Card c2(testCase[4], testCase[5]);
        int winCardIndex = partyState.chooseWinnerCard(c0, c1, c2);
        if (winCardIndex != testCase[6]) {
            for (int value : testCase) {
                std::cout << value << " ";
            }
            std::cout << std::endl;
        }
        EXPECT_EQ(winCardIndex, testCase[6]);        
    }
}

TEST(TreePathCoderTest, TestDigitGetterSetter) {
    auto tpc = ulti_minimax::TreePathCoder();
    int n = ulti_minimax::N_PLAYER * ulti_minimax::N_CARD_IN_HAND;

    for (int i = 0; i < n; ++i) {
        tpc.setDigit(i, i % 10);
    }
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(i % 10, tpc.getDigit(i));
    }

    for (int i = 0; i < n; ++i) {
        tpc.setDigit(i, 1);
    }
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(1, tpc.getDigit(i));
    }

    for (int i = 0; i < n; ++i) {
        tpc.setDigit(i, (i + 1) % 10);
    }
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ((i + 1) % 10, tpc.getDigit(i));
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}