package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strings"
	"time"

	"github.com/gin-gonic/gin"
)

// LogFileReader is a struct responsible for reading the response count from the log file.
type LogFileReader struct {
	logFile string
}

// NewLogFileReader creates a new LogFileReader instance.
func NewLogFileReader(logFile string) *LogFileReader {
	return &LogFileReader{logFile: logFile}
}

// ReadResponseCount reads the response count from the log file.
// If the log file is not found or an invalid line is encountered, it resets the counter and clears the file.
func (lfr *LogFileReader) ReadResponseCount() int {
	file, err := os.Open(lfr.logFile)
	if err != nil {
		fmt.Printf("%s file not found\n", lfr.logFile)
		return 0
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	lineCount := 0

	for scanner.Scan() {
		line := scanner.Text()
		if !lfr.isValidLine(line) {
			lfr.resetCounterAndClearFile()
			fmt.Println("Invalid line found in log.txt. Resetting the counter and clearing the file.")
			return 0
		}
		lineCount++
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Successfully read response count from %s\n", lfr.logFile)
	fmt.Printf("Response Count is set to: %d\n", lineCount)
	return lineCount
}

// isValidLine checks the validity of the line.
func (lfr *LogFileReader) isValidLine(line string) bool {
	// Checking the validity of the line (customize according to your own structure)
	// Example: "IP Address: [ip_address], Date: [date], Time: [time]\n"
	return strings.HasPrefix(line, "IP Address:") && strings.Count(line, ", Date:") == 1 && strings.Count(line, ", Time:") == 1
}

// resetCounterAndClearFile resets the response count and clears the log file.
func (lfr *LogFileReader) resetCounterAndClearFile() {
	file, err := os.Create(lfr.logFile)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()
}

// LogFileWriter is a struct responsible for writing log entries to the file.
type LogFileWriter struct {
	logFile string
}

// NewLogFileWriter creates a new LogFileWriter instance.
func NewLogFileWriter(logFile string) *LogFileWriter {
	return &LogFileWriter{logFile: logFile}
}

// WriteToFile writes the log entry to the file.
func (lfw *LogFileWriter) WriteToFile(ipAddress string, now time.Time) {
	file, err := os.OpenFile(lfw.logFile, os.O_APPEND|os.O_WRONLY, os.ModeAppend)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	_, err = file.WriteString(fmt.Sprintf("IP Address: %s, Date: %s, Time: %s\n",
		ipAddress, now.Format("2006-01-02"), now.Format("15:04:05")))
	if err != nil {
		log.Fatal(err)
	}
}

// ResponseBuilder is a struct responsible for constructing the response.
type ResponseBuilder struct {
	ipAddress     string
	responseCount int
}

// NewResponseBuilder creates a new ResponseBuilder instance.
func NewResponseBuilder(ipAddress string, responseCount int) *ResponseBuilder {
	return &ResponseBuilder{ipAddress: ipAddress, responseCount: responseCount}
}

// BuildResponse constructs the response JSON object.
func (rb *ResponseBuilder) BuildResponse() gin.H {
	now := time.Now()

	response := gin.H{
		"date":          now.Format("2006-01-02"),
		"time":          now.Format("15:04:05"),
		"responseCount": rb.responseCount,
	}

	fmt.Printf("Request from IP: %s at %s\n", rb.ipAddress, now.Format("2006-01-02 15:04:05"))
	fmt.Printf("Response: %v\n", response)

	return response
}

// ResponseHandler is a struct responsible for handling the response and response count.
type ResponseHandler struct {
	logFileReader *LogFileReader
	logFileWriter *LogFileWriter
	responseCount int
}

// NewResponseHandler creates a new ResponseHandler instance.
func NewResponseHandler(logFile string) *ResponseHandler {
	logFileReader := NewLogFileReader(logFile)
	logFileWriter := NewLogFileWriter(logFile)
	responseCount := logFileReader.ReadResponseCount()
	return &ResponseHandler{logFileReader: logFileReader, logFileWriter: logFileWriter, responseCount: responseCount}
}

// HandleResponse handles the incoming request, increments the response count, writes to the log file, and generates the response.
func (rh *ResponseHandler) HandleResponse(c *gin.Context) {
	ipAddress := c.ClientIP()

	rh.incrementCount()
	rh.logFileWriter.WriteToFile(ipAddress, time.Now())

	responseBuilder := NewResponseBuilder(ipAddress, rh.getCount())
	response := responseBuilder.BuildResponse()

	c.JSON(200, response)
}

// incrementCount increments the response count.
func (rh *ResponseHandler) incrementCount() {
	rh.responseCount++
}

// getCount retrieves the current response count.
func (rh *ResponseHandler) getCount() int {
	return rh.responseCount
}

func main() {
	logFile := "log.txt"
	responseHandler := NewResponseHandler(logFile)

	router := gin.Default()
	router.GET("/api/data", responseHandler.HandleResponse)

	if err := router.Run(":8080"); err != nil {
		log.Fatal(err)
	}
}
