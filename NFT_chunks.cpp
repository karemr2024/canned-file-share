#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

void splitFile(const std::string& inputFileName, const std::string& outputPrefix, size_t chunkSize) {
    std::ifstream inputFile(inputFileName, std::ios::binary);

    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFileName << std::endl;
        return;
    }

    size_t fileCounter = 1;
    char buffer[chunkSize];

    while (!inputFile.eof()) {
        inputFile.read(buffer, chunkSize);

        std::streamsize bytesRead = inputFile.gcount();

        if (bytesRead > 0) {
            std::string outputFileName = "./store/" + outputPrefix + std::to_string(fileCounter++);
            std::ofstream outputFile(outputFileName, std::ios::binary);
            outputFile.write(buffer, bytesRead);
        }
    }

    std::cout<<"fileCounter: " << fileCounter << std::endl;

    inputFile.close();
}


void mergeChunks(const std::string& inputPrefix, const std::string& outputFileName) {
    std::ofstream outputFile(outputFileName, std::ios::binary);

    if (!outputFile) {
        std::cerr << "Error opening output file: " << outputFileName << std::endl;
        return;
    }

    size_t chunkCounter = 1;
    std::string inputFileName;

    do {
        inputFileName = "./store/" + inputPrefix + std::to_string(chunkCounter++);
        std::ifstream inputFile(inputFileName, std::ios::binary);

        if (!inputFile) {
            break;  // No more chunks to read
        }

        outputFile << inputFile.rdbuf();

        inputFile.close();
    } while (true);

    outputFile.close();
}


int main() {
    std::string inputFileName = "sample.txt";
    std::string outputPrefix = "chunk_";
    size_t chunkSize = 4096; // Set your desired chunk size

    splitFile(inputFileName, outputPrefix, chunkSize);

    std::string inputPrefix = "chunk_";
    std::string outputFileName = "reconstructed_file.txt";

    mergeChunks(inputPrefix, outputFileName);

    return 0;
}