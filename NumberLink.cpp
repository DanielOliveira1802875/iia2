#include "NumberLink.h"
#include "MinHeap.h"
#include <cstring>


int NumberLink::outOfBoundsPosition = 0;
int NumberLink::qntColumns = 0;
int NumberLink::qntLines = 0;
Letter NumberLink::numbers[totalNumbers];
const char NumberLink::outOfBoundsChar;
Priority Node::priority;


NumberLink::NumberLink()
{
    pathHead = currPosSize = currentNumber = 0;
    state = nullptr;
}

void NumberLink::moveTo(int position)
{
    if (isOutOfBounds(position))
        throw std::invalid_argument("NumberLink::moveTo() Posicao invalida!");
    pathHead = position;
    state[position] = maskChar;
    setPosAround(pathHead, aroundPathHead);
    if (inArray(currentPositions, currPosSize, position))
        return;
    currentPositions[currPosSize++] = position;
}


const char* NumberLink::look(Direction direction, int startPosition, const char* someState, int& endPosition)
{
    bool isPositionValid = false;
    const int line = startPosition / qntColumns;
    endPosition = startPosition;
    switch (direction)
    {
    case Direction::up:
        endPosition -= qntColumns;
        isPositionValid = !isOutOfBounds(endPosition);
        break;
    case Direction::down:
        endPosition += qntColumns;
        isPositionValid = !isOutOfBounds(endPosition);
        break;
    case Direction::left:
        endPosition -= 1;
        isPositionValid = endPosition / qntColumns == line && !isOutOfBounds(endPosition);
        break;
    case Direction::right:
        endPosition += 1;
        isPositionValid = endPosition / qntColumns == line && !isOutOfBounds(endPosition);
        break;
    }
    if (isPositionValid)
        return &(someState[endPosition]);
    endPosition = outOfBoundsPosition;
    return &outOfBoundsChar;
}


void NumberLink::setPosAround(int position, int* aroundArray)
{
    int upPos, downPos, leftPos, rightPos;
    look(Direction::up, position, state, upPos);
    look(Direction::down, position, state, downPos);
    look(Direction::left, position, state, leftPos);
    look(Direction::right, position, state, rightPos);
    aroundArray[0] = upPos;
    aroundArray[1] = leftPos;
    aroundArray[2] = rightPos;
    aroundArray[3] = downPos;
}

// Devolve um clone deste estado
Node* NumberLink::getClone()
{
    NumberLink* newState = new NumberLink();
    newState->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
    memcpy(newState->state, state, static_cast<size_t>(outOfBoundsPosition) + 2);
    memcpy(newState->aroundPathHead, aroundPathHead, sizeof(int) * 4);
    memcpy(newState->connected, connected, sizeof(bool) * totalNumbers);
    memcpy(newState->currentPositions, currentPositions, sizeof(int) * MAXOCC);
    newState->currPosSize = currPosSize;
    newState->pathHead = pathHead;
    newState->currentNumber = currentNumber;

    return static_cast<Node*>(newState);
}


void NumberLink::unmask()
{
    connected[currentNumber] = true;
    for (int i = 0; i < currPosSize; i++)
    {
        if (i < numbers[currentNumber].numOcc)
            state[currentPositions[i]] = numbers[currentNumber].upperLetter;
        else
            state[currentPositions[i]] = numbers[currentNumber].lowerLetter;
    }
}

void NumberLink::setNextConnection()
{
    // enquanto a distancia manhatan de todos os pontos for maior que zero, ainda faltam conexoes
    if ((HeuristicFunc(currentPositions, currPosSize) > 0 && currentNumber >= 0) || currentNumber >= totalNumbers)
        return;
    if (currentNumber >= 0) unmask();    
    currentNumber++;
    for (; currentNumber < totalNumbers; currentNumber++)
    {   // Procura o proximo numero, e prepara a proxima conexao
        if (!connected[currentNumber])
        {
            memcpy(currentPositions, numbers[currentNumber].positions, MAXOCC);
            currPosSize = numbers[currentNumber].numOcc;
            pathHead = numbers[currentNumber].positions[0];
            state[pathHead] = maskChar;
            setPosAround(pathHead, aroundPathHead);
            return;
        }
    }    
    pathHead = outOfBoundsPosition; // Numeros acabaram
}

