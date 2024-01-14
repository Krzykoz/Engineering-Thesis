#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int fibonacci(int n) {
    if (n <= 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

void writeAndPrintData(ofstream &file, int n, double time, int result) {
    cout << "The Fibonacci number for " << n << " is: " << result << endl;
    cout << "Execution time: " << time << " seconds" << endl;

    file << n << "," << time << endl;
}

void calculateFibonacciNumber(int n, ofstream &file) {
    high_resolution_clock::time_point start_time = high_resolution_clock::now();
    int result = fibonacci(n);
    high_resolution_clock::time_point end_time = high_resolution_clock::now();

    duration<double> execution_time = duration_cast<duration<double>>(end_time - start_time);
    writeAndPrintData(file, n, execution_time.count(), result);
}

int main() {
    ofstream file("fibonacci_data.csv");
    file << "Sequence Number,Time\n";

    for (int i = 30; i <= 45; i++) {
        calculateFibonacciNumber(i, file);
    }

    return 0;
}
