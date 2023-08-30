#include <ncurses.h> 
#include <iostream>
#include <chrono>
#include <thread>

#include "3d_tictactoe_ai.hpp"

// move the cursor position to a game board cell i
#define mv_cursor(i) {wmove(game_board[i], height / 2, width / 2); wrefresh(game_board[i]); curr_board_pos =  i;}

using namespace std;

/*
    MAIN PROGRAM of 3D tic-tac-toe:
        This program consists of the terminal interface of the game using ncurses library to implement.
        User has to enter whether he/she plays with computer or another human opponent, then the length of the tic-tac-toe game cube.
        After that, a game cube with the user specified length (let that be n) is generated. The game board is displayed as n squares with dimension n*n.
        User uses the keys 'h' (left), 'j' (down), 'k' (up), 'l' (right) to navigate the game board (yep just like vi editor), and 'f' to place his/her move.
*/

WINDOW **game_board;                                                    // The WINDOW * array storing the window of each cell in the game board
WINDOW *msg_box;                                                        // The WINDOW * storing the window of the message box below the game board, which displays the game result
WINDOW *instruct_box;                                                   // The WINDOW * sotring the window of the instruction box above the game board,
                                                                        // which displays a simple menu for users to enter options and the instructions for nagivating the game board when the game starts

int start_col = 10;                                                     // The character no./row/x-coordinate the first cell of the game board starts at
int start_row = 9;                                                      // The line no./column/y-coordinate the first cell of the game board starts at 
int height = 3, width = 5;                                              // height and width of a cell of the game board must be odd numbers, otherwise move char (ie 'O' or 'X') can't be centered in the cells
int space_betwe_win = 3;                                                // no. of lines separation between windows

int ttt_cube_length = 0;                                                // length of each side of the game cube, to be determined by user
int ttt_cube_sur_area;                                                  // ttt_cube_length to the power 2
int ttt_cube_size;                                                      // ttt_cube_length to the power 3

int curr_move_x, curr_move_y, curr_move_z;                              // The move to be made by the current player
int winpat1_x, winpat1_y, winpat1_z, winpat2_x, winpat2_y, winpat2_z;   // The winning pattern is from (winpat1_z, winpat1_y, winpat1_x) to (winpat2_z, winpat2_y, winpat2_x)
                                                                        // with z being the surface no., y being the row no. and x being the col no.

bool vs_ai = false;                                                     // true -- user vs computer; false -- user vs another user
bool turn = true;                                                       // true -- it's now the turn of 1st player; false -- it's now the turn of 2nd player
char game_result;                                                       // store the game result, can be 'O' (player 1 wins), 'X' (player 2 wins) or '-' draw

void get_player_move(tictactoe &game_obj);
void draw_player(int win_index);
void menu_display();
void highlight_winpat();

int main() {
    int c;

    // initialization: initialize screen w/ ncurses + make sure inputs are not automatically echoed onto the screen
    initscr();
    noecho();

    // initialize WINDOW *instruct_box with space_betwe_win number of lines above the game board
    instruct_box = newwin(start_row - space_betwe_win, 100, 0, 0);

    // allow users to enter options for the game in menu_display() and then output instructions on how to play the game
    menu_display();
    wprintw(instruct_box, "Press 'h' to go left\nPress 'l' to go right\nPress 'j' to go down\nPress 'k' to go right\nPress 'f' to place your move");
    wrefresh(instruct_box);

    // initialize WINDOW *msg_box with space_betwe_win number of lines below the game board
    msg_box = newwin(20, 100, start_row + height * ttt_cube_length + space_betwe_win, 0);

    // initialize the game
    ai_tictactoe ttt(ttt_cube_length);
    
    // allocate memory of a WINDOW * array with ttt_cube_size to game board and initialize each of the WINDOW * indicating each cell on the game board
    game_board = new WINDOW*[ttt_cube_size];    
    for (int i = 0; i < ttt_cube_size; i++) {
        game_board[i] = newwin(height, width, start_row+height*(i/ttt_cube_length%ttt_cube_length), start_col*(i/ttt_cube_sur_area+1)+width*(i%ttt_cube_length)+width*4*(i/ttt_cube_sur_area));
        box(game_board[i], 0, 0);
        wrefresh(game_board[i]);
    }

    // get the players' move alternately until the game ends
    do {
        if (turn) {
            // it's the turn of first player
            get_player_move(ttt);
        } else {
            if (vs_ai) {
                // it's the turn of computer
                // get the most optimal move of computer side
                ttt.best_move(curr_move_x, curr_move_y, curr_move_z);
                draw_player(curr_move_z * ttt_cube_sur_area + curr_move_y * ttt_cube_length + curr_move_x);
            } else {
                // it's the turn of second human player
                get_player_move(ttt);
            }
        }
        // check whether the game has ended and who wins after each move
        game_result = ttt.evaluate(winpat1_x, winpat1_y, winpat1_z, winpat2_x, winpat2_y, winpat2_z);
        turn = !turn;
    } while (game_result == '\0');

    // check what's the result of the game: 'O' -- player 1 wins; 'X' -- player 2 wins; '-' game draws
    // if someone wins, highlight the winning pattern using highlight_winpat()
    wmove(msg_box, 0, 0);
    if (game_result == 'O') {
        wprintw(msg_box, "Player 1 wins.\n");
        highlight_winpat();
    } else if (game_result == 'X') {
        (vs_ai) ? wprintw(msg_box, "Computer wins.\n") : wprintw(msg_box, "Player 2 wins.\n");
        highlight_winpat();
    } else
        wprintw(msg_box, "The game ends with a draw.\n");
    
    // wait for key press to terminate the program
    wprintw(msg_box, "Press any key to quit this session.\n");
    wrefresh(msg_box);
    c = wgetch(msg_box);
    delete[] game_board;
    endwin();
}

