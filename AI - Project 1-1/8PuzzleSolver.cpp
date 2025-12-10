//Rafid Ahmed
//8 Puzzle Problem

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <climits>
#include <cmath>
using namespace std;

// Node Class
// Each Node represents one configuration (state) of the 8-puzzle board.
// It stores the board state, cost values (g and h), parent pointer,
// move direction from the parent, and its child nodes.
class Node {
public:
    vector<vector<string>> state;   // 3x3 puzzle board
    Node* parent;                   // Pointer to parent node
    vector<Node*> child;            // List of child nodes
    string move;                    // Move that led to this node ("L", "R", "U", "D")
    int g;                          // Cost so far (depth level)
    int h;                          // Heuristic value (estimated cost to goal)

    // Constructor: initializes a node with the given puzzle state
    Node(vector<vector<string>> puzz) {
        state = puzz;
        parent = nullptr;
        move = "";
        g = 0;
        h = 0;
    }

    // Helper function to print the node's details for debugging
    void display() {
        cout << "state:" << endl;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                cout << state[i][j] << " ";
            }
            cout << endl;
        }
        cout << "parent:" << endl;
        if (parent) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    cout << parent->state[i][j] << " ";
                }
                cout << endl;
            }
        }
        cout << "move: " << move << endl;
        cout << "g: " << g << endl;
        cout << "h: " << h << endl;
    }
};

// EightPuzzlesSolver Class
// Handles reading input, performing the A* search, and writing the output.
// Two heuristics are supported:
//   h1 = Manhattan Distance
//   h2 = Linear Conflict (Manhattan + 2×conflicts)
class EightPuzzlesSolver {
private:
    // Internal state variables
    vector<vector<string>> initial_state;
    vector<vector<string>> goal_state;
    vector<Node*> expanded;                           // List of nodes already expanded
    vector<Node*> front;                              // Frontier (open list)
    vector<vector<vector<string>>> viewed_state;      // States already visited
    vector<vector<vector<string>>> front_state;       // States currently in frontier
    int heuristic_choice;                             // 1 for h1, 2 for h2

