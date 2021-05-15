
#include "Search.h"
#include "NumberLink.h"

int main()
{

    for (int i = 1; i < 11; i++)
    {   // Best first search
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
    for(int i = 1; i < 11; i++)
    {   // A* sem Limite
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
    for (int i = 1; i < 11; i++)
    {   // A* Limitado (300)
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


