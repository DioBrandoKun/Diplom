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
#include "ActivityClasses.h"
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
		output = "";
		m_activTable = input; 
		AcivInit();
		vector<unsigned long> path;
		for (unsigned i = 0; i < m_head.size(); i++)
		{
			InsideFork(m_head[i], path);
			MakeGraph(path);
			path = vector<unsigned long>();
			m_graph = Graph();
		}
	};
	void AcivInit();
	void MakeGraph(vector<unsigned long>&);
	void Inside(const unsigned long,vector<unsigned long>&, vector<unsigned long>&);
	void InsideFork(const unsigned long, vector<unsigned long>&);
	map<unsigned long, ActivityTrans*>	m_activTable;	//Коллекциях всех диаграмм активности
	vector<unsigned long>				m_head;			//Все стартовые вершины
	Graph								m_graph;		//Графы для отображения всех диаграмм активности 
	std::string						    output;			//Сгенерированные строки
};