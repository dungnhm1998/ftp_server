#include "random.h"
#include <string>
#include <ctime>

const char ALPHABET_DIGIT[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

const char HEXA_DIGIT[] = "0123456789ABCDEF";


void seed()
{
    srand(time(NULL));
}

std::string createAlphabetDigitString(unsigned int len)
{
    unsigned int i;
    std::string str;
    str.push_back(ALPHABET_DIGIT[rand() % 52]);
    for(i=1;i<len;i++)
    {
        str.push_back(ALPHABET_DIGIT[rand() % 62]);
    }
    return str;
}

std::string createHexaString(unsigned int len)
{
    unsigned int i;
    std::string str;
    for(i = 0;i<len;i++)
    {
        str.push_back(HEXA_DIGIT[rand() % 16]);
    }
    return str;
}
