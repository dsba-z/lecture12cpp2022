#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using Table = std::vector<std::vector<std::string>>;
using Row = std::vector<std::string>;


Row parseLine(const std::string& line)
{
    Row lineData;
    std::string token;
    bool inQuotes = false;
    bool endingQuote = false;
    for (char c: line)
    {
        if (endingQuote)
        {
            switch(c)
            {
            case ',':
            //  1,2,"word",4,5
            //            ^ here
                endingQuote = false;
                inQuotes = false;
                lineData.push_back(token);
                token.clear();
                break;
            case '"':
            //  1,2,"word ""apple""",4,5
            //             ^ here
                endingQuote = false;
                inQuotes = true;
                token.push_back('"');
                break;
            default:
            // ??? special case for broken CSV
                endingQuote = false;
                inQuotes = false;
                token.push_back(c);
                break;
            }
            continue;
        }

        if (inQuotes)
        {
            switch(c)
            {
            case '"':
                endingQuote = true;
                break;
            default:
                token.push_back(c);
                break;
            }
        }
        else
        {
            switch(c)
            {
            case ',':
                lineData.push_back(token);
                token.clear();
                break;
            case '"':
                inQuotes = true;
                break;
            default:
                token.push_back(c);
                break;
            }
        }
    }
    lineData.push_back(token);
    return lineData;
}


Row parseLineNoEscape(const std::string& line)
{
    Row lineData;
    std::string token;
    bool inQuotes = false;
    for (char c: line)
    {
        if (c == ',')
        {
            if (inQuotes)
            {
                token.push_back(c);
            }
            else
            {
                lineData.push_back(token);
                token.clear();
            }
        }
        else if (c == '"')
        {
            // inQuotes = !inQuotes;
            if (inQuotes)
            {
                inQuotes = false;
            }
            else
            {
                inQuotes = true;
            }
        }
        else
        {
            token.push_back(c);
        }
    }
    lineData.push_back(token);
    return lineData;
}


int readCsvFileErrorCode(const std::string& fileName, Table data)
{
    std::string line;
    std::ifstream inFile(fileName);
    int lineLength = -1;
    while(std::getline(inFile, line))
    {
        Row row = parseLine(line);
        if (lineLength == -1)
        {
            lineLength = row.size();
        }
        if (lineLength != row.size())
        {
            // error "incorrect line count"
            return 1;
        }
        data.push_back(row);
    }
    return 0;
}



Table readCsvFile(const std::string& fileName)
{
    Table data;
    std::string line;
    std::ifstream inFile(fileName);
    while(std::getline(inFile, line))
    {
        Row row = parseLine(line);
        data.push_back(row);
    }
    return data;
}

void printTable(const Table& v)
{
    for (const Row& r: v)
    {
        for(const std::string& item: r)
        {
            std::cout << item << '\t';
        }
        std::cout << std::endl;
    }
}

int main()
{

    const std::string fileName = "../data/example3.csv";
    Table table =  readCsvFile(fileName);
    printTable(table);
    return 0;
}
