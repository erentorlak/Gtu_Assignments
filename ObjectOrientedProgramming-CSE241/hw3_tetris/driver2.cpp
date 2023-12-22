#include "tetris.hpp"

using namespace ns_Tetromino;
using namespace ns_Tetris;

int main()
{	
	srand(time(0));	
	char TypeTetro_type;
	int num;
	int width_inp, height_inp;

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
	
	Tetris tetris_obj(width_inp, height_inp);	

	while(1) {

		cout << "Enter the tetromino type : " << endl;
		cin >> TypeTetro_type;
		if (TypeTetro_type == 'I' || TypeTetro_type == 'O' || TypeTetro_type == 'T' || TypeTetro_type == 'J' 
			|| TypeTetro_type == 'L' || TypeTetro_type == 'S' || TypeTetro_type == 'Z'|| TypeTetro_type == 'R'|| TypeTetro_type == 'Q'){

			if( TypeTetro_type == 'Q')
				exit(5);
			if( TypeTetro_type == 'R'){
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
			myTetro.set_shape( myTetro.get_TypeTetro(TypeTetro_type) );

			tetris_obj.animate(myTetro);		
		}
		else	
			cout << "Invalid entry.Please try again." << endl;
	}

	return 0;
}