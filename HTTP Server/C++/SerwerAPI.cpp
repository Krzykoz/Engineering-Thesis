#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace json = boost::json;
using tcp = net::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

class LogFileHandler {
public:
    LogFileHandler(const std::string& file) : logFile(file) {}

    int readResponseCount() {
        std::ifstream file(logFile);
        if (!file.is_open()) {
            std::cerr << logFile << " file not found\n";
            return 0;
        }

        int lineCount = 0;
        std::string line;
        while (getline(file, line)) {
            if (!isValidLine(line)) {
                resetCounterAndClearFile();
                std::cerr << "Invalid line found. Resetting counter and clearing file.\n";
                return 0;
            }
            ++lineCount;
        }
        return lineCount;
    }

    void writeToLogFile(const std::string& ipAddress) {
        std::ofstream file(logFile, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Error opening file for writing.\n";
            return;
        }

        // Get current time
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        file << "IP Address: " << ipAddress << ", Date: ";
        file << std::put_time(&tm, "%Y-%m-%d") << ", Time: ";
        file << std::put_time(&tm, "%H:%M:%S") << std::endl;
    }

private:
    std::string logFile;

    bool isValidLine(const std::string& line) {
        // Implement your validation logic here
        return true;
    }

    void resetCounterAndClearFile() {
        std::ofstream file(logFile, std::ofstream::out | std::ofstream::trunc);
    }
};

std::string createResponse(int responseCount) {
    json::object obj;
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream date_stream, time_stream;
    date_stream << std::put_time(&tm, "%Y-%m-%d");
    time_stream << std::put_time(&tm, "%H:%M:%S");

    obj["date"] = date_stream.str();
    obj["time"] = time_stream.str();
    obj["response_count"] = responseCount;

    return json::serialize(obj);
}

void handleRequest(tcp::socket& socket, LogFileHandler& logHandler, int& responseCount) {
    beast::flat_buffer buffer;

    // Read the request
    http::request<http::dynamic_body> req;
    http::read(socket, buffer, req);

    // Increment the response count and write to log
    responseCount++;
    logHandler.writeToLogFile(socket.remote_endpoint().address().to_string());

    // Create the HTTP response
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    res.body() = createResponse(responseCount);
    res.prepare_payload();

    // Write the response back to the client
    http::write(socket, res);
}

int main() {
    try {
        net::io_context ioc{1};

        tcp::acceptor acceptor{ioc, {net::ip::make_address("127.0.0.1"), 8080}};
        LogFileHandler logHandler("log.txt");
        int responseCount = logHandler.readResponseCount();

        while (true) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            handleRequest(socket, logHandler, responseCount);
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
