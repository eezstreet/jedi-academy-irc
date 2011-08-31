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

#pragma once

//Function header for IRC commands in JAIRC
//by eezstreet

#include "irc.h"
#include "irc_connection.h"
#include "irc_strings.h"

void NoticeCMD(IrcCommand_t *cmd);
void PrivateMessageCMD(SOCKET socket, IrcCommand_t *cmd);
void StatusReceiveCMD(IrcCommand_t *cmd);
void QuitCMD(IrcCommand_t *cmd);
void PartCMD(IrcCommand_t *cmd);
void ModeCMD(IrcCommand_t *cmd);
void KickCMD(IrcCommand_t *cmd);

int WINAPI LaunchBotCommandHandler(SOCKET *socket);
void ExecuteCommandInIRCQueue(int number, SOCKET socket);

void AddCommandToIRCQueue(char *command, const char *arg);

#define IRC_Say(x) AddCommandToIRCQueue("say", x)
#define IRC_Quit(x) AddCommandToIRCQueue("quit", x)
#define IRC_Join(x) AddCommandToIRCQueue("join", x)
#define IRC_Leave(x) AddCommandToIRCQueue("leave", x)
#define IRC_Stats(x) AddCommandToIRCQueue("stats", x)
#define IRC_List(x) AddCommandToIRCQueue("list", x)
#define IRC_Nick(x) AddCommandToIRCQueue("nick", x)