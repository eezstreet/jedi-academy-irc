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

#include "irc_commands.h"
#include "cg_local.h"
#include "q_shared.h"


//==========================================================
// RECEIVED COMMANDS
//==========================================================


void NoticeCMD(IrcCommand_t *cmd)
{

	if(!Q_stricmp(cmd->argv[0], "AUTH"))
	{
		Com_TPrintf(va("^2%s\n", cmd->argv[1]));
	}
}

void PrivateMessageCMD(SOCKET socket, IrcCommand_t *cmd)
{

	if(_strnicmp(cmd->argv[1], "\001PING", 5) == 0)
	{
		send(socket, va("NOTICE %s :\001PING %s\r\n", cmd->nick, cmd->argv[1]+7), strlen(va("NOTICE %s :\001PING %s\r\n", cmd->nick, cmd->argv[1]+7)), 0);
		Com_TPrintf(va("^2*** CTCP PING from %s\n", cmd->nick));
		return;
	} else if(_strnicmp(cmd->argv[1], "\001ACTION", 7) == 0)
	{
		Com_TPrintf(va("^3<^7%s^3>^5 ** ^4%s %s ^5**\n", cmd->argv[0], cmd->nick, cmd->argv[1]+7));
		return;
	}

	Com_TPrintf(va("^4<^7%s^4> ^3<^7%s^3> ^7%s\n", cmd->nick, cmd->argv[0], cmd->argv[1]));
}

void StatusReceiveCMD(IrcCommand_t *cmd)
{
	Com_TPrintf(va("%s (%i)\n", cmd->argv[0], cmd->cmdNum));
}

void QuitCMD(IrcCommand_t *cmd)
{

	Com_TPrintf(va("^3%s has left ArloriaNet (%s)\n", cmd->nick, cmd->argv[0]));
}

void PartCMD(IrcCommand_t *cmd)
{
	Com_TPrintf(va("^5%s has left %s\n", cmd->nick, cmd->argv[0]));
}

void ModeCMD(IrcCommand_t *cmd)
{
	Com_TPrintf(va("^2%s set mode %s\n", cmd->nick, cmd->argv[0]));
}

void KickCMD(IrcCommand_t *cmd)
{

	Com_TPrintf(va("^6%s KICKED %s from %s\n", cmd->nick, cmd->argv[1], cmd->argv[0]));
}

//=======================================================================
//Sent commands
//=======================================================================
//Thread Handler for command sending

