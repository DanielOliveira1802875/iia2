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

void NumberLink::maskPathRoot()
{
    if (state[pathRoot] == outOfBoundsChar)
        return;
    state[pathRoot] = maskChar;
}

void NumberLink::unmaskPathRoot()
{
    if (state[pathRoot] == outOfBoundsChar)
        return;
    state[pathRoot] = numbers[currentNumber].upperLetter;
}

void NumberLink::moveTo(int position)
{
    if (position < 0 || position >= outOfBoundsPosition || state[position] != '.')
        throw std::invalid_argument("NumberLink::moveTo() Posicao invalida!");
    pathHead = position;
    state[position] = numbers[currentNumber].lowerLetter;
    setPosAround(pathHead,aroundPathHead);
}

bool NumberLink::isConnected(int* pathAround)
{
    for (int i = 0; i < 4; ++i)
        if (/*state[pathHead] == numbers[currentNumber].lowerLetter &&*/ state[pathAround[i]] == numbers[currentNumber].upperLetter /*||
            state[pathHead] == numbers[currentNumber].upperLetter && state[pathAround[i]] == numbers[currentNumber].lowerLetter*/)
            return true;
    return false;
}

bool NumberLink::isLastConnected(int* pathAround)
{
    for (int i = 0; i < 4; ++i)
        if (state[pathHead] == numbers[currentNumber].lowerLetter && state[pathAround[i]] == numbers[currentNumber].upperLetter ||
            state[pathHead] == numbers[currentNumber].upperLetter && state[pathAround[i]] == numbers[currentNumber].lowerLetter)
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
    memcpy(newState->islands, islands, sizeof(int) * MAXOCC);
    newState->pathHead = pathHead;
    newState->numbersRemaining = numbersRemaining;
    newState->currentNumber = currentNumber;
    newState->pathRoot = pathRoot;
    
    newState->occurrenceIndex = occurrenceIndex;
    return static_cast<Node*>(newState);
}

void NumberLink::setNextConnection()
{
    /*if (!isConnected(aroundPathHead) || pathHead == outOfBoundsPosition)
        return;*/
    unmaskPathRoot();

    for (int i = 0; i< 4;i++)
    {
        for (int j= 0; j< numbers[currentNumber].occurrences; j++)
        {
            if (aroundPathHead[i] == numbers[currentNumber].positions[j])
                islands[j] = islands[occurrenceIndex];
        }
    }
    int* islandsCpy = new int[MAXOCC];
    memcpy(islandsCpy, islands, sizeof(int) * MAXOCC);
    Connection conn = nextOccurance(numbers[currentNumber].positions, numbers[currentNumber].occurrences, islandsCpy);
    if( conn.element1 != MAXOCC)
    {
        currentConnectionCost = conn.distance;
        std::cout << toString();
        pathRoot = pathHead = numbers[currentNumber].positions[occurrenceIndex];
        setPosAround(pathHead, aroundPathHead);
        maskPathRoot();
        return;
    }
    //for (++occurrenceIndex; occurrenceIndex < numbers[currentNumber].occurrences; ++occurrenceIndex)
    //{   // verifica a conecao das ocorrencias do numero atual
    //    pathRoot = pathHead = numbers[currentNumber].positions[occurrenceIndex];  
    //    setPosAround(pathHead, aroundPathHead);
    //    if ( occurrenceIndex == numbers[currentNumber].occurrences -1 && isLastConnected(aroundPathHead))        
    //        break;
    //    
    //    if (!isConnected(aroundPathHead))
    //    {
    //        maskPathRoot();
    //        return;
    //    }
    //}
    // O numero atual esta conectado, passa para o proximo
    connected[currentNumber] = true;
    currentNumber++;
    for (; currentNumber < totalNumbers; currentNumber++)
    {   // Procura o proximo numero
        if (!connected[currentNumber])
        {
            occurrenceIndex = 0;
            pathRoot = pathHead = numbers[currentNumber].positions[occurrenceIndex];
            maskPathRoot();
            setPosAround(pathHead, aroundPathHead);
            return;
        }
    }
    // Numeros acabaram
    pathRoot = pathHead = outOfBoundsPosition;
}

