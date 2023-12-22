#include "DynamicArray.hpp"
using namespace MyNs;
template <typename T>
DynamicArray<T>::DynamicArray() : ptr_size(0), shared_arr(nullptr) {}

template <typename T>
DynamicArray<T>::DynamicArray(int size) : ptr_size(size)
{
    shared_arr = shared_ptr<T>(new T[ptr_size], default_delete<T[]>());
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray &other)
{
    ptr_size = other.ptr_size;
    shared_arr = shared_ptr<T>(new T[ptr_size], default_delete<T[]>());
    copy(other.shared_arr.get(), other.shared_arr.get() + ptr_size, shared_arr.get()); // bak
}

template <typename T>
DynamicArray<T> &DynamicArray<T>::operator=(const DynamicArray &other)
{
    if (this != &other)
    {
        ptr_size = other.ptr_size;
        shared_arr = shared_ptr<T>(new T[ptr_size], default_delete<T[]>());
        copy(other.shared_arr.get(), other.shared_arr.get() + ptr_size, shared_arr.get());
    }
    return *this;
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray &&other) noexcept
{
    ptr_size = other.ptr_size;
    shared_arr = move(other.shared_arr);
    other.ptr_size = 0;
}

template <typename T>
DynamicArray<T> &DynamicArray<T>::operator=(DynamicArray &&other) noexcept
{
    if (this != &other)
    {
        ptr_size = other.ptr_size;
        shared_arr = move(other.shared_arr);
        other.ptr_size = 0;
    }
    return *this;
}

template <typename T>
bool DynamicArray<T>::empty() const
{
    return ptr_size == 0;
}

template <typename T>
int DynamicArray<T>::size() const
{
    return ptr_size;
}

template <typename T>
void DynamicArray<T>::clear()
{
    ptr_size = 0;
    shared_arr = shared_ptr<T>(new T[ptr_size], default_delete<T[]>());
}

template <typename T>
void DynamicArray<T>::erase(iterator iter)
{
    if (iter.get_index() >= ptr_size)
        throw out_of_range("Index out of range");

    shared_ptr<T> new_shared_arr(new T[ptr_size - 1], default_delete<T[]>());
    
	for (int i = 0; i < iter.get_index(); ++i)                  // assigning the old values except erased one
		new_shared_arr.get()[i] = shared_arr.get()[i];
	for (int i = iter.get_index() + 1; i < ptr_size; ++i)
		new_shared_arr.get()[i - 1] = shared_arr.get()[i];
    
	shared_arr = move(new_shared_arr);  
	--ptr_size;
}
template <typename T>
void DynamicArray<T>::addElement(const T element)
{
	shared_ptr<T> new_shared_arr(new T[ptr_size + 1], default_delete<T[]>());
	for (int i = 0; i < ptr_size; ++i)
		new_shared_arr.get()[i] = shared_arr.get()[i];
	
	new_shared_arr.get()[ptr_size] = element;
	shared_arr = move(new_shared_arr);
	++ptr_size;
}
template <typename T>
T &DynamicArray<T>::operator[](int index)
{
    if (index >= ptr_size)
        throw out_of_range("Index out of range");
    return shared_arr.get()[index];
}

template <typename T>
const T &DynamicArray<T>::operator[](int index) const
{
    if (index >= ptr_size)
        throw out_of_range("Index out of range");
    return shared_arr.get()[index]; 
}
template <typename T>
DynamicArray<T>::iterator::iterator(T *data, int r_index)
{
    iter = data;
    index = r_index;
}

template <typename T>
typename DynamicArray<T>::iterator DynamicArray<T>::begin()
{
    return iterator(shared_arr.get(), 0);
}

template <typename T>
typename DynamicArray<T>::iterator DynamicArray<T>::end()
{
    return iterator(shared_arr.get() + ptr_size, ptr_size - 1);
}

template <typename T>
typename DynamicArray<T>::const_iterator DynamicArray<T>::cbegin() const
{
    return const_iterator(shared_arr.get(), 0);
}

template <typename T>
typename DynamicArray<T>::const_iterator DynamicArray<T>::cend() const
{
    return const_iterator(shared_arr.get() + ptr_size, ptr_size - 1);
}

template <typename T>
T& DynamicArray<T>::iterator::operator*() const
{
    return *iter;
}

template <typename T>
T DynamicArray<T>::iterator::operator->() const
{
    return *iter;
}

template <typename T>
typename DynamicArray<T>::iterator &DynamicArray<T>::iterator::operator++()
{
    ++iter;
    return *this;
}

