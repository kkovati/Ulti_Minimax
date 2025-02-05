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

TEST(PartyStateTest, simplifyPlayableCards) {
    // Special card game
    std::string deal = "00071315172021303336370001030405111214223202061016232426273134";
    ulti_minimax::PartyState partyStateSpecial;
    bool prerequisites = partyStateSpecial.init(deal);

    // No special card game
    deal = "50071315172021303336370001030405111214223202061016232426273134";
    ulti_minimax::PartyState partyStateNoSpecial;
    prerequisites = partyStateNoSpecial.init(deal);

    // Create test cases
    std::vector<std::tuple<std::vector<int>, std::vector<int>, std::vector<int>>> testCases = {
        // Each tuple consist two lines is:
        // First line is original card vector: suit, value, suit, value, ...
        // Second line is simplified card vector WITH special cards: suit, value, suit, value, ...
        // Third line is simplified card vector WITHOUT special cards: suit, value, suit, value, ...

        {
            {0, 1, 0, 2, 0, 3, 0, 4}, // Original
            {0, 1}, // Simplified WITH special cards
            {0, 1}, // Simplified WITHOUT special cards
        }, 
        {
            {0, 1, 0, 2, 0, 3, 0, 4, 0, 5},
            {0, 1},
            {0, 1},
        },        
        {
            {0, 1, 0, 2, 0, 4, 0, 5},
            {0, 1, 0, 4},
            {0, 1, 0, 4},
        },
        {
            {0, 0, 0, 2, 0, 4, 0, 5},
            {0, 0, 0, 2, 0, 4},
            {0, 0, 0, 2, 0, 4},
        },        
        {
            {0, 0, 0, 1, 0, 2, 0, 4, 0, 5},
            {0, 0, 0, 1, 0, 4},
            {0, 0, 0, 4},
        },
        {
            {0, 0, 0, 2, 0, 3, 0, 4, 0, 5},
            {0, 0, 0, 2},
            {0, 0, 0, 2},
        },
        {
            {0, 0, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7},
            {0, 0, 0, 2, 0, 6, 0, 7},
            {0, 0, 0, 2},
        },
        {
            {0, 0, 0, 2, 0, 3, 0, 4, 0, 5, 0, 7},
            {0, 0, 0, 2, 0, 7},
            {0, 0, 0, 2, 0, 7},
        },            
        {
            {0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 7},
            {0, 0, 0, 1, 0, 7},
            {0, 0, 0, 7},
        },          
        {
            {0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7},
            {0, 0, 0, 1, 0, 6, 0, 7},
            {0, 0},
        },        
        {
            {1, 4, 1, 5, 1, 6},
            {1, 4, 1, 6},
            {1, 4},
        },        
        {
            {2, 4, 2, 5, 2, 6, 2, 7},
            {2, 4, 2, 6, 2, 7},
            {2, 4},
        },        
        {
            {1, 4, 1, 5, 2, 6, 2, 7},
            {1, 4, 2, 6, 2, 7},
            {1, 4, 2, 6},
        },        
        {
            {0, 0, 0, 1, 1, 0, 1, 1, 2, 1, 2, 2, 3, 3},
            {0, 0, 0, 1, 1, 0, 1, 1, 2, 1, 3, 3},
            {0, 0, 1, 0, 2, 1, 3, 3},
        },
    };

    // Iterate through test cases
    for (const auto& testCase : testCases) { 

        // SPECIAL CARD TEST CASE
        auto originalCardsStr = std::get<0>(testCase);
        auto simplifiedCardsStr = std::get<1>(testCase);
        // Create original card vector
        std::vector<ulti_minimax::Card> originalCardVector;
        for (int i = 0; i < originalCardsStr.size(); i += 2) {
            originalCardVector.push_back(ulti_minimax::Card(originalCardsStr[i], originalCardsStr[i + 1]));
        }
        // Create simplified target card vector WITH special cards
        std::vector<ulti_minimax::Card> simplifiedCardVector;
        for (int i = 0; i < simplifiedCardsStr.size(); i += 2) {
            simplifiedCardVector.push_back(ulti_minimax::Card(simplifiedCardsStr[i], simplifiedCardsStr[i + 1]));
        }        
        // Simplify
        partyStateSpecial.simplifyPlayableCards(originalCardVector);
        // Check simplification results
        EXPECT_EQ(originalCardVector.size(), simplifiedCardVector.size());
        if (originalCardVector.size() != simplifiedCardVector.size()) break;
        for (int i = 0; i < originalCardVector.size(); ++i) {
            EXPECT_TRUE(originalCardVector[i].equals(simplifiedCardVector[i]));
        }


        // NO SPECIAL CARD TEST CASE
        simplifiedCardsStr = std::get<2>(testCase);
        // Create original card vector
        originalCardVector.clear();
        for (int i = 0; i < originalCardsStr.size(); i += 2) {
            originalCardVector.push_back(ulti_minimax::Card(originalCardsStr[i], originalCardsStr[i + 1]));
        }
        // Create simplified target card vector WITHOUT special cards
        simplifiedCardVector.clear();
        for (int i = 0; i < simplifiedCardsStr.size(); i += 2) {
            simplifiedCardVector.push_back(ulti_minimax::Card(simplifiedCardsStr[i], simplifiedCardsStr[i + 1]));
        }
        // Simplify
        partyStateNoSpecial.simplifyPlayableCards(originalCardVector);
        // Check simplification results
        EXPECT_EQ(originalCardVector.size(), simplifiedCardVector.size());
        if (originalCardVector.size() != simplifiedCardVector.size()) break;
        for (int i = 0; i < originalCardVector.size(); ++i) {
            EXPECT_TRUE(originalCardVector[i].equals(simplifiedCardVector[i]));
        }
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