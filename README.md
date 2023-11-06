# 15-puzzle_A_star_search
This C program is an implementation of the 15-Puzzle solver using the A* search algorithm. The 15-Puzzle is a classic sliding puzzle game that consists of a 4x4 grid with 15 tiles numbered from 1 to 15, leaving one tile empty. The goal is to rearrange the tiles from an initial state to a goal state, typically in ascending order.

Usage
Compile the program using a C compiler:
gcc 15_puzzle_solver.c -o 15_puzzle_solver

Run the program with the initial state as command-line arguments. The initial state should be a sequence of 16 integers representing the puzzle layout, with '0' representing the empty space. For example:
./15_puzzle_solver 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0

The program will output the path from the initial state to the goal state, indicating the moves needed to solve the puzzle.

Code Structure
- `struct state`: A structure representing a state of the puzzle, including the puzzle layout, f, g, and h values, as well as links to the parent state.
- `initialize()`: Initializes the initial and goal states.
- `expand()`: Generates successor states by moving tiles in different directions.
- `filter()`: Filters out repeated states to avoid exploring the same state multiple times.
- `merge_to_fringe()`: Merges successor states into the fringe based on their f values.
- `update_fgh()`: Updates the f, g, and h values for a state.
- Main loop: The main loop implements the A* search algorithm, exploring states until the goal state is reached or no solution is found.

Example:
Here's an example of running the program with the provided initial state:
./15_puzzle_solver 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0

The program will output the path to the goal state:

Path (length=4):
 1  2  3  4
 5  6  7  8
 9 10 11 12
13 14 15  0

 1  2  3  4
 5  6  7  8
 9 10 11 12
13 14  0 15

 1  2  3  4
 5  6  7  8
 9 10 11 12
13  0 14 15

 1  2  3  4
 5  6  7  8
 9 10 11  0
13 14 12 15

This represents the sequence of moves needed to solve the 15-Puzzle.
