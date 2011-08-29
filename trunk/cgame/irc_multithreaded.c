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

#include "irc.h"
#include "irc_strings.h"
#include "q_shared.h"

//============================
// BEGIN NEW THREAD
//============================
void InitQueue(void)
{
	int i;
	for(i = 0; i < EEZ_NUM_QUEUES; i++)
	{
		ThreadActionQueue[i][0] = '\0';
	}
}
//=============================
// END NEW THREAD
//=============================

int FindFirstUnusedQueue(void)
{
	int i;
	for(i = 0; i < EEZ_NUM_QUEUES; i++)
	{
		if(ThreadActionQueue[i][0] == '\0')
		{
			return i;
		}
	}
	return -1;
}

void ClearQueue(int queuenum)
{
	ThreadActionQueue[queuenum][0] = 0;
}

void AddCommandToQueue(char *command, char *message)
{
	int queueNum = FindFirstUnusedQueue();
	if(queueNum == -1)
		return;

	if(!Q_stricmp(command, "Com_Printf"))
	{
		char buffer[1024];
		ValidateInput(message, buffer);
		//strcpy(ThreadActionQueue[queueNum], va("P: %s", buffer)); //va isn't thread-safe?
		sprintf(ThreadActionQueue[queueNum], "P: %s", buffer);
	}
}

void ExecuteCommandInQueue(char *command, char *message)
{
	if(!Q_stricmp(command, "P"))
	{
		Com_Printf(message);
	}
}

void ParseActionQueue(void)
{
	int i, j, k, l;
	char command[8];
	char rest[EEZ_QUEUE_SIZE-8];
	qboolean commandShift;
	for(i = 0; i < EEZ_NUM_QUEUES; i++)
	{
		j = 0;
		k = 0;
		l = 0;
		commandShift = qfalse;
		//Parse everythang!
		while(ThreadActionQueue[i][j] != '\0')
		{
			if(!commandShift && ThreadActionQueue[i][j] == ':')
			{
				commandShift = qtrue;
				j++;
			}
			else if (!commandShift)
			{
				command[k] = ThreadActionQueue[i][j];
				k++;
			}
			else
			{
				rest[l] = ThreadActionQueue[i][j];
				l++;
			}
			j++;
		}
		command[k] = '\0';
		rest[l] = '\0';
		if(command[0] == 0 && i <= 1) //First command is blank
			return;
		ExecuteCommandInQueue(command, rest);
		ClearQueue(i);
	}
}