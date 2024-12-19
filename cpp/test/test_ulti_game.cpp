#include <iostream>
#include <vector>
#include <gtest/gtest.h>

#include "game_manager.hpp"
#include "rules.hpp"

TEST(PartyStateTest, TestChooseWinnerCard) {
    ulti_minimax::PartyState partyState;

    std::vector<std::vector<int>> testCases = {
        // Each line is:
        // suit, value, suit, value, suit, value, trump, winning card index

        // These cases has intentionally uses invalid trump index (4)
        {0, 2, 0, 0, 0, 1, 4, 0},
        {0, 2, 0, 4, 0, 3, 4, 1},
        {0, 2, 0, 0, 0, 3, 4, 2},

        {0, 2, 1, 4, 2, 4, 4, 0},
        {0, 2, 1, 1, 2, 1, 4, 0},

        {0, 2, 0, 4, 1, 3, 4, 1},
        {0, 2, 0, 0, 1, 3, 4, 0},
        {0, 2, 0, 0, 1, 3, 4, 0},
        {0, 2, 1, 4, 0, 3, 4, 2},
        {0, 2, 1, 4, 0, 1, 4, 0},

        // Valid trump cases
        {0, 2, 0, 0, 0, 1, 0, 0},
        {0, 2, 0, 4, 0, 3, 0, 1},
        {0, 2, 0, 0, 0, 3, 0, 2},

        {0, 2, 1, 4, 2, 4, 1, 1},
        {0, 2, 1, 1, 2, 1, 2, 2},
        {0, 2, 1, 1, 2, 1, 0, 0},

        {0, 2, 0, 4, 1, 3, 0, 1},
        {0, 2, 0, 0, 1, 3, 1, 2},
        {0, 2, 0, 0, 1, 3, 0, 0},
        {0, 2, 1, 4, 0, 3, 1, 1},
        {0, 2, 1, 4, 0, 1, 1, 1},

        {0, 2, 1, 4, 1, 1, 1, 1},
        {0, 2, 1, 4, 0, 3, 1, 1},
        {0, 2, 0, 3, 1, 4, 1, 2},
    };

    for (const auto& testCase : testCases) {
        ulti_minimax::Card c0(testCase[0], testCase[1]);
        ulti_minimax::Card c1(testCase[2], testCase[3]);
        ulti_minimax::Card c2(testCase[4], testCase[5]);
        uint8_t trump = testCase[6];
        int winCardIndex = partyState.chooseWinnerCard(c0, c1, c2, trump);
        if (winCardIndex != testCase[7]) {
            for (int value : testCase) {
                std::cout << value << " ";
            }
            std::cout << std::endl;
        }
        EXPECT_EQ(winCardIndex, testCase[7]);        
    }
}

TEST(TreePathCoderTest, TestValueGetterSetter) {
    auto tpc = ulti_minimax::TreePathCoder();
    int n = ulti_minimax::N_PLAYER * ulti_minimax::N_CARD_IN_HAND;

    for (int i = 0; i < n; ++i) {
        tpc.setValue(i, i % 10);
    }
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(tpc.getValue(i), i % 10);
    }

    for (int i = 0; i < n; ++i) {
        tpc.setValue(i, 1);
    }
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(tpc.getValue(i), 1);
    }

    for (int i = 0; i < n; ++i) {
        tpc.setValue(i, (i + 1) % 10);
    }
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(tpc.getValue(i), (i + 1) % 10);
    }

    tpc.setResult(0);
    EXPECT_EQ(tpc.getResult(), 0);

    tpc.setResult(1);
    EXPECT_EQ(tpc.getResult(), 1);
}

TEST(TreePathCoderTest, TestCopy) {
    auto tpc0 = ulti_minimax::TreePathCoder(1, 25, 3);
    auto tpc1 = ulti_minimax::TreePathCoder(tpc0);
    EXPECT_EQ(tpc1.getResult(), 1);
    EXPECT_EQ(tpc1.getValue(25), 3);

    tpc0 = ulti_minimax::TreePathCoder(2, 29, 4);
    tpc1 = ulti_minimax::TreePathCoder(tpc0);
    EXPECT_EQ(tpc1.getResult(), 2);
    EXPECT_EQ(tpc1.getValue(29), 4);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}