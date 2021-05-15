#include "NumberLink.h"

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

// Verifica se o caminho esta em contacto com ele proprio
bool NumberLink::isSelfConnectingPath()
{
    int count = 0;
    for (int i = 0; i < 4; ++i)
        if (state[aroundPathHead[i]] == newPathChar || state[aroundPathHead[i]] == oldPathChar)
            count++;
    return count > 1;
}

// Verifica se o caminho faz curva de 360, com 1 espaco de intervalo (tipo U)
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
        if (*look(Direction::left, aroundPathHead[i], state, discard) == newPathChar &&
            *look(Direction::right, aroundPathHead[i], state, discard) == newPathChar)
        {
            if ((*look(Direction::down, aroundPathHead[i], state, nextPosition) == newPathChar &&
                 *look(Direction::left, nextPosition, state, discard) == newPathChar &&
                 *look(Direction::right, nextPosition, state, discard) == newPathChar) ||
                (*look(Direction::up, aroundPathHead[i], state, nextPosition) == newPathChar &&
                 *look(Direction::left, nextPosition, state, discard) == newPathChar &&
                 *look(Direction::right, nextPosition, state, discard) == newPathChar))
                flag = true;
        }
        // verifica horizontalmente
        else if (*look(Direction::up, aroundPathHead[i], state, discard) == newPathChar &&
                 *look(Direction::down, aroundPathHead[i], state, discard) == newPathChar)
        {
            if ((*look(Direction::left, aroundPathHead[i], state, nextPosition) == newPathChar &&
                 *look(Direction::up, nextPosition, state, discard) == newPathChar &&
                 *look(Direction::down, nextPosition, state, discard) == newPathChar) ||
                (*look(Direction::right, aroundPathHead[i], state, nextPosition) == newPathChar &&
                 *look(Direction::up, nextPosition, state, discard) == newPathChar &&
                 *look(Direction::down, nextPosition, state, discard) == newPathChar))
                flag = true;
        }
    }
    return flag;
}


// Funcao recursiva que tenta alcancar um ou mais caracteres apartir de uma posicao.
// IMPORTANTE: passar uma copia do estado, pois este e alterado.
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


// Verifica se e possivel conectar as restantes letras
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
