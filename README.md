# **8-Puzzle A Solver**

A C++ implementation of an AI solver for the classic 8-Puzzle problem. This project uses the A* search algorithm with two heuristics: Manhattan Distance and Linear Conflict. It reads puzzle configurations from input files, computes optimal solutions, and outputs the solution depth, move sequence, and f(n) values.

<br> 
<br>

## **Features:**

- A* search algorithm

- Two heuristic options:

- Manhattan Distance (h1)

- Linear Conflict (h2)

- Node expansion tracking

- Cycle detection to avoid revisiting states

- Full solution path reconstruction

- Outputs move sequence, f(n) values, and total nodes generated

- Automatically generates two output files per input (h1 and h2)

<br> 
<br>

## **Technologies Used:**

- C++

- STL containers (vector, string, algorithm)

- File I/O

- A* search algorithm

- Heuristic design (Manhattan, Linear Conflict)

<br> 
<br>

## **How It Works:**

- Reads a 3×3 initial state and 3×3 goal state from an input file

- Runs the A* search algorithm twice:

- Once using heuristic h1

- Once using heuristic h2

- Generates corresponding output files containing:

- Initial state

- Goal state

- Solution depth

- Total nodes generated

- Move sequence (L, R, U, D)

- f(n) values along the solution path

<br> 
<br>

## **Input Format:**

- Each input file contains 6 lines:

  - 3 lines = initial state
  - 3 lines = goal state

## **Controls (Program Usage):**

- When prompted, enter the input file name (e.g., Input1.txt)

- **The program automatically:**

    - Extracts the puzzle number

    - Runs A* with both heuristics

- Writes results to OutputNh1.txt and OutputNh2.txt

<br> 
<br>

## **Extra Details:**

- Includes cycle checking through visited state arrays

- Child generation supports all valid moves (L, R, U, D)

- Calculates f(n) = g + h for priority selection

- Fully reconstructs solution path from goal node to root

<br>
<br>

## **Project Structure:**

- main.cpp

- Node class

- EightPuzzlesSolver class

- **Heuristics:**

    - manhattan_distance()

    - linear_conflict()

- State checking utilities

- A* frontier/expansion management

- Three sample input files and their corresponding outputs (for both h1 and h2) are included in the repository.

<br>
<br>
