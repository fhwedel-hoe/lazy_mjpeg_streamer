#pragma once

#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include <boost/asio.hpp>

#include "publisher.hpp"
#include "placeholder.hpp"

using boost::asio::ip::tcp;

const int http_header_line_max_length = 512; // needs to be big enough to store i.e. the user agent string
const unsigned short server_port = 8080;
const std::string http_request_line = "POST / HTTP/1.1";
const std::string delimiter = "\r\n";

class http_request_error : public std::runtime_error {
  public:
    http_request_error() : std::runtime_error("http_request_error") {};
};
class unsupported_request_line_error : public http_request_error {};
class overlong_request_line_error : public http_request_error {};

std::string read_line_from_buffered_socket(
  tcp::socket & socket, 
  boost::asio::streambuf & buffer
) {
  try {
    std::size_t length = boost::asio::read_until(socket, buffer, delimiter);
    std::string line = std::string(
      boost::asio::buffers_begin(buffer.data()), 
      boost::asio::buffers_begin(buffer.data()) + length - delimiter.length()
    );
    buffer.consume(length);
    return line;
  } catch (boost::system::system_error & bsse){
    if (bsse.code() == boost::asio::error::misc_errors::not_found) {
      throw overlong_request_line_error();
    } else {
      throw;
    }
  }
}

class StreamWriter {
  IPC_globals & ipc;
  public:
    StreamWriter(IPC_globals & ipc) : ipc(ipc) {
      ipc.readers.update([](unsigned int & i){i++;});
    }
    ~StreamWriter() {
      ipc.readers.update([](unsigned int & i){i--;});
    }
    size_t send(tcp::socket & socket, const std::vector<unsigned char> & d) {
      std::ostringstream answer; answer <<
        "--BOUNDARY\r\n" << 
        "Content-Type: image/jpeg\r\n" <<
        "Content-Length: " << d.size() << "\r\n" <<
        "\r\n";
      size_t sent = 0;
      sent += boost::asio::write(socket, boost::asio::buffer(answer.str()));
      sent += boost::asio::write(socket, boost::asio::buffer(d.data(), d.size()));
      //std::cerr << "Sent " << sent << " bytes of data (" << d.size() << " bytes of user-data)." << std::endl;
      return sent;
    }
    void stream(tcp::socket & socket) {
      this->send(socket, placeholder);
      for (;;) {
        this->send(socket, ipc.data.read());
      }
    }
};

void session(tcp::socket socket, IPC_globals & ipc) {
  
  boost::asio::streambuf buffer(http_header_line_max_length);
  std::string answer("HTTP/1.1 500 Internal Server Error\r\nConnection: Closed\r\n\r\n");
  try {
    
    std::string request_header = read_line_from_buffered_socket(socket, buffer);
    if (request_header == "GET / HTTP/1.1") {
      answer = "HTTP/1.1 200 OK\r\n" \
      "Content-Type: multipart/x-mixed-replace;boundary=BOUNDARY\r\n" \
      "\r\n";
      boost::asio::write(socket, boost::asio::buffer(answer));
      StreamWriter(ipc).stream(socket);
    } else {
      throw unsupported_request_line_error();
    }
    
  } catch (unsupported_request_line_error & urle) {
    answer = "HTTP/1.1 400 Bad Request\r\nConnection: Closed\r\n\r\n";
  } catch (boost::system::system_error & bsse) {
    if (bsse.code() == boost::asio::error::eof) {
      // silently ignore client not sending
    } else if (bsse.code() == boost::asio::error::broken_pipe) {
      // silently ignore client disconnecting
    } else if (bsse.code() == boost::asio::error::connection_reset) {
      // silently ignore client disconnecting
    } else {
      std::cerr << "Unexpected boost system exception: " << bsse.what() << "\n";
    }
  } catch (std::exception & se) {
    std::cerr << "Unexpected exception: " << se.what() << "\n";
  }
  try {
    boost::asio::write(socket, boost::asio::buffer(answer));
  } catch (boost::system::system_error & bsse) {
    if (bsse.code() == boost::asio::error::broken_pipe) {
      // silently ignore client disconnecting
    } else {
      std::cerr << "Unexpected boost system exception while sending answer: " << bsse.what() << "\n";
    }
  } catch (std::exception& se) {
    std::cerr << "Unexpected exception while sending answer: " << se.what() << "\n";
  }
  std::cerr << "Session ended. Readers remaining: " << ipc.readers.read_unsafe() << "\n";
}

void server(boost::asio::io_service& io_service, unsigned short port, IPC_globals & ipc) {
  //tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v6(), port));
  //tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string("::1"), port));
  tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
  std::cerr << "HTTP server listening on " <<
    acceptor.local_endpoint().address().to_string() << ":"
    << acceptor.local_endpoint().port() << "..." << std::endl;
  for (;;) {
    tcp::socket sock(io_service);
    acceptor.accept(sock);
    std::thread(session, std::move(sock), std::ref(ipc)).detach();
  }
}

void serve(IPC_globals & ipc) {
  try {
    boost::asio::io_service io_service;
    server(io_service, server_port, ipc);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
