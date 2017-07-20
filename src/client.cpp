#include "client.h"
#include "server.h"
#include <istream>

void Client::start() {
	buf.prepare(255);
	boost::asio::async_read_until(socket_, buf, '\n',
							boost::bind(&Client::handle_read, shared_from_this(),
										boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Client::handle_read(const boost::system::error_code& error, size_t len) {
	if (!error) {
		if (len > 255)
			output = "INVALID_MESSAGE\n";
		else {
			std::istream is(&buf);
			std::string msg;
			std::getline(is, msg);
			output = execute(db, msg);
		}
		buf.consume(buf.size() + 1);
		boost::asio::async_write(socket_, boost::asio::buffer(output),
								 boost::bind(&Client::handle_write, shared_from_this(),
											 boost::asio::placeholders::error));
	}
	else
		server->disconnecting();
}

void Client::handle_write(const boost::system::error_code& error) {
	if (!error)
		start();
	else
		server->disconnecting();
}