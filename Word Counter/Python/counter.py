import time

def count_e_in_file(filename):
    try:
        with open(filename, 'r') as file:
            text = file.read()
            text = text.lower()
            count = text.count('e')
            return count
    except FileNotFoundError:
        return 0

file_path = '../bee-movie.txt'

start_time = time.time()
count = count_e_in_file(file_path)
end_time = time.time()
execution_time = end_time - start_time
print(f"Number of 'e' characters (non-case-sensitive) in the file: {count}")
print(f"Execution time: {execution_time} seconds")
