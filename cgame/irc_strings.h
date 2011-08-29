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

#include "irc.h"

//Standard string library for JAIRC
//Written by eezstreet
void ParseIRCString(char *buffer, char *host, char *command);
void ClipFirstChar(char *buffer, qboolean count);
void ClipNumberChars(char *buffer, int number, qboolean count);
void StripNick(char *nickString, char *buffer);
void ValidateInput(char *string, char *buffer);
void CleanString(char *string, char *cleanstring);
void StopAtSpace(char *string, char *buffer);
void GrabParam(char *buffer, char *param);
void GrabMessage(char *buffer, char *param, qboolean colons);
void HandleNextCommand(char *buffer, SOCKET socket);