#include "tetrisvector.hpp"
#include "tetrisarray.hpp"
#include "tetrisadapter.hpp"
#include "tetrisadapter.cpp"

using namespace ns_TetrisVector;
using namespace ns_TetrisArray;
using namespace ns_TetrisAdapter;

void Game(AbstractTetris *tetris_obj);

int main()
{	
	
	char TypeTetro_type;
	int num,i=0;
	int width_inp, height_inp;
 	char myDirection, type;
    int myMoveCount;
	string input = "invalid input";
	
	Tetromino myTetro;		
	
	while(1) {
        cout << "Enter Width :" << endl;
        if (cin >> width_inp && width_inp >= 6) {
            break;
        } 
        else {
            cout << "Enter a number bigger than 6 !!! " << endl;
            cin.clear();
            cin.ignore(80, '\n');
        }
    }
	while(1) {
        cout << "Enter Height :" << endl;
        if (cin >> height_inp && height_inp >= 6) {
            break;
        } 
        else {
            cout << "Enter a number bigger than 6 !!! " << endl;
            cin.clear();
            cin.ignore(80, '\n');
        }
    }	
	while(1) {
        cout << "Enter type of the Tetris class (V for vector, 1 for array1D, A for adaptor) : " << endl;
        if (cin >> type && (type == 'V' || type == '1' || type == 'A') ) {
            break;
        } 
        else {
            cout << "Enter V for vector, 1 for array1D, A for adaptor !!! :  " << endl;
            cin.clear();
            cin.ignore(80, '\n');
        }
    }
	
	
	AbstractTetris* tetris_obj_ptr;
	
	if (type=='V'){
		tetris_obj_ptr = new TetrisVector(width_inp, height_inp);
	
		Game(tetris_obj_ptr);
	}
																																																	
	if (type=='1'){
   		tetris_obj_ptr = new TetrisArray(width_inp, height_inp);
		Game(tetris_obj_ptr);
	}
	if (type=='A'){
		tetris_obj_ptr = new TetrisAdapter< deque<deque<char>> > (width_inp, height_inp);	
							//TetrisAdapter< deque<vector<char>> > tetris_obj(width_inp, height_inp);	//Program can also use these template
							//TetrisAdapter< vector<vector<char>> > tetris_obj(width_inp, height_inp);	
							//TetrisAdapter< vector<deque<char>> > tetris_obj(width_inp, height_inp);	
		Game(tetris_obj_ptr);
	}

	return 0;
}	
void Game(AbstractTetris *tetris_obj){
	
	int width_inp, height_inp;	
	char myDirection;
	int myMoveCount,num;
	Tetromino myTetro;
	char TypeTetro_type;
	string input = "invalid input";

	(*tetris_obj).fill_dots();
	(*tetris_obj).draw();
	
	while (1){
	try {
		cout << "Enter the tetromino type : " << endl;
		cin >> TypeTetro_type;
		if (TypeTetro_type == 'I' || TypeTetro_type == 'O' || TypeTetro_type == 'T' || TypeTetro_type == 'J' 
			|| TypeTetro_type == 'L' || TypeTetro_type == 'S' || TypeTetro_type == 'Z'|| TypeTetro_type == 'R'|| 
			TypeTetro_type == 'Q'){

			if( TypeTetro_type == 'Q')
				exit(5);
			if( TypeTetro_type == 'R'){
				srand(time(0));	
    			num = (rand() % 7) ;   			
				if(num == 0)
					TypeTetro_type = 'I';				
				else if(num == 1)
					TypeTetro_type = 'O';			
  				else if(num == 2)
					TypeTetro_type = 'T';				
				else if(num == 3)
					TypeTetro_type = 'J';				
				else if(num == 4)
					TypeTetro_type = 'L';				
				else if(num == 5)
					TypeTetro_type = 'S';				
				else if(num == 6)
					TypeTetro_type = 'Z';	
			}
		}	
		else	
			throw string(1,TypeTetro_type);	
		
		myTetro.set_shape( myTetro.get_TypeTetro(TypeTetro_type ));	
		myTetro.rotate(myTetro.rot_direction(), myTetro.rot_count() ); 
		if((*tetris_obj).lastMove(myTetro)) { break; }				// controls if game is over
		(*tetris_obj) += myTetro;
		(*tetris_obj).draw();
		myDirection = (*tetris_obj).move_direction();					
		myMoveCount = (*tetris_obj).move_count(myTetro, myDirection);	
		(*tetris_obj).animate(myTetro, myDirection, myMoveCount);	
		(*tetris_obj).draw();
		(*tetris_obj).writeToFile("myText.txt");
		if((*tetris_obj).lastMove(myTetro)) { break; }				// controls if game is over
	}
	catch (string fileName){
		cout << fileName << " is invalid" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}	
	}
	(*tetris_obj).readFromFile("myText.txt");
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}