#pragma once

#include "common.h"
#include <netinet/in.h>
#include <string>

namespace Communication {

	class Socket {
		private:
			std::string _hostname;
			int _port;
			int _fd;
		public:
			Socket(const std::string& hostname, int port) : _hostname(hostname), _port(port) {}
			bool lookupHost(const std::string& host, struct in_addr * ipaddr) const;
			std::string hostname() const { return _hostname; }
			int port() const { return _port; }
			bool connect();
			bool disconnect();
			int read(void * data, std::size_t size);
			int write(const void * data, std::size_t size);
			bool setBlocking(bool blocking);
	};

}



