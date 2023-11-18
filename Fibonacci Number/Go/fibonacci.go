package main

import (
	"fmt"
	"os"
	"time"
)

func fibonacci(n int) int {
	if n <= 0 {
		return 0
	} else if n == 1 {
		return 1
	} else {
		return fibonacci(n-1) + fibonacci(n-2)
	}
}

func writeAndPrintData(file *os.File, n int, duration time.Duration, result int) {
	fmt.Printf("The Fibonacci number for %d is: %d\n", n, result)
	fmt.Printf("Execution time: %s seconds\n", duration.String())

	file.WriteString(fmt.Sprintf("%d,%s\n", n, duration.String()))
}

func calculateFibonacciNumber(n int, file *os.File) {
	startTime := time.Now()
	result := fibonacci(n)
	endTime := time.Now()

	executionTime := endTime.Sub(startTime)
	writeAndPrintData(file, n, executionTime, result)
}

func main() {
	file, err := os.Create("fibonacci_data.csv")
	if err != nil {
		fmt.Println("Error creating file:", err)
		return
	}
	defer file.Close()

	file.WriteString("Sequence Number,Time\n")

	for i := 1; i <= 40; i++ {
		calculateFibonacciNumber(i, file)
	}
}
