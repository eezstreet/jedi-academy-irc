//Copyright (c) 2011 eezstreet
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of 
//this software and associated documentation files (the "Software"), to deal in the 
//Software without restriction, including without limitation the rights to use, copy, 
//modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
//and to permit persons to whom the Software is furnished to do so, subject to the 
//following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies 
//or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
//FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "irc_connection.h"
#include "irc_strings.h"
#include "irc.h"
#include "irc_commands.h"

#pragma comment(lib, "ws2_32.lib")

int WINAPI LaunchBot(void *dummy);
void PerformActionForIRCEvent(char *buffer, SOCKET socket);
void PongHost(SOCKET socket, char *sendBack);
void StripNick(char *nickString, char *buffer);

void IRC_Initiate(void)
{
	Com_Printf("^6Initating IRC...\n");
	//POSIX _beginthread. Do not want.
	//_beginthread(LaunchBot, 0, NULL);
	NewThreadHandle = CreateThread(NULL, 0, LaunchBot, NULL, 0, &LaunchBotProcess);
}

//--------------------------------------
// NEW THREAD
//--------------------------------------
int WINAPI LaunchBot(void *dummy)
{
	struct addrinfo hints, *serv;
	struct sockaddr_in *sip;
	int status, receivecount;
	SOCKET sfd;
	//char ip[INET_ADDRSTRLEN];
	char *ip = malloc(INET_ADDRSTRLEN);
	char buf[BOT_BUFSIZE];
	IRCInfo_t *irc = malloc(sizeof(IRCInfo_t));
	const char *host = "aurora.irc.arloria.net";
	const char *port = "6667";
	const char *chan = "#JA-Global";
	const char *user = "JAIRC";
	const char *nick = "Padawan";

	IRCRunning = qtrue;

	AddCommandToQueue("Com_Printf", "New thread created successfully.\n");
	strcpy(Nickname, nick);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(host, port, &hints, &serv);
	if(status < 0)
		//Com_Printf("^1getaddrinfo failed.\n");
		Com_TPrintf("^1getaddrinfo failed.\n");
	sfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol);
	if(sfd < 0)
		//Com_Printf("^1socket failed.\n");
		Com_TPrintf("^1socket failed.\n");
	status = connect(sfd, serv->ai_addr, serv->ai_addrlen);
	if(status < 0)
		//Com_Printf("^1connect failed.\n");
		Com_TPrintf("^1connect failed.\n");

	sip = (struct sockaddr_in*)serv->ai_addr;
	//inet_ntop(serv->ai_family, &sip->sin_addr, ip, sizeof(ip));

	sprintf(buf, "USER %s 8 * :\x50\x73\x79\x63\x68\x6F\x20\x42\x6F\x74\r\nNICK %s\r\nJOIN %s\r\n", user, nick, chan);
	send(sfd, buf, strlen(buf), 0);

	KeepRunning = qtrue;

	CommandTotal = 0;
	buf[0] = '\0';
	CommandThreadHandler = CreateThread(NULL, 0, LaunchBotCommandHandler, &sfd, 0, &CMDProcess);
	while(KeepRunning)
	{
		while((receivecount=recv(sfd, buf, BOT_BUFSIZE-1, 0)) > 0)
		{
			buf[receivecount] = 0;
			PerformActionForIRCEvent(buf, sfd);
		}
		if(receivecount <= 0)
			KeepRunning = qfalse;
	}
	Com_TPrintf("^1Disconnected from IRC.\n");
	WaitForSingleObject(CommandThreadHandler, 5000);
	
	closesocket(sfd);
	free(ip);
	free(irc);
	//POSIX _endthread. Do not want.
	//_endthread();
	IRCRunning = qfalse;
	return 0;
}

void PerformActionForIRCEvent(char *buffer, SOCKET socket)
{
	int j;
	char host[1024];
	char command[1024];
	char param[1024];
	char message[1024];
	char strippedNick[32];

	j = 0;
	if(!buffer)
	{
		return;
	}
	ParseIRCString(buffer, host, command);

	if(atoi(command)) { //Status message, don't even bother picking them out.
		StatusReceiveCMD(buffer, param, message, atoi(command));
	} else if(!Q_stricmp(command, "NOTICE")) {
		NoticeCMD(buffer, param, message);
	} else if(!Q_stricmp(host, "PING")) {
		PongHost(socket, command);
	} else if(!Q_stricmp(command, "JOIN")) {
		//Grab the channel name
		while(1)
		{
			if(buffer[0] == '\0' || buffer[0] == '\r' || buffer[0] == '\n')
				break;
			if(buffer[0] == ':')
			{
				ClipFirstChar(buffer, qtrue);
				continue;
			}
			message[j] = buffer[0];
			j++;
			ClipFirstChar(buffer, qtrue);
		}
		StripNick(host, strippedNick);
		message[j] = '\0';
		Com_TPrintf(va("^5%s has joined %s\n", strippedNick, message));
	} else if(!Q_stricmp(command, "PRIVMSG")) {
		PrivateMessageCMD(socket, buffer, host, param, message);
	} else if(!Q_stricmp(command, "QUIT")) {
		QuitCMD(buffer, host, message);
	} else if(!Q_stricmp(command, "PART")) {
		PartCMD(buffer, host, message);
	} else if(!Q_stricmp(command, "MODE")) {
		ModeCMD(buffer, host, message);
	} else if(!Q_stricmp(command, "KICK")) {
		KickCMD(buffer, host, param, message);
	}
	HandleNextCommand(buffer, socket);
} //<-- stack corruption

void PongHost(SOCKET socket, char *sendBack)
{
	char CleanedString[1024];
	CleanString(sendBack, CleanedString);
	send(socket, va("PONG :%s\r\n", CleanedString), (int)strlen(va("PONG :%s\r\n", CleanedString)), 0);
}
//--------------------------------------
// END NEW THREAD
//--------------------------------------