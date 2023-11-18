import struct
import time

def convert_to_black_and_white(input_path, output_path):
    with open(input_path, "rb") as input_file:
        bmp_header = input_file.read(54)
        width, height = struct.unpack("<II", bmp_header[18:26])
        channels = 3
        image_data = bytearray(input_file.read())

        for i in range(width * height):
            pixel_start = i * channels
            grayscale_value = sum(image_data[pixel_start:pixel_start + channels]) // channels
            for j in range(channels):
                image_data[pixel_start + j] = grayscale_value

    with open(output_path, "wb") as output_file:
        output_file.write(bmp_header)
        output_file.write(image_data)

def write_and_print_data(file, execution_time, n):
    print(f"Image nr.{n}, Execution time: {execution_time} seconds")
    file.write(f"{n},{execution_time}\n")

def convert(input_path, output_path, log_file, n):
    start_time = time.time()
    convert_to_black_and_white(input_path, output_path)
    end_time = time.time()
    execution_time = end_time - start_time
    write_and_print_data(log_file, execution_time, n)

with open('image_conversion_data.csv', 'w') as file:
    file.write("Image,Time\n")

    for i in range(1, 5):
        input_photo_path = f"input/input{i}.bmp"
        output_photo_path = f"output/output{i}.bmp"
        convert(input_photo_path, output_photo_path, file, i)
