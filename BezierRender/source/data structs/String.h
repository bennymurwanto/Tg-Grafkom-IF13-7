/////////////////////////////////////////////////////////////////////
//
// String.h
// Efficient String buffer class (uses array doubling to resize)
// Version :1.2
//
// By Ben Woodhouse
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_STRING_H__B0440E4F_1175_452C_AD69_B39293D7E01F__INCLUDED_)
#define AFX_STRING_H__B0440E4F_1175_452C_AD69_B39293D7E01F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class String 
{
public:
	String();
	String(const char * src);
	String(const String &src);
	
	virtual ~String();

	const String& operator=(const char * src); //for null-terminated strings
	const String& operator=(const String& src);

	const String& operator+=(const String& appendix); //use for efficient concatenation 
	const String& operator+=(const char * appendix);  //for null-terminated strings

	const String operator+(const String& appendix); //For convenience, not efficiency

	bool operator==(const String& sb) const;
	bool operator!=(const String& sb) const;

	void append(const char * appendix, int appLength); 
	void append(const char * appendix);
	void append(const String &src);

	void printf(const char * format, ...);
	
	void copyFrom(const char * src, int srcLength);
	void copyFrom(const String &src);
	void copyFrom(const char * src);

	//returns the index of the text string (if found) or -1
	int search(const String &text) const						{return search(0,text);} 
	int search(int startOffset, const String &text) const; 

	//repeated search (position is maintained in pos)
	bool searchRep(int &pos, const String &text) const;

	bool writeToFile(FILE *file);
	bool readFromFile(FILE *file);

	bool readFile(FILE *file);
	bool writeFile(FILE *file);

	String trimStart(int offset)								{return subString(offset,length);}
	String trimEnd(int offset)									{return subString(0,length-offset);}

	const char * cStr() const									{return buf;}

	int getLength() const								{return length;} 

	String subString(int start, int end) const;				

	void insert(int pos, String& src);
	void remove(int pos, int len);
	void insertChar(int pos, char c);
	void removeChar(int pos);
	void insertCharAtEnd(char c)
	{
		resize(length+1);
		buf[length-1]=c; //write the character
	}
	void clear()	{resize(0);}
	
	int parseInt();


	operator const char * () {return cStr();}

protected:
	void resize(int newLength);			//resizes the buffer (making room for NULL character)
	//void allocate(int newMaxSize);	    //initialises the buffer

	char * buf;
	int length;

	int maxSize;
	int minSize;
};




#endif // !defined(AFX_STRING_H__B0440E4F_1175_452C_AD69_B39293D7E01F__INCLUDED_)
