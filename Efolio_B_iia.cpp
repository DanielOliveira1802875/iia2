
#include "Search.h"
#include "NumberLink.h"

int main()
{
    /*NumberLink* state = new NumberLink(6);
    Search search(state);
    search.bestFS();
    delete state;*/

    // Best first search
    for (int i = 1; i < 11; i++)
    {   
        std::cout << "################################\n";
        std::cout << "INSTANCIA " << i << std::endl;
        std::cout << "Algoritmo Best First Search\n";
        std::cout << "Sem limite\n";
        std::cout << "Todas as otimizacoes\n";
        NumberLink* state = new NumberLink(i);
        Search search(state);
        search.bestFS();
        delete state;
    }
    // A* sem Limite
    for(int i = 1; i < 11; i++)
    {   
        std::cout << "################################\n";
        std::cout << "INSTANCIA " << i  <<std::endl;
        std::cout << "Algoritmo A*\n";
        std::cout << "Sem limite\n";
        std::cout << "Todas as otimizacoes\n";
        NumberLink* state = new NumberLink(i);
        Search search(state);
        search.aStar();
        delete state;
    }
    // A* Limitado (300)
    for (int i = 1; i < 11; i++)
    {   
        std::cout << "################################\n";
        std::cout << "INSTANCIA " << i << std::endl;
        std::cout << "Algoritmo A*\n";
        std::cout << "Limitado a 300 estados\n";
        std::cout << "Todas as otimizacoes\n";        
        NumberLink* state = new NumberLink(i);
        Search search(state);
        search.aStar(300);
        delete state;
    }
    std::cout << "################################\n";
    return 0;
}


