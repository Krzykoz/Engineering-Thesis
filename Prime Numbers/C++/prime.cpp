#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <cmath>
#include <numbers>

bool is_prime(int num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
    }
    return true;
}

std::vector<int> calculate_prime_numbers(int limit) {
    std::vector<int> primes;
    for (int num = 2; num < limit; ++num) {
        if (is_prime(num)) {
            primes.push_back(num);
        }
    }
    return primes;
}

void write_and_print_data(std::ofstream &file, int limit, double execution_time, const std::vector<int> &result) {
    std::cout << "Prime Numbers in Limit: " << limit << " are: " << result.size() << std::endl;
    std::cout << "Execution time: " << execution_time << " seconds" << std::endl;
    file << limit << "," << execution_time << "\n";
}

void prime_numbers(int limit, std::ofstream &file) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<int> result = calculate_prime_numbers(limit);
    auto end_time = std::chrono::high_resolution_clock::now();
    double execution_time = std::chrono::duration<double>(end_time - start_time).count();
    write_and_print_data(file, limit, execution_time, result);
}

int main() {
    std::ofstream file("prime_numbers_data.csv");
    file << "Limit,Time\n";

    for (int i = 1; i < 9; ++i) {
        prime_numbers(static_cast<int>(std::pow(10, i)), file);
    }

    return 0;
}
