#pragma once
#include "DLList.h"
#include "Node.h"
#include "MinHeapPtr.h"
#include "HashTable.h"

class Search
{
    // lista de nodes para serem expandidas 
    DLList<Node*> open;
    // lista de prioridade com nodes para serem expandidas 
    MinHeapPtr<Node*> minOpen;
    // lista de nodes expandidas
    DLList<Node*> closed;
    // Para localizar duplicados de forma mais rapida, a chave é uma string, usar (node->toString).
    // IMPORTANTE: nao utilizar com profundidade limitada, pode levar a que não se encontre uma solução.
    HashTable<Node*>* knownStates;

    int totGenerations;
    int totExpansions;
    bool withDuplicates;
    bool reconstructPath;
    bool useHashTable;
    // limite de estados em open ou custo
    int limit;    
    Node* root;

public:   
    Search(Node* root);
    ~Search();
    void resetSearch(); 
    // Atualiza o atributo pai e custo total de cada sucessor
    void updateNodeStats(DLList<Node*>& successors, Node* parent);
    void printPath(Node* currentNode);
    void printStats(Node* node = nullptr);
    void initializeKnownStates(Node* rootCopy);

    bool dFS();
    bool bFS();
    bool aStar(int maxOpen = -1, Priority priority = Priority::costPlusHeuristic);
    bool bestFS();

    void removeDuplicates(DLList<Node*>& successors);

    void clearLists();

};

