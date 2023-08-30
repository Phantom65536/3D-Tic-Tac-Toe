#include "3d_tictactoe.hpp"

/* 
    class tictactoe by Elsie Chan:
        This class uses a 3D character array to represent the game board.
        This game comprises of two players: 'O' refers to the first player and 'X' refers to the second player
        This class allows gameplay of any length of the tic-tac-toe cube, regardless of 3x3x3, 4x4x4 or even 5x5x5 -- you only have to define it when contructing an instance of this class
*/

// only used by macros check_row_col, check_diag_per_suface and check_diag_across_surface
#define check_cell      if (board[z][y][x] != curr_player) {found = false; break;}
#define win_pat_found   first_x = curr_x; first_y = curr_y; first_z = curr_z; last_x = x; last_y = y; last_z = z; return (curr_player == 'O') ? 'O' : 'X' ;

// when two of the x, y, z variables are held constant, check winning pattern by looping through all values of the remaining variable
// parameters: first, second, third -- order of x, y, z variables to be put into the for loops eg check_row_col(z, y, x)
#define check_row_col(first, second, third) for (first = 0; first < SIZE; first++) {                                                    \
                                                for (second = 0; second < SIZE; second++) {                                             \
                                                    third = 0;                                                                          \
                                                    found = true;                                                                       \
                                                    curr_player = board[z][y][x];                                                       \
                                                    curr_x = x; curr_y = y; curr_z = z;                                                 \
                                                    if (curr_player != '_') {                                                           \
                                                        for (third = 1; third < SIZE; third++)                                         \
                                                            check_cell;                                                                 \
                                                        if (found) {                                                                \
                                                            third--;                                                                    \
                                                            win_pat_found;                                                          \
                                                        }                                                                                     \
                                                    }                                                                                   \
                                                }                                                                                       \
                                            }                                                                                           

// when one of the x, y, z variables are held constant, check winning pattern by looping through all values of the remaining two variables in one go
// with one variable increasing and another decreasing or both variable increasing
// parameters: first, second, third -- order of x, y, z variables to be put into the for loops eg check_diag_per_surface(y, z, x, )
//             third_order -- whether the value of x, y or z variable substituting third increases (third_order = +1) or decreases (third_order = -1) through the loop for checking winning pattern
#define check_diag_per_suface(first, second, third, third_order)    for (first = 0; first < SIZE; first++) {                                                  \
                                                                        second = 0;                                                                   \
                                                                        third = (third_order == +1) ? 0 : SIZE-1;                                   \
                                                                        found = true;                                                                           \
                                                                        curr_player = board[z][y][x];                                                           \
                                                                        curr_x = x; curr_y = y; curr_z = z;                                             \
                                                                        if (curr_player != '_') {                                                                  \
                                                                            for (second = 1, third += third_order; second < SIZE; second++, third += third_order)                         \
                                                                                check_cell;                                                                 \
                                                                            if (found) {                                                                \
                                                                                third -= third_order; second--;                                             \
                                                                                win_pat_found;                                                          \
                                                                            }                                                                 \
                                                                        }                                                                          \
                                                                    }

// when none of x, y, z var are held constant, check winning pattern by looping though all values of x, y, z in one go
// with z increasing, y increasing/decreasing and x increasing/decreasing
// parameters: y_order -- whether the value of y increases (y_order = +1) or decreases (y_order = -1) in the loop
//             x_order -- whether the value of x increases (y_order = +1) or decreases (y_order = -1) in the loop
#define check_diag_across_surface(y_order, x_order)     y = (y_order == +1) ? 0 : SIZE-1;                                                             \
                                                        x = (x_order == +1) ? 0 : SIZE-1;                                                             \
                                                        found = true;                                                                               \
                                                        curr_player = board[0][y][x];                                                               \
                                                        curr_x = x; curr_y = y; curr_z = 0;                                                         \
                                                        if (curr_player != '_') {                                                                   \
                                                            for (z = 1, y += y_order, x += x_order; z < SIZE; z++, y += y_order, x += x_order)      \
                                                                check_cell;                                                                         \
                                                            if (found) {                                                                \
                                                                z--; y -= y_order; x -= x_order;                                             \
                                                                win_pat_found;                                                          \
                                                            }                                                                          \
                                                        }                                                                                           

