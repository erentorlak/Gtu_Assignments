#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "DynamicArray.hpp"
#include "DynamicArray.cpp"

int main()
{	
try
{
	DynamicArray<int> arr;
	cout << "Creating empty array: " << endl;
	cout << "Size: " << arr.size() << endl;
	cout << "Empty: " << arr.empty() << endl;

	cout << "\nCreating array of size 5: " << endl;
	DynamicArray<int> arr2(5);
	cout << "Size: " << arr2.size() << endl;
	cout << "Empty: " << arr2.empty() << endl;

	cout << "\nAdding elements to array: " << endl;
	for (int i = 0; i < 5; i++)
		arr2.addElement(i);
	cout << "Size: " << arr2.size() << endl;
	cout << "Elements: ";
	for (auto it = arr2.begin(); it != arr2.end(); it++)
		cout << *it << " ";
	cout << endl;

	cout << "\nTesting copy constructor: " << endl;
	DynamicArray<int> arr3(arr2);
	cout << "Size: " << arr3.size() << endl;
	cout << "Elements: ";
	for (auto it = arr3.begin(); it != arr3.end(); it++)
		cout << *it << " ";
	cout << endl;

	cout << "\nTesting copy assignment operator: " << endl;
	DynamicArray<int> arr4;
	arr4 = arr2;
	cout << "Size: " << arr4.size() << endl;
	cout << "Elements: ";
	for (auto it = arr4.begin(); it != arr4.end(); it++)
		cout << *it << " ";
	cout << endl;

	cout << "\nTesting move constructor: " << endl;
	DynamicArray<int> arr5(move(arr2));
	cout << "Size: " << arr5.size() << endl;
	cout << "Elements: ";
	for (auto it = arr5.begin(); it != arr5.end(); it++)
		cout << *it << " ";
	cout << endl;

	cout << "\nTesting move assignment operator: " << endl;
	DynamicArray<int> arr6;
	arr6 = move(arr3);
	cout << "Size: " << arr6.size() << endl;
	cout << "Elements: ";
	for (auto it = arr6.begin(); it != arr6.end(); it++)
		cout << *it << " ";
	cout << endl;

	cout << "\nTesting operator[]: " << endl;
	cout << "arr6[3] = " << arr6[3] << endl;
	cout << "We do this -> arr6[3] = 10;" << endl;
	arr6[3] = 10;
	cout << "arr6[3] = " << arr6[3] << endl;
	cout << "Elements: ";
	for (auto it = arr6.begin(); it != arr6.end(); it++)
		cout << *it << " ";
	cout << endl;
	
	cout << "\nTesting erase(): " << endl;
	cout << "Size before erase: " << arr6.size() << endl;
	arr6.erase(arr6.end());
	cout << "Size after erase: " << arr6.size() << endl;
	cout << "Elements: ";
	for (auto it = arr6.begin(); it != arr6.end(); it++)
		cout << *it << " ";
	cout << endl;

	cout << "\nTesting clear(): " << endl;
	arr6.clear();
	cout << "Size: " << arr6.size() << endl;
	cout << "Empty: " << arr6.empty() << endl;

	arr.addElement(30);
	arr.addElement(10);
	arr.addElement(20);
	arr.addElement(40);

	cout << endl;
	cout << "Original array: ";
	for (auto it = arr.begin(); it != arr.end(); it++)
		cout << *it << " ";
	cout << endl;

	sort(arr.begin(), arr.end());

	cout << "Sorted array: ";
	for (auto it = arr.begin(); it != arr.end(); it++)
		cout << *it << " ";
	cout << endl;

	auto it = find(arr.begin(), arr.end(), 20);
	if (it != arr.end())
		cout << "20 found in array (find() is used)" << endl;
	else
		cout << "20 not found in array (find() is used)" << endl;
	auto it1 = find(arr.begin(), arr.end(), 50);
	if (it1 != arr.end())
		cout << "50 found in array (find() is used)" << endl;
	else
		cout << "50 not found in array (find() is used)" << endl;
	cout << endl;

	cout << "Doubling every element using for_each :" << endl;
	for_each(arr.begin(), arr.end(), [](int &x)	{ x *= 2; } );

	cout << "Modified array: ";
	for (auto it = arr.begin(); it != arr.end(); it++)
		cout << *it << " ";
	cout << endl
		 << endl;

	cout << "Iterating over elements using iterators: ";
	for (auto it = arr.begin(); it != arr.end(); it++)
		cout << *it << " ";
	cout << endl;

	cout << "Iterating over elements using const_iterators: ";
	for (auto it = arr.cbegin(); it != arr.cend(); it++)
		cout << *it << " ";
	cout << endl
		 << endl;

	cout << "Incrementing iterator (now shows: arr[1]) : ";
	it = arr.begin();
	it++;
	cout << *it << endl;

	cout << "Decrementing iterator (now shows: arr[0]): ";
	it--;
	cout << *it << endl
		 << endl;
	cout << "Adding to iterator by 2 : ";
	it += 2;
	cout << *it << endl;

	cout << "Subtracting from iterator by 2: ";
	it -= 2;
	cout << *it << endl;

	cout << "Comparing iterators: ";
	auto it2 = arr.begin();
	if (it == it2)
		cout << "iterators are equal" << endl;
	else
		cout << "iterators are not equal" << endl;

	arr[12];		// throws exception
		
}
catch(...){
	cout << "\n out of boundaries: exception throwed" << endl;
}
	return 0;
}
