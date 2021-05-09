#include "NumberLink.h"

std::string NumberLink::toString()
{
    //unmaskPathRoot();
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
   // maskPathRoot();
    return returnValue;
}

std::string NumberLink::toString(char* state_)
{
   // unmaskPathRoot();
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
    //maskPathRoot();
    return returnValue;
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
bool NumberLink::canConnect(char* stateCopy, int startPosition, char letter, int& numOfHits)
{
    stateCopy[startPosition] = '#';
    //std::cout << toString(stateCopy);
    int upPos, downPos, leftPos, rightPos;
    const char* leftChar = look(Direction::left, startPosition, stateCopy, leftPos);
    const char* rightChar = look(Direction::right, startPosition, stateCopy, rightPos);
    const char* upChar = look(Direction::up, startPosition, stateCopy, upPos);
    const char* downChar = look(Direction::down, startPosition, stateCopy, downPos);


    if (*leftChar == letter)
    {
        numOfHits--;
        stateCopy[leftPos] = '#';
    }
    if (*rightChar == letter)
    {
        numOfHits--;
        stateCopy[rightPos] = '#';
    }
    if (*upChar == letter)
    {
        numOfHits--;
        stateCopy[upPos] = '#';
    }
    if (*downChar == letter)
    {
        numOfHits--;
        stateCopy[downPos] = '#';
    }
    if(numOfHits == 0)
        return true;
    if (*upChar == '.' && canConnect(stateCopy, upPos, letter, numOfHits)) return true;
    if (*leftChar == '.' && canConnect(stateCopy, leftPos, letter, numOfHits)) return true;
    if (*rightChar == '.' && canConnect(stateCopy, rightPos, letter, numOfHits)) return true;
    if (*downChar == '.' && canConnect(stateCopy, downPos, letter, numOfHits)) return true;

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

            memcpy(stateCpy, state, static_cast<size_t>(outOfBoundsPosition) + 2);
            int numOfHits = numbers[i].numOcc - 1;
            isDead = !canConnect(stateCpy, numbers[i].positions[0], numbers[i].upperLetter, numOfHits);
        
    }
    //if (isDead)
        //std::cout << toString();
    delete[] stateCpy;

    return isDead;
}