#pragma once
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <boost/container/stable_vector.hpp>
#include "present.h"
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <set>
#include <iostream>
using boost::property_tree::ptree;
using boost::container::stable_vector;
using boost::container::flat_set;
using namespace std;
class CustomParser
{
public:
	CustomParser(const ptree& tree) : m_root(tree) {}
	void Parse(const ptree& Pack=ptree(), const bool& PackSearch = true, const bool& ElementsSearch = true, const bool& OtherTree=false);//PackSearch - ���� � ���� Packagedelements, ElementsSearch - ���� � ���� elements
	void Normalize();
private:
	ClassTrans Class(const ptree& tree, const int& Interface=0);//����� ���������� ��������� �����, ������� ����� ����������� � ����� ������������� �����������, ������� � �����������
	ClassValueTrans ClassValue(const ptree& pt);  //��������� ������ ������
	ClassOperTrans ClassOperations(const ptree& pt, const string& ClassName); //��������� ������� ������
	void Realizat(const ptree& pt); //��������� ����������
	void Assosiation(const ptree& pt); //����������, ����������, ���������
	void Interface(const ptree& pt); //����������
	void Enum(const ptree& pt);   //�����������
	void Activity(const ptree& pt, int ActivNum =0);//����� ���������� ���������� ������� ����� �����������
	void StateNote(const ptree& pt); //��������� ���������� ���� ������-�����
	void Decision(const ptree& pt); //��������� ���������� ���� ������
	void Fork(const ptree& pt); //��������� ���������� ���� ���������� �������
	void EdgeCheck(const ptree& pt); //��������� ���������� ����� ���� ��� link ������ � ���� fork
	string Inhert(const ptree& pt); //��������� ������������
	set<string> Link;
	vector<string> ActivType = {"Activity","StateNode","DecisionNode","ForkNode"};
	const ptree m_root;
	vector<ActivityTrans> AllActivity;//��� ����� ����������, �������� ��������� ���������� �� ��������� 
	vector<ClassTrans> AllClass;//��� ������
	vector<Assos> AllAssos;
	set<unsigned long> Realized;//��� ���������������� ����� ������ � ���������� �������, ����� ��������� ������������ �������
	vector<Realization> AllRealiz;//��� ���������� �� XMI
};
