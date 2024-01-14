import time
import sympy

def calculate_prime_numbers(limit):
    primes = [num for num in range(2, limit) if sympy.isprime(num)]
    return primes

def write_and_print_data(file, limit, execution_time, result):
    print(f"Prime Numbers in Limit: {limit} are: {len(result)}")
    print(f"Execution time: {execution_time} seconds")
    file.write(f"{limit},{execution_time}\n")

def prime_numbers(limit, file):
    start_time = time.time()
    result = calculate_prime_numbers(limit)
    end_time = time.time()
    execution_time = end_time - start_time
    write_and_print_data(file, limit, execution_time, result)

with open('prime_numbers_data.csv', 'w') as file:
    file.write("Limit,Time\n")

    for i in range(1, 9):
        prime_numbers(10**i, file)
