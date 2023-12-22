#ifndef TETRISARRAY_HPP
#define TETRISARRAY_HPP

#include "abstract.hpp"

namespace ns_TetrisArray {

using namespace ns_Abstract;

class TetrisArray : public AbstractTetris
{
public:

    TetrisArray(int width, int height);
    void draw() const override;
    void readFromFile(const string& fileName) override;
    void writeToFile(const string& fileName) override;
    void animate(Tetromino tetro, char move_direction, int move_slide) override;
  
    
    char move_direction();
    int move_count(Tetromino tetro, char direction);
    int can_moveDown(const Tetromino tetro);
    void fill_dots();
    int lastMove(const Tetromino tetro) override;
    void move_right(Tetromino tetro, int move_slide) override;
    void move_left(Tetromino tetro, int move_slide) override;
    void operator += (const Tetromino& tetro);
    ~TetrisArray();
   // pair<int,int> lastMove() const override;
   int numberOfMoves() const override { return totalMoves; }
   pair<int,int> get_lastMove() const override { return lastmove; }
private:
    char* board;
    int width;
    int height;
    int totalMoves=0;
    pair<int,int> lastmove;

};

}// end namespace

#endif