# Tic Tac Toe

This is an implementation of the classic Tic Tac Toe game in C. The game allows players to play against each other or against an AI opponent.

## Features

- Human vs Human gameplay
- Human vs AI gameplay
- AI vs AI gameplay
- Simple command-line interface
- Minimax algorithm for AI decision making

## Getting Started

### Prerequisites

- C compiler (e.g., CLANG, GCC)

### Building and Running

1. Clone the repository:

```shell
git clone https://github.com/coding-pelican/tic-tac-toe-c.git
```

2. Navigate to the project directory:

```shell
cd tic-tac-toe-c
```

3. Compile the source code:

```shell
clang tic_tac_toe.c -o tic_tac_toe
```

4. Run the game:

```shell
./tic_tac_toe
```

## How to Play

1. Launch the game executable.
2. From the main menu, select the game mode:
   - Play with Another Player
   - Play with A.I.
   - Watch A.I. game play
3. Follow the on-screen instructions to make your moves.
4. The game will display the winner or a draw when the game ends.

## Code Structure

The source code is organized as follows:
- `tic_tac_toe.c`: Contains the main game logic, including the game loop, input handling, game state management, and AI decision making.
- `README.md`: Provides an overview of the game and instructions for building and running the code.

## Future improvements

- [ ] AI difficulty selection option (Easy, Medium, Hard)
- [ ] Game statistics tracking (wins, losses, draws)
- [ ] Game board size selection option (3x3, 4x4, 5x5)
- [ ] Option to display game board size (3x3, 4x4, 5x5)
- [ ] Game messages and formatting improvements
- [ ] Provide localization support for multiple languages
- [ ] Optimize the codebase further for better performance

## Acknowledgements

- The Minimax algorithm implementation is based on the tutorial by [GeeksforGeeks](https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-1-introduction/).

## Contact

For any questions or suggestions, please contact at
[codingpelican@gmail.com](mailto:codingpelican@gmail.com).
