#include "tetris.hpp"
using namespace ns_Tetromino;
using namespace ns_Tetris;


Tetris::Tetris(int width_inp, int height_inp)
{
    height = height_inp;
    width = width_inp;

    fitGround = new char*[height];
    
    for(int i = 0; i < height; i++)
        fitGround[i] = new char[width];

    fill_dots();                    // fills fitGround with '.'
}

void Tetris::fill_dots(){

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            fitGround[i][j] = '.';
}
void Tetris::draw() const {

   cout << "\x1b[H";    // go start of screen
   cout << "\x1b[0J";   // erase until end

    for (int i = 0; i < height; i++){       // printing fitGround 
    	 for (int j = 0; j < width; j++){
             	cout << fitGround[i][j];
         }
		cout << endl;	
	}
    std::this_thread::sleep_for(std::chrono::milliseconds(50));         // wait for 50 milisecond
   
}
char Tetris::move_direction(){

    char inp_direction;
    while(1) {
        cout << "Enter move direction (l) or (r) :" << endl;
        if (cin >> inp_direction && (inp_direction == 'l' || inp_direction == 'r') ) {
            break;
        } 
        else {
            cout << "Enter character (l) or (r)!!!" << endl;
            cin.clear();
            cin.ignore(80, '\n');
        }
    }

    return inp_direction;
}
int Tetris::move_count(Tetromino const tetro, char move_direction){     //gets input of right or left move 
                                                                    //and checks validity
    int inp_count;
    while(1) {
        cout << "Enter wanted move number :" << endl;
        if(move_direction == 'l'){

            if (cin >> inp_count && inp_count < width / 2 ) {               
                break;
            } 
        }
        else if(move_direction == 'r'){

            if (cin >> inp_count && (inp_count <= width - (width / 2 - 1) - tetro.get_tetro_width() ) ){   
                break;
            }         
            else {
                cout << "Enter VALID number !!!" << endl;
                cin.clear();
                cin.ignore(80, '\n');
            }    
        }
        else {
            cout << "Enter valid number !!!" << endl;
            cin.clear();
            cin.ignore(80, '\n');
        }
    }
    return inp_count;
}

void Tetris::operator+=(Tetromino const tetro) {    //adds tetromino to the up-middle of board
    
    
    for (int i = 0; i < tetro.get_tetro_height(); i++){
    
        for (int j = 0; j < tetro.get_tetro_width(); j++){
            if( fitGround[i+1][j + width / 2 - 1] != '.') {     // controls if added tetro intersects with another tetro
                cout << endl;
                cout << "****************GAME OVER*******************" << endl;  
                exit(1); 
            };                                                                 
            fitGround[i+1][j + width / 2 - 1] = tetro.get_tetros_shape()[i][j];     
        }
    }
    draw();
}
int Tetris::can_moveDown(Tetromino const tetro) {
    
    if(tetro.get_tetro_height() + tetro.head_coordinate_y == height ) { return 0; } // touching the ground

    for (int j = 0; j < tetro.get_tetro_width(); j++){ 
        for (int i = 0; i < tetro.get_tetro_height(); i++){
    
            if( '.' != fitGround[i + tetro.head_coordinate_y][j + tetro.head_coordinate_x] && '.' != tetro.get_tetros_shape()[i][j] ) {
                if(i + 1 == tetro.get_tetro_height() ){
                    
                    if('.' != fitGround[i + tetro.head_coordinate_y + 1][j + tetro.head_coordinate_x]) { return 0; } // touches another tetromino
                }
                else if(tetro.get_tetros_shape()[i + 1][j] != fitGround[i + tetro.head_coordinate_y + 1][j + tetro.head_coordinate_x]) { return 0;};// touches another tetromino
            }
        }
    }
    return 1;   // can go down
}
void Tetris::animate(Tetromino tetro) {
    int down;

    for(int i = 0; i < width; i++){                 // controls if max height is reached
        if( fitGround[1][i] != '.') {
            cout << endl;
            cout << "****************GAME OVER***************" << endl;
            exit(2);
        } 
    }
    tetro.rotate( tetro.rot_direction(), tetro.rot_count() );       
    
    (*this) += tetro;               //adding tetro to the up-middle

    char move_direction = (*this).move_direction();                     //getting inputs
    int move_slide = (*this).move_count(tetro, move_direction);             
    
      
    for (int i = 0; i < tetro.get_tetro_height(); i++){            
        for (int j = 0; j < tetro.get_tetro_width(); j++){
            fitGround[i+1][j + width / 2 - 1] = '.';
        }
    }  

    if( move_direction == 'r' ) {               //right slide 

        for (int k = 0; k < move_slide; k++){
        
            for (int i = 0; i < tetro.get_tetro_height(); i++){
                for (int j = 0; j < tetro.get_tetro_width(); j++){
                    if(k != 0) 
                        fitGround[i+1][j + width / 2 + k - 1] = '.';
                }
            }
            
            for (int i = 0; i < tetro.get_tetro_height(); i++){
                for (int j = 0; j < tetro.get_tetro_width(); j++){

                    fitGround[i+1][j + width / 2 + k] = tetro.get_tetros_shape()[i][j];
                }
            }
            draw();
        }
    
    }
    if( move_direction == 'l' ) {               // left slide

        for (int k = 0; k < move_slide; k++){
        
            for (int i = 0; i < tetro.get_tetro_height(); i++){
                for (int j = 0; j < tetro.get_tetro_width(); j++){
                    if(k != 0)
                        fitGround[i+1][j + width / 2 - k - 1] = '.';
                }
            }
            
            for (int i = 0; i < tetro.get_tetro_height(); i++){
                for (int j = 0; j < tetro.get_tetro_width(); j++){

                    fitGround[i+1][j + width / 2 - k - 2] = tetro.get_tetros_shape()[i][j];
                }
            }
            draw();
        }
    }
    if(move_direction == 'l')
        move_slide = -move_slide;
        
    tetro.head_coordinate_x = (width / 2 - 1 + move_slide);         //carrys tetros [0][0] coordinates
    tetro.head_coordinate_y = 1;

    down = 0;
    while (1 == can_moveDown(tetro))   {                    // go down until touch

        down++;
        tetro.head_coordinate_y++;
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                fitGround[i+1+down-1][j + width / 2 - 1 + move_slide] = '.';
            }
        }
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                
                if(tetro.get_tetros_shape()[i][j] != '.')
                    fitGround[i+1+down][j + width / 2 - 1 + move_slide] = tetro.get_tetros_shape()[i][j];
            }
        }
        draw();
    }
}

Tetris::~Tetris(){                      //destructor
    for (int j = 0; j < width; j++)            
        delete fitGround[j];
    delete [] fitGround;
    fitGround = nullptr;
}
