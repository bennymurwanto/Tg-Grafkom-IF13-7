//////////////////////////////////////////////////////////////////////
// Log.cpp
// Description:		Class to write to a log file
//
// By:				Ben Woodhouse (0145394)
// Date:			12/2003
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Log.h"


void dateToString(char * stringp)
{
	struct tm *today;
	time_t ltime;

	time( &ltime );
	today = localtime( &ltime );

	strftime( stringp, 10,
	"%d/%m/%y", today );
}

Log::Log(char * fn, bool autoUpdate_in)
{
	autoUpdate=autoUpdate_in;
	char sdate[128],stime[128];
	open=false;
	filename=new char[strlen(fn)+1];
	sprintf(filename,fn);
	file=fopen(fn,"wt");
	if (file)
	{
		write("DebugLog file\n");
		dateToString(sdate);
		_strtime(stime);
		write("created: %s %s\n",sdate,stime);
		write("________________________________\n\n",sdate,stime);
		enabled=true;
		open=true;
	}
	else
	{
		enabled=false;
	}
}

Log::~Log()
{
	if (filename!=NULL) delete [] filename;
	if (open) fclose(file);
}

void Log::write(const char *format, ...)
{
	if (enabled && open)
	{
		char buffer[2048];
		va_list args;
		va_start(args,format);
		_vsnprintf(buffer, 2047, format, args);
		va_end( args );
		fwrite(buffer,1,strlen(buffer),file);
		fwrite("\n",1,1,file);
		if (autoUpdate) update();
	}
}

void Log::write(String& text)
{
	if (enabled && open)
	{
		text.writeFile(file);
		fwrite("\n",1,1,file);
		if (autoUpdate) update();
	}
}

void Log::update()
{
	if (open)
		fflush(file);
}
