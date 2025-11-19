// nc_client.cpp
// Simple netcat-style TCP client using Boost.Asio.
// Usage: nc_client <host> <port>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <string>

namespace asio = boost::asio;
using asio::ip::tcp;

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
        return 2;
    }

    const std::string host = argv[1];
    const std::string port = argv[2];

    try {
        asio::io_context io;

        // Resolve and connect
        tcp::resolver resolver(io);
        auto endpoints = resolver.resolve(host, port);
        tcp::socket socket(io);
        asio::connect(socket, endpoints);

        std::cout << "Connected to " << host << ":" << port << "\n";

        std::atomic<bool> done{false};

        // Reader thread: read from socket and write to stdout
        std::thread reader([&]() {
            try {
                std::array<char, 4096> buf;
                for (;;) {
                    boost::system::error_code ec;
                    std::size_t n = socket.read_some(asio::buffer(buf), ec);
                    if (ec) {
                        if (ec == asio::error::eof) {
                            // orderly shutdown by peer
                            // print remaining and exit
                            if (n) std::cout.write(buf.data(), n);
                            // drop newline to ensure flushed output
                            std::cout << std::flush;
                        } else {
                            std::cerr << "\nSocket read error: " << ec.message() << "\n";
                        }
                        break;
                    }
                    if (n) {
                        std::cout.write(buf.data(), n);
                        std::cout << std::flush;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "\nReader exception: " << e.what() << "\n";
            }
            done = true;
        });

        // Writer loop: read from stdin (line-by-line) and send to socket.
        // Works with interactive or piped stdin. If stdin closes, shut down send side.
        try {
            std::string line;
            // Use std::getline so interactive Enter behaves normally.
            while (!done.load() && std::getline(std::cin, line)) {
                // std::getline strips newline; re-add it to mimic netcat behaviour
                line.push_back('\n');
                boost::system::error_code ec;
                asio::write(socket, asio::buffer(line), ec);
                if (ec) {
                    std::cerr << "\nSocket write error: " << ec.message() << "\n";
                    break;
                }
            }

            // If stdin EOF (piped mode) or user closed input, gracefully shutdown send side.
            boost::system::error_code shutdown_ec;
            socket.shutdown(tcp::socket::shutdown_send, shutdown_ec);
            // Keep reading until server closes connection (reader thread finishes).
        } catch (const std::exception& e) {
            std::cerr << "\nWriter exception: " << e.what() << "\n";
        }

        // Wait for reader to finish
        if (reader.joinable()) reader.join();

        std::cout << "\nConnection closed.\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
