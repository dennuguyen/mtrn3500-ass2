#include <string>
#include <Windows.h>

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