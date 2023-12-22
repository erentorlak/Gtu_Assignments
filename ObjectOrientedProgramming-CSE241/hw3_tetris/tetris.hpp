#ifndef TETRIS_HPP
#define TETRIS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>
#include "tetromino.hpp"

namespace ns_Tetris {


using namespace ns_Tetromino;

class Tetris {

    public:
        
        Tetris(int width_inp, int height_inp);


        char move_direction();                                              // takes input from user for direction
        int move_count(Tetromino const tetro,char move_direction);         // takes input from user for move 

        void draw() const;
        void fill_dots();                                     // fills fitground with '.'
        

        
        
        int can_moveDown(Tetromino const tetro);        
        
        void operator+=(Tetromino const tetro);         // adds tetro to the board
        
        void animate(Tetromino const tetro);            // animates the tetros

        ~Tetris();
    private:

        int height;
        int width;
        char** fitGround;
};

}// end namespace

#endif