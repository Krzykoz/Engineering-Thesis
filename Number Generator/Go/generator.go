package main

import (
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"time"
)

const (
	LOWER_LIMIT = 1
	UPPER_LIMIT = 2147483647
)

func generateAndSaveRandomNumbers(filePath string, numNumbers int) {
	file, err := os.Create(filePath)
	if err != nil {
		fmt.Println("Error creating file:", err)
		return
	}
	defer file.Close()

	for i := 0; i < numNumbers; i++ {
		randomNumber := rand.Intn(UPPER_LIMIT-LOWER_LIMIT+1) + LOWER_LIMIT
		file.WriteString(strconv.Itoa(randomNumber) + "\n")
	}
}

func writeAndPrintData(file *os.File, numbersGenerated int, executionTime float64) {
	fmt.Printf("Generated %d numbers, Execution time: %f seconds\n", numbersGenerated, executionTime)
	file.WriteString(fmt.Sprintf("%d,%f\n", numbersGenerated, executionTime))
}

func generate(filePath string, numbersGenerated int, logFile *os.File) {
	startTime := time.Now()
	generateAndSaveRandomNumbers(filePath, numbersGenerated)
	endTime := time.Now()
	executionTime := endTime.Sub(startTime).Seconds()
	writeAndPrintData(logFile, numbersGenerated, executionTime)
}

func main() {
	logFile, err := os.Create("number_generator_data.csv")
	if err != nil {
		fmt.Println("Error creating log file:", err)
		return
	}
	defer logFile.Close()

	logFile.WriteString("Numbers,Time\n")

	for i := 1; i < 9; i++ {
		numbersGenerated := int(1e6) * i
		filePath := fmt.Sprintf("numbers%d.txt", numbersGenerated)
		generate(filePath, numbersGenerated, logFile)
	}
}
