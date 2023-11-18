package main

import (
	"fmt"
	"math"
	"os"
	"time"
)

func isPrime(num int) bool {
	if num < 2 {
		return false
	}
	for i := 2; i <= int(math.Sqrt(float64(num))); i++ {
		if num%i == 0 {
			return false
		}
	}
	return true
}

func calculatePrimeNumbers(limit int) []int {
	var primes []int
	for num := 2; num < limit; num++ {
		if isPrime(num) {
			primes = append(primes, num)
		}
	}
	return primes
}

func writeAndPrintData(file *os.File, limit int, executionTime float64, result []int) {
	fmt.Printf("Prime Numbers in Limit: %d are: %d\n", limit, len(result))
	fmt.Printf("Execution time: %f seconds\n", executionTime)
	file.WriteString(fmt.Sprintf("%d,%f\n", limit, executionTime))
}

func primeNumbers(limit int, file *os.File) {
	startTime := time.Now()
	result := calculatePrimeNumbers(limit)
	endTime := time.Since(startTime).Seconds()
	writeAndPrintData(file, limit, endTime, result)
}

func main() {
	file, err := os.Create("prime_numbers_data.csv")
	if err != nil {
		fmt.Println("Error creating file:", err)
		return
	}
	defer file.Close()

	file.WriteString("Limit,Time\n")

	for i := 1; i <= 25; i++ {
		primeNumbers(int(math.Pow(2, float64(i))), file)
	}
}