// Inicia todos os atributos referentes a uma instancia e prepara-se para conetar a letra A
void NumberLink::loadInstace(int number)
{
    priority = Priority::cost;
    if (state != nullptr)
        delete[] state;
    const char inst1[] = "AB.....AB";
    const char inst2[] = "A.B...B.A";
    const char inst3[] = "C........C.........BB.........A........A";
    const char inst4[] = "AB..CD...........................AB...CD";
    const char inst5[] = "............A....B....B.CD.A..C........D";
    const char inst6[] = "D........C..A....B....B....A..C........D";
    const char inst7[] = "C..............D.BA............DA........E...B..E.......C...";
    const char inst8[] =
        "D.............D..C.....A...........B...............C.....A..............AB...B.......C......D.......";
    //const char inst9[] = "........A.............A...................................A.A.................................A.....";
    const char inst9[] = ".A......A.....A.......A.....A......A......A..............A.A.............A...................A.....";
    const char inst10[] = "............BC..D......D.E..............F.E.G........................H..ABCAHF..G";
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
        qntLines = 0;
        qntColumns = 0;
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
        qntLines = 4;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst5, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 6:
        qntLines = 4;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst6, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 7:
        qntLines = 6;
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
        qntLines =10;
        qntColumns = 10;
        outOfBoundsPosition = qntColumns * qntLines;
        this->state = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
        memcpy(state, inst9, static_cast<size_t>(outOfBoundsPosition));
        break;
    case 10:
        qntLines = 9;
        qntColumns = 9;
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
    occurrenceIndex = 0;
    pathRoot = outOfBoundsPosition;
    pathHead = outOfBoundsPosition;
    for (short i = 0; i < 26; ++i)
    {
        numbers[i].upperLetter = static_cast<char>(A + i);
        numbers[i].lowerLetter = static_cast<char>(a + i);
        numbers[i].occurrences = 0;
        memset(numbers[i].positions, outOfBoundsPosition, MAXOCC);
        numbers[i].manhattanDistance = 0;
        connected[i] = true;
    }
    for (int i = 0; i < outOfBoundsPosition; i++)
    {
        if (isalpha(state[i]))
        {
            const int numberIndex = state[i] - A;
            numbers[numberIndex].positions[numbers[numberIndex].occurrences] = i;
            numbers[numberIndex].occurrences++;            
            connected[numberIndex] = false;
            ++numbersRemaining;
        }        
    }
    for (int i = 0; i < totalNumbers; i++)
    {
        if(numbers[i].occurrences > 0)
        {
            int islands[MAXOCC];
            for (int i = 0; i < MAXOCC; ++i)
                islands[i] = i;
            numbers[i].manhattanDistance += netManDist(numbers[i].positions, numbers[i].occurrences, islands);
        }
    }
    --numbersRemaining;
    pathRoot = pathHead = numbers[0].positions[0];
    setPosAround(pathHead, aroundPathHead);
    for (int i = 0; i < MAXOCC; ++i)
        islands[i]= i;
    setNextConnection();
    /*--numbersRemaining;
    pathRoot = pathHead = numbers[0].positions[0];
    maskPathRoot();
    setPosAround(pathHead, aroundPathHead);
    for (int i = 0; i < MAXOCC; ++i)
        islands[i]= i;*/
}

