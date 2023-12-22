#include "tetromino.hpp"
using namespace ns_Tetromino;

Tetromino::Tetromino(void) {      }

Tetromino::Tetromino(TypeTetro input)     
{
    myType = input;
    set_shape(myType);
}

void Tetromino::set_shape(TypeTetro inp_TypeTetro)        
{
    tetros_shape.clear();
    if (inp_TypeTetro == TypeTetro::I)
    {
        tetros_shape.push_back({'I','I','I','I'});

        tetro_height = 1;
        tetro_width = 4;
    }
    else if (inp_TypeTetro == TypeTetro::O)
    {
        tetros_shape.push_back({'O','O'});
		tetros_shape.push_back({'O','O'});

        tetro_height = 2;
        tetro_width = 2;
    }
    else if (inp_TypeTetro == TypeTetro::T)
    {
		tetros_shape.push_back({'T','T','T'});
		tetros_shape.push_back({'.','T','.'});

        tetro_height = 2;
        tetro_width = 3;
    }
    else if (inp_TypeTetro == TypeTetro::J)
    {
        tetros_shape.push_back({'.','J'});
		tetros_shape.push_back({'.','J'});
        tetros_shape.push_back({'J','J'});
        
        tetro_height = 3;
        tetro_width = 2;
    }
    else if (inp_TypeTetro == TypeTetro::L)
    {
        tetros_shape.push_back({'L','.'});
		tetros_shape.push_back({'L','.'});
        tetros_shape.push_back({'L','L'});

        tetro_height = 3;
        tetro_width = 2;
    }
    else if (inp_TypeTetro == TypeTetro::S)
    {
		tetros_shape.push_back({'.','S','S'});
		tetros_shape.push_back({'S','S','.'});

        tetro_height = 2;
        tetro_width = 3;
    }
    else if (inp_TypeTetro == TypeTetro::Z)
    {
        tetros_shape.push_back({'Z','Z','.'});
		tetros_shape.push_back({'.','Z','Z'}); 

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

void Tetromino::rotate(char direction, int count)    {	

    for (int i = 0; i < count; i++) {
        
        if ('r' == direction)  {        
   
            this->mirror_y();
            this->transpose();
        }
        else if ('l' == direction)  {
            this->transpose();            
            this->mirror_y();
        }
    }
}           

void Tetromino::mirror_y(){
    reverse(tetros_shape.begin(), tetros_shape.end());
}

void Tetromino::transpose(){
   
    vector <vector<char>> temp;
    temp.resize(tetros_shape[0].size());

    for (int i = 0; i < tetro_height; i++) 
        for (int j = 0; j < tetro_width; j++) 
            temp[j].push_back(tetros_shape[i][j]);
        
    tetros_shape = temp;
    swap(tetro_height , tetro_width);
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
