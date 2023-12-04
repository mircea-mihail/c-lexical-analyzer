#include "token.h"

std::string token::getTypeString(int p_type)
{
    switch (p_type)
    {
    case KEYWORD:
        return "keyword";
        break;

    case IDENTIFIER:
        return "identifier";
        break;

    case CONSTANT_NUMERAL:
        return "constant numeral";
        break;

    case STRING:
        return "string";
        break;

    case SPECIAL_SYMBOL:
        return "special symbol";
        break;
        
    case OPERATOR:
        return "operator";
        break;
    
    default:
        break;
    }
    return "ERROR";
}

void token::printToken(std::ofstream *p_outputStream)
{

    std::cout  << "value: " << *m_value  << "\ttype: " << getTypeString(m_type)<< std::endl; 
    (*p_outputStream) << "value: " << *m_value  << "\ttype: " << getTypeString(m_type)<< std::endl;
}