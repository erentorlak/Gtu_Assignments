#ifndef ABSTRACT_HPP
#define ABSTRACT_HPP

#include "tetromino.hpp"

namespace ns_Abstract {

using namespace ns_Tetromino;

class AbstractTetris
{
public:
    virtual void draw() const  = 0;
    virtual void readFromFile(const string& fileName) = 0;
    virtual void writeToFile(const string& fileName) = 0;
    virtual void operator+= (const Tetromino& t) = 0;
    virtual void animate(Tetromino tetro, char move_direction, int move_slide) = 0;
    virtual int lastMove(const Tetromino tetro) = 0;
    virtual void move_left(Tetromino tetro, int move_slide) = 0;
    virtual void move_right(Tetromino tetro, int move_slide) = 0;
    virtual pair<int,int> get_lastMove() const = 0;
    virtual int numberOfMoves() const =0;
    virtual char move_direction() = 0;
    virtual int move_count(Tetromino tetro, char direction) = 0;
    virtual int can_moveDown(const Tetromino tetro) = 0;
    virtual void fill_dots() = 0 ;
    
};


}// end namespace

#endif