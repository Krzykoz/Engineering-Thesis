#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <ctime>

void counting_sort(std::vector<int>& arr, int exp) {
    int n = arr.size();
    std::vector<int> output(n, 0);
    std::vector<int> count(10, 0);

    // Count occurrences of each digit at the current place
    for (int i = 0; i < n; ++i) {
        int index = arr[i] / exp;
        count[index % 10] += 1;
    }

    // Update count[i] to store the position of the next occurrence
    for (int i = 1; i < 10; ++i) {
        count[i] += count[i - 1];
    }

    // Build the output array
    for (int i = n - 1; i >= 0; --i) {
        int index = arr[i] / exp;
        output[count[index % 10] - 1] = arr[i];
        count[index % 10] -= 1;
    }

    // Copy the output array to arr, so that arr now contains sorted numbers
    for (int i = 0; i < n; ++i) {
        arr[i] = output[i];
    }
}

void radix_sort(std::vector<int>& arr) {
    // Find the maximum number to determine the number of digits
    int max_num = *std::max_element(arr.begin(), arr.end());
    int exp = 1;

    while (max_num / exp > 0) {
        counting_sort(arr, exp);
        exp *= 10;
    }
}

void write_and_print_data(std::ofstream& file, int limit, double execution_time) {
    std::cout << "For " << limit << " Numbers, Execution time: " << execution_time << " seconds\n";
    file << limit << "," << execution_time << "\n";
}

std::vector<int> read_numbers_from_file(const std::string& file_path) {
    std::ifstream file(file_path);
    std::vector<int> numbers;
    int number;
    
    while (file >> number) {
        numbers.push_back(number);
    }

    return numbers;
}

void sort(int limit, std::vector<int>& numbers, std::ofstream& log_file) {
    std::clock_t start_time = std::clock();
    radix_sort(numbers);
    std::clock_t end_time = std::clock();
    double execution_time = (end_time - start_time) / static_cast<double>(CLOCKS_PER_SEC);
    write_and_print_data(log_file, limit, execution_time);
}

int main() {
    std::ofstream file("../radix_sort_data.csv");
    file << "Limit,Time\n";

    std::string file_path = "../../numbers.txt";
    std::vector<int> big_numbers_list = read_numbers_from_file(file_path);

    for (int i = 1; i < 8; ++i) {
        int num_elements = static_cast<int>(pow(10, i));
        std::vector<int> numbers_list(big_numbers_list.begin(), big_numbers_list.begin() + num_elements);
        sort(num_elements, numbers_list, file);
    }

    return 0;
}