// get human player's move, including first player, and second player's move if second player is not computer
// parameters (pass by reference): game_obj -- pass the tictactoe game object declared in main() so that its content can be changed in this func
void get_player_move(tictactoe &game_obj) {
    int add_result;                                                     // indicate whether the operation game_obj.add(...) is successful (value is 0 when successful)
    int c;

    // move the cursor to the first cell
    int curr_board_pos = 0;                                             // indicate current position on the game board, which is game_board[curr_board_pos]
    mv_cursor(0);

    for (;;) {
        // let user navigate the game board until he/she attempts to place a move in one of the cells (ie pressing 'f' on a cell)
        do {
            c = wgetch(game_board[curr_board_pos]);
            
            if (c == 'k' && curr_board_pos >= ttt_cube_length) {
                // cursor moves up when 'k' is pressed and there are cells above it (the last row of cells in the previous surface are considered cells above the first row of cells in the current surface)
                mv_cursor(curr_board_pos - ttt_cube_length);
            } else if (c == 'j' && curr_board_pos < (ttt_cube_size - ttt_cube_length)) {
                // cursor moves down when 'j' is pressed and there are cells below it (the first row of cells in the next surface are considered cells below the last row of cells in the current surface)
                mv_cursor(curr_board_pos + ttt_cube_length);
            } else if (c == 'l') {
                if ((curr_board_pos % ttt_cube_length) < (ttt_cube_length - 1)) {
                    // cursor moves right when 'l' is pressed and there are cells on the right of the current cell in the same surface
                    mv_cursor(curr_board_pos + 1);
                } else if (curr_board_pos < ttt_cube_sur_area * (ttt_cube_length - 1)) {
                    // cursor moves right when 'l' is pressed and there are cells on the right of the current cell in different surface
                    mv_cursor(curr_board_pos + ttt_cube_sur_area - ttt_cube_length + 1);
                }
            } else if (c == 'h') { 
                if ((curr_board_pos % ttt_cube_length) > 0) {
                    // cursor moves left when 'h' is pressed and there are cells on the left of the current cell in the same surface
                    mv_cursor(curr_board_pos - 1);
                } else if (curr_board_pos >= ttt_cube_sur_area) {
                    // cursor moves left when 'h' is pressed and there are cells on the left of the current cell in different surface
                    mv_cursor(curr_board_pos - ttt_cube_sur_area + ttt_cube_length - 1);
                }
            }
        } while (c != 'f');              

        werase(msg_box);
        wrefresh(msg_box);
        
        // get the x, y, z coordinates with z being the surface no., y being the row no. and x being the col no. in the game board
        curr_move_z = curr_board_pos / ttt_cube_sur_area;
        curr_move_y = (curr_board_pos - curr_move_z * ttt_cube_sur_area) / ttt_cube_length;
        curr_move_x = curr_board_pos % ttt_cube_length;
        // attempt to add the user's move in the cell he/she chooses
        if (turn) 
            add_result = game_obj.add('O', curr_move_x, curr_move_y, curr_move_z);
        else 
            add_result = game_obj.add('X', curr_move_x, curr_move_y, curr_move_z);
        
        // if game_obj.add() operation is successful, plot the move on the window
        if (add_result == 0) {
            draw_player(curr_board_pos);
            return;
        }

        // these are implemented only when game_obj.add() is unsuccessful, an error message is displayed
        wmove(msg_box, 0, 0);
        wprintw(msg_box, "This cell is already occupied.\n");
        wrefresh(msg_box);
        mv_cursor(curr_board_pos);
    }
}

