#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

class LogFileReader {
public:
    LogFileReader(const std::string& log_file) : log_file(log_file) {}

    int read_response_count() {
        try {
            std::ifstream file(log_file);
            if (!file)
                throw std::runtime_error(log_file + " file not found");

            int line_count = 0;
            std::string line;
            while (std::getline(file, line)) {
                if (!is_valid_line(line)) {
                    reset_counter_and_clear_file();
                    std::cout << "Invalid line found in log.txt. Resetting the counter and clearing the file." << std::endl;
                    return 0;
                }
                line_count++;
            }

            std::cout << "Successfully read response count from log.txt" << std::endl;
            std::cout << "Response Count is set to: " << line_count << std::endl;
            return line_count;
        }
        catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
            return 0;
        }
    }

private:
    bool is_valid_line(const std::string& line) const {
        // Checking the validity of the line (customize according to your own structure)
        // Example: "IP Address: [ip_address], Date: [date], Time: [time]\n"
        return line.rfind("IP Address:", 0) == 0 &&
            std::count(line.begin(), line.end(), ',') == 2 &&
            line.find(", Date:") != std::string::npos &&
            line.find(", Time:") != std::string::npos;
    }

    void reset_counter_and_clear_file() {
        std::ofstream file(log_file, std::ofstream::trunc);
    }

private:
    std::string log_file;
};

class LogFileWriter {
public:
    LogFileWriter(const std::string& log_file) : log_file(log_file) {}

    void write_to_file(const std::string& ip_address, const std::string& now) {
        std::ofstream file(log_file, std::ofstream::app);
        if (file) {
            file << "IP Address: " << ip_address << ", Date: " << now.substr(0, 10) << ", Time: " << now.substr(11) << "\n";
        }
    }

private:
    std::string log_file;
};

class ResponseBuilder {
public:
    ResponseBuilder(const std::string& ip_address, int response_count) : ip_address(ip_address), response_count(response_count) {}

    web::json::value build_response() {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream date_stream, time_stream;
        date_stream << std::put_time(std::localtime(&now), "%Y-%m-%d");
        time_stream << std::put_time(std::localtime(&now), "%H:%M:%S");

        std::string formatted_date = date_stream.str();
        std::string formatted_time = time_stream.str();

        web::json::value response;
        response[U("date")] = web::json::value::string(formatted_date);
        response[U("time")] = web::json::value::string(formatted_time);
        response[U("response_count")] = web::json::value::number(response_count);

        std::cout << "Request from IP: " << ip_address << " at " << formatted_date << " " << formatted_time << std::endl;
        std::cout << "Response: " << response.serialize() << std::endl;

        return response;
    }

private:
    std::string ip_address;
    int response_count;
};

class ResponseHandler {
public:
    ResponseHandler(const std::string& log_file) : log_file_reader(log_file), log_file_writer(log_file), response_count(log_file_reader.read_response_count()) {}

    web::json::value handle_response(const http::http_request& request) {
        std::string ip_address = utility::conversions::to_utf8string(request.remote_address());

        increment_count();
        log_file_writer.write_to_file(ip_address, current_time());

        ResponseBuilder response_builder(ip_address, get_count());
        web::json::value response = response_builder.build_response();

        return response;
    }

private:
    void increment_count() {
        response_count++;
    }

    int get_count() const {
        return response_count;
    }

    std::string current_time() const {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream stream;
        stream << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
        return stream.str();
    }

private:
    LogFileReader log_file_reader;
    LogFileWriter log_file_writer;
    int response_count;
};

int main() {
    utility::string_t host_uri = U("http://localhost:8080");
    http_listener listener(host_uri);

    std::string log_file = "log.txt";
    ResponseHandler response_handler(log_file);

    listener.support(methods::GET, [&](http_request request) {
        web::json::value response = response_handler.handle_response(request);

        request.reply(status_codes::OK, response);
    });

    try {
        listener.open().wait();
        std::cout << "Server listening on " << host_uri << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.ignore();
        listener.close().wait();
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
