#include "NumberLink.h"
#include <limits>
#include <cstring>
#include <algorithm>

#include "MinHeapPtr.h"


int NumberLink::outOfBoundsPosition = 0;
int NumberLink::qntColumns = 0;
int NumberLink::qntLines = 0;
Letter NumberLink::numbers[26];
const char NumberLink::outOfBoundsChar;
Priority Node::priority;


NumberLink::NumberLink()
{
    numbersRemaining = currentNumber = 0;
    pathHead = pathRoot = outOfBoundsPosition;
    state = nullptr;
}

template <class T>
bool NumberLink::inArray(T* arr, int size, T value)
{
    for (int i = 0; i < size; ++i)
        if (arr[i] == value)
            return true;
    return false;
}

void NumberLink::moveTo(int position)
{
    if (isOutOfBounds(position))
        throw std::invalid_argument("NumberLink::moveTo() Posicao invalida!");
    pathHead = position;
    state[position] = newPathChar;
    setPosAround(pathHead, aroundPathHead);
    if (inArray(pos, posSize, position))
        return;
    pos[posSize] = position;
    //islands[posSize] = islands[0];
    posSize++;
}

bool NumberLink::isConnected(int* pathAround)
{
    for (int i = 0; i < 4; ++i)
        if (/*state[pathHead] == numbers[currentNumber].lowerLetter &&*/
            state[pathAround[i]] == numbers[currentNumber].upperLetter /*||
            state[pathHead] == numbers[currentNumber].upperLetter && state[pathAround[i]] == numbers[currentNumber].lowerLetter*/
        )
            return true;
    return false;
}

bool NumberLink::isLastConnected(int* pathAround)
{
    for (int i = 0; i < 4; ++i)
        if (state[pathHead] == numbers[currentNumber].lowerLetter && state[pathAround[i]] == numbers[currentNumber].
            upperLetter ||
            state[pathHead] == numbers[currentNumber].upperLetter && state[pathAround[i]] == numbers[currentNumber].
            lowerLetter)
            return true;
    return false;
}

const char* NumberLink::look(Direction direction, int startPosition, const char* someState, int& endPosition)
{
    bool isPositionValid = false;
    int line = startPosition / qntColumns;
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
    //memcpy(newState->islands, islands, sizeof(int) * MAXOCC);
    memcpy(newState->pos, pos, sizeof(int) * MAXOCC);
    newState->posSize = posSize;
    newState->pathHead = pathHead;
    newState->numbersRemaining = numbersRemaining;
    newState->currentNumber = currentNumber;
    newState->pathRoot = pathRoot;

    return static_cast<Node*>(newState);
}

void NumberLink::setNextConnection()
{
    // entuanto a distancia manhatan de todas as ocurrencias for maior que zero, ainda faltam conexoes
    if (netManDist(pos, posSize) > 0 && currentNumber >= 0 || currentNumber >= totalNumbers)
        return;

    if (currentNumber >= 0)
    {
        connected[currentNumber] = true;
        for (int i = 0; i < posSize; i++)
        {
            if (i < numbers[currentNumber].numOcc)
                state[pos[i]] = numbers[currentNumber].upperLetter;
            else
                state[pos[i]] = numbers[currentNumber].lowerLetter;
        }
    }
    currentNumber++;
    for (; currentNumber < totalNumbers; currentNumber++)
    {   // Procura o proximo numero
        if (!connected[currentNumber])
        {
            memcpy(pos, numbers[currentNumber].positions, MAXOCC);
            /*for (int i = 0; i < MAXOCC; i++)
                islands[i] = i;*/
            posSize = numbers[currentNumber].numOcc;
            pathHead = numbers[currentNumber].positions[0];
            state[pathHead] = newPathChar;
            setPosAround(pathHead, aroundPathHead);
            return;
        }
    }
    // Numeros acabaram
    pathRoot = pathHead = outOfBoundsPosition;
}

