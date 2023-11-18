import random
import time

LOWER_LIMIT = 1
UPPER_LIMIT = 2147483647

def generate_and_save_random_numbers(file_path, num_numbers):
    with open(file_path, 'w') as file:
        for _ in range(num_numbers):
            random_number = random.randint(LOWER_LIMIT, UPPER_LIMIT)
            file.write(str(random_number) + '\n')

def write_and_print_data(file, numbers_generated, execution_time):
    print(f"Generated {numbers_generated} numbers, Execution time: {execution_time} seconds")
    file.write(f"{numbers_generated},{execution_time}\n")

def generate(file_path, numbers_generated, log_file):
    start_time = time.time()
    generate_and_save_random_numbers(file_path, numbers_generated)
    end_time = time.time()
    execution_time = end_time - start_time
    write_and_print_data(log_file, numbers_generated, execution_time)


with open("number_generator_data.csv", 'w') as log_file:
    log_file.write("Numbers,Time\n")

    for i in range(1, 9):
        numbers_generated = 10 ** i
        file_path = f'numbers{numbers_generated}.txt'
        generate(file_path, numbers_generated, log_file)
