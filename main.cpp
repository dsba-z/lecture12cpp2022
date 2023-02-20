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

}


Row parseLineNoEscape(const std::string& line)
{
    Row lineData;
    std::string token;
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


    // while(inFile.good())
    // {
    //     std::getline(inFile, line); // 1,2,Bread; Butter,91.89\n
    //     // stream is good
    //     Row row = parseLine(line);
    //     data.push_back(row);
    // }
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
}
