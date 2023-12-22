#ifndef TETRISADAPTER_HPP
#define TETRISADAPTER_HPP

#include "abstract.hpp"

namespace ns_TetrisAdapter {


using namespace ns_Abstract;

template <typename T>
class TetrisAdapter : public AbstractTetris
{
public:
  
    TetrisAdapter(int width, int height);
    void draw() const override;
    void readFromFile(const string& fileName) override;
    void writeToFile(const string& fileName) override;
    void animate(Tetromino tetro, char move_direction, int move_slide) override;
    
    char move_direction() override;
    int move_count(Tetromino tetro, char direction) override;
    int can_moveDown(const Tetromino tetro) override;
    void fill_dots() override;
    int lastMove(const Tetromino tetro) override;
    void move_right(Tetromino tetro, int move_slide) override;
    void move_left(Tetromino tetro, int move_slide) override;
    
    void operator += (const Tetromino& tetro) override;
    int numberOfMoves() const override { return totalMoves; }
    pair<int,int> get_lastMove() const override { return lastmove; }

private:
    T board;
    int width;
    int height;
    int totalMoves=0;
    pair<int,int> lastmove;
};
template class TetrisAdapter< vector<vector<char>> >;           // these are the possible templates
template class TetrisAdapter< vector<deque<char>> >;
template class TetrisAdapter< deque<deque<char>> >;
template class TetrisAdapter< deque<vector<char>> >;


}// end namespace

#endif