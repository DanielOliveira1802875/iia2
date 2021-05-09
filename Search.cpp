#include "Search.h"
#include <iostream>

Search::Search(Node* root)
{
    this->root = root;
    resetSearch();
    startSearch();
}

void Search::setRootNode(Node* root)
{
    this->root = root;
    resetSearch();
    startSearch();
}


void Search::resetSearch()
{
    totGenerations = totExpansions = limit = 0;
    root->cost = 0;
    root->parent = nullptr;
    root->resetState();
    knownStates = nullptr;
    withDuplicates = reconstructPath = useHashTable = false;
}

Search::~Search()
{
}

void Search::startSearch()
{
    limit = -1;
    reconstructPath = false;
    withDuplicates = false;
    useHashTable = false;


    /*if (!aStar())
        printStats();*/

    if (!aStar(/*Priority::heuristic*/))
        printStats();

    // DFS
     /*if (!dFS())
        printStats();*/

    //BFS
    /*if (!bFS())
        printStats();*/


}


// Atualiza o atributo pai e custo total de cada sucessor
void Search::updateNodeStats(DLList<Node*>& successors, Node* parent)
{
    successors.setIteratorToHead();
    while (successors.isIteratorValid())
    { 
        Node* tmp = successors.getIteratorValue();
        tmp->cost += parent->cost;
        if(withDuplicates || reconstructPath)
            tmp->parent = parent;
        successors.iteratorNext();
    }
}

void Search::printPath(Node* currentNode)
{
    std::cout << "------------------";
    while (currentNode != nullptr)
    {
        std::cout << currentNode->toString() << "------------------";
        currentNode = currentNode->parent;
    }
}

void Search::printStats(Node* node)
{
    if(node != nullptr && node->isSolution())
    {        
        if (reconstructPath) 
            printPath(node);
        else 
            std::cout << node->toString();
        std::cout << "\nSolucao encontrada\n";
        std::cout << "Custo da Solucao:    " << node->cost << std::endl;
    }
    else    
        std::cout << "\nSolucao nao encontrada\n";       
    std::cout << "Numero de expancoes: " << totExpansions << std::endl;
    std::cout << "Numero de geracoes:  " << totGenerations << std::endl;
}


void Search::initializeKnownStates(Node* rootCopy)
{
    if (knownStates != nullptr)
        knownStates->clear();
    knownStates = new HashTable<Node*>(9887);
    knownStates->add(rootCopy->toString(), nullptr);
}

bool Search::dFS()
{
    
    if(limit == 0) // DFS iterativo
    {
        limit++;
        while (dFS() == false)        
            limit++;
        clearLists();
        return true;
    }
    Node* rootCopy = root->getClone();
    open.addToHead(rootCopy);
    if(withDuplicates && useHashTable)    
        initializeKnownStates(rootCopy);    
    while (!open.isEmpty())
    {
        Node* currentNode = open.deleteFromHead();
        //std::cout << currentNode->toString();
        if (currentNode->isSolution())
        {            
            printStats(currentNode);
            clearLists();
            return true;
        }
        // Se existe um limite no custo e este foi alcancado, nao expande este no.
        if(limit > 0 && currentNode->cost >= limit)
        {
            if (withDuplicates || reconstructPath)
                closed.addToHead(currentNode);
            else
                delete currentNode;
            continue;
        }
        ++totExpansions;
        DLList<Node*> newNodes;
        currentNode->genSuccessors(newNodes);
        updateNodeStats(newNodes, currentNode);
        if (withDuplicates)  
            removeDuplicates(newNodes);
        totGenerations += newNodes.getSize();
        open.addToHead(newNodes);
        if (withDuplicates || reconstructPath)
            closed.addToHead(currentNode);
        else
            delete currentNode;
    }
    clearLists();    
    return false;
}

bool Search::bFS()
{
    Node* rootCopy = root->getClone();
    open.addToHead(rootCopy);
    if (useHashTable)
        initializeKnownStates(rootCopy);
    while (!open.isEmpty())
    {
        Node* currentNode = open.deleteFromHead();
        //std::cout << currentNode->toString();
        if (currentNode->isSolution())
        {
            printStats(currentNode);
            clearLists();
            return true;
        }
        ++totExpansions;
        DLList<Node*> newNodes;
        currentNode->genSuccessors(newNodes);
        updateNodeStats(newNodes, currentNode); // atualiza pais e custos
        removeDuplicates(newNodes);
        totGenerations += newNodes.getSize();
        open.addToTail(newNodes);
        if (withDuplicates || reconstructPath)
            closed.addToHead(currentNode);
        else
            delete currentNode;
    }
    clearLists();
    printStats();
    return false;
}

bool Search::aStar(Priority priority)
{
    Node* rootCopy = root->getClone();
    Node::priority = priority;
    minOpen.addValue(rootCopy);
    if (withDuplicates && useHashTable)
        initializeKnownStates(rootCopy);
    while (!minOpen.isEmpty())
    {
        Node* currentNode = minOpen.removeMin();
        std::cout << currentNode->toString();
        if (currentNode->isSolution())
        {
            printStats(currentNode);
            clearLists();
            return true;
        }
        ++totExpansions;
        DLList<Node*> newNodes;
        currentNode->genSuccessors(newNodes);
        updateNodeStats(newNodes, currentNode);
        if (withDuplicates)
            removeDuplicates(newNodes);
        totGenerations += newNodes.getSize();
        while (!newNodes.isEmpty())
        {
            Node* successor = newNodes.deleteFromHead();
            minOpen.addValue(successor);
        }
        if (withDuplicates || reconstructPath)
            closed.addToHead(currentNode);
        else
            delete currentNode;
    }
    clearLists();
    return false;
}

void Search::removeDuplicates(DLList<Node*>& successors)
{
    if (!withDuplicates)
        return;
    // Verifica na hastable
    if(useHashTable && knownStates != nullptr)
    {
        successors.setIteratorToHead();
        while (successors.isIteratorValid() && !successors.isEmpty())
        {
            Node* discardNode = nullptr;
            Node* tmp = successors.getIteratorValue();
            std::string key = tmp->toString();
            if (knownStates->isInList(key))
            {
                if (successors.deleteIterator(discardNode))
                    delete discardNode;
            }
            else
            {
                knownStates->add(key, nullptr); // passo nullptr porque já tenho referencia para ele em open ou closed
                successors.iteratorNext();
            }
        }
        return;
    }

    // verifica só no proprio caminho (até à root)
    successors.setIteratorToHead();
    while (successors.isIteratorValid())
    {
        Node* discardNode = nullptr;
        Node* tmp = successors.getIteratorValue()->parent;
        while(tmp != nullptr)
        {            
            if(*tmp == *(successors.getIteratorValue()))
            {
                if (successors.deleteIterator(discardNode))
                    delete discardNode;
                break;
            }
            tmp = tmp->parent;
        }
        successors.iteratorNext();
    }

}

void Search::clearLists()
{
    while (!open.isEmpty())
        delete open.deleteFromHead();
    while (!closed.isEmpty())
        delete closed.deleteFromHead();
    minOpen.clear();

    if (knownStates != nullptr)
    {   // NOTA: Porque passei nullptr no valor do no. Caso contrario, era necessário desalocar a memoria (aqui OU em open e close).
        knownStates->clear();        
        delete knownStates;
        knownStates = nullptr;
    }   
}
