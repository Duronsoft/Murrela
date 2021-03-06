#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <libssh2.h>

#pragma comment(lib,"ws2_32.lib")

const wchar_t* GetAppFolderPath()
{
#ifdef _UWP
	return Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data();
#else
	return L"\\";
#endif
}

void ReadFileFromAppFolderW(std::wstring PathToFile, void** result)
{
	FILE* fp;
	_wfopen_s(&fp, (GetAppFolderPath() + PathToFile).c_str(), L"r");
	fseek(fp, SEEK_END, 0);
	long fileSize = ftell(fp);
	fseek(fp, SEEK_SET, 0);
	*result = new wchar_t[fileSize];
	fgetws((wchar_t*)*result, fileSize, fp);
	fclose(fp);
}

#define httpRequest struct http_req
#define httpResponse struct http_res

struct http_req
{
	char* hbStr;
	const char crlf[5] = "\r\n\r\n";
	void* resBox;
	struct sockaddr_in target_addr;
};

struct http_res
{
	char* hbStr;
	const char crlf[5] = "\r\n\r\n";
	void* resBox;
};

struct sockaddr_in initAddr_shd(UINT32 ip, int port)
{
	struct sockaddr_in target_addr;
	target_addr.sin_family = AF_INET;
	target_addr.sin_addr.s_addr = ip;
	target_addr.sin_port = htons(port);

	return target_addr;
}


struct sockaddr_in initAddr(const wchar_t* host, int port)
{
	ADDRINFOW hints;
	PADDRINFOW addr = NULL;
	struct sockaddr_in* result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	if (GetAddrInfoW(host, nullptr, &hints, &addr) == -1)
		exit(-1);

	//		InetPton(AF_INET, host, &addr_p);
	result = (struct sockaddr_in*)addr->ai_addr;
	result->sin_port = htons(port);
	return *result;
}

SOCKET initSocket()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;

	SOCKET socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	return socket_fd;
}

int sockConn(const SOCKET* s_fd, const struct sockaddr_in* target_addr)
{
	return connect(*s_fd, (struct sockaddr*)target_addr, sizeof(*target_addr));
}

SOCKET listenSocket(const SOCKET s_fd, const int port)
{
	struct sockaddr_in srv_addr = initAddr(L"0.0.0.0", port);
	int s_len = sizeof(srv_addr);
	if (bind(s_fd, (struct sockaddr*)&srv_addr, s_len) < 0)
	{
		goto error;
	}

	if (listen(s_fd, 5) < 0)
	{
		goto error;
	}
	return accept(s_fd, (struct sockaddr*)&srv_addr, &s_len);
error:
	return -1;
}

void closeSocket(const SOCKET* s_fd)
{
	closesocket(*s_fd);
	WSACleanup();
}

std::string wctos(const wchar_t* source)
{
	size_t sLen = wcslen(source);
	char* c = new char[sLen * 3 + 1];
	WideCharToMultiByte(CP_UTF8, 0, source, (int)(sLen + 1), c, (int)(sLen * 3 + 1), 0, 0);

	std::string result = c;
	delete[] c;
	return result;
}

wchar_t* ctowc(const char* source)
{
	wchar_t* result;
	int sLen, rLen = MultiByteToWideChar(CP_UTF8, 0, source, sLen = (int)strlen(source), NULL, 0);
	result = new wchar_t[rLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, source, sLen, result, rLen);
	result[rLen] = '\0';
	return result;
}

void SSHConnect(const wchar_t* ip, LONG port, const wchar_t* username, const wchar_t* passwd)
{
	LIBSSH2_SESSION* session;
	//	LIBSSH2_CHANNEL* channel;

	SOCKET s_fd = initSocket();
	auto addr = initAddr(ip, port);
	if (sockConn(&s_fd, &addr) == SOCKET_ERROR)
	{
		goto Error;
	}

	session = libssh2_session_init();

	closeSocket(&s_fd);
	return;
Error:

}