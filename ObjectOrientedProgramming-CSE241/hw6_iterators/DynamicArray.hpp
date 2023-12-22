#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

using namespace std;
namespace MyNs{

template <typename T>
class DynamicArray
{
public:
	DynamicArray();
	explicit DynamicArray(int size);
	DynamicArray(const DynamicArray &other);
	DynamicArray(DynamicArray &&other) noexcept;
	
	DynamicArray &operator=(const DynamicArray &other);
	DynamicArray &operator=(DynamicArray &&other) noexcept;

	T &operator[](int index);
	const T &operator[](int index) const;

	class iterator
	{
	public:
		
        using value_type = T;		//these are for the stl functions like sort,find,for_each
		using reference =  T &;
		using pointer =  T *;
		using difference_type = ptrdiff_t;
		using iterator_category = random_access_iterator_tag;

		iterator(T *data, int r_index);
		reference operator*() const;
		value_type operator->() const;
		iterator& operator++();
		iterator operator++(int);
		iterator& operator--();
		iterator operator--(int);
		iterator& operator+=(difference_type n);
		iterator& operator-=(difference_type n);
		iterator operator+(difference_type n) const;
		iterator operator-(difference_type n) const;
		difference_type operator-(const iterator &other) const;
		bool operator==(const iterator &other) const;
		bool operator!=(const iterator &other) const;
		bool operator<(const iterator &other) const;
		bool operator>(const iterator &other) const;
		bool operator<=(const iterator &other) const;
		bool operator>=(const iterator &other) const;

		T get_iter() { return *iter; }
		int get_index() { return index; }

	private:
		T* iter;
		int index;
	};

	class const_iterator
	{
	public:
	
        using value_type = T;		//these are for the stl functions like sort,find,for_each
		using reference = const T &;
		using pointer = const T *;
		using difference_type = ptrdiff_t;	
		using iterator_category = random_access_iterator_tag;

		const_iterator(const T *data, int r_index);
		reference operator*() const;
		value_type operator->() const;
		const_iterator &operator++();	
		const_iterator operator++(int);
		const_iterator &operator--();
		const_iterator operator--(int);
		const_iterator &operator+=(difference_type n);
		const_iterator &operator-=(difference_type n);
		const_iterator operator+(difference_type n) const;
		const_iterator operator-(difference_type n) const;
		difference_type operator-(const const_iterator &other) const;
		bool operator==(const const_iterator &other) const;
		bool operator!=(const const_iterator &other) const;
		bool operator<(const const_iterator &other) const;
		bool operator>(const const_iterator &other) const;
		bool operator<=(const const_iterator &other) const;
		bool operator>=(const const_iterator &other) const;

		const T get_iter() { return *c_iter; }
		int get_index() { return index; }
	private:

		const T* c_iter;
		int index;
	};

	bool empty() const;
	int size() const;
	void erase(iterator iter);	
	void clear();
	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;
	void addElement(const T element);

private:
	int ptr_size;
	shared_ptr<T> shared_arr;
	
};
template class DynamicArray<int*>;
template class DynamicArray<int>;
template class DynamicArray<char>;
template class DynamicArray<char*>;

}

#endif