int NumberLink::HeuristicFunc(int* positions, int size)
{    
    // adiciona a uma min heap todas as distancias entre todos os pontos
    MinHeap<Connection> connections;
    int totalManhattanDistance = 0;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
            connections.addValue(Connection(calcManhattanDistance(positions[i], positions[j]), i, j));

    // cria uma "ilha" desconexa para cada ponto, com um id diferente de todos os outros
    int* islands = new int[MAXOCC];
    for (int i = 0; i < MAXOCC; i++)
        islands[i] = i;

    // Vai extraido da min heap as distancias menores at� que todos os pontos estejam conexos,
    // para o efeito, uso o array ilhas, em que as ocurrencias conexas passam a ter o mesmo id.
    // De notar que os pontos conexos tem uma distancia manhattan de 0.
    int numOfIslands = 0;
    while (!connections.isEmpty() && numOfIslands != 1)
    {
        const Connection conn = connections.removeMin();
        // Se as ilhas ja se encontram conexas, pode avancar para a proxima conexao.
        if (islands[conn.element1] == islands[conn.element2])
            continue; 
        // nova conexao, todas as ilhas conexas passam a ter o mesmo id [ X-X-X + Y-Y ] -> [ X-X-X-X-X ]
        const int mainIsland = islands[conn.element1];
        const int mergeIsland = islands[conn.element2];        
        for (int i = 0; i < size; ++i)
            if (islands[i] == mergeIsland)
                islands[i] = mainIsland;
        totalManhattanDistance += conn.distance;
        // verifica quantas ilhas existem no total, caso exista so uma, termina o ciclo
        const int island = islands[0];
        numOfIslands = 1;
        for (int i = 1; i < size && numOfIslands == 1; ++i)
            if (islands[i] != island)
                numOfIslands++;
    }
    delete[] islands;
    if(totalManhattanDistance <= 1)
        return totalManhattanDistance;
    return static_cast<int>(ceil(static_cast<double>(totalManhattanDistance) * 0.75));
}

NumberLink::NumberLink(int instance)
{
    qntColumns = qntLines = outOfBoundsPosition =  0;
    pathHead = currPosSize = currentNumber = 0;
    state = nullptr;
    loadInstace(instance);
}

NumberLink::~NumberLink() { delete[] state; }

bool NumberLink::operator==(Node& node) { return strcmp(state, ((NumberLink&)node).state) == 0; }

bool NumberLink::isSolution()
{
    // Se o numero seguinte n�o esta conectado, entao ainda faltam conexoes
    if (currentNumber +1 < totalNumbers && numbers[currentNumber + 1].numOcc > 0)
        return false;
    // Se todos os numeros estao conetados, entao setNextConnection() coloca pathHead == outOfBoundsPosition.
    setNextConnection();
    return pathHead == outOfBoundsPosition;
}

void NumberLink::resetState()
{
    // nao e necessario, uma vez que os estados sao estaticos
}

int NumberLink::calcManhattanDistance(int startPosition, int endPosition)
{
    const int horizontalDistance = abs(startPosition % qntColumns - endPosition % qntColumns);
    const int verticalDistance = abs(startPosition / qntColumns - endPosition / qntColumns);
    // o objetivo � estar conexo com o destino e n�o coincidir com ele, dai o -1
    return horizontalDistance + verticalDistance - 1;
}

int NumberLink::remainingHeuristics()
{   // devolve a soma do valor heuristico dos restantes numeros
    int returnValue = 0;
    for (int i = currentNumber + 1; i < 26; i++)
        returnValue += numbers[i].numHeuristic;
    return returnValue;
}


void NumberLink::updateSuccessorStats(NumberLink* successor)
{
    if (priority == Priority::cost) return;
    const int thisNumHeuristic = HeuristicFunc(successor->currentPositions, successor->currPosSize);
    successor->heuristic = thisNumHeuristic  + remainingHeuristics();
}


bool NumberLink::isConnected()
{
    int connections = 0;
    int conns[4];
    for (int j = 0; j < 4; j++)    
        if (!isOutOfBounds(aroundPathHead[j]) && state[aroundPathHead[j]] == numbers[currentNumber].upperLetter)        
            conns[connections++] = aroundPathHead[j];
    for (int i = 0; i < connections; i++)
        moveTo(conns[i]);
    return connections;
}

void NumberLink::genSuccessors(DLList<Node*>& successors)
{
    setNextConnection();
    if (!isConnected())
    {   // Gera os sucessores adjacentes a pathHead, maximo de 3 sucessores
        for (int i = 0; i < 4; i++)
        {
            if (state[aroundPathHead[i]] != '.')
                continue;
            NumberLink* successor = (NumberLink*)getClone();
            successor->moveTo(aroundPathHead[i]);

            if (successor->isSelfConnectingPath() || successor->is360() || successor->isDeadState())
                delete successor; 
            else
            {
                updateSuccessorStats(successor);                
                successors.addToTail((Node*)successor);
            }
        }
    }
    else
    {   // gera um sucessor para cada posicao adjacente as posicoes conexas e respetivos caminhos
       
        for (int j = 0; j < currPosSize; j++)
        {
            if (state[currentPositions[j]] != maskChar)
                continue;
            int aroundPos[4];
            setPosAround(currentPositions[j], aroundPos);            
            for (int i = 0; i < 4; i++)
            {
                if (state[aroundPos[i]] != '.')
                    continue;
                NumberLink* successor = (NumberLink*)getClone();
                successor->moveTo(aroundPos[i]);
                if (successor->isSelfConnectingPath() || successor->isDeadState())
                    delete successor; 
                else
                {
                    updateSuccessorStats(successor);
                    successors.addToTail((Node*)successor);
                }
            }
        }
    }
}


