#include "CXMLParser.h"
#include <string>
#include <fstream>
using namespace std;

CXMLParser* CXMLParser::instance = 0;

CXMLParser* CXMLParser::GetInstance()
{
	if(instance == 0)
		instance = new CXMLParser();

	return instance;
}

CXMLParser::CXMLParser()
{
}

CXMLParser::~CXMLParser()
{
	if(instance != 0)
		delete instance;
}

enum XMLParseStates { TAGSEARCH, READINGDATA };

string CXMLParser::GetTagData(ifstream& file, string requestedTag, int number)
{
//	ifstream::pos_type begin = file.tellg();
	
	int filesize = 0;
	file.seekg(0, ios::end);
	filesize = file.tellg();
	file.seekg(0, ios::beg);

	char * fileBuffer = new char[filesize+1];
    memset( fileBuffer, 0, filesize );
	file.read(fileBuffer, filesize);
	fileBuffer[filesize] = '\0';

	string fileData;
	fileData.assign(fileBuffer);

	delete [] fileBuffer;
	file.close();

	return GetTagData(fileData, requestedTag, number);

	/*
	char tag[256];
	char character;
	string tagData;
	XMLParseStates state = TAGSEARCH;

	int tagCount = 0;

	while( !file.eof() )
	{
		file.get(character);

		if(state == TAGSEARCH)
		{
			if(character == '<')
			{
				file.getline(tag, 256, '>');
				if(requestedTag == tag)
				{
					if(tagCount == number)
					{
						state = READINGDATA;
					}
					else
					{
						tagCount++;
					}
				}
			}
		}
		else if(state == READINGDATA)
		{
			if(character == '<')
			{
				ifstream::pos_type interrupt = file.tellg();

				file.getline(tag, 256, '>');
				
				if((string("/") + requestedTag) == tag)
				{
					break;
				}
				else
				{
					file.seekg(interrupt);
				}
			}

			tagData += character;
		}
	}*/
	
	//return tagData;
}

string CXMLParser::GetTagData(string source, string tag, int number)
{
	string tagData;
	
	string::size_type start = 0;
	string::size_type end = 0;

	for(int i = 0; i <= number; i++) // find the starting tag
	{
		start = source.find(string("<") + tag + string(">"), start);
		if(start == -1)
			return "";
		start += tag.size() + 2;

		end = source.find(string("</") + tag + string(">"), end + ((end > 0) ? (tag.size() + 3) : 0));
		if(end == -1)
			return "";
	}
		
	tagData = source.substr(start, end-start);
	
	return tagData;
}

vector<string> CXMLParser::GetTagDataSet(string source, string tag)
{
	vector<string> tagDataSet;
	
	string::size_type start = 0;
	string::size_type end = 0;

	while(true) // find all tags
	{
		start = source.find(string("<") + tag + string(">"), start);
		if(start == -1)
			break;
		start += tag.size() + 2;

		end = source.find(string("</") + tag + string(">"), end + ((end > 0) ? (tag.size() + 3) : 0));
		if(end == -1)
			break;

		tagDataSet.push_back(source.substr(start, end-start));
	}
		
	return tagDataSet;
}