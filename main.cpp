#include <iostream>
#include <fstream>
#include <list>
#include <cstring>

#define NUMBER_OF_ALLOWED_ARGUMENTS 2

#define KEYWORD 0
#define IDENTIFIER 1
#define CONSTANT_NUMERAL 2
#define STRING 3
#define SPECIAL_SYMBOL 4 
#define OPERATOR 5
#define ERROR 6

#define SINGLE_LINE_COMMENT 7
#define MULTI_LINE_COMMENT 8

#define NUMBER_OF_C_OPERATORS 36
#define NUMBER_OF_C_KEYWORDS 32
#define NUMBER_OF_C_SPECIAL_SYMBOLS 14

class token
{
private:
    int m_type;
    std::string *m_value;

public:
    token(int p_type, std::string *p_value):m_type(p_type), m_value(p_value) {}

    std::string getTypeString(int p_type)
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

    void printToken()
    {
        std::cout  << " value: " << *m_value  << "\ttype: " << getTypeString(m_type)<< std::endl; 
    }
};

class lexicalAnalyser
{
private:
    int m_currentChar = 0;
    int m_currentLine = 0;

    std::string m_fileName;
    std::ifstream m_readStream;
    std::string m_readStringBuffer;

    std::list<std::string> m_valueTable;

    std::string m_operatorTable[NUMBER_OF_C_OPERATORS] = {
        "+", "-", "*", "/", "%", "++", "--", "==", "!=", ">", "<", ">=", 
        "<=", "&&", "||", "!", "&", "|", "^", "~", ">>", "<<", "=", 
        "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=", "?:" // last operator special case
    };

