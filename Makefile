SOURCE_FILES := main.cpp 3d_tictactoe.cpp 3d_tictactoe_ai.cpp
OBJ_FILES := main.o 3d_tictactoe.o 3d_tictactoe_ai.o	
HEADER_FILES := 3d_tictactoe.hpp 3d_tictactoe_ai.hpp

main:		$(SOURCE_FILES) $(OBJ_FILES) $(HEADER_FILES)	
			g++ -lncurses $(OBJ_FILES) -o ./main

main.o:		3d_tictactoe_ai.o
			g++ -c main.cpp -o main.o

3d_tictactoe_ai.o:	3d_tictactoe.o 3d_tictactoe_ai.hpp 3d_tictactoe_ai.cpp
					g++ -c 3d_tictactoe_ai.cpp -o 3d_tictactoe_ai.o

3d_tictactoe.o:		3d_tictactoe.hpp 3d_tictactoe.cpp
					g++ -c 3d_tictactoe.cpp -o 3d_tictactoe.o
