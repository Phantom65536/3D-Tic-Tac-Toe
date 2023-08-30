#include "3d_tictactoe_ai.hpp"
using namespace std;

/*
    class ai_tictactoe by Elsie Chan:
        minimax with alpha-beta pruning is used to find optimal move
        warning: time used increases exponentially when the length of each side of the game cube increases by 1
        Difficulty to beat this AI: Medium (Well, you can beat it if you have some brain cells)
*/

// set the value of max_depth according to s so that it won't take too long for minimax to return a move
// parameters: s -- define length of the tic-tac-toe cube (which is passed to class tictactoe constructor)
ai_tictactoe::ai_tictactoe(int s) : tictactoe(s){
    if (s == 3)
        max_depth = 4;
    else if (s == 4)
        max_depth = 3;
    else
        max_depth = 1;
    srand(time(0));
}

// determine the score of game board according to who wins
// return value: +10 -- 'O' wins (maximizer); -10 -- 'X' wins (minimizer); -1 -- draw; 0 -- game hasn't ended and nobody wins
int ai_tictactoe::move_score() {
    int a, b, c, d, e, f;                                       // temp var for evaluate() func
    char result = evaluate(a, b, c, d, e, f);

    if (result == 'O')                              // 'O', ie maximizer, wins
        return +10; 
    else if (result == 'X')                         // 'X', ie minimizer, wins
        return -10;
    else if (result == '-')                         // Game ends in a draw
        return -1;                                  // returns -1 instead of 0 to differentiate it from game hasn't ended and nobody wins
    else                                            // '\0' is returned by evaluate, so game hasn't ended and nobody wins
        return 0;
}

// use of minimax with alpha-beta pruning to find the best move
// maximizer is 'O'; minimizer is 'X'
// return value: The score of a move
// parameters: depth -- what is the level of the current func call; is_max -- true if it's maximizer's turn false if it's minimizer's turn; 
//             alpha -- the smallest value maximizer can guarantee; beta -- the largest value minimizer can guarantee
int ai_tictactoe::minimax(int depth, bool is_max, int alpha, int beta) {
    int x, y, z;                                                // temp var for x, y, z coordinates on game board
    

    // obtain the score of a move
    int score = move_score();

    // if a win is reached, fewer the steps taken, higher the score in amplitude
    if (score == 10 || score == -10) {
        return score * (max_depth + 2 - depth);
    }

    // disregard no. of steps taken when game draws
    // return 0 because even though game ends nobody wins, meaning it carries the same significance as when the game hasn't ended and no player wins (evaluate() returns '\0')
    if (score == -1)
        return 0;

    // make sure it won't take too long to return a move by limiting the steps to look ahead
    if (depth <= max_depth) {
        if (is_max) {
            // maximizer's move
            for (z = 0; z < SIZE; z++) {
                for (y = 0; y < SIZE; y++) {
                    for (x = 0; x < SIZE; x++) {
                        // find out the score using minimax if the step board[z][y][x] is added
                        if (add('O', x, y, z) == 0) {
                            // for maximizer, higher the score better the move
                            alpha = max(alpha, minimax(depth+1, !is_max, alpha, beta));
                            board[z][y][x] = '_';
                            // if condition is true, the value of maximizer must be larger than that of minimizer so there is no reason to loop through the board
                            if (alpha >= beta) {
                                return alpha;
                            }
                        }
                    }
                }
            }
        } else {
            // minimizer's move
            for (z = 0; z < SIZE; z++) {
                for (y = 0; y < SIZE; y++) {
                    for (x = 0; x < SIZE; x++) {
                        // find out the score using minimax if the step board[z][y][x] is added
                        if (add('X', x, y, z) == 0) {
                            // for minimizer, lower the score better the move
                            beta = min(beta, minimax(depth+1, !is_max, alpha, beta));
                            board[z][y][x] = '_';
                            // if condition is true, the value of maximizer must be larger than that of minimizer so there is no reason to loop through the board
                            if (alpha >= beta) {
                                return beta;
                            }
                        }
                    }
                }
            }
        }
        return (is_max) ? alpha : beta;
    }
    // no win is found
    return 0;
}

// Computer side is minimizer, so seek the minimum value in minimax
// parameters (all pass by reference): move_x, move_y, move_z -- indicates the x, y, z coordinates of the move this AI is gonna take computed by this func
void ai_tictactoe::best_move(int &move_x, int &move_y, int &move_z) {
    int best_x, best_y, best_z;                             // describe coordinates of the best point on the game board to plot
    int x, y, z;                                            // temp var for x, y, z coordinates on game board
    int min_val = 1000, val;                                // min_val -- minimizer find the min minimax value for all possible steps to be taken
    bool first_time = true;                                 // first time to pass through statements in if (val < min_val)
    bool all_val_same = true;                               // values returned by minimax for every possible move of 'X' are same with each other
                                                            // ie no true best move can be found
    
    for (z = 0; z < SIZE; z++) {
        for (y = 0; y < SIZE; y++) {
            for (x = 0; x < SIZE; x++) {
                if (add('X', x, y, z) == 0) {
                    // find the best move computer should take by seeking the minimum score of all possible moves
                    val = minimax(0, true, -100000, 100000);
                    board[z][y][x] = '_';
                    if (val < min_val) {
                        if (first_time) 
                            first_time = false;
                        else
                            // the value returned by the curr minimax is different from the true min_value
                            all_val_same = false;
                        min_val = val;
                        best_x = x;
                        best_y = y;
                        best_z = z;
                    } else if (val != min_val) {
                        // the value returned by the curr minimax is different from the true min_value
                        all_val_same = false;
                    }
                }
            }
        }
    }

    // no true best move can be found
    if (all_val_same) {
        // fill center of cube first, but if best_x, best_y and best_z obtained is occupied, 
        // instead of filling another center, it fills any available space on the game board
        best_x = rand() % (SIZE - 2) + 1;
        best_y = rand() % (SIZE - 2) + 1;
        best_z = rand() % (SIZE - 2) + 1;
        while (add('X', best_x, best_y, best_z) != 0) {
            best_x = rand() % (SIZE) + 1;
            best_y = rand() % (SIZE) + 1;
            best_z = rand() % (SIZE) + 1;
        }
    } else
        // best move is found, so just add it to board
        add('X', best_x, best_y, best_z);
    
    // let the caller knows the best move computed by this func
    move_x = best_x;
    move_y = best_y;
    move_z = best_z;
}