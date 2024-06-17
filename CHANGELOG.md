# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.2] - 2024-04-07

### Added
- Basic unit tests for critical game functions
  - Test cases for win condition checking
  - Verification of minimax algorithm's correctness
  - Testing of game initialization and reset functionality

### Changed
- Refactored code for better modularity and readability
  - Separated game logic into smaller functions
  - Improved function and variable naming conventions
  - Added more comments to explain code functionality
- Implemented alpha-beta pruning algorithm for AI player
  - Optimized minimax algorithm with alpha-beta pruning
  - Improved AI player's decision-making process
- Optimized code performance and efficiency
  - Reduced redundant calculations and function calls
  - Improved memory management and allocation
  - Optimized game board rendering and updating process

### Fixed
- AI player's suboptimal moves in certain scenarios
- Game crashes caused by invalid user input
- Memory leaks during prolonged gameplay sessions
- Compatibility issues across different platforms

### Known Issues
- AI player's decision-making time increases exponentially with larger board sizes
- Game UI responsiveness may degrade on low-end systems
- Some edge cases in game logic may not be fully covered by unit tests
- Localization support is limited to a single language

## [0.1] - 2023-04-15

### Added
- Initial version of the game
- Basic game logic and user interface
  - Drawing 3x3 tic-tac-toe board
  - Displaying player turn
  - Handling human player input
  - Checking game end conditions (win or draw)
  - Displaying game messages through message queue
- Support for different game modes
  - Human vs Human
  - Human vs AI (Easy/Hard mode using Minimax algorithm)
  - AI vs AI
- Additional features
  - Main menu to select game mode and start the game
  - Exit scene to confirm game exit
  - Hiding/showing cursor
  - Clearing screen
  - Waiting for user input
  - Toggle for displaying tile hints on the game board
  - Turn counter and display for current player's turn
  - Simple delay function for simulating AI thinking time
  - Basic error handling and assertions for debugging
  - Inline functions for common operations to improve performance
  - Documented the code using Doxygen-style comments

### Known Issues
- AI player sometimes makes suboptimal moves in Hard mode
- Game may crash if user enters invalid input during gameplay
- Memory leaks detected during prolonged gameplay sessions
- Compatibility issues reported on certain platforms
- Game state not properly reset when starting a new game after finishing one
