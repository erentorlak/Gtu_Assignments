#include "day_of_year.hpp"

using namespace myNs;

void DayOfYearSet::operator+(const DayOfYear& obj) 
{
    if (p_dayOfYear != nullptr){
        for (int i = 0; i < set_size; i++){     //control for if it is already exist
            if (p_dayOfYear[i].get_day() == obj.get_day() && p_dayOfYear[i].get_month() == obj.get_month())
                return;
        }
    }
    set_size++;

    vector<DayOfYear> temp;
 
    for (int i = 0; i < set_size - 1; i++)
        temp.push_back(p_dayOfYear[i]);
    
    temp.push_back(obj);
    
    if (p_dayOfYear != nullptr) 
        delete [] p_dayOfYear;
    p_dayOfYear = nullptr;

    p_dayOfYear = new DayOfYear[set_size];
    
    for (int i = 0; i < set_size; i++)
    {
        p_dayOfYear[i].set_day(temp[i].get_day());
        p_dayOfYear[i].set_month(temp[i].get_month());
    } 
}



DayOfYearSet DayOfYearSet::operator+(const DayOfYearSet& obj)
{
    DayOfYearSet temp;

    for (int i = 0; i < this -> set_size; i++)
        temp + this->p_dayOfYear[i];            
    
    for (int i = 0; i < obj.set_size; i++)
        temp + obj.p_dayOfYear[i];          
    
    return (temp);
}
DayOfYearSet& DayOfYearSet::operator=(const DayOfYearSet& obj){ 

    set_size = obj.set_size;
    p_dayOfYear = obj.p_dayOfYear;

    return (*this);
}
int DayOfYearSet::operator==(const DayOfYearSet& rhs){
    int found;
    if (set_size != rhs.set_size)
        return 0;
  
    // Check if all elements in rhs are in the set
    for (int i = 0; i < rhs.set_size; i++){
        found = 0;
        for (int j = 0; j < set_size; j++){
            if (rhs.p_dayOfYear[i].get_day() == p_dayOfYear[j].get_day() &&
              rhs.p_dayOfYear[i].get_month() == p_dayOfYear[j].get_month())
            {
                found = 1;
                break;
            }
        }

    if (!found)
        return 0;
    }

    // Check if all elements in the set are in rhs
    for (int i = 0; i < set_size; i++){
        found = 0;
        for (int j = 0; j < rhs.set_size; j++){
            if (p_dayOfYear[i].get_day() == rhs.p_dayOfYear[j].get_day() &&
              p_dayOfYear[i].get_month() == rhs.p_dayOfYear[j].get_month())
            {
                found = 1;
                break;
            }
        }
    if (!found)     
        return 0;

    }
    return 1;
}

int DayOfYearSet::operator!=(const DayOfYearSet& rhs){ return !( (*this) == rhs); } 

DayOfYearSet DayOfYearSet::operator-(const DayOfYearSet& rhs){
    int found;
    DayOfYearSet difference_set;
    for (int i = 0; i < set_size; i++){

        found = 0;
        for (int j = 0; j < rhs.set_size; j++){
            
            if (p_dayOfYear[i].get_day() == rhs.p_dayOfYear[j].get_day() &&
                p_dayOfYear[i].get_month() == rhs.p_dayOfYear[j].get_month())
            {
                found = 1;
                break;
            }
        }
        if (!found)
            difference_set + p_dayOfYear[i];    // if not in rhs add 
    }
    return difference_set;
}
void DayOfYearSet::operator-(const DayOfYear& rhs){
   
    DayOfYearSet result;
    // Iterate over the elements in the original set
    for (int i = 0; i < set_size; i++){
        // Skip the element that we want to remove
        if (p_dayOfYear[i].get_day() == rhs.get_day() &&
            p_dayOfYear[i].get_month() == rhs.get_month())
            continue;
    
        // Add all other elements to the result set
        result + p_dayOfYear[i];
    }
    delete [] p_dayOfYear;
    set_size = result.set_size;
    p_dayOfYear = move(result.p_dayOfYear); // uses move assignment

    result.p_dayOfYear = nullptr;
}

DayOfYearSet DayOfYearSet::operator^(const DayOfYearSet& rhs){

    DayOfYearSet intersection;
    int found;
    // Iterate over the elements in the first set
    for (int i = 0; i < set_size; i++){
        found = 0;
        for (int j = 0; j < rhs.set_size && !found; j++){
            // Check if the current element is in the second set
            if (p_dayOfYear[i].get_day() == rhs.p_dayOfYear[j].get_day() &&
                p_dayOfYear[i].get_month() == rhs.p_dayOfYear[j].get_month())
            {
                // If the element is found in the second set, add it to the intersection set
                intersection + p_dayOfYear[i];
                found = 1;
            }
        }
    }
    // Return the intersection set
    return intersection;
}
DayOfYearSet DayOfYearSet::operator!(){ //returns the complement set.
  
    DayOfYearSet days;
    DayOfYearSet complement;
   
    int arr[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 ,31 };

    for (int i = 1; i <= 12; i++)
        for(int j = 1; j <= arr[i]; j++)
            days + DayOfYear(j,i);      //adding all the 365 days to days
               
    // Remove the objects of this set from days
    complement = (days - *this);

    return complement;
}
DayOfYearSet::DayOfYear& DayOfYearSet::operator[](int index){

	if (index >= set_size || index < 0 ){	
    	cout << "Array index out of bound, exiting" << endl;
    	return p_dayOfYear[0];
    }
    return p_dayOfYear[index];
}

void DayOfYearSet::remove(int index){

    DayOfYearSet result;
    // Iterate over the elements in the original set
    for (int i = 0; i < set_size; i++){
        // Skip the element that we dont want to remove
        if (i == index)
            continue;

        // Add all other elements to the result set
        result + p_dayOfYear[i];
    }
    delete [] p_dayOfYear;
    set_size = result.set_size;
    p_dayOfYear = move(result.p_dayOfYear); // uses move assignment

    result.p_dayOfYear = nullptr;
}


