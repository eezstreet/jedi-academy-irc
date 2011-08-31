// IRC message parser
// By BobaFett

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

#include "irc.h"
#include "irc_strings.h"

char lineStorage;

int ParseIRCMessage(const char *message, IrcCommand_t *cmd)
{
	int i;
	const char *ref;
	char *data;

	// Sanity checks
	if (!message || !cmd)
		return ERR_BADARGS;

	memset(cmd, 0, sizeof(IrcCommand_t));

	if (!*message)
		return ERR_NULLMESSAGE;

	i = strlen(message);
	if (i > 512)
		return ERR_BADLEN;

	if (message[i-2] != '\r' && message[i-1] != '\n')
		return ERR_INVALIDMESSAGE;

	// Parse prefix
	if (*message == ':')
	{
		const char *t = ++message;
		cmd->hasPrefix = 1;
		cmd->isClient = 1;	// Assume nickname unless proven otherwise

		// Do forward scan to check for message type
		for (;;)
		{
			char c = *t++;
			if (c == '.') { cmd->isClient = 0; break; }
			if (c == '!') { cmd->isClient = 1; break; }
			if (c == '@') {	cmd->isClient = 1; break; }
			if (c == '\r' || c == '\n' || c == ' ' || c == '\0') break;
		}

		if (cmd->isClient == 0) {
			// Parse server name
			char *w = cmd->host;
			const char *l = cmd->host + sizeof(cmd->host);
			for (;;)
			{
				char c = *message++;
				if (w == l) break;
				if (c == '\r' || c == '\n' || c == '\0') return ERR_INVALIDMESSAGE;
				if (c == ' ') break;

				if (w < l)
					*w++ = c;
			}

		} else {
			// Parse client name
			char *w = cmd->nick;
			const char *l = cmd->nick + sizeof(cmd->nick);
			for (;;)
			{
				char c = *message++;
				if (c == '\r' || c == '\n' || c == '\0') return ERR_INVALIDMESSAGE;
				if (c == '!') { w =  cmd->user; l = cmd->user + sizeof(cmd->user); continue; }
				if (c == '@') { w =  cmd->host; l = cmd->host + sizeof(cmd->host); continue; }
				if (c == ' ') break;

				if (w < l)
					*w++ = c;
			}
		}
	}

	ref = cmd->data;
	data = cmd->data;

	// Parse command

	if (*message >= '0' && *message <= '9')
	{
		// Numerical command
		for (i = 0; i < 3; i++)
		{
			char c = *message++;
			if (c < '0' && c > '9') return ERR_INVALIDMESSAGE;
			*data++ = c;
		}
		message++;
		
		cmd->command = ref;
		cmd->cmdNum = atoi(ref);
	} else {
		for (;;)
		{
			char c = *message++;
			if (c == ' ') break;
			if (c == '\n' || c == '\0') return ERR_INVALIDMESSAGE;
			if (c == '\r') { cmd->command = ref; return ERR_OK; }		
			*data++ = c;
		}
		cmd->command = ref;
	}

	// Parse arguments

	for (i = 0; i < 15; i++) {
		data++; ref = data;

		if (*message == ':') {
			message++;
			for (;;)
			{
				char c = *message++;
				if (c == '\n' || c == '\0') return ERR_INVALIDMESSAGE;
				if (c == '\r') { cmd->argv[cmd->argc++] = ref; return ERR_OK; }		
				*data++ = c;
			}
		} else {
			for (;;)
			{
				char c = *message++;
				if (c == ' ') break;
				if (c == '\n' || c == '\0') return ERR_INVALIDMESSAGE;
				if (c == '\r') { cmd->argv[cmd->argc++] = ref; return ERR_OK; }		
				*data++ = c;
			}
			cmd->argv[cmd->argc++] = ref;
		}
	}

	return ERR_OK;
}

//eezstreet add
qboolean GrabParsingLine(char *line, char *buffer, SOCKET socket)
{
	int i;
	size_t lineSize = sizeof(line);
	for(i = 0;; i++)
	{
		if(line[i] == '\0')
		{
			return qfalse;
		}
		else if(line[i] == '\r')
		{
			lineStorage = line[i+2];
			line[i+2] = '\0';
			break;
		}
	}
	strcpy(line, buffer);
	line[i+2] = lineStorage;
	memmove(line, line+(i+2), lineSize);
	return qtrue;
}

qboolean DoneWithCommand(char *line)
{
	int i;
	for(i = 0;; i++)
	{
		if(line[i] == '\0')
			return qfalse;
		else if(line[i] == '\r')
			break;
	}
	strc
}