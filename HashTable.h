#pragma once
#include <iostream>
#include <string>
#include <vector>


////////////////// class SNode ///////////////////////////

template <class T>
class SNode
{
public:
    std::string key;
    T value;
    SNode* next;
    SNode(std::string key, T value, SNode* next = nullptr);

    ~SNode()
    {
    }
};

template <class T>
SNode<T>::SNode(std::string key, T value, SNode<T>* next)
{
    this->key = key;
    this->value = value;
    this->next = next;
}

////////////////// class HashTable ///////////////////////////

template <class T>
class HashTable
{
    std::hash<std::string> hasher;
    // Numero de nos
    unsigned size;
    // Tamanhdo do vetor (buckets)
    unsigned containerSize;
    std::vector<SNode<T>*> container;
    // Funcao hash, que devolve o index de uma key (string)
    unsigned hashFunction(std::string& key) const;
    // Remove um no de um index, e coloca o seu valor em returnValue
    bool deleteFromIndex(unsigned index, T& returnValue);
public:
    // IMPORTANTE: usar numero primo para uma distribuicao uniforme.
    HashTable(unsigned containerSize);
    // Elimina o primeiro nó da lista, coloca o seu valor em returnValue
    bool deleteFirst(T& returnValue);
    // Adiciona um valor a lista
    void add(std::string key, T value);
    // Apaga o no correspondente a chave e coloca o seu valor em returnValue.
    bool deleteKey(std::string key, T& returnValue);
    // Verifica se uma chave existe na lista
    bool isInList(std::string& key);
    // Verifica se a lista esta vazia
    bool isEmpty() const;
    // Elimina todos os nos da lista
    // IMPORTANTE: se T tem memoria alocada, este método nao desaloca essa memoria.
    bool clear();
    ~HashTable();
};

template <class T>
unsigned HashTable<T>::hashFunction(std::string& key) const
{
    return hasher(key) % containerSize;
    /*unsigned index = 0;
    for (char ch : key)        
        index = (31 * index) + ch;
    return index % containerSize;*/
}

template <class T>
HashTable<T>::HashTable(unsigned containerSize)
{
    size = 0;
    this->containerSize = containerSize;
    container.resize(containerSize);
    for (unsigned i = 0; i < containerSize; ++i)
        container[i] = nullptr;
}

template <class T>
bool HashTable<T>::deleteFirst(T& returnValue)
{
    for (int i = 0; i < containerSize; ++i)
        if (container[i] != nullptr)
            return deleteFromIndex(i, returnValue);        
    return false;
}

template <class T>
void HashTable<T>::add(std::string key, T value)
{
    const unsigned index = hashFunction(key);
    container[index] = new SNode<T>(key, value, container[index]);
    ++size;
}

template <class T>
bool HashTable<T>::deleteFromIndex(unsigned index, T& returnValue)
{
    if (container[index] == nullptr)
        return false;
    returnValue = container[index]->value;
    SNode<T>* tmpPtr = container[index];
    container[index] = container[index]->next;
    delete tmpPtr;
    --size;
    return true;
}


template <class T>
bool HashTable<T>::deleteKey(std::string key, T& returnValue)
{
    const unsigned index = hashFunction(key);
    SNode<T>* currentNode = container[index];
    SNode<T>* previousNode = nullptr;
    while (currentNode != nullptr && currentNode->key != key)
    {
        previousNode = currentNode;
        currentNode = currentNode->next;
    }
    if (currentNode == nullptr)
        return false;
    returnValue = currentNode->value;
    if (previousNode != nullptr)
        previousNode->next = currentNode->next;
    else
        container[index] = currentNode->next;
    delete currentNode;
    --size;
    return true;
}


template <class T>
bool HashTable<T>::isInList(std::string& key)
{
    const unsigned index = hashFunction(key);
    SNode<T>* tmp = container[index];
    while (tmp != nullptr)
    {
        if (key == tmp->key)
            return true;
        tmp = tmp->next;
    }
    return false;
}

template <class T>
bool HashTable<T>::isEmpty() const
{
    return size == 0;
}

// Elimina todos os nos da lista
// IMPORTANTE: se T tem memoria alocada, este método nao desaloca essa memoria.
template <class T>
bool HashTable<T>::clear()
{
    T discardValue;
    for (unsigned i = 0; i < containerSize; ++i)
        while (container[i] != nullptr)
            deleteFromIndex(i, discardValue);
    return isEmpty();
}

template <class T>
HashTable<T>::~HashTable()
{
    clear();
}
