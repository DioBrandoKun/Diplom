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
/*��� ������ ����� ���������� ����������� - vecS, vecS, bidirectionalS
������������ ������������� �� �������� - listS �� �������� ���������������� ������ ������������
�� ����� ������ ������������
����� ����������� ������� - Vertex std::shared_ptr<ActivityTrans>
� ����� - std::shared_ptr<LinkTrans>
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
		m_activTable = input; 
		AcivInit();
		vector<unsigned long> path;
		InsideFork(m_head[0], path);
		MakeGraph(path);
		//vector<unsigned long> help;
		//Gen(head[0]);
		//for (auto helpeml : help)
		//	cout << helpeml<<" ";
	};
	void AcivInit();
	void MakeGraph(vector<unsigned long>&);
	void Inside(const unsigned long,vector<unsigned long>&, vector<unsigned long>&);
	void InsideFork(const unsigned long, vector<unsigned long>&);
	map<unsigned long, ActivityTrans*>	m_activTable;	//���������� ���� �������� ����������
	vector<unsigned long>				m_head;			//��� ��������� �������
	Graph								m_graph;		//����� ��� ����������� ���� �������� ���������� 
};