int NumberLink::netManDist(int* positions, int size, int* islands_)
{
    MinHeap<Connection> connections;
    int totalManhattanDistance = 0;
    for(int i = 0; i < size -1; i++)
    {        
        for (int j = i +1 ; j < size; j++)        
            connections.addValue(Connection(calcManhattanDistance(positions[i], positions[j]),i,j));
    }
    while(!connections.isEmpty())
    {
        const Connection conn = connections.removeMin();
        if(islands_[conn.element1] != islands_[conn.element2])
        {            
            const int mergedIsland = islands_[conn.element2];
            for (int i = 0; i < size; ++i)
                if(islands_[i] == mergedIsland)
                    islands_[i] = islands_[conn.element1];
            totalManhattanDistance += conn.distance;                   
            const int island = islands_[0];
            int numOfIslands = 1;
            for (int i = 1; i < size && numOfIslands == 1; ++i)
                if (islands_[i] != island)
                    numOfIslands++;
            if (numOfIslands == 1)
                break;
        }
    }
    return totalManhattanDistance;
}

Connection NumberLink::nextOccurance(int* positions, int size, int* islands_)
{
    MinHeap<Connection> connections;
    int totalManhattanDistance = 0;
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
            connections.addValue(Connection(calcManhattanDistance(positions[i], positions[j]), i, j));
    }
    while (!connections.isEmpty())
    {
        const Connection conn = connections.removeMin();
        if (islands_[conn.element1] != islands_[conn.element2])
        {
            return conn;
        }
    }
    return Connection(0, MAXOCC, MAXOCC);
}

NumberLink::NumberLink(int instance)
{
    qntColumns = qntLines = outOfBoundsPosition = currentNumber = 0;
    pathRoot = pathHead = numbersRemaining = occurrenceIndex = 0;
    state = nullptr;
    loadInstace(instance);
}

NumberLink::~NumberLink() { delete[] state; }

std::string NumberLink::toString()
{
    unmaskPathRoot();
    std::string returnValue = "\n";
    for (int i = 0; i < qntLines; i++)
    {
        for (int j = 0; j < qntColumns; j++)
        {
            returnValue += state[qntColumns * i + j];
            returnValue += ' ';
        }
        returnValue += '\n';
    }
    maskPathRoot();
    return returnValue;
}

std::string NumberLink::toString(char* state_)
{
    unmaskPathRoot();
    std::string returnValue = "\n";
    for (int i = 0; i < qntLines; i++)
    {
        for (int j = 0; j < qntColumns; j++)
        {
            returnValue += state_[qntColumns * i + j];
            returnValue += ' ';
        }
        returnValue += '\n';
    }
    maskPathRoot();
    return returnValue;
}

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

int NumberLink::calcCurrentNumberManhattanDistance(NumberLink* _state)
{
    std::cout << toString(_state->state);

    int* islandsCpy = new int[MAXOCC];
    memcpy(islandsCpy, islands, sizeof(int) * MAXOCC);
    numbers[currentNumber].positions[numbers[currentNumber].occurrences] = _state->pathHead;
    islandsCpy[numbers[currentNumber].occurrences] = islandsCpy[occurrenceIndex];
   
    int total = netManDist(numbers[currentNumber].positions, numbers[currentNumber].occurrences +1, islandsCpy);

    
    return total;
    //std::cout << toString(_state->state);
    /*MinHeap<int> bestDistances;
    const int backUp = numbers[_state->currentNumber].positions[_state->occurrenceIndex];
    numbers[_state->currentNumber].positions[_state->occurrenceIndex] = _state->pathHead;
    int islands[MAXOCC];
    for (int i = 0; i < MAXOCC; ++i)
        islands[i] = i;
    islands[numbers[_state->currentNumber].occurrences + 1] = islands[_state->occurrenceIndex];
    numbers[_state->currentNumber].positions[numbers[_state->currentNumber].occurrences + 1] = pathHead;
    const int distance = netManDist(numbers[_state->currentNumber].positions, numbers[_state->currentNumber].occurrences +1, islands);
    numbers[_state->currentNumber].positions[_state->occurrenceIndex] = backUp;
    return distance;*/
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
    successor->heuristic = calcCurrentNumberManhattanDistance(successor);
    successor->heuristic += remainingManhattanDistances();
}


