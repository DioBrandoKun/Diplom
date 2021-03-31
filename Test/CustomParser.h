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
#include <utility>
#include <iostream>
using boost::property_tree::ptree;
using boost::container::stable_vector;
using boost::container::flat_set;
using namespace std;

class CustomParser
{
public:
	CustomParser(const ptree& tree) : m_root(tree) {}
	void Parse(const ptree& Pack=ptree(),  const bool& OtherTree=false);//PackSearch - ищем в теге Packagedelements, ElementsSearch - ищем в теге elements
	map<unsigned long, ActivityTrans*> Normalize();
private:
	void Class(const ptree& tree, const int& Interface=0);//Будет возвращать кастомный класс, который будет разбираться в соотв конструкторах Интерфейсов, Классов и Нумераторов
	ClassValueTrans ClassValue(const ptree& pt);  //Обработка членов класса
	ClassOperTrans ClassOperations(const ptree& pt, const string& ClassName); //Обработка методов класса
	void Realizat(const ptree& pt); //Обработка реализаций
	void Assosiation(const ptree& pt); //Ассоциации, композиции, агрегации
	void Interface(const ptree& pt); //Интерфейсы
	void Enum(const ptree& pt);   //Инумераторы
	void Activity(const ptree& pt);//Будет возвращать активность которая будет разбираться
	//void EdgeCheck(const ptree& pt); //Диаграмма активности поиск тела для link идущих в блок fork
	void LinkParse(const ptree& pt); //Если парсить надо элемент ControlFlow
	string Inhert(const ptree& pt); //Обработка наследования
	void SetLinks();//добавляет всем элементам класса ActivityTrans outgoing/ingoing
	void SetComments();//Восстанавливаем код для блоков активности

	const ptree m_root;
	list<LinkTrans> allLink;
	vector<pair<unsigned long, string>> comments;//Блок комментариев, хранит id блока и тело комментария
	map<unsigned long,ActivityTrans*> AllActivity;//Все блоки активности, элементы диаграммы активности не различимы 
	vector<ClassTrans> AllClass;//Все классы
	vector<Assos> AllAssos;
	set<unsigned long> Realized;//Для пользовательских типов данных и выведенных классов, чтобы правильно восстановить порядок
	vector<Realization> AllRealiz;//Все реализации из XMI
};
