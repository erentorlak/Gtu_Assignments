#include "day_of_year.hpp"
using namespace std;
using namespace myNs;
int main() {
    ofstream fp ;
    
    // Clear old text
    fp.open("myText.txt", std::ofstream::out | std::ofstream::trunc);
    fp.close();

    fp.open("myText.txt", ios::app);

    if (!fp)
       cout << "Couldnt open the file" << endl;
    
    /**
     * DayOfYear constructers are called and get_day() and get_month() functions are used.
     * 
     */
    cout << "DayOfYear constructer is called: " << endl;
    DayOfYearSet::DayOfYear obj(1, 2);      
    cout << "Day : " << obj.get_day() << ", " << "Month : " << obj.get_month() << endl;
    cout << "DayOfYear constructer is called: " << endl;
    DayOfYearSet::DayOfYear obj1(2, 3);
    cout << "Day : " << obj1.get_day() << ", " << "Month : " << obj1.get_month() << endl;
    cout << "DayOfYear constructer is called: " << endl;
    DayOfYearSet::DayOfYear obj2(3, 4);
    cout << "Day : " << obj2.get_day() << ", " << "Month : " << obj2.get_month() << endl;
    cout << "DayOfYear constructer is called: " << endl;
    DayOfYearSet::DayOfYear obj3(4, 5);
    cout << "Day : " << obj3.get_day() << ", " << "Month : " << obj3.get_month() << endl;
    cout << "DayOfYear constructer is called: " << endl;
    DayOfYearSet::DayOfYear obj4(5, 6);
    cout << "Day : " << obj4.get_day() << ", " << "Month : " << obj4.get_month() << endl << endl;

    // February is 29 day so it makes the day 1 as default.
    DayOfYearSet::DayOfYear obj5(31, 2);                                                           
    cout << "Day : " << obj5.get_day() << ", " << "Month : " << obj5.get_month() << endl << endl;
    // Month is invalid so month = 1 as default.
    DayOfYearSet::DayOfYear obj6(5, 13);                                                           
    cout << "Day : " << obj6.get_day() << ", " << "Month : " << obj6.get_month() << endl << endl;

    obj5.set_day(6);    obj5.set_month(6);    obj5.control_day(obj5);   // sets and controls if day is valid if not changes to 1

    // List initilasier constructers
    DayOfYearSet set0({ obj, obj1, obj2, obj3 });     
    DayOfYearSet set1({ obj, obj1, obj2 });     
    DayOfYearSet set2({ obj2, obj3 ,obj4 });
    DayOfYearSet set3({ obj, obj, obj1, obj2 ,obj4, obj3 });    //Same obj duplicate is here but it takes only one 
    DayOfYearSet setequal1({ obj, obj1, obj2 });
    DayOfYearSet setequal2({ obj2, obj1, obj });
    DayOfYearSet setmove({ obj});

    cout << "set1 -> " << set1 << "set2 -> " << set2 << "set3 -> " << set3 << endl;   // overloaded << operator prints the elements of sets 
    fp << set1 << set2 << set3 << endl;     // overloaded << operator prints the elements of sets to the text file

    DayOfYearSet set4(set1); // Copy constructer
    DayOfYearSet set5(set3); // Copy constructer
    cout << "set4 -> " << set4 << "set5 -> " << set5 << endl;

    // No parameter constructers
    DayOfYearSet diff_set;  
    DayOfYearSet diff_set2; 
    DayOfYearSet union_set; 
    DayOfYearSet union_set2;
    DayOfYearSet intersection_set; 
    DayOfYearSet intersection_set2;
    DayOfYearSet complement_set;  
    DayOfYearSet complement_set2;  

    DayOfYearSet move_set(move(setmove)); // Move constructer
    
    cout << "From set0 removing obj1 : " << set0 << endl;
    fp << "Before removing obj1 : " << set0 << endl;
    set0 - obj1;                                        // Removing a element from set 
    cout << "After removing obj1 : " << set0 << endl;
    fp << "After removing obj1 : " << set0 << endl;

    cout << "From set0 removing obj1 : " << set0 << endl;
    fp << "Before removing obj1 : " << set0 << endl;
    set0 - obj1;                                        // Try to remove a already removed element from set 
    cout << "After removing obj1 : " << set0 << endl;
    fp << "After removing obj1 : " << set0 << endl;

    cout << "Adding obj1 to set0 : " << set0 << endl;
    fp << "Before adding obj1 to set0: " << set0 << endl;
    set0 + obj1;                                        // Adding a element to set 
    cout << "After adding obj1 to set0 : " << set0 << endl;
    fp << "After adding obj1 to set0 : " << set0 << endl;

    cout << "Adding obj1 to set0 : " << set0 << endl;
    fp << "Before adding obj1 to set0: " << set0 << endl;
    set0 + obj1;                                        // Try to add a already added element to set 
    cout << "After adding obj1 to set0 : " << set0 << endl;
    fp << "After adding obj1 to set0 : " << set0 << endl;
    
    diff_set = (set1 - set2);   // Move assignment operator and diffrence operator
    cout << "Diffrence set (set1 - set2) -> " << diff_set << endl;
    fp << "Diffrence set (set1 - set2) -> " << diff_set << endl;
    
    diff_set2 = (set2 - set3);   // Move assignment operator and diffrence operator
    cout << "Diffrence set (set2 - set3) -> " << diff_set2 << endl;
    fp << "Diffrence set (set2 - set3)-> " << diff_set2 << endl;

    union_set = (set1 + set2);  // Move assignment operator and union operator
    cout << "Union set (set1 + set2)-> " << union_set << endl;
    fp << "Union set (set1 + set2)-> " << union_set << endl;

    union_set2 = (set2 + set3);  // Move assignment operator and union operator
    cout << "Union set (set2 + set3) -> " << union_set2 << endl;
    fp << "Union set (set2 + set3) -> " << union_set2 << endl;
    
    intersection_set = (set1 ^ set3);    // Move assignment operator and intersection operator
    cout << "Intersection set (set1 ^ set3) -> " << intersection_set << endl;
    fp << "Intersection set (set1 ^ set3) -> " << intersection_set << endl;

    intersection_set2 = (set3 ^ set4);    // Move assignment operator and intersection operator
    cout << "Intersection set (set3 ^ set4) -> " << intersection_set2 << endl;
    fp << "Intersection set (set3 ^ set4) -> " << intersection_set2 << endl;

    complement_set = (!set1);   // Move assignment operator and complement operator
    cout << "Complement set -> " << complement_set << endl;
    fp << "Complement set -> " << complement_set << endl;

    complement_set2 = (!!set1);   // Move assignment operator and complement operator
    cout << "Complement set2 -> " << complement_set2 << endl;
    fp << "Complement set2 -> " << complement_set2 << endl;

    cout << "Set1 zero index day and month : ( " << set1[0].get_day() << ", " 
    << set1[0].get_month() << " )" << endl << endl;                 // Indice operator[] 

    cout << "Set2 1. index day and month : ( " << set2[1].get_day() << ", " 
    << set2[1].get_month() << " )" << endl << endl;                 // Indice operator[] 
    
    cout << "1 means equal, 0 means not equal : " << (set1 == set2) << endl;    // Equal operator
    cout << set1 << set2 << endl;
    
    cout << "1 means equal, 0 means not equal : " << (setequal1 == setequal2) << endl;  // Equal operator
    cout << setequal1 << setequal2 << endl;
    
    cout << "1 means not equal, 0 means equal : " << (set1 != set2) << endl;    // Equal operator
    cout << set1 << set2 << endl;
    
    cout << "1 means not equal, 0 means equal : " << (setequal1 != setequal2) << endl;  // Equal operator
    cout << setequal1 << setequal2 << endl;

    cout << "Before remove : " << set1 << endl;
    set1.remove(0);                                     // removes element using index
    cout << "After remove : " << set1 << endl;
    set1.remove(1);                                     // removes element using index
    cout << "After remove : " << set1 << endl;

    fp.close();

    return (0);
}
