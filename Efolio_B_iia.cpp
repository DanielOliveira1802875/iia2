
#include "Search.h"
#include "NumberLink.h"

int main()
{
    //     INSTANCIA EXEMPLO
    /*char state[] = "...D....C..BE....CA.....E............A....B...D..";
    NumberLink* netManDist = new NumberLink(state, 7, 7);*/

    NumberLink* state = new NumberLink(8);
    Search search(state);
    delete state;

    /*for(int i = 1; i < 13; i++)
    {
        NumberLink* state = new NumberLink(i);
        Search search(state);
        delete state;
    } */   

    /*std::cout << "\n#############################\n        INSTANCIA 12\n#############################";
    char inst13[] = "..@......@......@..A.....@......@......@..@.....@......@....@....@.@....@.....@...@......@....@.........@.@....@..@.@..@..@.........@.......@......@.@....@..@.....@.@..@...@......@.@..@..@..@.@..@.......@....@.....@...@...@...@....@....@.......@....@....@...@...@....@....@....@...@....@....@....@...@....@....@....@......@....@....@....@...@....@....@....@...@.......@....@....@.A.@....@....@....@..";
    NumberLink* state13 = new NumberLink(inst13, 20, 20, Priority::heuristic);
    Search search13(state13);
    delete state13;*/
    
    return 0;
}


