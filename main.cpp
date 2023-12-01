#include <iostream>
#include <fstream>

#define NUMBER_OF_ALLOWED_ARGUMENTS 2

class lexicalAnalyser
{
private:
    std::string m_fileName;
    std::ifstream m_readStream;

    int getCharFromStream()
    {
        return m_readStream.get(); 
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

    void parseFile()
    {
        int left = 0;
        left = getCharFromStream();
        
        while(left != EOF);
        {
            std::cout << char(left);
        
            left = getCharFromStream();
        }
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
}