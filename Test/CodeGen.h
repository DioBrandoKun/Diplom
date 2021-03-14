#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "present.h"
using namespace std;
class CodeGen
{
public:
	CodeGen(map<unsigned long, ActivityTrans*> input) 
	{ 
		activTable = input; 
		AcivInit();
		vector<unsigned long> help;
		Gen(head[0]);
		//for (auto helpeml : help)
		//	cout << helpeml<<" ";
	};
	//enum CodeGenTypes
	//{
	//	ifelse,
	//	dowhile
	//};
	//vector<CodeGenTypes> queue;
	void AcivInit();
	void Gen(const unsigned long);
	int Connect(const unsigned long, unsigned long& );
	void InsideEnd(const unsigned long, bool&);
	void Inside(const unsigned long,vector<unsigned long>&);

	list<unsigned long> dowhile;
	map<unsigned long, ActivityTrans*> activTable;
	vector<unsigned long> head;
};

