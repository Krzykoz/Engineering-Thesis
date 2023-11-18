package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
	"time"
)

func radixSort(arr []int) {
	maxNum := findMax(arr)
	exp := 1

	for maxNum/exp > 0 {
		countingSort(arr, exp)
		exp *= 10
	}
}

func countingSort(arr []int, exp int) {
	n := len(arr)
	output := make([]int, n)
	count := make([]int, 10)

	for i := 0; i < n; i++ {
		index := arr[i] / exp
		count[index%10]++
	}

	for i := 1; i < 10; i++ {
		count[i] += count[i-1]
	}

	i := n - 1
	for i >= 0 {
		index := arr[i] / exp
		output[count[index%10]-1] = arr[i]
		count[index%10]--
		i--
	}

	for i := 0; i < n; i++ {
		arr[i] = output[i]
	}
}

func findMax(arr []int) int {
	maxNum := math.MinInt32
	for _, num := range arr {
		if num > maxNum {
			maxNum = num
		}
	}
	return maxNum
}

func writeAndPrintData(file *os.File, limit int, executionTime float64) {
	fmt.Printf("For %d Number, Execution time: %f seconds\n", limit, executionTime)
	file.WriteString(fmt.Sprintf("%d,%f\n", limit, executionTime))
}

func readNumbersFromFile(filePath string) []int {
	content, err := ioutil.ReadFile(filePath)
	if err != nil {
		log.Fatal(err)
	}

	lines := strings.Split(string(content), "\n")
	var numbers []int

	for _, line := range lines {
		if line == "" {
			continue
		}
		num, err := strconv.Atoi(line)
		if err != nil {
			log.Fatal(err)
		}
		numbers = append(numbers, num)
	}

	return numbers
}

func sort(limit int, numbers []int, logFile *os.File) {
	startTime := time.Now()
	radixSort(numbers)
	endTime := time.Since(startTime).Seconds()
	writeAndPrintData(logFile, limit, endTime)
}

func main() {
	file, err := os.Create("radix_sort_data.csv")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	file.WriteString("Limit,Time\n")

	filePath := "../numbers.txt"
	bigNumbersList := readNumbersFromFile(filePath)

	for i := 1; i < 8; i++ {
		numElements := int(math.Pow10(i))
		numbersList := bigNumbersList[:numElements]
		sort(numElements, numbersList, file)
	}
}
