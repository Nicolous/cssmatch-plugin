#include "UpdateNotifier.h"
#include "ServerPlugin.h"

#include <sstream>

using namespace cssmatch;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::getline;

UpdateNotifier::UpdateNotifier() : alive(true), version(CSSMATCH_VERSION)
{
#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		throw UpdateNotifierException("WSAStartup failed");
	}
#endif // _WIN32
}

UpdateNotifier::~UpdateNotifier()
{
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32
}

void UpdateNotifier::query(const SOCKADDR_IN & serv, const SOCKET & socketfd, const string & hostname)
{
	// Connect to the host
	if (connect(socketfd,(SOCKADDR *)&serv,sizeof(serv)) == SOCKET_ERROR)
	{
		CSSMATCH_PRINT("connect() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
		return;
	}
	
	// Query the file
	ostringstream requestbuf;
	requestbuf	<< "GET " CSSMATCH_VERSION_FILE " HTTP/1.1\r\n"
				   "Host: " << hostname << "\r\n"
				   "Connection: Close\r\n"
				   "\r\n";
	const string & tempRequest = requestbuf.str();
	const char * request = tempRequest.c_str();
	if (send(socketfd,request,strlen(request),0) == SOCKET_ERROR)
	{
		CSSMATCH_PRINT("send() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
		return;
	}

	// Wait for a response
	fd_set readfs;
	FD_ZERO(&readfs);
	FD_SET(socketfd, &readfs);
	timeval timeout = {30L,0L}; // 30 sec + 0 microsec
	int ready = select(socketfd + 1,&readfs,NULL,NULL,&timeout);
	switch(ready)
	{
	case SOCKET_ERROR:
		CSSMATCH_PRINT("select() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
		break;

	case 0: // timeout expired
		CSSMATCH_PRINT(hostname + " timeout");
		break;

	default:
		if(FD_ISSET(socketfd,&readfs))
		{
			char buffer[300];
			int received = recv(socketfd,buffer,sizeof(buffer)-1,0);
			if (received == SOCKET_ERROR)
			{
				CSSMATCH_PRINT("recv() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
				return;
			}
			
			buffer[received] = '\0';
			istringstream stream(buffer);

			// Check for the success
			string temp;
			stream >> temp; // HTTP/x.x
			stream >> temp; // status  code
			if (temp == "200") // 200 OK
			{
				while(getline(stream,temp)){} // get the last line

				// Update the last version name found
				mutex.Lock();
				version = temp;
				mutex.Unlock();
			}
#ifdef _DEBUG
			else
			{
				CSSMATCH_PRINT("Error code " + temp);
			}
#endif // _DEBUG
		}
	}
}

int UpdateNotifier::Run()
{
    wake.Wait(1);

	while(alive)
	{
		wake.Reset();

		SOCKADDR_IN serv;
		memset(&serv,0,sizeof(serv));

		const char * hostname = ServerPlugin::getInstance()->getConVar("cssmatch_updatesite")->GetString();

		hostent * host = gethostbyname(hostname);
		if (host == NULL)
		{
			CSSMATCH_PRINT("gethostbyname() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
			continue;
		}

		memcpy(&serv.sin_addr,host->h_addr,host->h_length);
	    
		serv.sin_port = htons(80);
		serv.sin_family = AF_INET;
	    
		SOCKET socketfd = socket(AF_INET,SOCK_STREAM,0);
		if (socketfd == INVALID_SOCKET)
		{
			CSSMATCH_PRINT("socket() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
			continue;
		}
		
		query(serv,socketfd,hostname);

		shutdown(socketfd,SD_BOTH);

#ifdef _DEBUG
		wake.Wait(1*60*1000);
#else
		wake.Wait(24*60*60*1000); // 24h, so open/shutdown the connection each iteration
#endif // _DEBUG
	}

	return 0;
}

void UpdateNotifier::End()
{
	alive = false;
	wake.Set();
}

std::string UpdateNotifier::getLastVer() const
{
	string result;
	
	mutex.Lock();
	result = version;
	mutex.Unlock();

	return result;
}
