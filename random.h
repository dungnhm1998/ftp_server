#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include <string>

void seed();
std::string createAlphabetDigitString(unsigned int len);
std::string createHexaString(unsigned int len);

#endif // RANDOM_H_INCLUDED
