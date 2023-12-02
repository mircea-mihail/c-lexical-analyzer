#include <iostream>
#include <fstream>
#include <list>
#include <cstring>

#define NUMBER_OF_ALLOWED_ARGUMENTS 2

#define KEYWORD 0
#define IDENTIFIER 1
#define CONSTANT 2
#define STRING 3
#define SPECIAL_SYMBOL 4 
#define OPERATOR 5
#define ERROR 6 

#define NUMBER_OF_C_OPERATORS 36
#define NUMBER_OF_C_KEYWORDS 32

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

        case CONSTANT:
            return "constant";
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
        std::cout << "type: " << getTypeString(m_type) << " value: " << *m_value << std::endl; 
    }
};

class lexicalAnalyser
{
private:
    std::string m_fileName;
    std::ifstream m_readStream;
    std::string m_readStringBuffer;

    std::list<token> m_tokenTable;

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

    int getCharFromStream()
    {
        return m_readStream.get(); 
    }

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

    int getType(std::string p_tokenValue)
    {   
        if(p_tokenValue == " ")
        {
            return SPECIAL_SYMBOL;
        }
        if(isOperator(p_tokenValue))
        {
            return OPERATOR;
        }
        if(isKeyword(p_tokenValue))
        {
            return KEYWORD;
        }
        if(isIdentifier(p_tokenValue))
        {
            return IDENTIFIER;
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

    void createToken()
    {
        std::string *pointerToTokenValue = checkValueInValueTable(m_readStringBuffer);            

        if(pointerToTokenValue == nullptr)
        {
            m_valueTable.emplace_back(m_readStringBuffer);

            pointerToTokenValue = &m_valueTable.back();
        }

        token tok(getType(m_readStringBuffer), pointerToTokenValue);

        m_tokenTable.emplace_back(tok);
    }

    bool isDelimiter(int p_character)
    {
        if( p_character == ' ' || p_character == '\n' || p_character == '\t' || p_character == ';' || p_character == EOF)
        //  || p_character == '+' || p_character == '-' || p_character == '*'  
        //  || p_character == '/' || p_character == ',' || p_character == ';' 
        //  || p_character == '<' || p_character == '=' || p_character == '>'
        //  || p_character == '[' || p_character == ']' || p_character == '{'
        //  || p_character == '(' || p_character == ')' || p_character == '}')
        {
            return true;
        }

        return false;
    }

    bool checkForNewToken(int &p_left, int &p_right )
    {
        if(isDelimiter(p_right))
        {
            int delimiter = p_right;

            m_readStringBuffer.pop_back();
        
            if(m_readStringBuffer != "")
            {
                createToken();
            }

            m_readStringBuffer = ""; 
            p_left = p_right;

            return true;
        }

        return false;
    }

public:
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

    // parses the file given and splits it into tokens
    void parseFile()
    {
        int left = 0;
        int right = 0;
        right = getCharFromStream();
        m_readStringBuffer = right;
        while(right != EOF)
        {
            std::cout << char(right);

            checkForNewToken(left, right);

            right = getCharFromStream();

            if(right != EOF)
            {
                m_readStringBuffer += right;
            }
        }
    }

    void printTokens()
    {
        std::list<token>::iterator tokenIt;
        for (tokenIt = m_tokenTable.begin(); tokenIt != m_tokenTable.end(); tokenIt++)
        {
            tokenIt->printToken();
        }
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
    lexAn.parseFile();
    lexAn.printTokens();
}