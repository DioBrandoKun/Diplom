#pragma once
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <boost/container/stable_vector.hpp>
#include "present.h"
using boost::property_tree::ptree;
using boost::container::stable_vector;
using namespace std;
class CustomParser
{
public:
	CustomParser(const ptree& tree) : m_root(tree) {}
	void Parse();
	ClassTrans Class(const ptree& tree,int Interface=0);//Будет возвращать кастомный класс, который будет разбираться в соотв конструкторах Интерфейсов, Классов и Нумераторов
	ClassValueTrans ClassValue(const ptree& pt);
	ClassOperTrans ClassOperations(const ptree& pt);
	void Realization(const ptree& pt);
	void Assosiation(const ptree& pt);
	void Interface(const ptree& pt);
	void Enum(const ptree& pt);
	void Activity(const ptree& pt, int ActivType=0);//Будет возвращать активность которая будет разбираться
	void StateNote(const ptree& pt);
	void Decision(const ptree& pt);
	void Fork(const ptree& pt);
	void EdgeCheck(const ptree& pt);
	string Inhert(const ptree& pt);
	stable_vector<string> Link;
	ptree m_root;
private:
	vector<ClassTrans> AllClass;
	vector<Assos> AllAssos;
};
