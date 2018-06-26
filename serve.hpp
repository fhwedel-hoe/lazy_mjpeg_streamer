#pragma once

#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include <boost/asio.hpp>

#include "publisher.hpp"

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

void session(tcp::socket socket, int & readers, Publisher<binary_data> & data) {
  readers++;
  boost::asio::streambuf buffer(http_header_line_max_length);
  std::string answer("HTTP/1.1 500 Internal Server Error\r\nConnection: Closed\r\n\r\n");
  try {
    
    std::string request_header = read_line_from_buffered_socket(socket, buffer);
    if (request_header == "GET / HTTP/1.1") {
      std::cerr << "Session begins OK" << "\n";
      answer = "HTTP/1.1 200 OK\r\n" \
      "Content-Type: multipart/x-mixed-replace;boundary=BOUNDARY\r\n" \
      "\r\n";
      boost::asio::write(socket, boost::asio::buffer(answer));
      for (;;) {
        std::vector<unsigned char> d = data.read();
        std::ostringstream answer; answer <<
          "--BOUNDARY\r\n" << 
          "Content-Type: image/jpeg\r\n" <<
          "Content-Length: " << d.size() << "\r\n" <<
          "\r\n";
        size_t sent = 0;
        sent += boost::asio::write(socket, boost::asio::buffer(answer.str()));
        sent += boost::asio::write(socket, boost::asio::buffer(d.data(), d.size()));
        std::cerr << "Sent " << sent << " bytes of data (" << d.size() << " bytes of user-data)." << std::endl;
      }
    } else {
      //std::cerr << "Unsupported" << "\n";
      throw unsupported_request_line_error();
    }
    
  } catch (unsupported_request_line_error & urle) {
    answer = "HTTP/1.1 400 Bad Request\r\nConnection: Closed\r\n\r\n";
  } catch (boost::system::system_error & bsse) {
    if (bsse.code() == boost::asio::error::eof) {
      // silently ignore premature client disconnects
    } else {
      std::cerr << "Unexpected boost system exception while receiving: " << bsse.what() << "\n";
    }
  } catch (std::exception & se) {
    std::cerr << "Unexpected exception while receiving: " << se.what() << "\n";
  }
  try {
    boost::asio::write(socket, boost::asio::buffer(answer));
  } catch (std::exception& se) {
    std::cerr << "Unexpected exception while sending answer: " << se.what() << "\n";
  }
  readers--;
  std::cerr << "Session ended. Readers remaining: " << readers << "\n";
}

void server(boost::asio::io_service& io_service, unsigned short port, int & readers, Publisher<binary_data> & data) {
  //tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v6(), port));
  //tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string("::1"), port));
  tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
  for (;;) {
    tcp::socket sock(io_service);
    acceptor.accept(sock);
    std::thread(session, std::move(sock), std::ref(readers), std::ref(data)).detach();
  }
}

void serve(int & readers, Publisher<binary_data> & data) {
  try {
    boost::asio::io_service io_service;
    server(io_service, server_port, readers, data);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
