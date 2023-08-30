#include <iostream>

class tictactoe {
    protected:
        int SIZE;                                                   // length of tic-tac-toe cube
        char ***board;                                              // dynamic 3D array with size board[SIZE][SIZE][SIZE] to save the game board
                                                                    // each element in board can be either '_' (empty), 'O' (player 1), 'X' (player 2)
        bool is_draw();
    public:
        tictactoe(int s);
        virtual ~tictactoe();
        int add(char player, int x, int y, int z);
        char evaluate(int &first_x, int &first_y, int &first_z, int &last_x, int &last_y, int &last_z);
};