// calcula a distancia entre todos as ocorrencias de um numero, e devolve a soma das menores
// distancias manhattan de forma a que todas as ocorrencias esteja conexas.
int NumberLink::netManDist(int* positions, int size)
{
    // adiciona a uma min heap todas as distancias entre todas as ocorrencias
    MinHeap<Connection> connections;
    int totalManhattanDistance = 0;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
            connections.addValue(Connection(calcManhattanDistance(positions[i], positions[j]), i, j));

    // cria uma "ilha" desconexa para cada ocurrencia, com um id diferente de todos os outros
    int* islands = new int[MAXOCC];
    for (int i = 0; i < MAXOCC; i++)
        islands[i] = i;

    // vai extraido da min heap as distancias menores até que todas as ocorrencias estejam conexas
    // para o efeito, uso variavel ilhas, em que as ocurrencias conexas passam a ter o mesmo id de ilha.
    // no final pode apenas existir uma ilha com o mesmo id.
    while (!connections.isEmpty())
    {
        const Connection conn = connections.removeMin();
        if (islands[conn.element1] == islands[conn.element2])
            continue; // estas ilhas ja se encontram conexas, pode avancar.
        // nova conexao, todas as ilhas conexas passam a ter o mesmo id [ X-X-X + Y-Y ] -> [ X-X-X-X-X ]
        const int mergedIsland = islands[conn.element2];
        for (int i = 0; i < size; ++i)
            if (islands[i] == mergedIsland)
                islands[i] = islands[conn.element1];
        totalManhattanDistance += conn.distance;
        // verifica quantas ilhas existem no total, caso exista so uma, então a funcao deve terminar
        const int island = islands[0];
        int numOfIslands = 1;
        for (int i = 1; i < size && numOfIslands == 1; ++i)
            if (islands[i] != island)
                numOfIslands++;
        if (numOfIslands == 1)
            break;
    }
    return totalManhattanDistance;
}

NumberLink::NumberLink(int instance)
{
    qntColumns = qntLines = outOfBoundsPosition = currentNumber = 0;
    pathRoot = pathHead = numbersRemaining = 0;
    state = nullptr;
    loadInstace(instance);
}

NumberLink::~NumberLink() { delete[] state; }


bool NumberLink::operator==(Node& node) { return strcmp(state, ((NumberLink&)node).state) == 0; }

bool NumberLink::isSolution()
{
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
    // o objetivo é estar conexo com o destino e não coincidir com ele, dai o -1
    return horizontalDistance + verticalDistance - 1;
}

int NumberLink::remainingManhattanDistances()
{
    int returnValue = 0;
    for (int i = currentNumber + 1; i < 26; i++)
        returnValue += numbers[i].manhattanDistance;
    return returnValue;
}


void NumberLink::updateSuccessorStats(NumberLink* successor)
{
    if (priority == Priority::cost) return;
    //int thisheu = calcCurrentNumberManhattanDistance(successor);
    const int thisheu = netManDist(successor->pos, successor->posSize);
    const int restheu = remainingManhattanDistances();
    successor->heuristic = thisheu + restheu;
}


bool NumberLink::isConnected()
{
    int connections = 0;
    for (int j = 0; j < 4; j++)
    {
        if (!isOutOfBounds(aroundPathHead[j]) && state[aroundPathHead[j]] == numbers[currentNumber].upperLetter)
        {
            connections++;
            moveTo(aroundPathHead[j]);
        }
    }
    return connections;
}

