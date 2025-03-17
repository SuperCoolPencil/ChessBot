# ChessEngine

ChessEngine is a C++ based chess bot that uses the SFML library for graphical rendering. This project aims to create a fully functional chess game with an AI opponent.

## Features

- Full chess game implementation
- Graphical interface using SFML
- Save and load game functionality

## Requirements

- C++17 or later
- SFML 2.5 or later

## Techniques

The ChessEngine uses several advanced techniques to provide a challenging opponent:

- **Minimax Algorithm**: This is the core algorithm used to evaluate the best move by simulating all possible moves and their outcomes.
- **Alpha-Beta Pruning**: This optimization technique is used to reduce the number of nodes evaluated by the minimax algorithm, improving efficiency.
- **Move Ordering**: This technique prioritizes moves that are likely to be stronger, allowing alpha-beta pruning to cut off more branches and speed up the decision-making process.
- **Transposition Tables**: These are used to store previously evaluated positions to avoid redundant calculations and improve performance.

## Acknowledgements

- [SFML](https://www.sfml-dev.org/) for the graphics library
- [Stockfish](https://stockfishchess.org/) for inspiration on AI algorithms
- [Sebastian Lague](https://youtu.be/U4ogK0MIzqk?si=KDfpXLyFQeylwAfx) for educational content
- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page) for valuable resources