#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

int countEInFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return 0;
        }

        std::string text;
        char ch;
        while (file.get(ch)) {
            text += std::tolower(ch);
        }

        int count = 0;
        for (char c : text) {
            if (c == 'e') {
                count++;
            }
        }

        return count;
    } catch (const std::exception& e) {
        return 0;
    }
}

int main() {
    const std::string file_path = "../bee-movie.txt";

    auto start_time = std::chrono::high_resolution_clock::now();
    int count = countEInFile(file_path);
    auto end_time = std::chrono::high_resolution_clock::now();
    double execution_time = std::chrono::duration<double>(end_time - start_time).count();

    std::cout << "Number of 'e' characters (non-case-sensitive) in the file: " << count << std::endl;
    std::cout << "Execution time: " << execution_time << " seconds" << std::endl;

    return 0;
}
