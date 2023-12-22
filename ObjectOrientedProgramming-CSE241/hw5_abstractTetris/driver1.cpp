#include "tetrisvector.hpp"
#include "tetrisarray.hpp"
#include "tetrisadapter.hpp"
#include "tetrisadapter.cpp"

using namespace ns_TetrisVector;
using namespace ns_TetrisArray;
using namespace ns_TetrisAdapter;

void Game(AbstractTetris *tetris_obj);	
void GameException(AbstractTetris *tetris_obj);

int main() {	
	
	srand(time(0));	

	int width_inp=10, height_inp=10;	
	char myDirection;
	int myMoveCount, i=0;
	Tetromino myTetro;

	AbstractTetris* tetris_obj_ptr;
	
   	

	tetris_obj_ptr = new TetrisAdapter< vector<vector<char>> >(10,10);
	GameException(tetris_obj_ptr);											// throws exception because of .txt file not found (Deliberatly not created) 

	tetris_obj_ptr = new TetrisVector(10, 10);
	Game(tetris_obj_ptr);

   	tetris_obj_ptr = new TetrisArray(10, 10);
	Game(tetris_obj_ptr);

	tetris_obj_ptr = new TetrisAdapter< vector<deque<char>> >(10,10);
	Game(tetris_obj_ptr);

	tetris_obj_ptr = new TetrisAdapter< deque<deque<char>> > (10, 10);	
	Game(tetris_obj_ptr);
						//TetrisAdapter< deque<vector<char>> > (width_inp, height_inp);	//Program can also use these template
						//TetrisAdapter< vector<vector<char>> > (width_inp, height_inp);	
						//TetrisAdapter< vector<deque<char>> > (width_inp, height_inp);

	return 0;
}
void Game(AbstractTetris *tetris_obj){
	
	int width_inp=10, height_inp=10;	
	char myDirection;
	int myMoveCount, i=0;
	Tetromino myTetro;
	char TypeTetro_type[4];
	TypeTetro_type[0] = 'Z';
	TypeTetro_type[1] = 'T';
	TypeTetro_type[2] = 'S';
	TypeTetro_type[3] = 'O';


	cout << "____Tetris game  is starting____" << endl << endl;
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	cout << "--------fill_dots()-draw()----------" << endl;
	(*tetris_obj).fill_dots();
	(*tetris_obj).draw();
	cout << "------------------------" << endl;
	cout << "----- set_shape()---operator +=() ----------" << endl;
	myTetro.set_shape( myTetro.get_TypeTetro(TypeTetro_type[0]) );
	(*tetris_obj) += myTetro;
	(*tetris_obj).draw();
	(*tetris_obj).fill_dots();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	cout << "---------  move_left()  ---------------" << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).move_left(myTetro, 2);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).fill_dots();
	(*tetris_obj).draw();
	cout << "---------  move_right()  ---------------" << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).move_right(myTetro, 2);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).fill_dots();
	(*tetris_obj).draw();
	
	while (1){
	try {
		
		myTetro.set_shape( myTetro.get_TypeTetro(TypeTetro_type[i]) );
		cout << "---------  rotate()  ---------------" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		myTetro.rotate( 'l', 1 ); 
		if((*tetris_obj).lastMove(myTetro)) { break; }				// controls if game is over
		(*tetris_obj) += myTetro;
		(*tetris_obj).draw();

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		cout << "----------lastmove()--animate()------------" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		myDirection = 'r';
		myMoveCount = 2;
		(*tetris_obj).animate(myTetro, myDirection, myMoveCount);	
		(*tetris_obj).draw();
		(*tetris_obj).writeToFile("myText.txt");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if((*tetris_obj).lastMove(myTetro)) { break; }				// controls if game is over
		i++;
		i = i%4;	
	}
	catch (string fileName){
		cout << fileName << " is invalid" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}	
	catch (...){
		break;
	}	
	}
	(*tetris_obj).readFromFile("myText.txt");
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	i=0;
}
void GameException(AbstractTetris *tetris_obj){
	
	int width_inp=10, height_inp=10;	
	char myDirection;
	int myMoveCount, i=0;
	Tetromino myTetro;
	char TypeTetro_type[4];
	TypeTetro_type[0] = 'O';
	TypeTetro_type[1] = 'T';
	TypeTetro_type[2] = 'S';
	TypeTetro_type[3] = 'L';
	
	cout << "____Tetris game is starting____" << endl << endl;
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	cout << "--------fill_dots()-draw()----------" << endl;
	(*tetris_obj).fill_dots();
	(*tetris_obj).draw();
	cout << "------------------------" << endl;
	cout << "----- set_shape()---operator +=() ----------" << endl;
	myTetro.set_shape( myTetro.get_TypeTetro(TypeTetro_type[0]) );
	(*tetris_obj) += myTetro;
	(*tetris_obj).draw();
	(*tetris_obj).fill_dots();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	cout << "---------  move_left()  ---------------" << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).move_left(myTetro, 2);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).fill_dots();
	(*tetris_obj).draw();
	cout << "---------  move_right()  ---------------" << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).move_right(myTetro, 2);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	(*tetris_obj).fill_dots();
	(*tetris_obj).draw();
	
	while (1){
	try {
		
		myTetro.set_shape( myTetro.get_TypeTetro(TypeTetro_type[i]) );
		cout << "---------  rotate()  ---------------" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		myTetro.rotate( 'l', 1 ); 
		if((*tetris_obj).lastMove(myTetro)) { break; }				// controls if game is over
		(*tetris_obj) += myTetro;
		(*tetris_obj).draw();

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		cout << "----------lastMove()--animate()------------" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		myDirection = 'r';
		myMoveCount = 2;
		(*tetris_obj).animate(myTetro, myDirection, myMoveCount);	
		(*tetris_obj).draw();
		// (*tetris_obj).writeToFile("myText.txt");						// deliberately commented for exception
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if((*tetris_obj).lastMove(myTetro)) { break; }				// controls if game is over
		i++;
		i = i%4;	
		(*tetris_obj).readFromFile("myText.txt");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	catch (string fileName){
		cout << fileName << " is couldnt opened : throwed exception" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}	
	}
	i=0;
}