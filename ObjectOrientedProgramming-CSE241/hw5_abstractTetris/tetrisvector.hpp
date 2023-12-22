#ifndef TETRISVECTOR_HPP
#define TETRISVECTOR_HPP

#include "abstract.hpp"

namespace ns_TetrisVector {

using namespace ns_Abstract;

class TetrisVector : public AbstractTetris
{
public:
   
    TetrisVector(int width, int height);
    void draw() const override;
    void readFromFile(const string& fileName) override;

    void writeToFile(const string& fileName) override;
    void animate(Tetromino tetro, char move_direction, int move_slide) override;
  
    
    char move_direction();
    int move_count(Tetromino tetro, char direction);
    int can_moveDown(const Tetromino tetro);
    void fill_dots();

    void operator += (const Tetromino& tetro);
    
    int lastMove(const Tetromino tetro) override;
    void move_left(Tetromino tetro, int move_slide) override;
    void move_right(Tetromino tetro, int move_slide) override;

    int numberOfMoves() const override { return totalMoves; }
    pair<int,int> get_lastMove() const override { return lastmove; }
private:
    vector<vector<char>> board;
    int width;
    int height;
    int totalMoves=0;
    pair<int,int> lastmove;
};  


}// end namespace

#endif