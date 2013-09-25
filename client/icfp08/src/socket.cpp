#include "socket.h"
#include <cerrno>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>

namespace Communication {

	bool Socket::lookupHost(const std::string& hostname, struct in_addr * ipaddr) const {
		const char *phost = hostname.c_str();
		struct hostent * he;
		if (!inet_aton(phost, ipaddr)) {
			if ((he = gethostbyname(phost)) == NULL) {
				perror("gethostbyname");
				return false;
			}
			*ipaddr = *(struct in_addr *)he->h_addr_list[0];
		}
		return true;
	}

	bool Socket::connect() {
		struct sockaddr_in host_addr;
		if (!lookupHost(_hostname, &host_addr.sin_addr))
			return false;

		_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_fd == -1) {
			perror("socket");
			return false;
		}

		host_addr.sin_family = AF_INET;
		host_addr.sin_port = htons(_port);
		memset(&host_addr.sin_zero, 0, 8);

		if (::connect(_fd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1) {
			perror("connect");
			return false;
		}

//		int flag = 1;
//		int result = setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
//		if (result < 0) {
//			perror("setsockopt");
//			// TODO must return false?
//		}

		return true;
	}

	bool Socket::disconnect() {
		if (close(_fd) == -1) {
			perror("close");
			return false;
		}
		_fd = -1;
		return true;
	}

	int Socket::read(void * data, size_t size) {
		int ret = recv(_fd, data, size, 0);
		if (ret == -1) {
			if (errno != EAGAIN)
				perror("recv");
		}
		return ret;
	}

	int Socket::write(const void * data, size_t size) {
		int ret = send(_fd, data, size, 0);
		if (ret == -1) {
			perror("send");
		}
		return ret;
	}

	bool Socket::setBlocking(bool blocking) {
		long flags = fcntl(_fd, F_GETFL, NULL);
		if (flags < 0) {
			perror("fcntl");
			return false;
		}
		if (blocking)
			flags &= ~O_NONBLOCK;
		else
			flags |= O_NONBLOCK;
		if (fcntl(_fd, F_SETFL, flags) < 0) {
			perror("fcntl");
			return false;
		}
		return true;
	}

}
