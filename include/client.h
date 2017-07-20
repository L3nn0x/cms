#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

#include "database.h"
#include "input_parser.h"

class Server;

using boost::asio::ip::tcp;

class Client : public boost::enable_shared_from_this<Client> {
public:
	typedef boost::shared_ptr<Client> ptr;

	static ptr create(Server *server, Database &db, boost::asio::io_service& io_service) {
		return ptr(new Client(server, db, io_service));
	}

	tcp::socket& socket() {
		return socket_;
	}

	void start();

	void handle_read(const boost::system::error_code& error, size_t len);

	void handle_write(const boost::system::error_code& error);

private:
	Client(Server *server, Database &db, boost::asio::io_service& io_service) : server(server), db(db), socket_(io_service) {}

	Server *server;
	boost::asio::streambuf buf;
	std::string output;
	Database &db;
	tcp::socket socket_;
};
