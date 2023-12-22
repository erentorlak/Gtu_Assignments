#ifndef DAY_OF_YEAR_HPP
#define DAY_OF_YEAR_HPP
#include <iostream>
#include <list>
#include <vector>
#include <fstream>

using namespace std;

namespace myNs{

class DayOfYearSet  
{
	public:

    class DayOfYear 
    {
    	public:

    	DayOfYear(int d, int m) { set_day(d) ; set_month(m); control_day(*this); }	//controls validity
    	DayOfYear() {   }
		
    	int get_day() const {return (day);}
    	int get_month() const {return (month);}
    	void set_day(int d) { day = d;}

    	void set_month(int m) { 
			if(m > 12 || m < 1){
				cout << "Cant set the month: Invalid month number! : default month=1" << endl;
				month = 1;
			}
			else
				month = m;
		}	
		void control_day(DayOfYear& obj){		// Controls if day is valid if not changes to 1
			int arr[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 ,31 };
			
			if( ( obj.get_day() > arr[obj.get_month()] ) || (obj.get_day() < 1) ){
				cout << "Cant set the day: Invalid day number! : default day=1" << endl;
				obj.set_day(1);
			}
		}
    	~DayOfYear() { }	
		
    	private:
			
    	int month;
    	int day;

    };// end of DayOfYear class

    DayOfYearSet() {			// No parameter constructer
    	set_size = 0;
    	p_dayOfYear = nullptr;
    }
    DayOfYearSet(DayOfYearSet& obj) {		
    	set_size = obj.set_size;
		p_dayOfYear = new DayOfYear[obj.set_size];
		for (int i = 0; i < obj.set_size; i++){
			p_dayOfYear[i].set_day(obj.p_dayOfYear[i].get_day());
			p_dayOfYear[i].set_month(obj.p_dayOfYear[i].get_month());
		}
    }

    // Initialize the set using the list of DayOfYear objects
    DayOfYearSet(const list<DayOfYear>& obj_list)
                : set_size(0), p_dayOfYear(nullptr) {

    	for(const auto& obj : obj_list)
    		*this + obj;
    }
	// Move constructor
	DayOfYearSet(DayOfYearSet&& other) noexcept :
	  set_size(other.set_size), p_dayOfYear(other.p_dayOfYear)
	{		

	  // Reset the other object's state
	  other.set_size = 0;
	  other.p_dayOfYear = nullptr;
	}

	// Move assignment operator
	DayOfYearSet& operator=(DayOfYearSet&& other) noexcept
	{
		// Check for self-assignment
		if (this == &other) 
			return *this;	
		// Deallocate memory for the current object
		
		if (set_size != 0) 
			delete [] p_dayOfYear;	

		// Transfer ownership of the other object's resources to the current object
		set_size = other.set_size;
		p_dayOfYear = other.p_dayOfYear;
	
		// Reset the other object's state
		other.set_size = 0;
		other.p_dayOfYear = nullptr;	
		return *this;
	}
	
    ~DayOfYearSet() {
		if (set_size != 0)
			delete [] p_dayOfYear;		
    	p_dayOfYear = nullptr;
    }
	void remove(int index);
	int size() const {	return set_size; }

    void operator+(const DayOfYear& obj);
    DayOfYearSet operator+(const DayOfYearSet& obj);
    DayOfYearSet& operator=(const DayOfYearSet& obj);
    int operator==(const DayOfYearSet& rhs);
    int operator!=(const DayOfYearSet& rhs);
    DayOfYearSet operator-(const DayOfYearSet& rhs);
	void operator-(const DayOfYear& rhs);
	DayOfYearSet operator^(const DayOfYearSet& rhs);
	DayOfYear& operator[](int index);
	DayOfYearSet operator!();
    friend ostream& operator<<(ostream& out, const DayOfYearSet& obj){
	
	    out << "(Size : " << obj.size() << " )"<< "Elements in set: {\n";
    	for (int i = 0; i < obj.set_size; i++)
    	    out << " ( Day: " << obj.p_dayOfYear[i].get_day() << ", Month: " << obj.p_dayOfYear[i].get_month() << ")\n";
	
    	out << " }" << endl;
    	return out;		
	}

    private:

    int set_size;
    DayOfYear *p_dayOfYear;
};

}//end of namespace
#endif