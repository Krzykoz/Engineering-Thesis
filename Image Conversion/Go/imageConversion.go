package main

import (
	"encoding/binary"
	"fmt"
	"os"
	"time"
)

func convertToBlackAndWhite(inputPath, outputPath string) {
	inputFile, err := os.Open(inputPath)
	if err != nil {
		fmt.Println("Error opening input file:", err)
		return
	}
	defer inputFile.Close()

	bmpHeader := make([]byte, 54)
	_, err = inputFile.Read(bmpHeader)
	if err != nil {
		fmt.Println("Error reading BMP header:", err)
		return
	}

	width := binary.LittleEndian.Uint32(bmpHeader[18:22])
	height := binary.LittleEndian.Uint32(bmpHeader[22:26])
	channels := 3

	imageData := make([]byte, int(width)*int(height)*channels)
	_, err = inputFile.Read(imageData)
	if err != nil {
		fmt.Println("Error reading image data:", err)
		return
	}

	for i := uint32(0); i < width*height; i++ {
		pixelStart := i * uint32(channels)
		grayscaleValue := uint8((uint16(imageData[pixelStart]) + uint16(imageData[pixelStart+1]) + uint16(imageData[pixelStart+2])) / 3)

		for j := 0; j < channels; j++ {
			imageData[pixelStart+uint32(j)] = grayscaleValue
		}
	}

	outputFile, err := os.Create(outputPath)
	if err != nil {
		fmt.Println("Error creating output file:", err)
		return
	}
	defer outputFile.Close()

	outputFile.Write(bmpHeader)
	outputFile.Write(imageData)
}

func writeAndPrintData(file *os.File, executionTime float64, n int) {
	fmt.Printf("Image nr.%d, Execution time: %f seconds\n", n, executionTime)
	file.WriteString(fmt.Sprintf("%d,%f\n", n, executionTime))
}

func convert(inputPath, outputPath string, logFile *os.File, n int) {
	startTime := time.Now()
	convertToBlackAndWhite(inputPath, outputPath)
	endTime := time.Now()
	executionTime := endTime.Sub(startTime).Seconds()
	writeAndPrintData(logFile, executionTime, n)
}

func main() {
	logFile, err := os.Create("image_conversion_data.csv")
	if err != nil {
		fmt.Println("Error creating log file:", err)
		return
	}
	defer logFile.Close()

	logFile.WriteString("Image,Time\n")

	for i := 1; i <= 4; i++ {
		inputPhotoPath := fmt.Sprintf("input/input%d.bmp", i)
		outputPhotoPath := fmt.Sprintf("output/output%d.bmp", i)
		convert(inputPhotoPath, outputPhotoPath, logFile, i)
	}
}
