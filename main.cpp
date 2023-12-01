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

class token
{
private:
    int m_type;
    std::string *m_value;
public:
    token(int p_type, std::string *p_value):m_type(p_type), m_value(p_value) {}

    void displayToken()
    {
        std::cout << "type: " << m_type << " value: " << *m_value << std::endl; 
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

    int getCharFromStream()
    {
        return m_readStream.get(); 
    }

    void addToken(token p_tok)
    {
    }

    int getType(std::string p_tokenValue)
    {   
        if(p_tokenValue == " ")
        {
            return SPECIAL_SYMBOL;
        }
        return KEYWORD;
    }

    std::string *checkValueInValueTable(std::string p_newValue)
    {
        for (std::list<std::string>::iterator i = m_valueTable.begin(); i != m_valueTable.end(); i++)
        {
            if(*i == p_newValue)
            {
                return &(*i);
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

    bool checkForNewToken(int &p_left, int &p_right )
    {
        if(p_right == ' ')
        {
            int delimiter = p_right;

            m_readStringBuffer.pop_back();
        
            createToken();

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
        // std::cout << std::endl << "token: \"" << m_readStringBuffer << "\"" << std::endl;

    }

    void printTokens()
    {
        for (std::list<token>::iterator i = m_tokenTable.begin(); i != m_tokenTable.end(); i++)
        {
            i->displayToken();
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