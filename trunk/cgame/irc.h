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


#pragma once //This is Windows-only, so what the hell...

#include "irc_connection.h"
#include "q_shared.h"

#define EEZ_NUM_QUEUES	64
#define EEZ_QUEUE_SIZE	512
#define MAX_CONTINUOUS_MESSAGES	10

typedef struct
{
	char nick[32];
	char user[32];
	char host[64];
	char chan[32];
	char message[512];
	int is_ready;
} IRCInfo_t;

unsigned long LaunchBotProcess;
unsigned long CMDProcess;
qboolean KeepRunning;
qboolean IRCRunning;
char ThreadActionQueue[EEZ_NUM_QUEUES][EEZ_QUEUE_SIZE];
char CommandQueue[EEZ_NUM_QUEUES][EEZ_QUEUE_SIZE];

int CommandTotal;

//Thread Handles
void *NewThreadHandle;
void *CommandThreadHandler;

SOCKET MainSocketHandle;

void IRC_Initiate(void);
void ParseActionQueue(void);
void AddCommandToQueue(char *command, char *message);

void PerformActionForIRCEvent(char *buffer, SOCKET socket);

char Nickname[32];

#define Com_TPrintf(x)	AddCommandToQueue("Com_Printf", x)
#define BOT_BUFSIZE		5096

#define JAIRC_VERSION "Alpha 1.001"

typedef struct
{
	// Internal data
	char data[512];			// use command and argv to access

	// Prefix
	int hasPrefix;			// 1 if prefix data is available
	int isClient;			// 1 if the data is from a client (user and nick are set), server host otherwise
	char user[16];			// username of the client (only if isClient == 1)
	char nick[64];			// nickname of the client (only if isClient == 1)
	char host[256];			// hostname of the client/server (depending on isClient)
	
	// Command information
	int cmdNum;				// If zero, assume text command
	const char *command;	// IRC command
	
	// Arguments
	unsigned int argc;		// Argument count (0-15)
	const char *argv[15];	// Arguments
} IrcCommand_t;

enum IrcParseErrorCodes
{
	ERR_OK = 0,
	ERR_BADLEN,
	ERR_BADARGS,
	ERR_NULLMESSAGE,
	ERR_INVALIDMESSAGE,
};


int ParseIRCMessage(const char *message, IrcCommand_t *cmd);
qboolean GrabParsingLine(char *line, char *buffer, SOCKET socket);