using namespace std;

struct invalid_size : public exception {
    const char * what () const throw () {
        return "The no. of rows and columns of tic-tac-toe should be at least 3.";
    }
};

// User define 
tictactoe::tictactoe(int s) {
    // size of tic-tac-toe should be at least 3x3x3, otherwise it won't make sense
    if (s < 3)
        throw invalid_size();

    // create a 3D array board[SIZE][SIZE][SIZE]
    SIZE = s;
    board = new char**[SIZE];
    for (int i = 0; i < SIZE; i++) {
        board[i] = new char*[SIZE];
        for (int j = 0; j < SIZE; j++)
            board[i][j] = new char[SIZE];
    }

    // Assign '_' (ie empty) to each element of board
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            for (int k = 0; k < SIZE; k++)
                board[i][j][k] = '_';
}

tictactoe::~tictactoe() {
    // Deallocate memory of board (dynamic 3d array)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            delete[] board[i][j];
        }
        delete[] board[i];
    }
    delete[] board;
}

// parameters: player -- current player; x -- x-coor of board; y -- y-coor of board; z -- z-coor of board
// return code: 0 -- operation successful; 1 -- board[z][y][x] occupied (by self or opponent); 2 -- x, y or z is/are out of boundary; 3 -- invalid input for player var
int tictactoe::add(char player, int x, int y, int z) {
    if (player == 'O' || player == 'X' || player == 'o' || player == 'x') {
        if (x >= 0 && x < SIZE && y >= 0 && y < SIZE && z >= 0 && z < SIZE) {
            if (board[z][y][x] == '_') {
                board[z][y][x] = toupper(player);
                return 0;
            } 
            return 1;
        }
        return 2;
    }
    return 3;
}

// check if the game draws by checking whether all elements of board is occupied (by self or opponent)
// return code: true -- game ends at draw; false -- game hasn't reached tie
bool tictactoe::is_draw() {
    int x, y, z;                                                    // temp var for x, y, z coordinates on game board
    for (x = 0; x < SIZE; x++)
        for (y = 0; y < SIZE; y++)
            for (z = 0; z < SIZE; z++)
                if (board[z][y][x] == '_')
                    return false;
    return true;
}

// find out who wins, and whether the game should continue
// return code: 'O' -- player 1 wins; 'X' -- player 2 wins; '\0' -- nobody wins; '-' -- game ends in tie
// parameters (all pass by reference): indicate the coordinates of the first and last move in the winning sequence
// Note: if '\0' or '-' is returned, the value of the parameters are left untempered 
char tictactoe::evaluate(int &first_x, int &first_y, int &first_z, int &last_x, int &last_y, int &last_z) {
    int x, y, z;                                                    // temp var for x, y, z coordinates on game board
    char curr_player;                                               
    int curr_x, curr_y, curr_z;                                     // coordinates of the move of curr_player
    bool found;                                                     // bool to indicate whether a winning pattern is found
    
    // check each row and column to find out if the whole row/col is linked by a certain player
    check_row_col(z, y, x);
    check_row_col(z, x, y);
    check_row_col(y, x, z);

    // check each diagonal between vertexes of the same surface to find out if the whole diag is linked by a certain player
    check_diag_per_suface(z, y, x, +1);
    check_diag_per_suface(z, y, x, -1);
    check_diag_per_suface(y, z, x, +1);
    check_diag_per_suface(y, z, x, -1);
    check_diag_per_suface(x, z, y, +1);
    check_diag_per_suface(x, z, y, -1);

    // check each diagonal between vertexes of different sufaces to find out if the whole diag is linked by a certain player
    check_diag_across_surface(+1, +1);
    check_diag_across_surface(+1, -1);
    check_diag_across_surface(-1, +1);
    check_diag_across_surface(-1, -1);

    // check if a draw is reached
    if (is_draw()) 
        return '-';
    
    // no win pattern or draw is found
    return '\0';
}