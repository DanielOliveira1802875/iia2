#pragma once
#include "MinHeap.h"
#include "Node.h"
const int MAXOCC = 200;
// Para manter uma lista com todas os numeros/letras e respetivas posicoes no estado (numbers[26])
struct Letter
{
    char upperLetter;
    char lowerLetter;
    int positions[MAXOCC];
    int numOcc;
    int manhattanDistance;
};

struct Connection
{
    int distance;
    int element1;
    int element2;
    Connection()
    {
        this->distance = 0;
        this->element1 = 0;
        this->element2 = 0;
    }
    Connection(int distance, int element1, int element2)
    {
        this->distance = distance;
        this->element1 = element1;
        this->element2 = element2;
    }
    bool operator<(Connection& other)
    {
        if (distance < other.distance)
            return true;
        return false;
    }
    bool operator>(Connection& other)
    {
        if (distance > other.distance)
            return true;
        return false;
    }
};


enum class Direction { up = 0, left, right, down };

class NumberLink : public Node
{
    // Variaveis globais
    static int qntLines;
    static int qntColumns;
    static int outOfBoundsPosition;
    static const int totalNumbers = 26;
    static const char outOfBoundsChar = '?';
    static const char maskChar = '%';
    static const int A = 65;
    static const int a = 97;
    // Mantem a lista de numeros/letras e a respetiva posicao no estado
    static Letter numbers[26];

    // Estado atual
    char* state;
    // Mantem a lista de conexoes 
    bool connected[totalNumbers] = {false};
    // Numero/letra da atual procura
    int currentNumber;
    // Numero da ocurrentica do Numero/letra da atual procura
    int occurrenceIndex;
    int posSize;
    int pos[MAXOCC];
    int islands[MAXOCC];
    // Posicao onde comecou caminho
    int pathRoot;
    // Posicao atual do caminho
    int pathHead;
    // Mantem as posicoes em redor de pathHead (cima, esquerda, direita, baixo)
    int aroundPathHead[4];
    // Guarda o numero de numeros/letras que falta conectar (atual nao incluida)
    int numbersRemaining;


    NumberLink();
    // Prepara o estado para a ligacao do proximo numero/letra
    void setNextConnection();
    void loadInstace(int number);
    int netManDist(int* positions, int size, int* islands_);
    // Altera a letra atual para outro caracter (evita que se conecte a ele proprio)
    void maskPathRoot();
    // Restaura a letra atual
    void unmaskPathRoot();
    // Altera pathHead para a proxima posicao
    void moveTo(int position);
    // Verifica se o numero/letra atual esta conectado
    bool isConnected(int* pathAround);
    bool isLastConnected(int* pathAround);
    // Devolve um apontador para o caracter correspondente a posicao (cima, esquerda, direita, baixo)
    // Para que is360() e canConnect() tambem possam usar esta funcao, e necessario passar um estado e uma posicao inicial.
    // Coloca a posicao final em endPosition
    const char* look(Direction direction, int startPosition, const char* someState, int& endPosition);
    // Preenche as posicoes em redor de pathHead (cima, esquerda, direita, baixo)
    void setPosAround(int position, int* aroundArray);
    // Verifica se o caminho esta em contacto com ele proprio
    bool isSelfConnectingPath();
    // Verifica se o caminho faz curva de 360, com 1 espaco de intervalo (tipo U)
    bool is360();
    // Funcao recursiva que tenta alcancar um caracter apartir de uma posicao.
    // IMPORTANTE: passar uma copia do estado, pois e alterado.
    bool canConnect(char* stateCopy, int startPosition, char letter, int& numOfHits);    
    // Verifica se e possivel conectar as restantes letras
    bool isDeadState();
    bool isOutOfBounds(int position);
public:
    // Devolve um clone deste estado
    Node* getClone() override;
    NumberLink(int instance);
    ~NumberLink() override;
    // Devolve uma representação visual deste estado.
    std::string toString() override;
    std::string toString(char* state_);
    // Compara este estado com outro
    bool operator==(Node& node) override;
    // Verifica se este estado é uma solução
    bool isSolution() override;
    // Neste caso, nada faz, pois sao instancias fixas
    void resetState() override;
    int calcCurrentNumberManhattanDistance(NumberLink* _state);
    int calcManhattanDistance(int startPosition, int endPosition);
    int remainingManhattanDistances();
    void updateSuccessorStats(NumberLink* successor);
    // Gera e devolve uma lista de estados sucessores
    void genSuccessors(DLList<Node*>& successors) override;
    int goBack(int position, int places, char* stateCpy);
    bool is360V2();
    // compara o valor heuristico
   
    int getPriority();
    // compara o valor heuristico
    bool operator<(Node& node) override;
    bool operator>(Node& node) override;
    bool operator<=(Node& node) override;
    bool operator>=(Node& node) override;
};
