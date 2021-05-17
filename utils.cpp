#include "NumberLink.h"
#include <cstring>

std::string NumberLink::toString()
{
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
    return returnValue;
}

std::string NumberLink::toString(char* state_)
{
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
    return returnValue;
}

bool NumberLink::isSelfConnectingPath()
{
    int count = 0;
    for (int i = 0; i < 4; ++i)
        if (state[aroundPathHead[i]] == maskChar)
            count++;
    return count > 1;
}

bool NumberLink::is360()
{
    bool flag = false;
    for (int i = 0; i < 4 && !flag; i++)
    {
        if (state[aroundPathHead[i]] != '.')
            continue;
        int nextPosition;
        int discard;
        // verifica verticalmente
        if (*look(Direction::left, aroundPathHead[i], state, discard) == maskChar &&
            *look(Direction::right, aroundPathHead[i], state, discard) == maskChar)
        {
            if ((*look(Direction::down, aroundPathHead[i], state, nextPosition) == maskChar &&
                 *look(Direction::left, nextPosition, state, discard) == maskChar &&
                 *look(Direction::right, nextPosition, state, discard) == maskChar) ||
                (*look(Direction::up, aroundPathHead[i], state, nextPosition) == maskChar &&
                 *look(Direction::left, nextPosition, state, discard) == maskChar &&
                 *look(Direction::right, nextPosition, state, discard) == maskChar))
                flag = true;
        }
        // verifica horizontalmente
        else if (*look(Direction::up, aroundPathHead[i], state, discard) == maskChar &&
                 *look(Direction::down, aroundPathHead[i], state, discard) == maskChar)
        {
            if ((*look(Direction::left, aroundPathHead[i], state, nextPosition) == maskChar &&
                 *look(Direction::up, nextPosition, state, discard) == maskChar &&
                 *look(Direction::down, nextPosition, state, discard) == maskChar) ||
                (*look(Direction::right, aroundPathHead[i], state, nextPosition) == maskChar &&
                 *look(Direction::up, nextPosition, state, discard) == maskChar &&
                 *look(Direction::down, nextPosition, state, discard) == maskChar))
                flag = true;
        }
    }
    return flag;
}

bool NumberLink::canConnect(char* stateCopy, int startPosition, char letter, int& numOfHits)
{    
    if (stateCopy[startPosition] == letter)
        numOfHits--;
    if (numOfHits == 0)
        return true;
    stateCopy[startPosition] = '#';
    //std::cout << toString(stateCopy);
    int upPos, downPos, leftPos, rightPos;
    const char* leftChar = look(Direction::left, startPosition, stateCopy, leftPos);
    const char* rightChar = look(Direction::right, startPosition, stateCopy, rightPos);
    const char* upChar = look(Direction::up, startPosition, stateCopy, upPos);
    const char* downChar = look(Direction::down, startPosition, stateCopy, downPos);

    if ((*upChar == '.' || *upChar == letter) && canConnect(stateCopy, upPos, letter, numOfHits)) return true;
    if ((*leftChar == '.' || *leftChar == letter) && canConnect(stateCopy, leftPos, letter, numOfHits)) return true;
    if ((*rightChar == '.' || *rightChar == letter) && canConnect(stateCopy, rightPos, letter, numOfHits)) return true;
    if ((*downChar == '.' || *downChar == letter) && canConnect(stateCopy, downPos, letter, numOfHits)) return true;

    return false;
}

bool NumberLink::isDeadState()
{
    bool isDead = false;
    char* stateCpy = new char[static_cast<size_t>(outOfBoundsPosition) + 2];
    // Testa todas as letras restantes
    const int nextLetterIndex = currentNumber + 1;

    for (int i = nextLetterIndex; i < totalNumbers && !isDead; i++)
    {
        if (connected[i])
            continue;
        memcpy(stateCpy, state, static_cast<size_t>(outOfBoundsPosition) + 2);
        int numOfHits = numbers[i].numOcc;
        isDead = !canConnect(stateCpy, numbers[i].positions[0], numbers[i].upperLetter, numOfHits);
    }
    delete[] stateCpy;
    return isDead;
}
