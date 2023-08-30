#include <iostream>
#include "3d_tictactoe.hpp"

class ai_tictactoe : public tictactoe {
    private:
        int max_depth;                                                          // max_depth+1 = max no. of steps taken by both comp & rival to look ahead
        int minimax(int depth, bool is_max, int alpha, int beta);
        int move_score();
    public:
        ai_tictactoe(int s);
        // default destructor is used
        void best_move(int &move_x, int &move_y, int &move_z);
};