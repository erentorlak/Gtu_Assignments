#include "tetris.hpp"

using namespace ns_Tetromino;
using namespace ns_Tetris;

int main() {	

	srand(time(0));	
	char TypeTetro_type;
	int num;
	int width_inp=10, height_inp=10;

	Tetromino myTetro;

	Tetris tetris_obj(width_inp, height_inp);	
	
	cout << "--------------------------------" << endl;
	tetris_obj.fill_dots();
	tetris_obj.draw();
	cout << "--------------------------------" << endl;

	while (1){

		myTetro.set_shape( myTetro.get_TypeTetro(TypeTetro_type) );

		tetris_obj.animate(myTetro);	
	}
	
	return 0;
}