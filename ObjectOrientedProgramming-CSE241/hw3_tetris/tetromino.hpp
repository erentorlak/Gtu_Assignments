#ifndef TETROMINO_HPP
#define TETROMINO_HPP

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>

namespace ns_Tetromino {

using std::cout;
using std::cin;
using std::endl;
using std::string;


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

        static char rot_direction();         // takes input from user for rotation direction
        static int rot_count();              // takes input from user for rotation num

        void transpose(void);
        void mirror_y(void);
        void rotate(char direction, int count);     // rotates tetro
        
        int get_tetro_height() const { return tetro_height; };
        int get_tetro_width() const { return tetro_width; };
        char** get_tetros_shape() const { return tetros_shape; };
        
        int head_coordinate_x;       //carrys tetros [0][0] coordinates       
        int head_coordinate_y;       //carrys tetros [0][0] coordinates   
        //~Tetromino();

    private:
        TypeTetro myType;
        int tetro_width;
        int tetro_height;
        char** tetros_shape;
};


}// end namespace

#endif