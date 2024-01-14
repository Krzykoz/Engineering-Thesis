from flask import Flask, jsonify, request
from datetime import datetime

app = Flask(__name__)


class LogFileReader:
    def __init__(self, log_file):
        self.log_file = log_file

    def read_response_count(self):
        try:
            with open(self.log_file, 'r') as file:
                line_count = 0
                for line in file:
                    if not self.is_valid_line(line):
                        self.reset_counter_and_clear_file()
                        print('Invalid line found in log.txt. Resetting the counter and clearing the file.')
                        return 0
                    line_count += 1
                else:
                    print('Successfully read response count from log.txt')
                    print(f'Response Count is set to: {line_count}')
                    return line_count

        except FileNotFoundError:
            print(f'{self.log_file} file not found')
            return 0

    @staticmethod
    def is_valid_line(line):
        # Checking the validity of the line
        # Example: "IP Address: [ip_address], Date: [date], Time: [time]\n"
        return line.startswith("IP Address:") and line.count(", Date:") == 1 and line.count(", Time:") == 1

    def reset_counter_and_clear_file(self):
        with open(self.log_file, 'w') as file:
            file.truncate(0)


class LogFileWriter:
    def __init__(self, log_file):
        self.log_file = log_file

    def write_to_file(self, ip_address, now):
        """
        Writes the log entry to the file.
        """
        with open(self.log_file, 'a') as file:
            file.write(
                f'IP Address: {ip_address}, Date: {now.strftime("%Y-%m-%d")}, Time: {now.strftime("%H:%M:%S")}\n'
            )


class ResponseBuilder:
    def __init__(self, ip_address, response_count):
        self.ip_address = ip_address
        self.response_count = response_count

    def build_response(self):
        now = datetime.now()

        response = {
            'date': now.strftime('%Y-%m-%d'),
            'time': now.strftime('%H:%M:%S'),
            'response_count': self.response_count
        }

        print(f'Request from IP: {self.ip_address} at {now.strftime("%Y-%m-%d %H:%M:%S")}')
        print(f'Response: {response}')

        return jsonify(response)


class ResponseHandler:
    def __init__(self, log_file):
        self.log_file_reader = LogFileReader(log_file)
        self.log_file_writer = LogFileWriter(log_file)
        self.response_count = self.log_file_reader.read_response_count()

    def handle_response(self):
        ip_address = request.remote_addr

        self.increment_count()
        self.log_file_writer.write_to_file(ip_address, datetime.now())

        response_builder = ResponseBuilder(ip_address, self.get_count())
        response = response_builder.build_response()

        return response

    def increment_count(self):
        self.response_count += 1

    def get_count(self):
        return self.response_count


log_file = 'log.txt'
response_handler = ResponseHandler(log_file)


@app.route('/api/data', methods=['GET'])
def get_data():
    return response_handler.handle_response()

if __name__ == '__main__':
    app.run()
