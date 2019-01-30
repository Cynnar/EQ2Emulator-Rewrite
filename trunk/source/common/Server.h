#pragma once

#include <map>
#include <string>
#include "Stream.h"

class Server {
public:

	Server();
	~Server();

	bool Open(bool tcp);
	bool Open(unsigned int port, bool tcp = false) { Port = port; return Open(tcp); }

	bool Process();
	int GetSocket() { return Sock; }

	void StreamDisconnected(Stream* stream);
	
protected:
	virtual Stream* GetNewStream(unsigned int ip, unsigned short port);

	std::map<std::string, Stream*> Streams;

private:
	int Sock;
	unsigned int Port;
	static unsigned int InitializeCount;

};