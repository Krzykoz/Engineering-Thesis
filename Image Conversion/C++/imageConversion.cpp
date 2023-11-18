#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

void convertToBlackAndWhite(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputPath << std::endl;
        return;
    }

    std::vector<char> bmpHeader(54);
    inputFile.read(bmpHeader.data(), 54);

    unsigned int width, height;
    inputFile.seekg(18);
    inputFile.read(reinterpret_cast<char*>(&width), sizeof(width));
    inputFile.read(reinterpret_cast<char*>(&height), sizeof(height));

    const int channels = 3;
    std::vector<char> imageData((std::istreambuf_iterator<char>(inputFile)), {});

    for (size_t i = 0; i < width * height; ++i) {
        size_t pixelStart = i * channels;
        int grayscaleValue = 0;

        for (int j = 0; j < channels; ++j) {
            grayscaleValue += static_cast<unsigned char>(imageData[pixelStart + j]);
        }

        grayscaleValue /= channels;

        for (int j = 0; j < channels; ++j) {
            imageData[pixelStart + j] = static_cast<char>(grayscaleValue);
        }
    }

    inputFile.close();

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file: " << outputPath << std::endl;
        return;
    }

    outputFile.write(bmpHeader.data(), bmpHeader.size());
    outputFile.write(imageData.data(), imageData.size());
}

void writeAndPrintData(std::ofstream& file, double executionTime, int n) {
    std::cout << "Image nr." << n << ", Execution time: " << executionTime << " seconds" << std::endl;
    file << n << "," << executionTime << "\n";
}

void convert(const std::string& inputPath, const std::string& outputPath, std::ofstream& logFile, int n) {
    clock_t startTime = clock();
    convertToBlackAndWhite(inputPath, outputPath);
    clock_t endTime = clock();
    double executionTime = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;
    writeAndPrintData(logFile, executionTime, n);
}

int main() {
    std::ofstream file("image_conversion_data.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening log file: image_conversion_data.csv" << std::endl;
        return 1;
    }

    file << "Image,Time\n";

    for (int i = 1; i <= 4; ++i) {
        std::string inputPhotoPath = "input/input" + std::to_string(i) + ".bmp";
        std::string outputPhotoPath = "output/output" + std::to_string(i) + ".bmp";
        convert(inputPhotoPath, outputPhotoPath, file, i);
    }

    return 0;
}
