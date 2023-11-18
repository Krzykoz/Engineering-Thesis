#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

class LogFileReader {
public:
    LogFileReader(const std::string& log_file) : log_file(log_file) {}

    int read_response_count() {
        try {
            std::ifstream file(log_file);
            if (!file.is_open()) {
                std::cerr << log_file << " file not found" << std::endl;
                return 0;
            }

            int line_count = 0;
            std::string line;
            while (std::getline(file, line)) {
                if (!is_valid_line(line)) {
                    reset_counter_and_clear_file();
                    std::cerr << "Invalid line found in log.txt. Resetting the counter and clearing the file." << std::endl;
                    return 0;
                }
                line_count++;
            }

            std::cout << "Successfully read response count from log.txt" << std::endl;
            std::cout << "Response Count is set to: " << line_count << std::endl;
            return line_count;
        } catch (const std::exception& e) {
            std::cerr << "Exception while reading response count: " << e.what() << std::endl;
            return 0;
        }
    }

private:
    std::string log_file;

    static bool is_valid_line(const std::string& line) {
        return line.find("IP Address:") == 0 && line.find(", Date:") != std::string::npos && line.find(", Time:") != std::string::npos;
    }

    void reset_counter_and_clear_file() {
        std::ofstream file(log_file, std::ios::out | std::ios::trunc);
        if (file.is_open()) {
            file.close();
        }
    }
};

class LogFileWriter {
public:
    LogFileWriter(const std::string& log_file) : log_file(log_file) {}

    void write_to_file(const std::string& ip_address, const std::tm& now) {
        try {
            std::ofstream file(log_file, std::ios::app);
            if (file.is_open()) {
                file << "IP Address: " << ip_address << ", Date: " << std::put_time(&now, "%Y-%m-%d")
                     << ", Time: " << std::put_time(&now, "%H:%M:%S") << "\n";
                file.close();
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception while writing to log file: " << e.what() << std::endl;
        }
    }

private:
    std::string log_file;
};

class ResponseBuilder {
public:
    ResponseBuilder(const std::string& ip_address, int response_count)
        : ip_address(ip_address), response_count(response_count) {}

    std::string build_response() {
        try {
            std::tm now;
            std::time_t t = std::time(nullptr);
            now = *std::localtime(&t);

            std::ostringstream oss;
            oss << "Request from IP: " << ip_address << " at " << std::put_time(&now, "%Y-%m-%d %H:%M:%S") << "\n";
            oss << "Response: {\"date\":\"" << std::put_time(&now, "%Y-%m-%d") << "\",\"time\":\"" << std::put_time(&now, "%H:%M:%S") << "\",\"response_count\":" << response_count << "}\n";

            std::cout << oss.str();

            return oss.str();
        } catch (const std::exception& e) {
            std::cerr << "Exception while building response: " << e.what() << std::endl;
            return "";
        }
    }

private:
    std::string ip_address;
    int response_count;
};

class ResponseHandler {
public:
    ResponseHandler(const std::string& log_file)
        : log_file_reader(log_file), log_file_writer(log_file), response_count(log_file_reader.read_response_count()) {}

    std::string handle_response(const std::string& ip_address) {
        try {
            increment_count();
            std::tm now;
            std::time_t t = std::time(nullptr);
            now = *std::localtime(&t);

            log_file_writer.write_to_file(ip_address, now);

            ResponseBuilder response_builder(ip_address, get_count());
            return response_builder.build_response();
        } catch (const std::exception& e) {
            std::cerr << "Exception while handling response: " << e.what() << std::endl;
            return "";
        }
    }

private:
    void increment_count() {
        response_count++;
    }

    int get_count() const {
        return response_count;
    }

    LogFileReader log_file_reader;
    LogFileWriter log_file_writer;
    int response_count;
};

class HttpServer {
public:
    HttpServer(asio::io_context& io_context, short port, ResponseHandler& response_handler)
        : io_context(io_context), acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), response_handler(response_handler) {
        accept();
    }

private:
    void accept() {
        acceptor.async_accept(
            [this](beast::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::make_shared<HttpSession>(std::move(socket), response_handler)->run();
                }
                accept();
            });
    }

    class HttpSession : public std::enable_shared_from_this<HttpSession> {
    public:
        explicit HttpSession(asio::ip::tcp::socket socket, ResponseHandler& response_handler)
            : socket(std::move(socket)), response_handler(response_handler) {}

        void run() {
            do_read();
        }

    private:
        void do_read() {
            req = {};
            http::async_read(socket, buffer, req,
                [self = shared_from_this()](beast::error_code ec, std::size_t) {
                    if (!ec) {
                        // Check if the request is for the "/api/data" endpoint
                        if (self->req.target() == "/api/data") {
                            self->do_write();
                        } else {
                            self->handle_bad_request();
                        }
                    }
                });
        }

        void do_write() {
            res = {};
            res.version(req.version());
            res.result(http::status::ok);
            res.set(http::field::server, "Boost Beast Server");
            res.set(http::field::content_type, "text/plain");
            res.body() = response_handler.handle_response(socket.remote_endpoint().address().to_string());
            res.prepare_payload();

            http::async_write(socket, res,
                [self = shared_from_this()](beast::error_code ec, std::size_t) {
                    self->socket.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
                    self->socket.close(ec);
                });
        }

        void handle_bad_request() {
            // Close the socket without sending any response
            beast::error_code ec;
            socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            socket.close(ec);
        }

        asio::ip::tcp::socket socket;
        beast::flat_buffer buffer{8192};
        http::request<http::string_body> req;
        http::response<http::string_body> res;
        ResponseHandler& response_handler;
    };

    asio::io_context& io_context;
    asio::ip::tcp::acceptor acceptor;
    ResponseHandler& response_handler;
};

int main() {
    try {
        asio::io_context io_context;
        ResponseHandler response_handler("log.txt");  // Initialize ResponseHandler
        HttpServer server(io_context, 8080, response_handler);
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