// Gera os sucessores
void NumberLink::genSuccessors(DLList<Node*>& successors)
{
    if (state[pathRoot] == outOfBoundsChar)
        return;
    // Se a letra atual esta conexa, avanca para a proxima.
    // Caso nao exista proxima letra termina a funçao.
    if (isConnected(aroundPathHead))
    {
        --numbersRemaining;
        setNextConnection();
        if (state[pathRoot] == outOfBoundsChar)
            return;
    }

    for (int i = 0; i < 4; i++)
    {
        if (state[aroundPathHead[i]] != '.')
            continue;
        NumberLink* successor = (NumberLink*)getClone();
        successor->moveTo(aroundPathHead[i]);
        if (/*successor->isSelfConnectingPath() ||*/ /*successor->is360V2() ||*/ successor->isDeadState())
            delete successor;
        else
        {            
            updateSuccessorStats(successor);            
            successors.addToTail((Node*)successor);
        }
    }
}

int NumberLink::goBack(int position, int places, char* stateCpy)
{
    if (isOutOfBounds(position))
        return outOfBoundsPosition;
    int upPos, downPos, leftPos, rightPos;
    for (int i = 0; i < places; i++)
    {
        stateCpy[position] = '#';
        //std::cout << toString(stateCpy);
        if (*look(Direction::left, position, stateCpy, leftPos) == state[pathHead])
            position = leftPos;
        else if (*look(Direction::right, position, stateCpy, rightPos) == state[pathHead])
            position = rightPos;
        else if (*look(Direction::up, position, stateCpy, upPos) == state[pathHead])
            position = upPos;
        else if (*look(Direction::down, position, stateCpy, downPos) == state[pathHead])
            position = downPos;
        else return outOfBoundsPosition;
    }

    return position;
}

bool NumberLink::is360V2()
{
    char* stateCpy = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
    memcpy(stateCpy, state, static_cast<size_t>(outOfBoundsPosition) + 2);
    stateCpy[pathRoot] = state[pathHead];
    const int back4Pos = goBack(pathHead, 4, stateCpy);

    if (!isOutOfBounds(back4Pos) && calcManhattanDistance(pathHead, back4Pos) == 1 && state[(pathHead + back4Pos) / 2]
        == '.')
    {
        //std::cout << toString(stateCpy);
        delete[] stateCpy;
        return true;
    }
    const int back5Pos = goBack(back4Pos, 1, stateCpy);
    if (!isOutOfBounds(back5Pos) && calcManhattanDistance(pathHead, back5Pos) == 2)
    {
        const int minPos = pathHead < back5Pos ? pathHead : back5Pos;
        const int maxPos = minPos == pathHead ? back5Pos : pathHead;
        if (pathHead / qntColumns == back5Pos / qntColumns)
        {   // Mesma linha, verifica se os espaços entre a curva estao vazios.
            if (stateCpy[minPos + 1] == '.' && stateCpy[minPos + 2] == '.' && minPos + 3 == maxPos)
            {
                //std::cout << toString(stateCpy);
                delete[] stateCpy;
                return true;
            }
        }
        else
        {   // mesma coluna, verifica se os espaços entre a curva estao vazios.
            if (stateCpy[minPos + qntColumns] == '.' && stateCpy[minPos + qntColumns * 2] == '.' && minPos + qntColumns
                * 3 == maxPos)
            {
                //std::cout << toString(stateCpy);
                delete[] stateCpy;
                return true;
            }
        }
    }

    delete[] stateCpy;
    return false;
}

// Verifica se o caminho esta em contacto com ele proprio
bool NumberLink::isSelfConnectingPath()
{
    const char pathLetter = numbers[currentNumber].lowerLetter;
    state[pathRoot] = pathLetter;
    int count = 0;
    for (int i = 0; i < 4; ++i)
        if (state[aroundPathHead[i]] == pathLetter)
            count++;
    maskPathRoot();
    return count > 1;
}

