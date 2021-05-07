#pragma once
#include "MinHeap.h"
#include "Node.h"

// Para manter uma lista com todas os numeros/letras e respetivas posicoes no estado (numbers[26])
struct Letter
{
    char upperLetter;
    char lowerLetter;
    int position1;
    int position2;
    int manhattanDistance;
};

// Para ajudar a funcao que verifica se todos os caminhos sao possiveis (canConnect())
struct Target
{
    int startPosition;
    int manhattanDist;
    bool operator<(Target& other) const
    {
        if (this->manhattanDist < other.manhattanDist)
            return true;
        return false;
    }
    bool operator>(Target& other) const
    {
        if (this->manhattanDist > other.manhattanDist)
            return true;
        return false;
    }
    Target(int startPosition, int manhattanDist)
    {
        this->startPosition = startPosition;
        this->manhattanDist = manhattanDist;
    }
    Target()
    {
        this->startPosition = 0;
        this->manhattanDist = 0;
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
    void setNextNumber();
    void loadInstace(int number);
    // Altera a letra atual para outro caracter (evita que se conecte a ele proprio)
    void maskPathRoot();
    // Restaura a letra atual
    void unmaskPathRoot();
    // Altera pathHead para a proxima posicao
    void moveTo(int position);
    // Verifica se o numero/letra atual esta conectado
    bool isConnected();
    // Devolve um apontador para o caracter correspondente a posicao (cima, esquerda, direita, baixo)
    // Para que is360() e canConnect() tambem possam usar esta funcao, e necessario passar um estado e uma posicao inicial.
    // Coloca a posicao final em endPosition
    const char* look(Direction direction, int startPosition, const char* someState, int& endPosition);
    // Preenche as posicoes em redor de pathHead (cima, esquerda, direita, baixo)
    void setPosAroundPathHead();
    // Verifica se o caminho esta em contacto com ele proprio
    bool isSelfConnectingPath();
    // Verifica se o caminho faz curva de 360, com 1 espaco de intervalo (tipo U)
    bool is360();
    // Funcao recursiva que tenta alcancar um caracter apartir de uma posicao.
    // IMPORTANTE: passar uma copia do estado, pois e alterado.
    bool canConnect(char* stateCopy, int startPosition, int endPosition);    
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
    int calcManhattanDistance(int startPosition, int endPosition);
    int remainingManhattanDistances();
    void updateSuccessorStats(NumberLink* successor);
    // Gera e devolve uma lista de estados sucessores
    void genSuccessors(DLList<Node*>& successors) override;
    int goBack(int position, int places, char* stateCpy);
    bool is360V2();
    // compara o valor heuristico
    bool operator>(Node& node) override;
    int getPriority();
    // compara o valor heuristico
    bool operator<(Node& node) override;
};
