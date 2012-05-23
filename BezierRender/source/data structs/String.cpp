/////////////////////////////////////////////////////////////////////
//
// String.cpp
// Efficient String buffer class (uses array doubling to resize)
// Version :1.2
//
// By Ben Woodhouse
//////////////////////////////////////////////////////////////////////

#include "..\stdafx.h"
#include "String.h"
#include "Math.h"
#include <stdarg.h>


using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

String::String()
{
	buf=new char[16];
	buf[0]='\0';
	maxSize=16;
	minSize=16;
	length=0;
}

String::String(const String &src)
{
	buf=NULL;
	maxSize=0;
	minSize=0;
	length=0;
	copyFrom(src);
}

String::String(const char * src)
{
	buf=NULL;
	maxSize=0;
	minSize=0;
	length=0;
	copyFrom(src);
}

String::~String()
{
	if (buf!=NULL) delete buf;
}

void String::copyFrom(const String &src)
{
	length=src.length;
	maxSize=src.maxSize;
	minSize=src.minSize;
	if (buf!=NULL) delete [] buf;
	buf=new char[maxSize];
	memcpy(buf,src.buf,maxSize);
}

void String::copyFrom(const char * src)
{
	int len=(int)strlen(src);
	resize(len);
//		allocate(max(length+1,16));
	memcpy(buf,src,len);
}

void String::copyFrom(const char * src, int srcLength)
{
	resize(srcLength);
	memcpy(buf,src,srcLength);
//		allocate(max(srcLength+1,16));
//		memcpy(buf,src,srcLength);
}


const String& String::operator=(const String& src)
{
	copyFrom(src);
	return (*this);
}

const String& String::operator=(const char * src)
{
	copyFrom(src);
	return (*this);
}


const String& String::operator+=(const String& appendix)
{
	append(appendix);
	return *this;
}

const String& String::operator+=(const char * appendix)
{
	int appLen=(int)strlen(appendix);
	append(appendix,appLen);
	return *this;
}


const String String::operator+(const String& appendix)
{
	String newString;
	newString.copyFrom(*this);
	newString+=appendix;
	return newString;
}


void String::append(const char * appendix, int appLength)
{
	int oldLength=length;
	resize(appLength+length);
	memcpy(&buf[oldLength],appendix,appLength);
}

void String::append(const char * appendix)
{
	append(appendix,(int)strlen(appendix));
}
void String::append(const String &appendix)
{
	int oldLength=length;
	resize(appendix.length+length);
	memcpy(&buf[oldLength],appendix.buf,appendix.length);
}


void String::resize(int newLength)
{
	int realNewLength=newLength+1; //allow for NULL character
	if (realNewLength>maxSize)
	{
		//expand the buffer
		int newMaxSize=max(maxSize,16);
		while (newMaxSize<=realNewLength)
		{
			newMaxSize=newMaxSize<<1;
		}
		char * newBuf=new char[newMaxSize];
		memcpy(newBuf,buf,sizeof(char)*length);
		if (buf!=NULL) delete [] buf;
		buf=newBuf;
		maxSize=newMaxSize;
		minSize=maxSize>>1;
	}
	else if (realNewLength<length)
	{
		if (minSize>16 && realNewLength<minSize)
		{
			//shrink the buffer
			int newMinSize=max(minSize,16);
			while (newMinSize>realNewLength)
			{
				newMinSize=newMinSize>>1;
			}
			int newMaxSize=newMinSize<<1;

			char * newBuf=new char[newMaxSize];
			memcpy(newBuf,buf,sizeof(char)*newLength);
			if (buf!=NULL) delete [] buf;
			buf=newBuf;
			maxSize=newMaxSize;
			minSize=newMinSize;
		}
	}
	length=newLength;
	buf[length]=0; //null-terminate the string
}

/*
void String::allocate(int newMaxSize)
{
	if (buf!=NULL) delete [] buf;
	maxSize=newMaxSize;
	buf=new char[maxSize];
}*/


String String::subString(int start, int end) const
{
	String sub;
	assert(end>=start && end<=length && start>=0);

	//sub.length=end-start;
	//sub.allocate(max(16,sub.length+1));
	sub.resize(end-start);
	memcpy(sub.buf,&buf[start],sub.length);
	return sub;
}

void String::insert(int pos, String &src)
{
	//int newLength=length+src.length;
	int oldLength=length;
	resize(length+src.length);

	//shift chars after pos forwards by src.length
	for (int i=length-1;i>=pos;i--)
	{
		buf[i+src.length]=buf[i];
	}
	//copy in src
	memcpy(&buf[pos],src.buf,src.length);
}
void String::remove(int pos, int len)
{
	assert(len>=0);
	if (len>length-pos)
	{
		len=length-pos;
	}
	for (int i=pos;i<length-len;i++)
	{
		buf[i]=buf[i+len];
	}
	resize(length-len);
}
void String::insertChar(int pos, char c)
{
	//int newLength=length+1;
	int oldLength=length;
	resize(length+1);

	//shift chars after pos forwards 
	for (int i=oldLength-1;i>=pos;i--)
	{
		buf[i+1]=buf[i];
	}
	buf[pos]=c; //write the character
}

void String::removeChar(int pos)
{
	if (length>0)
	{
		for (int i=pos;i<length-1;i++)
		{
			buf[i]=buf[i+1];
		}
		resize(length-1);
	}
}

void String::printf(const char * format, ...)
{
	char msg[1000];
	int len;
	va_list args;

	va_start(args,format);
	len=_vsnprintf( msg, 999, format, args);
	va_end( args );
	append(msg,len);
}

bool String::operator==(const String& sb) const
{
	if (length!=sb.length) return false;

	for (int a=0;a<length;a++)
	{
		if (buf[a]!=sb.buf[a]) return false;
	}
	return true;
}
bool String::operator!=(const String& sb) const
{
	return !((*this)==sb);
}

bool String::readFile(FILE *file)
{
	while (!feof(file))
	{
		if (!feof(file)) insertCharAtEnd(fgetc(file));
	}
	resize(length-1);
	return true;
}

bool String::writeFile(FILE *file)
{
	if (fwrite(buf,1,length,file)==length) return true;
	return false;
}

bool String::writeToFile(FILE *file)
{
	_putw(length,file);
	return writeFile(file);
}

bool String::readFromFile(FILE *file)
{
	int flen=_getw(file);
	if (flen<0) return false;
	resize(flen);
	for (int a=0;a<flen;a++)
	{
		buf[a]=fgetc(file);
	}
	return true;
}




int String::search(int startOffset, const String &text) const
{
	assert(startOffset>=0);
	if (startOffset>=length) return -1;
	//if (length==0) return -1;
	//assert(startOffset>=0 && startOffset<length);

	int textLen=text.getLength();
	char *textBuf=text.buf;

	if (startOffset+textLen>length) return -1;

	int b=0; //text index
	for (int a=startOffset;a<length;a++)
	{
		if (buf[a]==textBuf[b]) 
		{
			if (b==textLen-1) 
			{
				int rv=a-b;
				return a-b;
			}
			b++;
		} 
		else b=0;
	}
	return -1;
}

//continuable search
bool String::searchRep(int &pos, const String &text) const
{
	int r=search(pos,text);
	if (r==-1) return false;
	pos=r+text.length;
	return true;
}


int String::parseInt()
{
	char *cstr=buf;
	char *endPtr=&cstr[length];
	return strtol(cstr,&endPtr,10);
}