    std::string m_keywordTable[NUMBER_OF_C_KEYWORDS] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do", 
        "double", "else", "enum", "extern", "float", "for", "goto", "if", 
        "int", "long", "register", "return", "short", "signed", "sizeof", "static", 
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
    };

    std::string m_specialSymbolTable[NUMBER_OF_C_SPECIAL_SYMBOLS] = {
        "[", "]", "{", "}", ".",
        "(", ")", ",", ";", 
        ":", "*", "...", "=", "#"
    };

    bool isOperator(std::string p_token)
    {
        for(int opIdx = 0; opIdx < NUMBER_OF_C_OPERATORS; opIdx++)
        {
            if (p_token == m_operatorTable[opIdx])
            {
                return true;
            }
        }
        return false;
    }

    bool isKeyword(std::string p_token)
    {
        for(int kwIdx = 0; kwIdx < NUMBER_OF_C_KEYWORDS; kwIdx++)
        {
            if (p_token == m_keywordTable[kwIdx])
            {
                return true;
            }
        }
        return false;
    }

    bool isDigit(char p_letter)
    {
        if(p_letter >= '0' && p_letter <= '9')
        {
            return true;
        }
        return false;
    }

    bool isAlphabetOrUnderscore(char p_letter)
    {
        if((p_letter >= 'a' && p_letter <= 'z') || (p_letter >= 'A' && p_letter <= 'Z') || p_letter == '_')
        {
            return true;
        }
        return false;
    }

    bool isIdentifier(std::string p_token)
    {
        if(p_token.length() <= 0)
        {
            return false;
        }
        
        if(!isAlphabetOrUnderscore(p_token[0]))
        {
            return false;
        }

        for(int chIdx = 0; chIdx < p_token.length(); chIdx ++)
        {
            if(!isAlphabetOrUnderscore(p_token[chIdx]) && !isDigit(p_token[chIdx]))
            {
                return false;
            }
        }

        return true;   
    }

    bool isConstantNumeral(std::string p_token)
    {
        for(int chIdx = 0; chIdx < p_token.length(); chIdx ++)
        {
            if(!isDigit(p_token[chIdx]) && (p_token[chIdx] != '.'))
            {
                return false;
            }
        }
        return true;
    }

    bool isCompleteString(std::string p_token)
    {
        if(p_token[0] == '"' && p_token[p_token.length() - 1] == '"')
        {
            return true;
        }
        return false;
    }   

    // to make it work with the general program logic i made it return true even when the string is "smth", it only returns false
    // when the string also has a char after the second quote
    bool isPartialString(std::string p_token)
    {
        if(p_token[0] != '"')
        {
            return false;
        }
        
        int numberOfQuotes = 0;
        for(int stIdx = 0; stIdx < p_token.length() - 1; stIdx ++)
        {
            if(p_token[stIdx] == '"')
            {
                numberOfQuotes ++;
            }
            
            if(p_token[stIdx] == '\n' && p_token[stIdx - 1] != '\\')
            {
                std::cout << "Bad string at line " << m_currentLine << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        
        if(numberOfQuotes > 1)
        {
            return false;
        }

        return true;
    }

    bool isSingleLineComment(std::string p_token)
    {
        if(!(p_token[0] == '/' && p_token[1] == '/'))
        {
            return false;
        }
        for(int i = 0; i < p_token.length(); i++)
        {
            if(p_token[i] == '\n')
            {
                return false;
            }
        }
        return true;
    }

    // similar trick to the one above, used for the strings
    bool isPartialMultiLineComment(std::string p_token)
    {
        if(!(p_token[0] == '/' && p_token[1] == '*'))
        {
            return false;
        }
        // multiline comment has to have at least a leangth of 4 : /**/ so return true as in 
        // it could still be a multiline comment 
        if(p_token.length() <= 4)
        {
            return true;
        }

        if(p_token[p_token.length() - 3] == '*' && p_token[p_token.length() - 2] == '/')
        {
            return false;
        }

        return true;
    }

    bool isSpecialSymbol(std::string p_token)
    {
        for(int spSyIdx = 0; spSyIdx < NUMBER_OF_C_SPECIAL_SYMBOLS; spSyIdx++)
        {
            if (p_token == m_specialSymbolTable[spSyIdx])
            {
                return true;
            }
        }
        return false;   
    }

    int getType(std::string p_token)
    {   
        if(isSingleLineComment(p_token))
        {
            return SINGLE_LINE_COMMENT;
        }
        if(isPartialMultiLineComment(p_token))
        {
            return MULTI_LINE_COMMENT;
        }
        if(isOperator(p_token))
        {
            return OPERATOR;
        }
        if(isKeyword(p_token))
        {
            return KEYWORD;
        }
        if(isIdentifier(p_token))
        {
            return IDENTIFIER;
        }
        if(isConstantNumeral(p_token))
        {
            return CONSTANT_NUMERAL;
        }
        if(isPartialString(p_token))
        {
            return STRING;
        }
        if(isSpecialSymbol(p_token))
        {
            return SPECIAL_SYMBOL;
        }

        return ERROR;
    }

    std::string *checkValueInValueTable(std::string p_newValue)
    {
        std::list<std::string>::iterator tokenValueIt;
        for (tokenValueIt = m_valueTable.begin(); tokenValueIt != m_valueTable.end(); tokenValueIt++)
        {
            if(*tokenValueIt == p_newValue)
            {
                return &(*tokenValueIt);
            }
        }
        return nullptr;
    }    

    token *createToken(std::string &p_currentPotentialToken)
    {
        std::string *pointerToTokenValue = checkValueInValueTable(p_currentPotentialToken);            

        if(pointerToTokenValue == nullptr)
        {
            m_valueTable.emplace_back(p_currentPotentialToken);

            pointerToTokenValue = &m_valueTable.back();
        }

        int tokenType = getType(p_currentPotentialToken);

        // comments are not tokens and must be ignored
        if(tokenType == SINGLE_LINE_COMMENT || tokenType == MULTI_LINE_COMMENT)
        {
            return nullptr;
        }

        if(tokenType == STRING)
        {
            if(!isCompleteString(p_currentPotentialToken))
            {
                tokenType = ERROR;
            }
        }

        token *tok = new token(tokenType, pointerToTokenValue);

        return tok;
    }

    // bool isDelimiter(int p_character)
    // {
    //     if( p_character == ' ' || p_character == '\n' || p_character == '\t' || p_character == ';' || p_character == EOF)
    //     //  || p_character == '+' || p_character == '-' || p_character == '*'  
    //     //  || p_character == '/' || p_character == ',' || p_character == ';' 
    //     //  || p_character == '<' || p_character == '=' || p_character == '>'
    //     //  || p_character == '[' || p_character == ']' || p_character == '{'
    //     //  || p_character == '(' || p_character == ')' || p_character == '}')
    //     {
    //         return true;
    //     }

    //     return false;
    // }
    

public:
    int getCharFromStream()
    {
        return m_readStream.get(); 
    }

    // initialises the m_readStream using the filename given as an argument
    lexicalAnalyser(std::string p_fileName):m_fileName(p_fileName)
    {
        m_readStream.open(p_fileName);
        if(!m_readStream.is_open())
        {
            std::cout << "No such file in the given directory\n";
            exit(EXIT_FAILURE);
        }
    };

    token *checkForNewToken(int &p_currentChar, std::string &p_currentPotentialToken )
    {
        // std::cout << std::endl << "current type: " << getType(p_currentPotentialToken) << std::endl;
        if(getType(p_currentPotentialToken) == ERROR)
        {
            std::string delimiter = ""; 
            token *tok = nullptr;
            delimiter += p_currentChar;

            p_currentPotentialToken.pop_back();
        
            if(p_currentPotentialToken != "")
            {
                tok = createToken(p_currentPotentialToken);
            }

            p_currentPotentialToken = "";

            if(getType(delimiter) != ERROR)
            {
                p_currentPotentialToken += delimiter; 
            }

            return tok;
        }

        return nullptr;
    }

    bool startReading()
    {
        m_currentChar = getCharFromStream();
        if(m_currentChar == EOF)
        {
            return false;
        }

        if(m_currentChar == '\n')
        {
            m_currentLine ++;
        }

        m_readStringBuffer = m_currentChar;
        
        return true;
    }

    token *getToken()
    {
        while(m_currentChar != EOF)
        {
            token *tok = checkForNewToken(m_currentChar, m_readStringBuffer);

            if(tok == nullptr)
            {
                m_currentChar = getCharFromStream();
                m_readStringBuffer += m_currentChar;

                if(m_currentChar == '\n')
                {
                    m_currentLine ++;
                }
            }
            else
            {
                return tok;
            }
        }
        return nullptr;
    }

    ~lexicalAnalyser()
    {
        m_readStream.close();
    }
};

void printInputRequirements()
{
    std::cout << "The allowed input should only consist of the name of the executable and the filename to read from.\n";
}

int main(int argc, char *argv[])
{
    // only one argument allowed -> the name of the input file
    if(argc < NUMBER_OF_ALLOWED_ARGUMENTS || argc > NUMBER_OF_ALLOWED_ARGUMENTS)
    {
        printInputRequirements();
        exit(EXIT_FAILURE);
    }

    // initialise the input stream with the file given in the argv
    lexicalAnalyser lexAn(argv[argc - 1]);

    if(lexAn.startReading())
    {
        token *tok = lexAn.getToken();
        
        while(tok != nullptr)
        {
            tok->printToken();
            delete tok;
            tok = lexAn.getToken();
        }
    }
    
    return 0;
}

/*
    sa tratez \ string

    sa se vada mai clar automatul
    
    si sa afisez eroarea respectiva
    
    sa treaca peste testele alea
*/