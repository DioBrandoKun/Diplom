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
	void Parse(const ptree& Pack=ptree(), const bool& PackSearch = true, const bool& ElementsSearch = true, const bool& OtherTree=false);//PackSearch - ищем в теге Packagedelements, ElementsSearch - ищем в теге elements
	void Normalize();
private:
	ClassTrans Class(const ptree& tree, const int& Interface=0);//Будет возвращать кастомный класс, который будет разбираться в соотв конструкторах Интерфейсов, Классов и Нумераторов
	ClassValueTrans ClassValue(const ptree& pt);  //Обработка членов класса
	ClassOperTrans ClassOperations(const ptree& pt, const string& ClassName); //Обработка методов класса
	void Realizat(const ptree& pt); //Обработка реализаций
	void Assosiation(const ptree& pt); //Ассоциации, композиции, агрегации
	void Interface(const ptree& pt); //Интерфейсы
	void Enum(const ptree& pt);   //Инумераторы
	void Activity(const ptree& pt, int ActivNum =0);//Будет возвращать активность которая будет разбираться
	void StateNote(const ptree& pt); //Диаграмма активности блок начала-конца
	void Decision(const ptree& pt); //Диаграмма активности блок выбора
	void Fork(const ptree& pt); //Диаграмма активности блок разделения потоков
	void EdgeCheck(const ptree& pt); //Диаграмма активности поиск тела для link идущих в блок fork
	string Inhert(const ptree& pt); //Обработка наследования
	set<string> Link;
	vector<string> ActivType = {"Activity","StateNode","DecisionNode","ForkNode"};
	const ptree m_root;
	vector<ActivityTrans> AllActivity;//Все блоки активности, элементы диаграммы активности не различимы 
	vector<ClassTrans> AllClass;//Все классы
	vector<Assos> AllAssos;
	set<unsigned long> Realized;//Для пользовательских типов данных и выведенных классов, чтобы правильно восстановить порядок
	vector<Realization> AllRealiz;//Все реализации из XMI
};