// ask for user's input to construct the game
void menu_display() {
    char c;
    bool field_entered = false;                                         // indicate whether the field for user to input length of game cube is entered
    int cursor_y, cursor_x;                                             // indicate the current y and x coordinate of the cursor on the screen
    bool invalid_len = false;                                           // indicate whether the length of game cube inputted by user is invalid

    wprintw(instruct_box, "3D Tic-tac-toe by Elsie Chan\n");

    // user can only input 'e' or 'c' to indicate whether he/she wants to play with another human player or computer
    wprintw(instruct_box, "Press 'e' for two players or 'c' for playing with computer: ");
    do {
        c = wgetch(instruct_box);
    } while (c != 'e' && c != 'c');
    wprintw(instruct_box, "%c\n", c);
    vs_ai = (c == 'c') ? true : false;

    // user can input length of game cube here, but the length should be at least 3
    wprintw(instruct_box, "Enter the length of the sides of this game cube: ");
    getyx(instruct_box, cursor_y, cursor_x);
    for (;;) {
        ttt_cube_length = 0;
        if (invalid_len) {
            // display error message when the length inputted is invalid and let the user input again until a valid length is entered
            wprintw(instruct_box, "\nLength of game cube should be at least 3.");
            wmove(instruct_box, cursor_y, cursor_x);
            clrtoeol();
            wrefresh(instruct_box);
        }

        // read each char from the screen to get ttt_cube_length, which is the length of game cube the user wants to enter
        // user can only input digits ie '0' to '9'
        do {
            c = wgetch(instruct_box);
            if (isdigit(c)) {
                field_entered = true;
                wprintw(instruct_box, "%c", c);
                ttt_cube_length = ttt_cube_length * 10 + (c - '0');
            }
        } while (c != '\n' || !field_entered);

        // user input is valid
        if (ttt_cube_length >= 3)
            break;
        
        invalid_len = true;
    } 
    // get area of each surface in game cube and its total size/volume
    ttt_cube_sur_area = ttt_cube_length * ttt_cube_length;
    ttt_cube_size = ttt_cube_sur_area * ttt_cube_length;

    werase(instruct_box);
}

// plot a user's valid move on the game board
// parameter: win_index -- an index of the WINDOW * in the game_board array indicating the cell in which the move will be placed
void draw_player(int win_index) {
    if (turn)
        mvwprintw(game_board[win_index], height / 2, width / 2, "O");
    else {
        mvwprintw(game_board[win_index], height / 2, width / 2, "X");
        if (vs_ai) {
            // if it's computer's turn, blink the 'X' in the cell for a few times to indicate where computer puts its new move
            for (int i = 0; i < 2; i++) {
                wrefresh(game_board[win_index]);
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                mvwprintw(game_board[win_index], height / 2, width / 2, " ");
                wrefresh(game_board[win_index]);
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                mvwprintw(game_board[win_index], height / 2, width / 2, "X");
            }
        }
    }
    wrefresh(game_board[win_index]);
}

// blink the winning pattern on the game board if the game ends in a win of 'O' or 'X'
void highlight_winpat() {
    int board_pos;                                              // an index of the WINDOW * in the game_board array indicating one of the cells in the winning pattern

    for (int i = 0; i < ttt_cube_length; i++) {
        /* calculate the board_pos according to the z (surface no.), y (row no.), x (col no.) coordinates of the cell */

        // account z-coor/surface no. of the cell into board_pos
        // winpat1_z can only be equal to or smaller than winpat2_z as board_pos of winpat1_x,y,z must be smaller than that of winpat2_x,y,z
        board_pos = (winpat2_z == winpat1_z) ? (winpat1_z * ttt_cube_sur_area) : ((winpat1_z + i) * ttt_cube_sur_area);
        
        // account y-coor/row no. of the cell into board_pos
        if (winpat1_y < winpat2_y)
            board_pos += (winpat1_y + i) * ttt_cube_length;
        else if (winpat1_y > winpat2_y)
            board_pos += (winpat1_y - i) * ttt_cube_length;
        else
            board_pos += winpat1_y * ttt_cube_length;

        // account x-coor/col no. of the cell into board_pos
        if (winpat1_x < winpat2_x)
            board_pos += winpat1_x + i;
        else if (winpat1_x > winpat2_x)
            board_pos += winpat1_x - i;
        else
            board_pos += winpat1_x;
    
        // set attribute of that cell to A_BLINK and display the content (ie 'X' or 'O') again
        // note: since this func is called only after game ends, the bool turn variable is flipped b4 processing game result, that's why 'X' is printed when turn is true and 'O' when turn is false
        wattron(game_board[board_pos], A_BLINK);
        if (turn)
            mvwprintw(game_board[board_pos], height / 2, width / 2, "X");
        else
            mvwprintw(game_board[board_pos], height / 2, width / 2, "O");
        wrefresh(game_board[board_pos]);
    }
}