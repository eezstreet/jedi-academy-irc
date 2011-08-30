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

#include "irc_strings.h"
#include "q_shared.h"

void ParseIRCString(char *buffer, char *host, char *command)
{
	int j = 0;

	while(1)
	{
		if(buffer[0] == '\0')
		{
			host[0] = 0;
			return;
		}
		if(buffer[0] == ':')
		{
			ClipFirstChar(buffer, qtrue);
			continue;
		}
		else if(buffer[0] != ' ')
		{
			host[j] = buffer[0];
			j++;
			ClipFirstChar(buffer, qtrue);
		}
		else
			break;
	}
	host[j] = '\0';
	j = 0;
	ClipFirstChar(buffer, qtrue);

	while(1)
	{
		if(buffer[0] == '\0')
			break;
		if(buffer[0] == ' ')
			break;
		if(buffer[0] == ':' && j == 0)
		{
			ClipFirstChar(buffer, qtrue);
			continue;
		}
		command[j] = buffer[0];
		j++;
		ClipFirstChar(buffer, qtrue);
	}
	command[j] = '\0';
	j = 0;
	ClipFirstChar(buffer, qtrue);
}

void ClipFirstChar(char *buffer, qboolean count)
{
	int i = 0;
	while(buffer[i] != 0)
	{
		buffer[i] = buffer[i+1];
		if(count)
			CommandTotal++;
		i++;
	}
	buffer[i] = '\0';
}

void ClipNumberChars(char *buffer, int number, qboolean count)
{
	int i;
	for(i = 0; i < number; i++)
		ClipFirstChar(buffer, count);
}

void ClipCharAtPoint(char *buffer, int place, qboolean count)
{
	int i = place;
	while(buffer[i] != 0)
	{
		buffer[i] = buffer[i+1];
		if(count)
			CommandTotal++;
		i++;
	}
	buffer[i] = '\0';
}

int CountLineEnds(char *string)
{
	int i = 0, count = 0;
	if(!string)
		return 0;
	while(string[i] != '\0')
	{
		if(string[i] == '\r' && string[i+1] == '\n')
			count++;
		i++;
	}
	return count;
}

void StripNick(char *nickString, char *buffer)
{
	int i = 0;
	int j = 0;

	while(nickString[i] != '!' && nickString[i] != '\0')
	{
		buffer[j] = nickString[i];
		i++;
		j++;
	}
	buffer[j] = '\0';
}

void ValidateInput(char *string, char *buffer)
{
	int i = 0;
	strcpy(buffer, string);
	while(buffer[i] != '\0')
	{
		if(buffer[i] == '%')
			ClipCharAtPoint(buffer, i, qfalse);
		i++;
	}
}

void CleanString(char *string, char *cleanstring)
{
	int i = 0;
	while(string[i] != '\n' && string[i] != '\r' && string[i] != '\0')
	{
		cleanstring[i] = string[i];
		i++;
	}
	cleanstring[i] = '\0';
}

void GrabParam(char *buffer, char *param)
{
	int j = 0;
	while(1)
	{
		if(buffer[0] == ' ')
			break;
		param[j] = buffer[0];
		j++;
		ClipFirstChar(buffer, qtrue);
	}
	param[j] = '\0';
	ClipFirstChar(buffer, qtrue);
	ClipFirstChar(buffer, qtrue);
}

void GrabMessage(char *buffer, char *message, qboolean colons)
{
	int j;
	j = 0;
	while(1)
	{
		if(buffer[0] == '\0' || buffer[0] == '\r' || buffer[0] == '\n')
			break;
		if(buffer[0] == ':' && colons)
			ClipFirstChar(buffer, qtrue);
		message[j] = buffer[0];
		j++;
		ClipFirstChar(buffer, qtrue);
	}
	message[j] = '\0';
}

void StopAtSpace(char *string, char *buffer)
{
	int i = 0;
	while(string[i] != ' ' && string[i] != '\0')
	{
		buffer[i] = string[i];
		i++;
	}
	buffer[i] = '\0';
}

int ReceiveMoreStuff(char *buffer, SOCKET socket)
{
	char passthrough[256];
	int bytes;
	bytes = recv(socket, passthrough, sizeof(passthrough)-1, 0);
	if(!bytes)
		return 0;
	passthrough[bytes] = '\0';
	Q_strcat(buffer, BOT_BUFSIZE, passthrough);
	CommandTotal = 0;
	return 1;
}

void HandleNextCommand(char *buffer, SOCKET socket)
{
	while(1)
	{
		if(buffer[0] == '\r' || buffer[0] == '\n')
		{
			if(CountLineEnds(buffer) <= 0)
			{
				if(!ReceiveMoreStuff(buffer, socket))
					return;
			}
			ClipNumberChars(buffer, 2, qtrue);
			PerformActionForIRCEvent(buffer, socket);
		}
		else if(buffer[0] == '\0')
			return;
		else
		{
			ClipFirstChar(buffer, qtrue);
			continue;
		}
	}
	return;
}



/*
============
Q_vsnprintf

vsnprintf portability:

C99 standard: vsnprintf returns the number of characters (excluding the trailing
'\0') which would have been written to the final string if enough space had been available
snprintf and vsnprintf do not write more than size bytes (including the trailing '\0')

win32: _vsnprintf returns the number of characters written, not including the terminating null character,
or a negative value if an output error occurs. If the number of characters to write exceeds count, then count 
characters are written and -1 is returned and no trailing '\0' is added.

Q_vsnprintf: always appends a trailing '\0', returns number of characters written (not including terminal \0)
or returns -1 on failure or if the buffer would be overflowed.
============
*/
/*int Q_vsnprintf( char *dest, int size, const char *fmt, va_list argptr ) {
	int ret;

#ifdef _WIN32
	ret = _vsnprintf( dest, size-1, fmt, argptr );
#else
	ret = vsnprintf( dest, size, fmt, argptr );
#endif

	dest[size-1] = '\0';
	if ( ret < 0 || ret >= size ) {
		return -1;
	}
	return ret;
}

//Ensiform provided this version of Com_sprintf, which is supposed to be overflow protected and less hacky.
void QDECL Com_sprintf( char *dest, int size, const char *fmt, ...) {
	int		ret;
	va_list		argptr;

	va_start (argptr,fmt);
	ret = Q_vsnprintf (dest, size, fmt, argptr);
	va_end (argptr);
	if (ret == -1) {
		Com_Printf ("Com_sprintf2: overflow of %i bytes buffer\n", size);
	}
}*/