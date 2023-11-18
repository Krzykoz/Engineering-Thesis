#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

const int LOWER_LIMIT = 1;
const int UPPER_LIMIT = 2147483647;

void generateAndSaveRandomNumbers(const std::string& filePath, int numNumbers) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    for (int i = 0; i < numNumbers; ++i) {
        int randomNumber = rand() % (UPPER_LIMIT - LOWER_LIMIT + 1) + LOWER_LIMIT;
        file << randomNumber << '\n';
    }

    file.close();
}

void writeAndPrintData(std::ofstream& file, int numbersGenerated, double executionTime) {
    std::cout << "Generated " << numbersGenerated << " numbers, Execution time: " << executionTime << " seconds\n";
    file << numbersGenerated << "," << executionTime << '\n';
}

void generate(const std::string& filePath, int numbersGenerated, std::ofstream& logFile) {
    std::clock_t startTime = std::clock();

    generateAndSaveRandomNumbers(filePath, numbersGenerated);

    std::clock_t endTime = std::clock();
    double executionTime = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;

    writeAndPrintData(logFile, numbersGenerated, executionTime);
}

int main() {
    std::ofstream logFile("../number_generator_data.csv");
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file." << std::endl;
        return 1;
    }

    logFile << "Numbers,Time\n";

    for (int i = 1; i < 9; ++i) {
        int numbersGenerated = static_cast<int>(pow(10, i));
        std::string filePath = "../numbers" + std::to_string(numbersGenerated) + ".txt";
        generate(filePath, numbersGenerated, logFile);
    }

    logFile.close();

    return 0;
}