bool NumberLink::isOutOfBounds(int position)
{
    return position < 0 ? true : (position >= outOfBoundsPosition ? true : false);
}

int NumberLink::getPriority()
{
    if (priority == Priority::costPlusHeuristic) return cost + heuristic;
    if (priority == Priority::heuristic) return heuristic;
    return cost;
}

bool NumberLink::operator>(Node& node)
{
    if (getPriority() > ((NumberLink&)node).getPriority())
        return true;
    return false;
}

bool NumberLink::operator>=(Node& node)
{
    if (getPriority() >= ((NumberLink&)node).getPriority())
        return true;
    return false;
}


bool NumberLink::operator<(Node& node)
{
    if (getPriority() < ((NumberLink&)node).getPriority())
        return true;
    return false;
}

bool NumberLink::operator<=(Node& node)
{
    if (getPriority() <= ((NumberLink&)node).getPriority())
        return true;    
    return false;
}

template <class T>
bool NumberLink::inArray(T* arr, int size, T value)
{
    for (int i = 0; i < size; ++i)
        if (arr[i] == value)
            return true;
    return false;
}

void NumberLink::loadInstace(int number)
{
    priority = Priority::cost;
    const char inst1[] = "AB.....AB";
    const char inst2[] = "A......B...........AB.BA";
    const char inst3[] = "C........C.........BB.........A........A";
    const char inst4[] = "B.A.C.CD.........C....B.CD.D..B........A";
    const char inst5[] = "..........A.B.A.B...........B.A.B.A..........";
    const char inst6[] = ".A.A....AB.B.B..........................A.A.A.....B.B.B.....";
    const char inst7[] = "A........GB..A.H...H...B.G.......C.F.......D.E.......................ECD.......F";
    const char inst8[] = "D.............D..C.....A...........B...............C.....A..............AB...B.......C......D.......";
    const char inst9[] = "........A.............A...................................A.A.................................A.....";
    const char inst10[] = "...........BABABABA............ABABABAB......................BABABABA............ABABABAB...........";
    switch (number)
    {
    case 1:
        qntLines = 3;
        qntColumns = 3;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst1, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 2:
        qntLines = 4;
        qntColumns = 6;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst2, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 3:
        qntLines = 4;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst3, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 4:
        qntLines = 4;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst4, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 5:
        qntLines = 5;
        qntColumns = 9;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst5, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 6:
        qntLines = 6;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst6, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 7:
        qntLines = 8;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst7, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 8:
        qntLines = 10;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst8, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 9:
        qntLines = 10;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst9, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 10:
        qntLines = 10;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst10, static_cast<size_t>(outOfBoundsPosition));
        break;
    default:
        throw std::invalid_argument("NumberLink::loadInstace() A instancia nao existe\n");
    }
    state[outOfBoundsPosition + 1] = '\0';
    state[outOfBoundsPosition] = outOfBoundsChar;
    pathHead = outOfBoundsPosition;
    for (short i = 0; i < 26; ++i)
    {
        numbers[i].upperLetter = static_cast<char>(A + i);
        numbers[i].lowerLetter = static_cast<char>(a + i);
        numbers[i].numOcc = 0;
        memset(numbers[i].positions, outOfBoundsPosition, MAXOCC* sizeof(int));
        numbers[i].numHeuristic = 0;
        connected[i] = true;
    }
    for (int i = 0; i < outOfBoundsPosition; i++)
    {
        if (isalpha(state[i]))
        {
            const int numberIndex = state[i] - A;
            numbers[numberIndex].positions[numbers[numberIndex].numOcc] = i;
            numbers[numberIndex].numOcc++;
            connected[numberIndex] = false;
        }
    }
    for (int i = 0; i < totalNumbers; i++)    
        if (numbers[i].numOcc > 0) 
            numbers[i].numHeuristic += HeuristicFunc(numbers[i].positions, numbers[i].numOcc);        
    
    currentNumber = -1;
    setNextConnection();
}