    // Utility function: check if two puzzle states are identical
    bool statesEqual(const vector<vector<string>>& s1, const vector<vector<string>>& s2) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (s1[i][j] != s2[i][j]) return false;
            }
        }
        return true;
    }

    // Utility function: check if a given state exists in a list
    bool stateInList(const vector<vector<string>>& state, const vector<vector<vector<string>>>& list) {
        for (const auto& s : list) {
            if (statesEqual(state, s)) return true;
        }
        return false;
    }

    // Finds the row, column position of a tile in the goal state
    vector<int> find_index(const string& num) {
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (goal_state[r][c] == num) {
                    return {r, c};
                }
            }
        }
        return {0, 0};
    }

    // Heuristic h1: Manhattan Distance
    int manhattan_distance(const vector<vector<string>>& puzz) {
        int distance = 0;
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                // Skip the blank tile (represented by "0")
                if (puzz[r][c] != goal_state[r][c] && puzz[r][c] != "0") {
                    vector<int> goal = find_index(puzz[r][c]);
                    distance += (abs(r - goal[0]) + abs(c - goal[1]));
                }
            }
        }
        return distance;
    }

    // Counts number of linear conflicts in rows and columns
    // Used in heuristic h2 = Manhattan + 2×Conflicts
    int count_linear_conflicts(const vector<vector<string>>& puzz) {
        int conflicts = 0;

        // Row conflicts
        for (int r = 0; r < 3; r++) {
            for (int c1 = 0; c1 < 3; c1++) {
                if (puzz[r][c1] == "0") continue;
                vector<int> goal_pos1 = find_index(puzz[r][c1]);
                if (goal_pos1[0] != r) continue; // Tile not in its goal row
                for (int c2 = c1 + 1; c2 < 3; c2++) {
                    if (puzz[r][c2] == "0") continue;
                    vector<int> goal_pos2 = find_index(puzz[r][c2]);
                    if (goal_pos2[0] != r) continue;
                    // Conflict if tiles are reversed relative to goal order
                    if (goal_pos1[1] > goal_pos2[1]) conflicts++;
                }
            }
        }

        //  Column conflicts
        for (int c = 0; c < 3; c++) {
            for (int r1 = 0; r1 < 3; r1++) {
                if (puzz[r1][c] == "0") continue;
                vector<int> goal_pos1 = find_index(puzz[r1][c]);
                if (goal_pos1[1] != c) continue;
                for (int r2 = r1 + 1; r2 < 3; r2++) {
                    if (puzz[r2][c] == "0") continue;
                    vector<int> goal_pos2 = find_index(puzz[r2][c]);
                    if (goal_pos2[1] != c) continue;
                    if (goal_pos1[0] > goal_pos2[0]) conflicts++;
                }
            }
        }

        return conflicts;
    }

    // Heuristic h2: Linear Conflict (Manhattan + 2×conflicts)
    int linear_conflict(const vector<vector<string>>& puzz) {
        return manhattan_distance(puzz) + 2 * count_linear_conflicts(puzz);
    }

    // Selects which heuristic to use (1 = h1, 2 = h2)
    int heuristic(const vector<vector<string>>& puzz) {
        return (heuristic_choice == 1) ? manhattan_distance(puzz) : linear_conflict(puzz);
    }

    // Expands a node: generates its possible children by moving
    // the blank tile (0) up/down/left/right.
    void expand(Node* node) {
        // Mark node as expanded
        expanded.push_back(node);
        viewed_state.push_back(node->state);

        // Remove from frontier
        auto it = find(front.begin(), front.end(), node);
        if (it != front.end()) front.erase(it);

        // Remove from front_state list as well
        for (auto it = front_state.begin(); it != front_state.end(); ++it) {
            if (statesEqual(*it, node->state)) {
                front_state.erase(it);
                break;
            }
        }

        int cost = node->g + 1; // cost to reach children
        vector<Node*> output;
        vector<vector<string>> puzz = node->state;

        // Find position of blank tile (0)
        int row = 0, col = 0;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (puzz[r][c] == "0") { row = r; col = c; }

        // Helper lambda to perform a move and create a child node
        auto tryMove = [&](int newRow, int newCol, const string& move) {
            swap(puzz[row][col], puzz[newRow][newCol]);
            if (!stateInList(puzz, viewed_state)) {
                Node* n = new Node(puzz);
                n->parent = node;
                n->move = move;
                n->g = cost;
                n->h = heuristic(puzz);
                output.push_back(n);
                front.push_back(n);
                front_state.push_back(puzz);
            }
            swap(puzz[row][col], puzz[newRow][newCol]); // Undo move
        };

        // Generate valid moves (L, R, U, D)
        if (col > 0) tryMove(row, col - 1, "L");
        if (col < 2) tryMove(row, col + 1, "R");
        if (row > 0) tryMove(row - 1, col, "U");
        if (row < 2) tryMove(row + 1, col, "D");

        node->child = output;
    }

    // Finds the node in the frontier with the smallest f = g + h
    int return_smallest_fn_index() {
        int index = 0;
        int smallest_f = INT_MAX;
        for (int i = 0; i < front.size(); i++) {
            int f = front[i]->g + front[i]->h;
            if (f < smallest_f) {
                smallest_f = f;
                index = i;
            }
        }
        return index;
    }

    // Main A* search function
    Node* astar_search(const vector<vector<string>>& initial, const vector<vector<string>>& goal) {
        Node* initial_node = new Node(initial);
        initial_node->h = heuristic(initial);

        // Add initial node to frontier
        front.push_back(initial_node);
        front_state.push_back(initial);

        // Start expanding nodes
        expand(initial_node);

        // Continue until goal is found or frontier is empty
        while (!stateInList(goal, front_state)) {
            if (front.empty()) return nullptr;
            expand(front[return_smallest_fn_index()]);
        }

        // Return the goal node
        for (int i = 0; i < front_state.size(); i++) {
            if (statesEqual(front_state[i], goal)) return front[i];
        }
        return nullptr;
    }

