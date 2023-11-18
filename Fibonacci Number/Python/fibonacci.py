import time

def fibonacci(n):
    if n <= 0:
        return 0
    elif n == 1:
        return 1
    else:
        return fibonacci(n - 1) + fibonacci(n - 2)

def writeAndPrintData(file, n, time, result):
    print(f"The Fibonacci number for {n} is: {result}")
    print(f"Execution time: {time} seconds")

    file.write(f"{n},{time}\n")

def calculateFibonacciNumber(n, file):
    start_time = time.time()
    result = fibonacci(n)
    end_time = time.time()

    execution_time = end_time - start_time
    writeAndPrintData(file, n, execution_time, result)


with open('fibonacci_data.csv', 'w') as file:
    file.write("Sequence Number,Time\n")

    for i in range(1, 41):
        calculateFibonacciNumber(i, file)
