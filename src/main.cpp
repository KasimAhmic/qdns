#include <span>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio.hpp>

#include "dns.hpp"
#include "logging.hpp"

constexpr uint8_t MAX_THREADS = 4;
constexpr uint16_t PORT = 55555;

static auto logger = std::make_unique<logging::Logger>("main", logging::LogLevel::Debug);

// boost::asio::awaitable<void> echo(boost::asio::ip::tcp::socket socket) {
//     try {
//         logger->info(std::format("Accepted connection from {}", socket.remote_endpoint().address().to_string()));
//
//         char data[MAX_READ_SIZE];
//
//         while (true) {
//             std::size_t bytes_read = co_await socket.async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
//
//             logger->info(std::format("Read %zu bytes: '%.*s'\n", bytes_read, static_cast<int>(bytes_read), data));
//
//             co_await boost::asio::async_write(socket, boost::asio::buffer(data, bytes_read), boost::asio::use_awaitable);
//         }
//     } catch (const std::exception& e) {
//         logger->error(std::format("echo Exception: %s\n", e.what()));
//     }
// }
//
// boost::asio::awaitable<void> listener() {
//     try {
//         auto ex = co_await boost::asio::this_coro::executor;
//         // Explicit open/bind/listen with reuse_address and loopback endpoint
//
//         boost::asio::ip::tcp::acceptor acceptor(ex);
//         acceptor.open(boost::asio::ip::tcp::v4());
//         acceptor.set_option(boost::asio::socket_base::reuse_address(true));
//         acceptor.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), PORT));
//         acceptor.listen();
//
//         logger->info(std::format("qDNS listening on port {}", PORT));
//
//         while (true) {
//             boost::asio::ip::tcp::socket socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
//             boost::asio::co_spawn(ex, echo(std::move(socket)), boost::asio::detached);
//         }
//     } catch (const std::exception& e) {
//         logger->error(std::format("listener Exception: %s\n", e.what()));
//     }
// }

boost::asio::awaitable<void> resolve(boost::asio::ip::udp::socket socket) {

}

boost::asio::awaitable<void> listener() {
    try {
        auto ex = co_await boost::asio::this_coro::executor;
        boost::asio::ip::udp::socket socket(ex, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), PORT));

        std::array<std::byte, dns::REQUEST_SIZE> buffer{};
        boost::asio::ip::udp::endpoint sender;

        logger->info(std::format("qDNS listening on port {}", PORT));

        while (true) {
            boost::system::error_code error_code;
            size_t bytes_received = socket.receive_from(boost::asio::buffer(buffer), sender, 0, error_code);

            if (error_code && error_code != boost::asio::error::message_size) {
                logger->error(std::format("Receive error: {}", error_code.message()));
                continue;
            }

            if (bytes_received < dns::HEADER_SIZE) {
                logger->warn(std::format("Malformed request, only received {} bytes", bytes_received));
                continue;
            }

            logger->info(std::format("Received {} bytes from {}", bytes_received, sender.address().to_string()));

            dns::request request = dns::request::from(buffer);

            logger->info(std::format("Request - {}", request.to_string()));
        }
    } catch (const std::exception& e) {
        logger->error(std::format("listener Exception: %s\n", e.what()));
    }
}

int main() {
    logger->info("Starting qDNS...");

    try {
        boost::asio::io_context io(MAX_THREADS);

        auto work = boost::asio::make_work_guard(io);

        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ io.stop(); });

        boost::asio::co_spawn(io, listener(),
            [&](const std::exception_ptr& ep) {
                if (ep) {
                    try { std::rethrow_exception(ep); }
                    catch (const std::exception& e) {
                        logger->error(std::format("co_spawn completion caught: %s\n", e.what()));
                    }
                }
            });

        io.run();
    } catch (const std::exception& e) {
        logger->error(std::format("Exception: %s\n", e.what()));
    }

    logger->info("qDNS stopped");
}