public:
    // Reads puzzle states from input file
    // File format:
    //   3 lines = initial state
    //   3 lines = goal state
    void read_input_file(const string& inputFile) {
        ifstream f(inputFile);
        if (!f.is_open()) {
            cerr << "Error opening file: " << inputFile << endl;
            return;
        }

        string line;
        vector<vector<string>> all_rows;
        while (getline(f, line)) {
            if (line.empty()) continue;
            vector<string> row;
            istringstream iss(line);
            string num;
            while (iss >> num) row.push_back(num);
            if (!row.empty()) all_rows.push_back(row);
        }
        f.close();

        // Validate input (should have exactly 6 lines)
        if (all_rows.size() != 6) {
            cerr << "Invalid input format! Expect 3 lines for initial and 3 for goal state." << endl;
            exit(1);
        }

        // Split into initial and goal states
        initial_state.assign(all_rows.begin(), all_rows.begin() + 3);
        goal_state.assign(all_rows.begin() + 3, all_rows.end());
    }

    // Solves the puzzle and writes the results to output file
    void solve(const string& outputFile, int heuristic_type) {
        heuristic_choice = heuristic_type;

        // Validate that states were loaded properly
        if (initial_state.size() != 3 || goal_state.size() != 3) {
            cerr << "Error: Initial or goal state not loaded correctly!" << endl;
            return;
        }

        // Clear previous search data
        expanded.clear();
        front.clear();
        viewed_state.clear();
        front_state.clear();

        // Perform A* search
        Node* goal_node = astar_search(initial_state, goal_state);

        // If goal not found, report failure
        if (!goal_node) {
            cerr << "No solution found!" << endl;
            return;
        }

        // Write output file
        ofstream f(outputFile);

        // Write initial state
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                f << initial_state[r][c];
                if (c < 2) f << " ";
            }
            f << endl;
        }

        f << endl;

        // Write goal state
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                f << goal_state[r][c];
                if (c < 2) f << " ";
            }
            f << endl;
        }

        f << endl;

        // Write depth (solution path length)
        f << goal_node->g << endl;

        // Write total nodes generated
        f << (expanded.size() + front.size()) << endl;

        // Reconstruct path (sequence of moves)
        vector<string> path;
        vector<int> fn_values;
        Node* node = goal_node;
        while (node) {
            fn_values.push_back(node->g + node->h);
            if (node->parent) path.push_back(node->move);
            node = node->parent;
        }

        // Reverse to get moves in correct order
        reverse(path.begin(), path.end());
        reverse(fn_values.begin(), fn_values.end());

        // Write move sequence
        for (int i = 0; i < path.size(); i++) {
            f << path[i];
            if (i < path.size() - 1) f << " ";
        }
        f << endl;

        // Write f(n) values along path
        for (int i = 0; i < fn_values.size(); i++) {
            f << fn_values[i];
            if (i < fn_values.size() - 1) f << " ";
        }
        f << endl;

        f.close();

        // Console output summary
        cout << "Output written to " << outputFile << endl;
        cout << "Depth: " << goal_node->g << endl;
        cout << "Total nodes generated: " << (expanded.size() + front.size()) << endl;
    }

    // Destructor: Free dynamically allocated memory
    ~EightPuzzlesSolver() {
        for (Node* node : expanded) delete node;
        for (Node* node : front) delete node;
    }
};

// Main Function
// Prompts the user for an input file name, extracts its number (e.g. "Input1.txt"),
// runs the solver twice using both heuristics (h1 and h2), and writes results
// to corresponding output files.
int main() {
    cout << "Enter input file name: ";
    string inputFile;
    cin >> inputFile;

    // Extract number from input file name (e.g., "Input1.txt" → "1")
    string baseNum = "";
    for (char c : inputFile) {
        if (isdigit(c)) baseNum += c;
    }

    // Generate output file names
    string outputH1 = "Output" + baseNum + "h1.txt";
    string outputH2 = "Output" + baseNum + "h2.txt";

    // Run with heuristic h1 (Manhattan)
    cout << "\nSolving with h1 (Manhattan Distance)..." << endl;
    EightPuzzlesSolver solver1;
    solver1.read_input_file(inputFile);
    solver1.solve(outputH1, 1);

    // Run with heuristic h2 (Linear Conflict)
    cout << "\nSolving with h2 (Linear Conflict)..." << endl;
    EightPuzzlesSolver solver2;
    solver2.read_input_file(inputFile);
    solver2.solve(outputH2, 2);

    cout << "\nBoth solutions completed!" << endl;
}
