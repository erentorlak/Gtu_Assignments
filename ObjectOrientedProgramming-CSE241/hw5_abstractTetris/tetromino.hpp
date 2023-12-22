#ifndef TETROMINO_HPP
#define TETROMINO_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <vector>
#include <algorithm>
#include <deque>
#include <typeinfo>

namespace ns_Tetromino {

using namespace std;

enum class TypeTetro : char
{
    I = 'I',
    O = 'O',
    T = 'T',
    J = 'J',
    L = 'L',
    S = 'S',
    Z = 'Z',
};

class Tetromino {                                 
   
    public:
        Tetromino(); //no-parameter constructor
        Tetromino(TypeTetro type); //constructor

        TypeTetro get_TypeTetro(char a);
    
        void set_shape(TypeTetro inp_TypeTetro);   

        char rot_direction();         // takes input from user for rotation direction
        int rot_count();              // takes input from user for rotation num

        void transpose();
        void mirror_y();
        void rotate(char direction, int count);     // rotates tetro
        
        int get_tetro_height() const { return tetro_height; };
        int get_tetro_width() const { return tetro_width; };
        vector<vector<char>> get_tetros_shape() const { return tetros_shape; };
        
        int head_coordinate_x;       //carrys tetros [0][0] coordinates       
        int head_coordinate_y;       //carrys tetros [0][0] coordinates   

    private:
        TypeTetro myType;
        int tetro_width;
        int tetro_height;
        vector<vector<char>> tetros_shape;
};


}// end namespace

#endif