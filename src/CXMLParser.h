#ifndef CXMLPARSER_H
#define CXMLPARSER_H

#include <fstream>
#include <string>
#include <vector>
using namespace std;

class CXMLParser
{
public:
	static CXMLParser* GetInstance();

	string GetTagData(ifstream& file, string tag, int number = 0);
	string GetTagData(string source, string tag, int number = 0);
	vector<string> GetTagDataSet(string source, string tag);
private:
	CXMLParser();
	~CXMLParser();

	static CXMLParser* instance;
};

#endif