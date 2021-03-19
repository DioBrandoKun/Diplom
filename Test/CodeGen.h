#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "present.h"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/labeled_graph.hpp"
#include <boost/format.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/depth_first_search.hpp>
/*Для работы графа необходимы определения - vecS, vecS, bidirectionalS
Альтернатива превосходящая по скорости - listS не способна инициализировать Классы пользователя
ей нужны пустые конструкторы
Затем указываются вершины - Vertex std::shared_ptr<ActivityTrans>
И ребра - std::shared_ptr<LinkTrans>
*/
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
	std::shared_ptr<ActivityTrans>, std::shared_ptr<LinkTrans>>
	Graph;
using namespace std;
class GraphGen
{
public:
	GraphGen(map<unsigned long, ActivityTrans*> input)
	{ 
		activTable = input; 
		AcivInit();
		vector<unsigned long> path;
		Inside(head[0], path);
		MakeGraph(path);
		//vector<unsigned long> help;
		//Gen(head[0]);
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
	void MakeGraph(vector<unsigned long>&);
	int Connect(const unsigned long, unsigned long& );
	void InsideEnd(const unsigned long, bool&);
	void Inside(const unsigned long,vector<unsigned long>&);

	list<unsigned long> dowhile;
	map<unsigned long, ActivityTrans*> activTable;
	vector<unsigned long> head;//Все стартовые вершины
	Graph m_graph;//Графы для отображения всех диаграмм активности 
};

