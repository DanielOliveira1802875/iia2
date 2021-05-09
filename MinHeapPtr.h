#pragma once
#include <string>
#include <type_traits>
#include "Node.h"

template <class T>
class MinHeapPtr
{
    int capacity;
    int size;
    T* array;
    void increaseSizeOfArray();
    int indexOfParent(int childIndex) const;
    int indexOfLeftChild(int indexOfParent) const;
    int indexOfRightChild(int indexOfParent) const;
    void swap(int index1, int index2);
    int minValueIndex(int index1, int index2);
    void bubbleDown(int index);
    void bubbleUp(int index);
    bool isValidIndex(int index);
    bool isPointer;
public:
    void addValue(T value);
    T removeMin();
    T peekMin();
    bool isEmpty();    
    MinHeapPtr();
    void clear();
    ~MinHeapPtr();
};

template <class T>
void MinHeapPtr<T>::increaseSizeOfArray()
{
    capacity *= 2;
    T* newArray = new T[capacity];
    memcpy(newArray, array, size * sizeof(T));
    delete array;
    array = newArray;
}

template <class T>
int MinHeapPtr<T>::indexOfParent(int childIndex) const
{
    return (childIndex -1) / 2;
}


template <class T>
int MinHeapPtr<T>::indexOfLeftChild(int indexOfParent) const
{
    return (indexOfParent * 2) + 1;
}

template <class T>
int MinHeapPtr<T>::indexOfRightChild(int indexOfParent) const
{
    return (indexOfParent * 2) + 2;
}

template <class T>
void MinHeapPtr<T>::addValue(T value)
{
    if (size == capacity)
        increaseSizeOfArray();
    array[size] = value;
    ++size;
    bubbleUp(size -1);
}

template <class T>
T MinHeapPtr<T>::removeMin()
{
    if (isEmpty())
        throw ("MinHeapPtr::removeMin() - Empty heap");
    T returnValue = array[0];
    array[0] = array[--size];
    bubbleDown(0);
    return returnValue;
}

template <class T>
T MinHeapPtr<T>::peekMin()
{
    if (isEmpty())
        throw ("MinHeapPtr::peekMin() - Heap is empty");
    return array[0];
}

template <class T>
bool MinHeapPtr<T>::isEmpty()
{
    return size == 0;
}

template <class T>
bool MinHeapPtr<T>::isValidIndex(int index)
{
    return index >= 0 && index < size;
}

template <class T>
void MinHeapPtr<T>::swap(int index1, int index2)
{
    if (!isValidIndex(index1) || !isValidIndex(index2))
        return;
    T aux = array[index1];
    array[index1] = array[index2];
    array[index2] = aux;
}

template <class T>
int MinHeapPtr<T>::minValueIndex(int index1, int index2)
{
    if (!isValidIndex(index1) && !isValidIndex(index2)) return -1;
    if (!isValidIndex(index1)) return index2;
    if (!isValidIndex(index2)) return index1;    
    if (*(array[index1]) < *(array[index2])) return index1;
    return index2;
}

template <class T>
void MinHeapPtr<T>::bubbleDown(int index)
{
    if (isEmpty() || !isValidIndex(index) || index == size -1)
        return;
    const int leftChildIndex = indexOfLeftChild(index);
    const int rightChildIndex = indexOfRightChild(index);
    const int minChildIndex = minValueIndex(leftChildIndex, rightChildIndex);
    const bool mustSwap = isValidIndex(minChildIndex) && *(array[index]) >= *(array[minChildIndex]);
    if (mustSwap)
    {
        swap(index, minChildIndex);
        bubbleDown(minChildIndex);
    }
    
}

template <class T>
void MinHeapPtr<T>::bubbleUp(int index)
{
    if (isEmpty() || !isValidIndex(index) || index == 0)
        return;
    int parentIndex = indexOfParent(index);
    const bool mustSwap = isValidIndex(parentIndex) && *(array[index]) <= *(array[parentIndex]);
    if (mustSwap)
    {
        swap(parentIndex, index);
        bubbleUp(parentIndex);
    }    
}

template <class T>
MinHeapPtr<T>::MinHeapPtr()
{
    isPointer = std::is_pointer<T>::value;
    capacity = 10;
    size = 0;
    array = new T[10];
}

template <class T>
void MinHeapPtr<T>::clear()
{
    for (int i = 0; i < size; ++i)
        delete array[i];
    size = 0;
}

template <class T>
MinHeapPtr<T>::~MinHeapPtr()
{
    clear();
    delete array;
}
