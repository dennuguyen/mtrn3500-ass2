#ifndef TCP_HPP_
#define TCP_HPP_

#include <string>
#include <Windows.h>

namespace tcp {
	class TCPClient {
	public:
		TCPClient();
		~TCPClient();
		bool connect();

	private:
		int socket;
		std::string address;
		int port;
	};
}  // namespace tcp

#endif // TCP_HPP_