void ExecuteCommandInIRCQueue(int number, SOCKET socket)
{
	if(_strnicmp(CommandQueue[number], "SC:", 3) == 0) {
		send(socket, va("PRIVMSG %s :%s\r\n", cg_activeIRCChan.string, CommandQueue[number]+3), strlen(va("PRIVMSG %s :%s\r\n", cg_activeIRCChan.string, CommandQueue[number]+3)), 0);
		Com_TPrintf(va("^2{^7%s^2} ^7%s\n", Nickname, CommandQueue[number]+3));
		CommandQueue[number][0] = '\0';
	} else if(_strnicmp(CommandQueue[number], "Q:", 2) == 0) {
		send(socket, va("QUIT :%s\r\n", CommandQueue[number]+2), strlen(va("QUIT :%s\r\n", CommandQueue[number]+2)), 0);
		Com_TPrintf("^3You have quit IRC.\n");
		CommandQueue[number][0] = '\0';
	} else if(_strnicmp(CommandQueue[number], "J:", 2) == 0) {
		char channelString[32];
		StopAtSpace(CommandQueue[number]+2, channelString);
		send(socket, va("JOIN %s\r\n", CommandQueue[number]+2), strlen(va("JOIN %s\r\n", CommandQueue[number]+2)), 0);
		Com_TPrintf(va("^5You have joined %s.\n", channelString));
		CommandQueue[number][0] = '\0';
	} else if(_strnicmp(CommandQueue[number], "L:", 2) == 0) {
		send(socket, va("PART %s %s\r\n", cg_activeIRCChan.string, CommandQueue[number]+2), strlen(va("PART %s\r\n", CommandQueue[number]+2)), 0);
		Com_TPrintf(va("^5You have left %s.\n", CommandQueue[number]+2));
		CommandQueue[number][0] = '\0';
	} else if(_strnicmp(CommandQueue[number], "S:", 2) == 0) {
		send(socket, va("STATS %s\r\n", CommandQueue[number]+2), strlen(va("STATS %s\r\n", CommandQueue[number]+2)), 0);
		Com_TPrintf("^2*** Stat Request\n");
		CommandQueue[number][0] = '\0';
	} else if(_strnicmp(CommandQueue[number], "N:", 2) == 0) {
		send(socket, va("NAMES %s\r\n", CommandQueue[number]+2), strlen(va("NAMES %s\r\n", CommandQueue[number]+2)), 0);
		Com_TPrintf(va("^2*** Names Request (%s)", CommandQueue[number]+2));
		CommandQueue[number][0] = '\0';
	} else if(_strnicmp(CommandQueue[number], "LI:", 3) == 0) {
		send(socket, va("LIST %s\r\n", CommandQueue[number]+3), strlen(va("LIST %s\r\n", CommandQueue[number]+3)), 0);
		Com_TPrintf(va("^2*** Listing %s\n", strlen(CommandQueue[number]+3) > 0 ? "all users in all channels" : va("users in channels %s", CommandQueue[number]+3)));
		CommandQueue[number][0] = '\0';
	} else if(_strnicmp(CommandQueue[number], "NC:", 3) == 0) {
		send(socket, va("NICK %s\r\n", CommandQueue[number]+3), strlen(va("LIST %s\r\n", CommandQueue[number]+3)), 0);
		Com_TPrintf(va("^5You are now known as %s\n", CommandQueue[number]+3));
		Q_strncpyz(Nickname, CommandQueue[number]+3, (int)sizeof(Nickname));
		CommandQueue[number][0] = '\0';
	}
}

int FirstUnusedIRCQueue()
{
	int i;
	for(i = 0; i < EEZ_NUM_QUEUES; i++)
	{
		if(CommandQueue[i][0] == 0)
			return i;
	}
	return -1;
}

void AddCommandToIRCQueue(char *command, const char *arg)
{
	int queueNum;

	if((queueNum=FirstUnusedIRCQueue()) == -1)
		return;

	if(!Q_stricmp(command, "say")) {
		sprintf(CommandQueue[queueNum], "SC:%s", arg);
	} else if(!Q_stricmp(command, "quit")) {
		sprintf(CommandQueue[queueNum], "Q:%s", arg);
	} else if(!Q_stricmp(command, "join")) {
		sprintf(CommandQueue[queueNum], "J:%s", arg);
	} else if(!Q_stricmp(command, "leave")) {
		sprintf(CommandQueue[queueNum], "L:%s", arg);
	} else if(!Q_stricmp(command, "stats")) {
		sprintf(CommandQueue[queueNum], "S:%s", arg);
	} else if(!Q_stricmp(command, "list")) {
		sprintf(CommandQueue[queueNum], "LI:%s", arg);
	} else if(!Q_stricmp(command, "nick")) {
		sprintf(CommandQueue[queueNum], "NC:%s", arg);
	}
}

int WINAPI LaunchBotCommandHandler(SOCKET *socket)
{
	int i;
	Com_TPrintf("Command Handler Thread successfully created.\n");
	while(KeepRunning)
	{
		for(i = 0; i < EEZ_NUM_QUEUES; i++)
		{
			if(CommandQueue[i][0] != 0)
			{
				ExecuteCommandInIRCQueue(i, *socket);
			}
		}
		Sleep(50);
	}
	return 0;
}
