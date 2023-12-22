#include "tetrisarray.hpp"
using namespace ns_TetrisArray;



TetrisArray::TetrisArray(int width_inp, int height_inp)
{
    height = height_inp;
    width = width_inp;

    board = new char[height * width];
    
    fill_dots();                    // fills board with '.'
}

void TetrisArray::fill_dots(){

    for (int i = 0; i < height*width; i++)
        board[i] = '.';
}
void TetrisArray::draw() const {

   cout << "\x1b[H";    // go start of screen
   cout << "\x1b[0J";   // erase until end

    for (int i = 0; i < height*width; i++){       // printing board 
        if(i % width == 0)
            cout << endl;
        if(i < width)       
            cout<< '_';
        else
            cout << board[i];
    }
    cout << endl;
    cout << "total moves : " << totalMoves << endl;
    cout << "The last move coordinates of tetromino is : " << '(' << get_lastMove().first << ','  
		<< get_lastMove().second << ')' << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));         // wait for 50 milisecond
}
char TetrisArray::move_direction(){

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
int TetrisArray::move_count(Tetromino const tetro, char move_direction){     //gets input of right or left move 
                                                                    //and checks validity
    int inp_count;
    while(1) {
        cout << "Enter wanted move number :" << endl;
        if(move_direction == 'l'){

            if (cin >> inp_count && inp_count < width / 2 ) {               
                break;
            } 
            else {
                cout << "Enter VALID number !!!" << endl;
                cin.clear();
                cin.ignore(80, '\n');
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
    }
    return inp_count;
}

void TetrisArray::operator+=(const Tetromino& tetro) {    //adds tetromino to the up-middle of board
    
    
    for (int i = 0; i < tetro.get_tetro_height(); i++){
    
        for (int j = 0; j < tetro.get_tetro_width(); j++){
                                                                              
            board[(i+1)*width + j + width / 2 - 1] = tetro.get_tetros_shape()[i][j];     
        }
    }
    draw();
}
int TetrisArray::can_moveDown(Tetromino const tetro) {
    
    if(tetro.get_tetro_height() + tetro.head_coordinate_y == height ) { return 0; } // touching the ground

    for (int j = 0; j < tetro.get_tetro_width(); j++){ 
        for (int i = 0; i < tetro.get_tetro_height(); i++){
    
            if( '.' != board[(i + tetro.head_coordinate_y)*width + j + tetro.head_coordinate_x] && '.' != tetro.get_tetros_shape()[i][j] ) {
                if(i + 1 == tetro.get_tetro_height() ){
                    
                    if('.' != board[(i + tetro.head_coordinate_y + 1)*width + j + tetro.head_coordinate_x] ) { return 0; } // touches another tetromino
                }
                else if(tetro.get_tetros_shape()[i + 1][j] != board[(i + tetro.head_coordinate_y + 1)*width + j + tetro.head_coordinate_x] ) { return 0;};// touches another tetromino
            }
        }
    }
    return 1;   // can go down
}
int TetrisArray::lastMove(const Tetromino tetro){

    for(int i = 0; i < width; i++){                 // controls if max height is reached
        if( board[1*width + i] != '.') {
            cout << endl;
            cout << "exception throwed"<< endl;
            cout << "****************GAME OVER***************" << endl << endl << endl;
            return 1;
        } 
    }
    for (int i = 0; i < tetro.get_tetro_height(); i++){
        for (int j = 0; j < tetro.get_tetro_width(); j++){
            if( board[ (i+1)*width + j + width / 2 - 1] != '.') {     // controls if added tetro intersects with another tetro
                cout << endl;
                cout << "exception throwed"<< endl;
                cout << "****************GAME OVER*******************" << endl << endl << endl;  
                return 1;
            };  
        }; 
    }   
    return 0;
}
void TetrisArray::move_right(Tetromino tetro, int move_slide){
    for (int k = 0; k < move_slide; k++){
    
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                if(k != 0) 
                    board[(i+1)*width + j + width / 2 + k - 1] = '.';
            }
        }
        
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                board[(i+1)*width + j + width / 2 + k] = tetro.get_tetros_shape()[i][j];
            }
        }
        totalMoves++;
        draw();
    }
}
void TetrisArray::move_left(Tetromino tetro, int move_slide){
    for (int k = 0; k < move_slide; k++){
    
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                if(k != 0)
                    board[(i+1)*width + j + width / 2 - k - 1] = '.';
            }
        }
        
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                board[(i+1)*width + j + width / 2 - k - 2] = tetro.get_tetros_shape()[i][j];
            }
        }
        totalMoves++;
        draw();
    }
    
}
void TetrisArray::animate(Tetromino tetro, char move_direction, int move_slide) {
    int down;

    for (int i = 0; i < tetro.get_tetro_height(); i++){            
        for (int j = 0; j < tetro.get_tetro_width(); j++){
            board[(i+1)*width + j + width / 2 - 1] = '.';
        }
    }  

    if( move_direction == 'r' ) {               //right slide 
        move_right(tetro, move_slide);
    }
    if( move_direction == 'l' ) {               // left slide
        move_left(tetro, move_slide);
        move_slide = -move_slide;
    }
        
    tetro.head_coordinate_x = (width / 2 - 1 + move_slide);         //carrys tetros [0][0] coordinates
    tetro.head_coordinate_y = 1;
    lastmove.first = tetro.head_coordinate_x;
    lastmove.second = tetro.head_coordinate_y;
    down = 0;
    while (1 == can_moveDown(tetro))   {                    // go down until touch

        down++;
        tetro.head_coordinate_y++;
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                board[(i+1+down-1)*width + j + width / 2 - 1 + move_slide] = '.';
            }
        }
        for (int i = 0; i < tetro.get_tetro_height(); i++){
            for (int j = 0; j < tetro.get_tetro_width(); j++){
                
                if(tetro.get_tetros_shape()[i][j] != '.')
                    board[(i+1+down)*width + j + width / 2 - 1 + move_slide] = tetro.get_tetros_shape()[i][j];
            }
        }
        totalMoves++;
        lastmove.first = tetro.head_coordinate_x;
        lastmove.second = tetro.head_coordinate_y;
        draw();
    }
}
void TetrisArray::writeToFile(const string& fileName) {
    ofstream f_write;

    //clear old text 
    f_write.open(fileName, std::ofstream::out | std::ofstream::trunc);
    if (!f_write){                 
        throw fileName;   
    }
    f_write.close();
    
    f_write.open(fileName);
    if (!f_write){                 
        throw fileName;   
    }    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if(i==0)
                f_write << '_';
            else
                f_write << board[i*width + j];
        }
        f_write << endl;
    }
    f_write.close();
}
   
void TetrisArray::readFromFile(const string& fileName)  {

    ifstream f_read;
    f_read.open(fileName);
    if (!f_read){                 
        throw fileName;   
    }    
    string line;
    vector<vector<char>> vect;
    while( getline(f_read,line) ){
        vector<char> temp;
        for(char c: line)
            temp.push_back(c);
        
        vect.push_back(temp);
    }
    
    f_read.close();   
    
    cout << "Saved text file : " << endl;
    for(int r=0; r<vect.size(); ++r){
        for(int c=0; c<vect[0].size(); ++c)
            cout << vect[r][c];

        cout << endl;
    }
}
TetrisArray::~TetrisArray(){                      //destructor
    
    delete [] board;
    board = nullptr;
}
