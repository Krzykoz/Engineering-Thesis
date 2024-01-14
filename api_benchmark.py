import requests
import time

def measure_average_response_time():
    url = "http://127.0.0.1:5000/api/data"
    total_time = 0
    num_requests = 10000

    for _ in range(num_requests):
        start_time = time.time()
        response = requests.get(url)
        end_time = time.time()
        total_time += (end_time - start_time)

    average_time = total_time / num_requests
    return average_time

# Example usage
average_response_time = measure_average_response_time()
print(f"Average Response Time: {average_response_time} seconds")
