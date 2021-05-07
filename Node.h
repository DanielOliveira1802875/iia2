#pragma once
#include <string>
#include "DLList.h"

////////////////////////////////////////////////////////////////////////////////////////////////
// A classe que usar os algoritmos implementados em Search.h deve herdar desta classe abstrata,
// implementar as fun��es virtuais, assim como, as propriedades/metodos que lhe sejam
// necessarios para definir e alterar o seu estado.
////////////////////////////////////////////////////////////////////////////////////////////////

enum class Priority { cost = 0, heuristic, costPlusHeuristic };

class Node
{    
    // variveis que definem o estado deste n�    


public:
    Node()
    {
        cost = 1;
        heuristic = 0;
    }
    virtual ~Node() {}

    ////////////////////////////////////////////////
    // OS METODOS DESTA CLASS DEVEM SER  
    // IMPLEMENTADOS  PELAS CLASSES HERDEIRAS
    ////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////
    // PROCURAS CEGAS (BFS,DFS E IDFS)
    //////////////////////////////////////////////////////////////

    // Custo total desde a root (custo dos n�s ascendentes mais o custo deste n�)
    // IMPORTANTE: devemos colocar apenas o custo desde o pai,
    // o algoritmo ir� posteriormente somar o custo dos ascendestes.
    // Caso o custo seja utit�rio, podemos ignorar esta variavel.
    int cost;    
    
    // Apontador para o pai desta Node
    // N�o � necess�rio definir, o algoritmo encarrega-se disso.
    Node* parent = nullptr; 

    // Gera um estado inicial, aleat�rio ou n�o.
    virtual void resetState() = 0;

    // Gera e devolve uma lista de estados sucessores
    // IMPORTANTE: caso o custo seja diferente de 1, e necessario defini-lo,
    // (variavel cost) com custo DESDE O PAI ATE AO SUCESSOR.
    virtual void genSuccessors(DLList<Node*>& successors) = 0;

    // Verifica se este estado � uma solu��o
    virtual bool isSolution() = 0;

    // Devolve um clone deste estado
    virtual Node* getClone() = 0;

    // Devolve uma representa��o visual deste estado.
    virtual std::string toString() = 0;

    // Compara este estado com outro, necessario para procurar duplicados
    virtual bool operator==(Node& node) = 0;

    //////////////////////////////////////////////////////////////
    // PROCURAS INFORMADAS (BESTFS E A*)
    //////////////////////////////////////////////////////////////

    // Para que os metodos dos operadores ('<' '>') saibam o que comparar.
    // Pode assumir os valores (cost, heuristic, costPlusHeuristic).
    // Necess�rio para algoritmos informados, implementados com a MinHeap
    // Por defeito � "cost", mas pode tomar o valor de "heuristic" e "costPlusHeuristic"
    static Priority priority;

    // IMPORTANTE:
    // Se o algoritmo utilizado for informado (bestFS, A*), o n� ter� que calcular este valor.
    // Dever� implementar um metodo que calcule o valor heuristico "[manhattan/euclidean] distance".
    int heuristic;
    
    // IMPORTANTE: Deve comparar o valor heuristico
    virtual bool operator>(Node& node) = 0;
    
    // IMPORTANTE: Deve comparar o valor heuristico
    virtual bool operator<(Node& node) = 0;

};

