#include "tetris.hpp"
using namespace ns_Tetromino;
using namespace ns_Tetris;

Tetromino::Tetromino(void) {      }

Tetromino::Tetromino(TypeTetro input)     
{
    myType = input;
    set_shape(myType);
}

void Tetromino::set_shape(TypeTetro inp_TypeTetro)        
{
    if (inp_TypeTetro == TypeTetro::I)
    {
        tetros_shape = new char*[1];
    
        for(int i = 0; i < 1; ++i)
            tetros_shape[i] = new char[4];
        
       tetros_shape[0][0] = 'I';
       tetros_shape[0][1] = 'I';
       tetros_shape[0][2] = 'I';
       tetros_shape[0][3] = 'I';

       tetro_height = 1;
       tetro_width = 4;
    }
    else if (inp_TypeTetro == TypeTetro::O)
    {
        tetros_shape = new char*[2];
    
        for(int i = 0; i < 2; ++i)
            tetros_shape[i] = new char[2];
        
        tetros_shape[0][0] = 'O';
        tetros_shape[0][1] = 'O';
        tetros_shape[1][0] = 'O';
        tetros_shape[1][1] = 'O';

        tetro_height = 2;
        tetro_width = 2;
    }
    else if (inp_TypeTetro == TypeTetro::T)
    {
        tetros_shape = new char*[2];
    
        for(int i = 0; i < 2; ++i)
            tetros_shape[i] = new char[3];
        
        tetros_shape[0][0] = '.';
        tetros_shape[0][1] = 'T';
        tetros_shape[0][2] = '.';
        tetros_shape[1][0] = 'T';
        tetros_shape[1][1] = 'T';
        tetros_shape[1][2] = 'T';

        tetro_height = 2;
        tetro_width = 3;
    }
    else if (inp_TypeTetro == TypeTetro::J)
    {
        tetros_shape = new char*[3];
    
        for(int i = 0; i < 3; ++i)
            tetros_shape[i] = new char[2];

        tetros_shape[0][0] = '.';
        tetros_shape[0][1] = 'J';
        tetros_shape[1][0] = '.';
        tetros_shape[1][1] = 'J';
        tetros_shape[2][0] = 'J';
        tetros_shape[2][1] = 'J';
        
        tetro_height = 3;
        tetro_width = 2;
    }
    else if (inp_TypeTetro == TypeTetro::L)
    {
        tetros_shape = new char*[3];
    
        for(int i = 0; i < 3; ++i)
            tetros_shape[i] = new char[2];
        
        tetros_shape[0][0] = 'L';
        tetros_shape[0][1] = '.';
        tetros_shape[1][0] = 'L';
        tetros_shape[1][1] = '.';
        tetros_shape[2][0] = 'L';
        tetros_shape[2][1] = 'L';

        tetro_height = 3;
        tetro_width = 2;
    }
    else if (inp_TypeTetro == TypeTetro::S)
    {
        tetros_shape = new char*[2];
    
        for(int i = 0; i < 2; ++i)
            tetros_shape[i] = new char[3];
        
        tetros_shape[0][0] = '.';
        tetros_shape[0][1] = 'S';
        tetros_shape[0][2] = 'S';
        tetros_shape[1][0] = 'S';
        tetros_shape[1][1] = 'S';
        tetros_shape[1][2] = '.';

        tetro_height = 2;
        tetro_width = 3;
    }
    else if (inp_TypeTetro == TypeTetro::Z)
    {
        tetros_shape = new char*[2];
    
        for(int i = 0; i < 2; ++i)
            tetros_shape[i] = new char[3];
        
        tetros_shape[0][0] = 'Z';
        tetros_shape[0][1] = 'Z';
        tetros_shape[0][2] = '.';
        tetros_shape[1][0] = '.';
        tetros_shape[1][1] = 'Z';
        tetros_shape[1][2] = 'Z';

        tetro_height = 2;
        tetro_width = 3;
    }
}

TypeTetro Tetromino::get_TypeTetro(char a)        // for enum operations
{
    if (a == 'I')
        return (TypeTetro::I);
    else if (a == 'O')
        return (TypeTetro::O);
    else if (a == 'T')
        return (TypeTetro::T);
    else if (a == 'J')
        return (TypeTetro::J);
    else if (a == 'L')
        return (TypeTetro::L);
    else if (a == 'S')
        return (TypeTetro::S);
    else
        return (TypeTetro::Z);
}

void Tetromino::transpose(void)
{   
    if(get_tetro_width() == 1 || get_tetro_height() == 1){  return; }  

    char** temp = new char*[ get_tetro_width() ];             
    
    for(int i = 0; i < get_tetro_width(); i++)
            temp[i] = new char[ get_tetro_height() ];
            
    for (int i = 0; i < get_tetro_height(); i++)
        for (int j = 0; j < get_tetro_width(); j++)
            temp[j][i] = this->tetros_shape[i][j];

    for (int j = 0; j < get_tetro_width(); j++)             // deleting old shape
        delete tetros_shape[j];
    delete [] tetros_shape;

    this->tetros_shape = temp;              
    temp = nullptr;
    
    int temp2 = tetro_height;       // swaping
    tetro_height = tetro_width;
    tetro_width = temp2;
}
void Tetromino::mirror_y(void)      
{
    if(get_tetro_width() == 1 || get_tetro_height() == 1){  return; }  // 'I' tetromino dont need this function
    char temp;
    int index_x;   

    for (int i = 0; i < get_tetro_height(); i++) {
        index_x = get_tetro_width() - 1;
        for (int j = 0; j < get_tetro_width() / 2; j++)  {
            temp = this->tetros_shape[i][j];
            this->tetros_shape[i][j] = this->tetros_shape[i][index_x];
            this->tetros_shape[i][index_x] = temp;
            index_x--;
        }
    }
}


void Tetromino::rotate(char direction, int count)    {	

    for (int i = 0; i < count; i++) {
        
        if ('r' == direction)  {        
            this->transpose();
            this->mirror_y();
        }
        else if ('l' == direction)  {
            this->mirror_y();
            this->transpose();
        }
    }
}                                                                                   
char Tetromino::rot_direction(){           

    char inp_direction;
    while(1) {
        cout << "Enter rotation direction (l) or (r) :" << endl;
        if (cin >> inp_direction && (inp_direction == 'l' || inp_direction == 'r') ) {
            break;
        } 
        else {
            cout << "Enter character !!!" << endl;
            cin.clear();
            cin.ignore(80, '\n');
        }
    }

    return inp_direction;
}
int Tetromino::rot_count(){   

    int inp_count;
    while(1) {
        cout << "Enter rotatiton number :" << endl;
        if (cin >> inp_count) {
            break;
        } 
        else {
            cout << "Enter number !!!" << endl;
            cin.clear();
            cin.ignore(80, '\n');
        }
    }
    return inp_count;
}
/*Tetromino::~Tetromino(){     
    for (int j = 0; j < get_tetro_width(); j++)            
        delete tetros_shape[j];
    delete [] tetros_shape;
    tetros_shape = nullptr;
}*/
