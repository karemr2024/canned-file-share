#include <iostream>
#include <fstream>
#include <vector>
#include "NFileTree.cpp"
using namespace std;

// size_t chunkSize = 4096;



void splitFile(const std::string& inputFileName, TreeNode* currNode) {
    std::ifstream inputFile(inputFileName, std::ios::binary);

    

    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFileName << std::endl;
        return;
    }

    size_t chunkCounter = gchunkcount;
    currNode->chunk_inds=gchunkcount;
    char buffer[chunkSize];

    while (!inputFile.eof()) {
        inputFile.read(buffer, chunkSize);

        std::streamsize bytesRead = inputFile.gcount();

        if (bytesRead > 0) {
            std::string outputFileName = "./store/" + storePrefix + std::to_string(chunkCounter++);
            std::ofstream outputFile(outputFileName, std::ios::binary);
            outputFile.write(buffer, bytesRead);
        }
    }

    currNode->numChunks=chunkCounter;
    gchunkcount=chunkCounter+1;
    std::cout<<"chunkCounter: " << chunkCounter << std::endl;
    std::cout<<"gchunkcount: " << gchunkcount << std::endl;

    inputFile.close();
}


void mergeChunks(const std::string& outputFileName, TreeNode* currNode) {
    std::ofstream outputFile(outputFileName, std::ios::binary);

    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputFileName << std::endl;
        return;
    }

    size_t chunkCounter = 1;
    std::string inputFileName;

    do {
        inputFileName = "./store/" + storePrefix + std::to_string(chunkCounter++);
        std::ifstream inputFile(inputFileName, std::ios::binary);

        if (!inputFile) {
            break;  // No more chunks to read
        }

        outputFile << inputFile.rdbuf();

        inputFile.close();
    } while (true);

    outputFile.close();
}


// int main() {
//     std::string inputFileName = "sample.txt"; 

//     // splitFile(inputFileName);

//     std::string outputFileName = "reconstructed_file.txt";

//     // mergeChunks(outputFileName);

//     return 0;
// }