// Verifica se o caminho faz curva de 360, com 1 espaco de intervalo (tipo U)
bool NumberLink::is360()
{
    const char pathLetter = numbers[currentNumber].lowerLetter;
    state[pathRoot] = pathLetter;
    bool flag = false;
    for (int i = 0; i < 4 && !flag; i++)
    {
        if (state[aroundPathHead[i]] != '.')
            continue;
        int nextPosition;
        int discard;
        // verifica verticalmente
        if (*look(Direction::left, aroundPathHead[i], state, discard) == pathLetter &&
            *look(Direction::right, aroundPathHead[i], state, discard) == pathLetter)
        {
            if ((*look(Direction::down, aroundPathHead[i], state, nextPosition) == pathLetter &&
                 *look(Direction::left, nextPosition, state, discard) == pathLetter &&
                 *look(Direction::right, nextPosition, state, discard) == pathLetter) ||
                (*look(Direction::up, aroundPathHead[i], state, nextPosition) == pathLetter &&
                 *look(Direction::left, nextPosition, state, discard) == pathLetter &&
                 *look(Direction::right, nextPosition, state, discard) == pathLetter))
                flag = true;
        }
            // verifica horizontalmente
        else if (*look(Direction::up, aroundPathHead[i], state, discard) == pathLetter &&
                 *look(Direction::down, aroundPathHead[i], state, discard) == pathLetter)
        {
            if ((*look(Direction::left, aroundPathHead[i], state, nextPosition) == pathLetter &&
                 *look(Direction::up, nextPosition, state, discard) == pathLetter &&
                 *look(Direction::down, nextPosition, state, discard) == pathLetter) ||
                (*look(Direction::right, aroundPathHead[i], state, nextPosition) == pathLetter &&
                 *look(Direction::up, nextPosition, state, discard) == pathLetter &&
                 *look(Direction::down, nextPosition, state, discard) == pathLetter))
                flag = true;
        }
    }
    maskPathRoot();
    return flag;
}


// Funcao recursiva que tenta alcancar um caracter apartir de uma posicao.
// IMPORTANTE: passar uma copia do estado, pois este e alterado.
bool NumberLink::canConnect(char* stateCopy, int startPosition, char letter)
{
    stateCopy[startPosition] = '#';
    //std::cout << toString(stateCopy);
    int upPos, downPos, leftPos, rightPos;
    const char* leftChar = look(Direction::left, startPosition, stateCopy, leftPos);
    const char* rightChar = look(Direction::right, startPosition, stateCopy, rightPos);
    const char* upChar = look(Direction::up, startPosition, stateCopy, upPos);
    const char* downChar = look(Direction::down, startPosition, stateCopy, downPos);

    const bool letterFound = (*leftChar == letter) || (*rightChar == letter) ||
                             (*upChar == letter) || (*downChar == letter);

    if (letterFound) return true;
    if (*upChar == '.' && canConnect(stateCopy, upPos, letter)) return true;
    if (*leftChar == '.' && canConnect(stateCopy, leftPos, letter)) return true;
    if (*rightChar == '.' && canConnect(stateCopy, rightPos, letter)) return true;
    if (*downChar == '.' && canConnect(stateCopy, downPos, letter)) return true;

    return false;
}

// Verifica se e possivel conectar as restantes letras
bool NumberLink::isDeadState()
{
    bool isDead = false;
    char* stateCpy = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
    //std::cout << toString();
    // Testa todas as letras restantes
    const int nextLetterIndex = currentNumber + 1;
    for (int i = nextLetterIndex; i < totalNumbers && !isDead; i++)
    {
        if (connected[i])
            continue;
        for (int j = 0; j < numbers[i].occurrences && !isDead; ++j)
        {
            memcpy(stateCpy, state, static_cast<size_t>(outOfBoundsPosition) + 2);
            isDead = !canConnect(stateCpy, numbers[i].positions[j], numbers[i].upperLetter);
        }
    }
    //if (isDead)
        //std::cout << toString();
    delete[] stateCpy;
    
    return isDead;
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


bool NumberLink::operator<(Node& node)
{
    if (getPriority() < ((NumberLink&)node).getPriority())
        return true;
    return false;
}
