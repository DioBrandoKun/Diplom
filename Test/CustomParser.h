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
#include "ActivityClasses.h"
#include "ClassDiagram.h"
using boost::property_tree::ptree;
using boost::container::stable_vector;
using boost::container::flat_set;
using namespace std;

class CustomParser
{
public:
	CustomParser(const ptree& tree) : m_root(tree) {}
	void Parse(const ptree& Pack=ptree(),  const bool& OtherTree=false);
	map<unsigned long, ActivityTrans*>				Normalize();
private:
	void			Class(const ptree& tree, const int& Interface=0);			//Будет возвращать кастомный класс, который будет разбираться в соотв конструкторах Интерфейсов, Классов и Нумераторов
	ClassValueTrans ClassValue(const ptree& pt);								//Обработка членов класса
	ClassOperTrans	ClassOperations(const ptree& pt, const string& ClassName);	//Обработка методов класса
	void			Realizat(const ptree& pt);									//Обработка реализаций
	void			Assosiation(const ptree& pt);								//Обработка Ассоциации, композиции, агрегации
	void			Interface(const ptree& pt);									//Обработка Интерфейса
	void			Enum(const ptree& pt);										//Обработка Инумератора
	void			Activity(const ptree& pt);									//Обработка блока активности
	void			LinkParse(const ptree& pt);									//Обработка ребер активности
	string			Inhert(const ptree& pt);									//Обработка наследования
	void			SetLinks();													//добавляет всем элементам класса ActivityTrans outgoing/ingoing
	void			SetComments();												//Восстанавливаем код для блоков активности

	const ptree							m_root;			//Корень xmi файла
	list<LinkTrans>						m_allLink;		//Все обработанные ребра
	vector<pair<unsigned long, string>> m_comments;		//Блок комментариев, хранит id блока и тело комментария
	map<unsigned long,ActivityTrans*>	m_AllActivity;	//Все блоки активности, элементы диаграммы активности не различимы 
	vector<ClassTrans>					m_AllClass;		//Все обработанные классы
	vector<Assos>						m_AllAssos;		//Все обработанные ассоцации
	set<unsigned long>					m_Realized;		//Все реализованные элементы (пользовательские типы данных, классы, которые мы вывели и т.д.)
	vector<Realization>					m_AllRealiz;	//Все реализации из XMI
};
