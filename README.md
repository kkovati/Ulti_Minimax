# 🎴 Ulti Minimax

## 💡 Introduction

Ulti Minimax is a simulator tool to find perfect strategies for the Hungarian card game, 
[Ulti](https://en.wikipedia.org/wiki/Ulti), 
using the 
[minimax algorithm](https://en.wikipedia.org/wiki/Minimax).<br>
Analyze possible moves, explore decision-making strategies, and improve your gameplay with brute-force simulation!

The simulator core is written in C++ and designed for performance. 
The project also features a user interface that runs in the browser and 
accesses the WebAssembly (WASM)-compiled simulator core, 
making it usable without requiring manual compilation and build.
Additionally, the simulator is accessible as a 
[webapp](https://kkovati.github.io/Ulti_Minimax/) 
hosted on GitHub Pages.

## 🖥️ Use the [Webapp](ttps://kkovati.github.io/Ulti_Minimax/)!

The primary way to use this tool is through the web application: 
[kkovati.github.io/Ulti_Minimax](https://kkovati.github.io/Ulti_Minimax/)

How to use:
- Deal cards to the Player and Opponents
- Select a trump suit
- Start the simulation

The simulator evaluates all possible plays based on the given deal and 
determines whether a perfect strategy exists for the Player.<br>
If a perfect strategy was found, a possible winning game for the Player is displayed.

Here are some ready-made deals for You:<br>
[Strong hand](https://kkovati.github.io/Ulti_Minimax/webapp/simulation.html?deal=0070605040302012737173112323313143435151620212322242526103011)<br>
[Great Betli](https://kkovati.github.io/Ulti_Minimax/webapp/simulation.html?deal=0202122232425262700013736351514021334331207060504031716323111)<br>
[Slow simulation](https://kkovati.github.io/Ulti_Minimax/webapp/simulation.html?deal=0070605040300173727021213323322232414341510112021303125263536)<br>

## 📟 Native Build (e.g. Visual Studio)

This is for building the project as a native C++ project to use in Visual Studio for example.
Use CMake to build the project with the following commands with a specified the generator:

```console
cd build_native
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
cd ..
```

Now you can open the ```build_native/Ulti_Minimax.sln``` with Visual Studio.<br>
In Visual Studio ```Solution Explorer -> ulti_minimax -> Set as Startup Project``` 
and now you can build and run the project.

## 📦 WASM Build

[WebAssembly](https://en.wikipedia.org/wiki/WebAssembly) 
(WASM) allows running high-performance code in the browser. 
To compile the project to WASM, we use 
[Emscripten](https://emscripten.org/), 
a toolchain for compiling C++ code to WebAssembly.

If you want to make any change in the simulator core or in any C++ code,
use the following steps to build it into WASM:

1) Download and install Emscripten: https://emscripten.org/docs/getting_started/downloads.html

2) Build the project with Emscripten.

```console
cd build_wasm
emcmake cmake ..
cmake --build .
cd ..
```

3) After the build copy the generated JS and WASM files to the ```webapp/```  directory.

```console
cp build_wasm/cpp/src/ulti_minimax.js webapp
cp build_wasm/cpp/src/ulti_minimax.wasm webapp
```

## 💻 Run in Localhost

1) Use Python’s Built-in HTTP Server to host the application from the root folder with this command:

```console
python -m http.server 8000
```

2) Open http://localhost:8000/index.html in browser.

## 🤖 Under the Hood

### Minimax Algorithm 
The minimax algorithm is a decision-making strategy used in turn-based games to find the optimal moves for the participants. 
It traverses a game tree, where each node represents a state of the game. 
The algorithm explores future states by making recursive function calls, simulating decisions of the players one-by-one.

Each player aims to maximize their best possible score, in this case they aim to win the game.
This version of Minimax evaluates states based on the final game outcomes, backpropagating values up the tree to determine the best moves.

### Party State

At each node, the algorithm searches for the best card to play that would lead the current player to win the game.
This is done by recursively evaluating each possible move (with a recursive call) and determining which decision (card in this case) is the best. 
In this case, each node represents a card played by the player, leading to the next state of the game.

The way data is structured and passed between parent and child nodes is crucial for performance.
This simulator aims to minimize the amount of data transferred between and stored inside the nodes.

The program uses a singleton object called **PartyState**, which stores all information about the party and its progression.
Each node only knows which turn and moment of the game it is responsible for, and its role is to check all possible actions and test them one by one.

However, instead of passing actions directly to the next node, they are recorded in PartyState.
The next node doesn’t inherit information from its parent but instead reads from PartyState to determine the current game state.
This approach minimizes memory usage since nodes don’t store any game data themselves, 
reducing the amount of data passed between them and making the program significantly faster.

The following tables provide visual clues about the data structures stored in **PartyState**.<br>
Check [this Excel spreadsheet](https://github.com/kkovati/Ulti_Minimax/blob/main/docs/party_state_docs.xlsx) for more details.

**ActionList** contains all events of the game. Tracks which player played which card, in which round, and in what order.

| index | round | pos in round | player to hit | card |
| -------- | ------- | ------- | ------- | ------- |
| 0 | 0 | 0 | 0 | A0 |
| 1 | 0 | 1 | 1 | B0 |
| 2 | 0 | 2 | 2 | C0 |
| 3 | 1 | 0 | 2 | C1 |
| 4 | 1 | 1 | 0 | A1 |
| 5 | 1 | 2 | 1 | B1 |
| ... | ... | ... | ... | ... |
| 27 | 9 | 0 | 1 | B9 |
| 28 | 9 | 1 | 2 | C9 |
| 29 | 9 | 2 | 0 | A9 |


**RoundResults** contains the results of each round. This means won points and special cards.

| round | winner player | points | trump 7 | ace | ten | 
| -------- | ------- | ------- | ------- | ------- | ------- |
| 0 | 2 | 1 | 0 | 0 | 0 |
| 1 | 0 | 2 | 0 | 1 | 0 |
| 2 | 0 | 0 | 0 | 2 | 1 |
| ... | ... | ... | ... | ... | ... |
| 9 | 1 | 0 | 1 | 0 | 0 |

**PlayerHands** keeps track of each players' cards in hand and records in which round the cards were played out.

| player 0 hand | player 0 used | player 1 hand | player 1 used | player 2 hand | player 2 used |
| -------- | ------- | ------- | ------- | ------- | ------- |
| A0 | 0 | B0 | 1 | C0 | 0 |
| A1 | 4 | B1 | 5 | C1 | 3 |
| A2 | 30 | B2 | 2 | C2 | 30 |
| ... | ... | ... | ... | ... | ... |
| A9 | 30 | B9 | 0 | C9 | 30 |

### Tree Path Encoding

Besides determining whether a perfect strategy exists, the simulator also returns a possible winning sequence of moves.
This means that nodes must propagate the series of actions (in this case, the cards played) back to their parents.
So by the time the simulation reaches the root node (the first action), both the existence of a perfect strategy and the full sequence of moves are known.

A minimal-size object called **TreePathCoder** is used for that, 
which is essentially an array that stores only the indices representing which card a player chose from their hand.
Each node has an instance of TreePathCoder, which it returns to its parent.
The parent extends it with its own information (optimal card) and recursion continues.

### Rules of Ulti
The core gameplay mechanics of Ulti are implemented using the recursive function 
```GameManager::minimax()```.
Each call to this function is responsible for managing an atomic steps of the game.

- Checks the current state of the gameplay, based on the round index and the player's position within the round,
- lists the playable cards for the current player and 
- verifies whether the game has reached an ending condition.<br>

Based on this, the function either proceeds with the next recursive call or returns the result 
if it has already been determined.

The **three fundamental rules** of the game are:
- Determining which cards a player can legally play at any given moment, 
which is implemented in ```PartyState::getPlayableCards()```.
- Identifying the next player who wins the current round, 
which is implemented in ```PartyState::setNextPlayer()```.
- Evaluating the winning conditions based on the chosen game type in ```PartyState::evaluateParty()```.

Ulti has two **card orders**, **ace-king order** and **ace-ten order**.<br>
During the simulation, the program represents card values as integers to determine their relative order, 
to apply the termination scheme (see it in chapter *Heuristics and Optimizations*) and to identify special cards.
This requires a conversion between the cards' "real" value and integer representation.
Functions ```GameManager::applyAceKingOrder``` and ```GameManager::applyAceTenOrder``` are responsible for that.

### Heuristics and Optimizations

**Early Stopping** - Each game type has a winning condition, which is checked in ```PartyState::evaluateParty()```.
During simulation, this function is called at the end of each round to determine whether a player has met 
a winning condition.<br>
If so, the simulation stops at that state and returns the result instead of continuing the game.
This prevents the simulator from traversing the full game tree, allowing it to stop at intermediate (non-leaf) nodes, 
significantly improving performance.

**Simplify Cards** - Certain sets of cards are interchangeable at any given points in the game.
In other words, if a player would play out any card from such a set, choosing another card from this set would not 
affect the game's outcome, so the program treats them as a single option.<br>
For example, if a player holds both the 8 and 9 of the same suit, 
either card can be played interchangeably since they form a strict sequence with no other card between them.<br>
During the simulation, when a player has to decide between interchangeable cards, 
the program simplifies the decision to a single representative card.
This reduces the number of child nodes in the game tree, decreasing its size and speeding up the simulation.<br>
However, this strict sequence can be broken by special cards, such as cards with points or 
cards a player aims to collect. These special cards depend on the chosen game type.
Each game type has a termination scheme that defines which cards can be simplified 
and which must be considered separately.<br>
See functions ```PartyState::simplifyPlayableCards``` and ```Card::isNextInSeries``` for implementation details.

**Prerequisites** - If the special requirements of the chosen game type is not met 
(e.g., in Ulti the player does not have the trump 7 or in 40-100 the player does not have 40), 
then the simulation does not start. 
These conditions are checked in ```PartyState::init()```.
