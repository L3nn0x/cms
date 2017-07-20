#pragma once

#include <boost/asio.hpp>

#include "client.h"

using boost::asio::ip::tcp;

class Server {
public:
	Server(Database &db, boost::asio::io_service& io_service, const tcp::endpoint& endpoint, bool oneConnection=false)
		: db(db), io_service(io_service), acceptor(io_service, endpoint), oneConnection(oneConnection), nbClients(0) {
		start_accept();
	}

	void start_accept() {
		Client::ptr new_client = Client::create(this, db, io_service);
		acceptor.async_accept(new_client->socket(),
							  boost::bind(&Server::handle_accept, this, new_client, boost::asio::placeholders::error));
	}

	void handle_accept(Client::ptr client, const boost::system::error_code& error) {
		if (!error) {
			++nbClients;
			client->start();
		}
		if (!nbClients || !oneConnection)
			start_accept();
	}

	void disconnecting() {
		--nbClients;
		if (!nbClients)
			io_service.stop();
	}

private:
	Database &db;
	boost::asio::io_service& io_service;
	tcp::acceptor acceptor;
	bool oneConnection;
	size_t nbClients;
};