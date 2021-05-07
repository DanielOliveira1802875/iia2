#pragma once
#include <iostream>

///////////////////////////////////////////////////////////
/////////////////// class DNode ///////////////////////////
///////////////////////////////////////////////////////////

template<class T>
class DNode // Node de uma doubly linked list
{
public:
    T value;
    DNode* next;
    DNode* prev;
    DNode(T value, DNode* prev, DNode* next);
    ~DNode()
    {
    }
};

template<class T>
DNode<T>::DNode(T value, DNode<T>* prev, DNode<T>* next)
{
    this->value = value;
    this->prev = prev;
    this->next = next;
}

///////////////////////////////////////////////////////////
////////////////// class DLList ///////////////////////////
///////////////////////////////////////////////////////////

template<class T>
class DLList
{
    DNode<T>* head;
    DNode<T>* tail;
    int size;
    DNode<T>* iterator;
public:
    DLList();
    int getSize();
    void addToHead(T value);
    void addToHead(DLList<T>& list);
    void addToTail(T value);
    void addToTail(DLList<T>& list);
    T deleteFromHead();
    T deleteFromTail();
    bool deleteNode(T value);
    bool deleteIterator(T& returnValue);
    bool isInList(T value);
    bool isEmpty();
    bool onlyOneNode();
    ~DLList();
    void setIteratorToHead();
    bool isIteratorValid();
    T getIteratorValue();
    bool iteratorNext();
};

template <class T>
DLList<T>::DLList()
{
    head = tail = iterator = nullptr;
    size = 0;
}

template <class T>
int DLList<T>::getSize()
{
    return size;
}

template<class T>
void DLList<T>::addToHead(T value)
{
    DNode<T>* tmp = new DNode<T>(value, nullptr,head);
    if (!isEmpty())
    {
        head->prev = tmp;
        head = tmp;
    }
    else
        head = tail = tmp;
    ++size;
}

template <class T>
void DLList<T>::addToHead(DLList<T>& list)
{
    if (list.isEmpty())
        return;

    size += list.size;
    if(!this->isEmpty())
    {
        list.tail->next = this->head;
        this->head->prev = list.tail;
        this->head = list.head;       
    }
    else
    {
        this->head = list.head;
        this->tail = list.tail;
    }    
    list.head = list.tail = list.iterator = nullptr;
    list.size = 0;
}

template<class T>
void DLList<T>::addToTail(T value)
{    
    if (!isEmpty())
    {
        tail->next = new DNode<T>(value,tail,nullptr);
        tail = tail->next;
    }
    else
        head = tail = new DNode<T>(value,nullptr,nullptr);
    ++size;
}

template <class T>
void DLList<T>::addToTail(DLList<T>& list)
{
    if (list.isEmpty())
        return;

    size += list.size;
    if (!this->isEmpty())
    {
        this->tail->next = list.head;
        list.head->prev = this->tail;
        this->tail = list.tail;
    }
    else
    {
        this->head = list.head;
        this->tail = list.tail;
    }    
    list.head = list.tail = list.iterator = nullptr;
    list.size = 0;
}

template <class T>
T DLList<T>::deleteFromHead()
{
    T value = head->value;
    if(!onlyOneNode())
    {
        if (iterator == head)
            iterator = nullptr;
        DNode<T>* tmp = head;        
        head = head->next;        
        head->prev = nullptr;
        delete tmp;
    }
    else if (onlyOneNode())
    {
        delete head;
        head = tail = iterator = nullptr;
    }
    --size;
    return value;
}

template <class T>
T DLList<T>::deleteFromTail()
{
    T value = tail->value;
    if (!onlyOneNode())
    {
        if (iterator == tail)
            iterator = nullptr;
        tail = tail->prev;
        delete tail->next;        
        tail->next = nullptr;
    }
    else if (onlyOneNode())
    {
        delete head;
        head = tail = iterator = nullptr;
    }
    --size;
    isEmpty();
    return value;
}

template <class T>
bool DLList<T>::deleteNode(T value)
{
    if (isEmpty() || (onlyOneNode() && value != head->value))
        return false;    

    if (onlyOneNode() && (value == head->value))
    {
        deleteFromHead();
        return true;
    }
    
    DNode<T>* tmp = head;

    while(tmp != nullptr)
    {
        if (tmp->value != value)
        {
            tmp = tmp->next;
            continue;
        }

        if (tmp == head)
        {            
            deleteFromHead();
            tmp = head;
        }
        else if(tmp == tail)
        {            
            deleteFromTail();
            return true;
        }
        else
        {
            if (tmp == iterator)
                iterator = nullptr;
            DNode<T>* prev = tmp->prev;
            DNode<T>* next = tmp->next;
            prev->next = next;
            next->prev = prev;            
            delete tmp;
            tmp = next;
            --size;
        }
        return true;
    }
    return false;
}

template <class T>
bool DLList<T>::deleteIterator(T& returnValue)
{
    if (iterator == nullptr || isEmpty())
        return false;
    returnValue = iterator->value;
    if (iterator == head)
    {
        deleteFromHead();
        iterator = head;
        return true; 
    }
    if (iterator == tail)
    {
        deleteFromTail();
        iterator = nullptr;
        return true;
    }
    DNode<T>* tmpPtr = head;
    while (tmpPtr->next != iterator)    
        tmpPtr = tmpPtr->next;    
    tmpPtr->next = iterator->next;
    delete iterator;
    iterator = tmpPtr->next;
    iterator->prev = tmpPtr;
    --size;
    return true;
}

template <class T>
bool DLList<T>::isInList(T value)
{
    DNode<T>* tmp = head;
    while (tmp != nullptr)
    {
        if((value) == (tmp->value))
            return true;
        tmp = tmp->next;
    }
    return false;
}

template <class T>
bool DLList<T>::isEmpty()
{
    if ((head == nullptr && tail != nullptr) || (head != nullptr && tail == nullptr))
        std::cout << "erro";
    return (head == nullptr && tail == nullptr);
}

template <class T>
bool DLList<T>::onlyOneNode()
{
    return !isEmpty() && (head == tail);
}

template <class T>
DLList<T>::~DLList()
{
    while(!isEmpty())
        deleteFromHead();
}

template <class T>
void DLList<T>::setIteratorToHead()
{
    iterator = head;
}

template <class T>
bool DLList<T>::isIteratorValid()
{
    return iterator != nullptr;
}

template <class T>
T DLList<T>::getIteratorValue()
{
    return iterator->value;
}

template <class T>
bool DLList<T>::iteratorNext()
{
    if (iterator == nullptr)
        return false;
    iterator = iterator->next;
    return true;
}


