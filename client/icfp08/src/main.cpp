#include <string>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include "socket.h"
#include "protocol.h"
#include "movement.h"
#include "pathfind.h"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "usage: <hostname> <port>" << std::endl;
		fprintf(stderr, "usage: <hostname> <port>\n");
		exit(1);
	}

	using namespace Communication;
	using namespace Movement;

	Socket sock(argv[1], atoi(argv[2]));
	ProtocolStream proto_stream(sock);
	ProtocolParser proto_parser;
	Controller controller(&proto_stream);
	PathFind path_finder(&controller);

	if (!sock.connect())
		return 0;
	std::cout << "connected to " << sock.hostname() << ":" << sock.port() << std::endl;
	sock.setBlocking(false);

	Protocol::Message message;
	std::string command;

	while (true) {
		proto_stream.poll();
		while (proto_stream.get(command)) {
			proto_parser.parse(message, command);
			controller.state().update(message);
			message.clear();
			sleep(0); // yield
		}
	}
	sock.disconnect();
	std::cout << std::endl << "done" << std::endl;

	return 0;
}
