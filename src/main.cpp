#include <iostream>

#include "database.h"
#include "server.h"

int main(int argc, char **argv) {
	Database db;
	if (argc != 2 && argc != 3) {
		std::cout << "Usage: " << argv[0] << " base\n\t"
			<< argv[0] << " ext1 <portNo>\n\t"
			<< argv[0] << " ext2 <portNo>" << std::endl;
		return -1;
	}
	if (argc == 2 && !std::strcmp(argv[1], "base")) {
		while (true) {
			std::string in;
			std::getline(std::cin, in);
			if (!std::cin)
				return 0;
			if (in.size() > 255)
				std::cout << "INVALID_MESSAGE" << std::endl;
			std::cout << execute(db, in);
		}
		return 0;
	}
	else if (argc == 3 && !std::strcmp(argv[1], "ext1")) {
		boost::asio::io_service io_service;
		tcp::endpoint endpoint(tcp::v4(), atoi(argv[2]));
		Server server(db, io_service, endpoint, true);
		io_service.run();
		return 1;
	}
	else if (argc == 3 && !std::strcmp(argv[1], "ext2")) {
		boost::asio::io_service io_service;
		tcp::endpoint endpoint(tcp::v4(), atoi(argv[2]));
		Server server(db, io_service, endpoint);
		io_service.run();
		return 2;
	}
	return -1;
}
