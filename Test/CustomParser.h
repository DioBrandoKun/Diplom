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
	void Parse(ptree Pack=ptree(), bool PackSearch = true, bool ElementsSearch = true,bool OtherTree=false);
	ClassTrans Class(const ptree& tree,int Interface=0);//Будет возвращать кастомный класс, который будет разбираться в соотв конструкторах Интерфейсов, Классов и Нумераторов
	ClassValueTrans ClassValue(const ptree& pt);
	ClassOperTrans ClassOperations(const ptree& pt,string ClassName);
	void Realizat(const ptree& pt);
	void Assosiation(const ptree& pt);
	void Interface(const ptree& pt);
	void Enum(const ptree& pt);
	void Activity(const ptree& pt, int ActivNum =0);//Будет возвращать активность которая будет разбираться
	void StateNote(const ptree& pt);
	void Decision(const ptree& pt);
	void Fork(const ptree& pt);
	void EdgeCheck(const ptree& pt);
	void Normalize();
	string Inhert(const ptree& pt);
	set<string> Link;
	vector<string> ActivType = {"Activity","StateNode","DecisionNode","ForkNode"};
	ptree m_root;
private:
	vector<ActivityTrans> AllActivity;//Все блоки активности, элементы диаграммы активности не различимы 
	vector<ClassTrans> AllClass;//Все классы
	vector<Assos> AllAssos;
	vector<Realization> AllRealiz;//Все реализации из XMI
};
