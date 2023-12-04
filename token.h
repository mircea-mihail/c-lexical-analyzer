#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <fstream>

#define KEYWORD 0
#define IDENTIFIER 1
#define CONSTANT_NUMERAL 2
#define STRING 3
#define SPECIAL_SYMBOL 4 
#define OPERATOR 5
#define ERROR 6

class token
{
private:
    int m_type;
    std::string *m_value;

public:
    token(int p_type, std::string *p_value):m_type(p_type), m_value(p_value) {}

    // returns the type of the token in string format
    std::string getTypeString(int p_type);

    // prints the type and value of the token
    void printToken(std::ofstream *p_outputStream);
};

#endif