template <typename T>
typename DynamicArray<T>::iterator DynamicArray<T>::iterator::operator++(int)
{
    iterator tmp(*this);
    ++iter;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::iterator &DynamicArray<T>::iterator::operator--()
{
    --iter;
    return *this;
}

template <typename T>
typename DynamicArray<T>::iterator DynamicArray<T>::iterator::operator--(int)
{
    iterator tmp(*this);
    --iter;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::iterator &DynamicArray<T>::iterator::operator+=(difference_type n)
{
    iter += n;
    return *this;
}

template <typename T>
typename DynamicArray<T>::iterator &DynamicArray<T>::iterator::operator-=(difference_type n)
{
    iter -= n;
    return *this;
}

template <typename T>
typename DynamicArray<T>::iterator DynamicArray<T>::iterator::operator+(difference_type n) const
{
    iterator tmp(*this);
    tmp += n;
    tmp.index += n;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::iterator DynamicArray<T>::iterator::operator-(difference_type n) const
{
    iterator tmp(*this);
    tmp -= n;
    tmp.index -= n;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::iterator::difference_type DynamicArray<T>::iterator::operator-(const iterator &other) const
{
    return iter - other.iter;
}

template <typename T>
bool DynamicArray<T>::iterator::operator==(const iterator &other) const
{
    return iter == other.iter;
}

template <typename T>
bool DynamicArray<T>::iterator::operator!=(const iterator &other) const
{
    return iter != other.iter;
}

template <typename T>
bool DynamicArray<T>::iterator::operator<(const iterator &other) const
{
    return iter < other.iter;
}

template <typename T>
bool DynamicArray<T>::iterator::operator>(const iterator &other) const
{
    return iter > other.iter;
}

template <typename T>
bool DynamicArray<T>::iterator::operator<=(const iterator &other) const
{
    return iter <= other.iter;
}

template <typename T>
bool DynamicArray<T>::iterator::operator>=(const iterator &other) const
{
    return iter >= other.iter;
}

template <typename T>
DynamicArray<T>::const_iterator::const_iterator(const T *data, int r_index)
{
    c_iter = data;
    index = r_index;
}

template <typename T>
typename DynamicArray<T>::const_iterator::reference DynamicArray<T>::const_iterator::operator*() const
{
    return *c_iter;
}

template <typename T>
typename DynamicArray<T>::const_iterator::value_type DynamicArray<T>::const_iterator::operator->() const
{
    return *c_iter;
}

template <typename T>
typename DynamicArray<T>::const_iterator &DynamicArray<T>::const_iterator::operator++()
{
    ++c_iter;
    return *this;
}

template <typename T>
typename DynamicArray<T>::const_iterator DynamicArray<T>::const_iterator::operator++(int)
{
    const_iterator tmp(*this);
    ++c_iter;
    tmp.index++;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::const_iterator &DynamicArray<T>::const_iterator::operator--()
{
    --c_iter;
    index--;
    return *this;
}

template <typename T>
typename DynamicArray<T>::const_iterator DynamicArray<T>::const_iterator::operator--(int)
{
    const_iterator tmp(*this);
    --c_iter;
    tmp.index--;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::const_iterator &DynamicArray<T>::const_iterator::operator+=(difference_type n)
{
    c_iter += n;
    index += n;
    return *this;
}

template <typename T>
typename DynamicArray<T>::const_iterator &DynamicArray<T>::const_iterator::operator-=(difference_type n)
{
    c_iter -= n;
    index -= n;
    return *this;
}

template <typename T>
typename DynamicArray<T>::const_iterator DynamicArray<T>::const_iterator::operator+(difference_type n) const
{
    const_iterator tmp(*this);
    tmp += n;
    tmp.index += n;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::const_iterator DynamicArray<T>::const_iterator::operator-(difference_type n) const
{
    const_iterator tmp(*this);
    tmp -= n;
    tmp.index -= n;
    return tmp;
}

template <typename T>
typename DynamicArray<T>::const_iterator::difference_type DynamicArray<T>::const_iterator::operator-(const const_iterator &other) const
{
    return c_iter - other.c_iter;
}

template <typename T>
bool DynamicArray<T>::const_iterator::operator==(const const_iterator &other) const
{
    return c_iter == other.c_iter;
}

template <typename T>
bool DynamicArray<T>::const_iterator::operator!=(const const_iterator &other) const
{
    return c_iter != other.c_iter;
}

template <typename T>
bool DynamicArray<T>::const_iterator::operator<(const const_iterator &other) const
{
    return c_iter < other.c_iter;
}

template <typename T>
bool DynamicArray<T>::const_iterator::operator>(const const_iterator &other) const
{
    return c_iter > other.c_iter;
}

template <typename T>
bool DynamicArray<T>::const_iterator::operator<=(const const_iterator &other) const
{
    return c_iter <= other.c_iter;
}

template <typename T>
bool DynamicArray<T>::const_iterator::operator>=(const const_iterator &other) const
{
    return c_iter >= other.c_iter;
}
