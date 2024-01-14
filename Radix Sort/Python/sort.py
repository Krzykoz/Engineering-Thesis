import time

def radix_sort(arr):
    max_num = max(arr)
    exp = 1

    while max_num // exp > 0:
        counting_sort(arr, exp)
        exp *= 10

def counting_sort(arr, exp):
    n = len(arr)
    output = [0] * n
    count = [0] * 10

    for i in range(n):
        index = arr[i] // exp
        count[index % 10] += 1

    for i in range(1, 10):
        count[i] += count[i - 1]

    i = n - 1
    while i >= 0:
        index = arr[i] // exp
        output[count[index % 10] - 1] = arr[i]
        count[index % 10] -= 1
        i -= 1

    for i in range(n):
        arr[i] = output[i]

def write_and_print_data(file, limit, execution_time):
    print(f"For {limit} Number, Execution time: {execution_time} seconds")
    file.write(f"{limit},{execution_time}\n")

def read_numbers_from_file(file_path):
    with open(file_path, 'r') as file:
        numbers = [int(line.strip()) for line in file]
    return numbers

def sort(limit, numbers, log_file):
    start_time = time.time()
    radix_sort(numbers)
    end_time = time.time()
    execution_time = end_time - start_time
    write_and_print_data(log_file, limit, execution_time)

with open('radix_sort_data.csv', 'w') as file:
    file.write("Limit,Time\n")

    file_path = '../numbers.txt'
    big_numbers_list = read_numbers_from_file(file_path)

    for i in range(4, 9):
        num_elements = 10 ** i
        numbers_list = big_numbers_list[:num_elements]
        sort(num_elements, numbers_list, file)