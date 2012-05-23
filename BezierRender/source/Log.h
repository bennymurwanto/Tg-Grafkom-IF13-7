//////////////////////////////////////////////////////////////////////
// Log.h
// Description:		Class to write to a log file
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#ifndef LogH
#define LogH

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

void dateToString(char * stringp); //converts date in proper format

class Log
{
	public:
		Log(char * fn, bool autoUpdate_in=false);
		~Log();
		void write(const char *format, ...); //takes same format as printf
		void write(String& text);
		void update(); //updates the log file 
		void enable()	{enabled=true;}
		void disable()	{enabled=false;}
	protected:
		bool open;
		bool enabled;
		FILE * file;
		char * filename;
		bool autoUpdate;
};
#endif

