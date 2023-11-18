package main

import (
	"fmt"
	"os"
	"strings"
	"time"
)

func countEInFile(filename string) int {
	content, err := os.ReadFile(filename)
	if err != nil {
		return 0
	}

	text := string(content)
	text = strings.ToLower(text)
	count := strings.Count(text, "e")
	return count
}

func main() {
	filePath := "../bee-movie.txt"

	startTime := time.Now()
	count := countEInFile(filePath)
	endTime := time.Now()
	executionTime := endTime.Sub(startTime).Seconds()

	fmt.Printf("Number of 'e' characters (non-case-sensitive) in the file: %d\n", count)
	fmt.Printf("Execution time: %f seconds\n", executionTime)
}
