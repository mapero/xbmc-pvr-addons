#include "HttpPostClient.h"

#ifdef _WIN32
#include <Winsock2.h>
#else
 #include <netdb.h>
#endif



#define SEND_RQ(MSG) \
	/*cout<<send_str;*/ \
	send(sock,MSG,strlen(MSG),0);


/* Converts a hex character to its integer value */
char from_hex(char ch) {
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(const char *str) {
	char *pstr = (char*) str, *buf = (char *)malloc(strlen(str) * 3 + 1), *pbuf = buf;
	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
			*pbuf++ = *pstr;
		else if (*pstr == ' ') 
			*pbuf++ = '+';
		else 
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_decode(const char *str) {
	char *pstr = (char *)str, *buf = (char*)malloc(strlen(str) + 1), *pbuf = buf;
	while (*pstr) {
		if (*pstr == '%') {
			if (pstr[1] && pstr[2]) {
				*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
				pstr += 2;
			}
		} else if (*pstr == '+') { 
			*pbuf++ = ' ';
		} else {
			*pbuf++ = *pstr;
		}
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}


HttpPostClient::HttpPostClient(CHelper_libXBMC_addon  *XBMC, const std::string& server, const int serverport)
{
	this->XBMC = XBMC;
	this->server = server;
	this->serverport = serverport;
}

int HttpPostClient::SendPostRequest(HttpWebRequest& request)
{
	std::string buffer;
	std::string message;

	buffer.append("POST /cs/ HTTP/1.0\r\n");
	buffer.append("Host: 10.0.0.150\r\n"); //TODO: PAE: Change ip
	buffer.append("Content-Type: application/x-www-form-urlencoded\r\n");
	char content_header[100];
	sprintf(content_header,"Content-Length: %d\r\n",request.ContentLength);
	buffer.append(content_header);
	buffer.append("\r\n");
	buffer.append(request.GetRequestData());


#ifdef _WIN32
	{
		WSADATA	WsaData;
		WSAStartup (0x0101, &WsaData);
	}
#endif

	sockaddr_in       sin;
	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		return -100;
	}
	sin.sin_family = AF_INET;
	sin.sin_port = htons( (unsigned short)serverport ); 

	struct hostent * host_addr = gethostbyname(server.c_str());
	if(host_addr==NULL) {
		return -103;
	}
	sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list) ;

	if( connect (sock,(const struct sockaddr *)&sin, sizeof(sockaddr_in) ) == -1 ) {
		return -101;
	}

	SEND_RQ(buffer.c_str());

	char c1[1];
	int l,line_length;
	bool loop = true;
	bool bHeader = false;


	while(loop) {
		l = recv(sock, c1, 1, 0);
		if(l<0) loop = false;
		if(c1[0]=='\n') {
			if(line_length == 0) loop = false;

			line_length = 0;
			
			if(message.find("200") != 0)
				bHeader = true;

		}
		else if(c1[0]!='\r') line_length++;
		message += c1[0];
	}

	message="";
	if(bHeader) {
		char p[1024];
		while((l = recv(sock,p,1023,0)) > 0)  {
			p[l] = '\0';
			message += p;
		}
	} else {
		return -102;
	}




	//If you need to send a basic authorization
	//string Auth        = "username:password";
	//Figureout a way to encode test into base64 !
	//string AuthInfo    = base64_encode(reinterpret_cast<const unsigned char*>(Auth.c_str()),Auth.length());
	//string sPassReq    = "Authorization: Basic " + AuthInfo;
	//SEND_RQ(sPassReq.c_str());


	//TODO: Use xbmc file code when it allows to post content-type application/x-www-form-urlencoded
	/*
	void* hFile = XBMC->OpenFileForWrite(request.GetUrl().c_str(), 0);
	if (hFile != NULL)
	{

		int rc = XBMC->WriteFile(hFile, buffer.c_str(), buffer.length());
		if (rc >= 0)
		{
			std::string result;
			result.clear();
			char buffer[1024];
			while (XBMC->ReadFileString(hFile, buffer, 1023))
				result.append(buffer);

		}
		else
		{
			XBMC->Log(LOG_ERROR, "can not write to %s",request.GetUrl().c_str());
		}

		XBMC->CloseFile(hFile);
	}
	else
	{
		XBMC->Log(LOG_ERROR, "can not open %s for write", request.GetUrl().c_str());
	}

	*/
	responseData.clear();
	responseData.append(message);
	return 200;
}

bool HttpPostClient::SendRequest(HttpWebRequest& request)
{
	lastReqeuestErrorCode = SendPostRequest(request);
	return (lastReqeuestErrorCode == 200);
}

HttpWebResponse* HttpPostClient::GetResponse()
{
	if (lastReqeuestErrorCode != 200){
		return NULL;
	}
	HttpWebResponse* response = new HttpWebResponse(200, responseData);
	return response;
}

void HttpPostClient::GetLastError(std::string& err)
{

}

void HttpPostClient::UrlEncode(const std::string& str, std::string& outEncodedStr)
{
	outEncodedStr.append(url_encode(str.c_str()));
}