// Gera os sucessores
void NumberLink::genSuccessors(DLList<Node*>& successors)
{
    setNextConnection();
    bool connection = isConnected();
    if (!connection)
        for (int i = 0; i < 4; i++)
        {
            if (state[aroundPathHead[i]] != '.')
                continue;
            NumberLink* successor = (NumberLink*)getClone();
            successor->moveTo(aroundPathHead[i]);

            if (successor->isSelfConnectingPath() || successor->is360() || successor->isDeadState() )
            {
                //std::cout << successor->toString();
                delete successor;
                continue;
            }
            updateSuccessorStats(successor);
            successors.addToTail((Node*)successor);
        }
    else
    {
        // marca todas as posicoes conexas e respetivo caminho com um caracter diferente
        // de forma a evitar que o novo caminho se conecte com as posicoes anteriores
        for (int i = 0; i < posSize; i++)
        {
            if (state[pos[i]] == newPathChar)
                state[pos[i]] = oldPathChar;
        }
        // gera um filho para cada posicao adjacente as posicoes conexas e respetivos caminhos
        for (int j = 0; j < posSize; j++)
        {
            if (state[pos[j]] != oldPathChar)
                continue;
            int aroundPos[4];
            setPosAround(pos[j], aroundPos);
            for (int i = 0; i < 4; i++)
            {
                if (state[aroundPos[i]] != '.')
                    continue;
                NumberLink* successor = (NumberLink*)getClone();
                successor->moveTo(aroundPos[i]);

                if (successor->isDeadState())
                {
                    //std::cout << successor->toString();
                    delete successor;
                    continue;
                }
                std::cout << successor->toString();
                updateSuccessorStats(successor);
                successors.addToTail((Node*)successor);
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


// Inicia todos os atributos referentes a uma instancia e prepara-se para conetar a letra A
void NumberLink::loadInstace(int number)
{
    priority = Priority::cost;
    if (state != nullptr)
        delete[] state;
    const char inst1[] = "AB.....AB";
    const char inst2[] = "A......B...........AB.BA";
    const char inst3[] = "C........C.........BB.........A........A";
    const char inst4[] = "B.A.C.CD.........C....B.CD.D..B........A";
    const char inst5[] = "..........A.B.A.B...........B.A.B.A..........";
    const char inst6[] = ".A.A....AB.B.B..........................A.A.A.....B.B.B.....";
    const char inst7[] = "A........GB..A.H...H...B.G.......C.F.......D.E.......................ECD.......F";
    const char inst8[] =
        "D.............D..C.....A...........B...............C.....A..............AB...B.......C......D.......";
    const char inst9[] =
        "........A.............A...................................A.A.................................A.....";
    const char inst10[] =
        "...........BABABABA............ABABABAB......................BABABABA............ABABABAB...........";
    const char inst11[] =
        "..........A..BA.HI..........I....G..F.E...F..H..JCBE........C.....G.J.D..................D..........";
    const char inst12[] =
        "AD............C..H...E......B.B.H..............................G........GF.....................A....D...F...E....C.......";
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
    case 11:
        qntLines = 10;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst11, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 12:
        qntLines = 11;
        qntColumns = 11;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst12, static_cast<size_t>(outOfBoundsPosition));
        break;
    default:
        throw std::invalid_argument("NumberLink::loadInstace() A instancia nao existe\n");
    }
    state[outOfBoundsPosition + 1] = '\0';
    state[outOfBoundsPosition] = outOfBoundsChar;
    numbersRemaining = 0;
    currentNumber = 0;
    pathRoot = outOfBoundsPosition;
    pathHead = outOfBoundsPosition;
    for (short i = 0; i < 26; ++i)
    {
        numbers[i].upperLetter = static_cast<char>(A + i);
        numbers[i].lowerLetter = static_cast<char>(a + i);
        numbers[i].numOcc = 0;
        memset(numbers[i].positions, outOfBoundsPosition, MAXOCC);
        numbers[i].manhattanDistance = 0;
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
            ++numbersRemaining;
        }
    }
    for (int i = 0; i < totalNumbers; i++)
    {
        if (numbers[i].numOcc > 0)
        {
            /*int islands[MAXOCC];
            for (int i = 0; i < MAXOCC; ++i)
                islands[i] = i;*/
            numbers[i].manhattanDistance += netManDist(numbers[i].positions, numbers[i].numOcc);
        }
    }
    currentNumber = -1;
    setNextConnection();
    /*--numbersRemaining;
    pathRoot = pathHead = numbers[0].positions[0];
    maskPathRoot();
    setPosAround(pathHead, aroundPathHead);
    for (int i = 0; i < MAXOCC; ++i)
        islands[i]= i;*/
}
