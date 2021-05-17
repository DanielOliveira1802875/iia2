#pragma once
#include "Node.h"

// Numero maximo de ocurrencias de um numero e pontos das respetivas conexoes
const int MAXOCC = 200;
// Para manter uma lista com todas os numeros/letras e respetivas posicoes no estado
struct Letter
{
    char upperLetter;
    char lowerLetter;
    int positions[MAXOCC];
    int numOcc;
    int numHeuristic;
};


// Estrutura usada pela funcao heuristica
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
    static const char maskChar = '@';   
    static const int A = 65;
    static const int a = 97;
    // Mantem a lista de numeros/letras, com as respetivas ocorrencias e
    // o valor heuristico de cada numero, isto previne que os calculos e
    // procura de posicoes sejam feitos repetidamente.
    static Letter numbers[totalNumbers];
    // Estado atual
    char* state;
    // Mantem a lista de conexoes 
    bool connected[totalNumbers] = {false};
    // Numero/letra da atual procura
    int currentNumber;
    // Mantem as ocorrencias do numero atual, assim como os pontos dos caminhos.
    int currentPositions[MAXOCC];
    int currPosSize; 
    // Posicao atual do caminho
    int pathHead;
    // Mantem as posicoes em redor de pathHead (cima, esquerda, direita, baixo)
    int aroundPathHead[4];
    NumberLink();
    // Inicia todos os atributos referentes a uma instancia e prepara-se para conetar a letra A
    void loadInstace(int number);
    // Procura um elemento num array
    template <class T>
    static bool inArray(T* arr, int size, T value);
    // Prepara o estado para a ligacao do proximo numero/letra
    void setNextConnection();
    // Calcula a distancia manhattan entre todos os pontos de um numero, 
    // e devolve a soma das menores distancias de forma a que todas os pontos estejam conexos.
    // Como se podem formar redes de conexoes, esta funcao pode sobrestimar o custo.
    int HeuristicFunc(int* positions, int size);
    // Altera pathHead para a proxima posicao
    void moveTo(int position);
    // Devolve um apontador para o caracter correspondente a posicao (cima, esquerda, direita, baixo)
    // Para que is360() e canConnect() tambem possam usar esta funcao, e necessario passar um estado e uma posicao inicial.
    // Coloca a posicao final em endPosition
    const char* look(Direction direction, int startPosition, const char* someState, int& endPosition);
    // Preenche as posicoes em redor de uma posicao (cima, esquerda, direita, baixo)
    void setPosAround(int position, int* aroundArray);
    // Verifica se o caminho esta em contacto com ele proprio
    bool isSelfConnectingPath();
    // Verifica se o caminho faz curva de 360, com 1 espaco de intervalo (tipo U)
    bool is360();
    // Funcao recursiva que tenta alcancar um ou mais caracters apartir de uma posicao.
    // IMPORTANTE: passar uma copia do estado, pois e alterado.
    bool canConnect(char* stateCopy, int startPosition, char letter, int& numOfHits);
    // Verifica se e possivel conectar as restantes letras
    bool isDeadState();
    // Verifica se uma posicao e valida
    bool isOutOfBounds(int position);
    // Remove todas as "mascaras" que previnem a conexao de pontos ja conectados
    void unmask();
    // calcula a distancia manhattan entre dois pontos
    int calcManhattanDistance(int startPosition, int endPosition);
    // Devolve os valores heuristicos dos restates numeros.
    int remainingHeuristics();
    // Calcula o valor heuristico do sucessor.
    void updateSuccessorStats(NumberLink* successor);
    // Verifica se um caminho esta conectado, em caso positivo, coloca a mascara nessa ocorrencia.
    bool isConnected();
    // Devolve a prioridade do estado
    int getPriority();
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
    // Gera e devolve uma lista de estados sucessores
    void genSuccessors(DLList<Node*>& successors) override;
    // compara a prioridade (necessario para a minHeap)
    bool operator<(Node& node) override;
    bool operator>(Node& node) override;
    bool operator<=(Node& node) override;
    bool operator>=(Node& node) override;
};
