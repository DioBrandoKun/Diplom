#pragma once
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <list>
#include <boost/lexical_cast.hpp>
using namespace std;
typedef map<const string, const unsigned long> IdMapType;
const string RightFormat(const string& Input);
const string Format(const string& Input);
/*
Класс ответственный за хранение таблицы Id
Формат значений
XML id : id полученное счетчиком

Так же хранит таблицу Id - Имя 
формат значений
id полученное счетчиком : имя из области
*/
class IdMap
{
public:
	static const IdMapType::value_type& Insert(const string& id)//Добавление нового Id или поиск существующего
	{
		auto it = m_ids.find(id);
		if (it == m_ids.end())
		{
			return *(m_ids.insert(make_pair(id, m_counter++)).first);
		}
		else
		{
			return *it;
		}
	}
	static void InputName(const string& id,const string& Name)//Добавление нового имени
	{
		auto it =Insert(id);
		m_IdName.insert(make_pair(to_string(it.second), Name));
	}
	static unsigned long GetLocalId(const string& id)
	{
		return m_ids.at(id);
	}
	static void Print()										//Вывод информации таблицы имен
	{
		for (auto i: m_IdName)
		{		
			cout << i.first << " " << i.second << endl;
		}
	}
	static map<const string, const string>	m_IdName;		//Таблица id - имя объектов
private:
	IdMap() {}

	static IdMapType						m_ids;			//Таблица id - xmi id объектов
	static unsigned long					m_counter;		//Счетчик объектов
};
/*
Представления Id классов
*/
class Id
{
public:
	Id(const string& id) : m_valuePair(IdMap::Insert(id))
	{}

	unsigned long GetLocalId() const
	{
		return m_valuePair.second;
	}

	const string& GetId() const
	{
		return m_valuePair.first;
	}

	string ToString() const
	{
		return
			"xmi:id=\"" + m_valuePair.first + "\"; " +
			"localId=" + to_string(m_valuePair.second) + "\n";
	}

private:
	const IdMapType::value_type& m_valuePair;//Id из XML файла - Id счетчика
};
/*
Обертка вокруг Id
Придерживаемся паттерна декоратор
*/
class INumerableElement 
{
public:
	INumerableElement(const string& id)
		: m_id(id)
	{}
	const string& GetId() const			//Возвращает ссылку на id xmi-формата объекта
	{
		return m_id.GetId();
	}
	unsigned int GetLocalId() const		//Возвращает локальное значение id
	{
		return m_id.GetLocalId();
	}
protected:
	const Id m_id;						//Хранит контейнер id объекта
};

/*
Класс для представления статических данных
хранится в формате true/false
*/
class IStaticSkel
{
public:
	IStaticSkel(const string& StaticType)
	{
		if (StaticType == "true")
			m_elem_staticSkel = true;
	}
	string ToString() const
	{
		string IsStatic = (m_elem_staticSkel) ? "static\t" : "not static\t";
		return "Static=\t"+ IsStatic;
	}
	string ToCode() const
	{
		string IsStatic = (m_elem_staticSkel) ? "static\t" : "";
		return IsStatic;
	}
	bool GetStatic() const						//Получение информации о статичности
	{
		return m_elem_staticSkel;
	}
private:
	 bool m_elem_staticSkel=false;				//Хранит информацию о статичности объекта
};
/*
Обертка вокруг IStaticSkel
*/
class IStatic 
{
public:
	IStatic(const string& StaticType) :m_Elem_static(StaticType)
	{}
	bool GetStatic() const						//Получение информации о статичности объектов
	{
		return m_Elem_static.GetStatic();
	}
protected:
	IStaticSkel m_Elem_static;					//Хранит контенер статичности объекта
};	
/*
Класс для представления типа элементов
*/
class ITypeSkel //: public virtual IElement
{
private:
	mutable string m_elem_typeSkel;				//Хранит тип объекта

public:
	ITypeSkel() {}
	ITypeSkel(const string& type)
	{
		m_elem_typeSkel = RightFormat(type);
	}
	void SetType(const string& Type) const		//Изменение типа
	{
		m_elem_typeSkel = Type;
	}
	string& GetType() const						//Получение типа
	{
		return m_elem_typeSkel;
	}
	string ToString() const
	{
		return "Type=\t"+m_elem_typeSkel + "\t";
	}
	string ToCode() const
	{
		if (m_elem_typeSkel == "none") m_elem_typeSkel = "";
		return m_elem_typeSkel+'\t';
	}
};
/*
Обертка вокруг ITypeSkel
*/
class IType
{
protected: 
	ITypeSkel m_Elem_type;						//Хранит контейнер типа объекта
public:
	string GetType() const						//Обращение к типу объкта
	{
		return m_Elem_type.GetType();
	}
	void SetType(const string& Type) const		//Изменение типа объекта
	{
		m_Elem_type.SetType(Type);
	}
	IType(const string& type) :m_Elem_type(type) {}
};
/*
Класс для представления имени объектов
*/
class INameSkel
{
private:
	 string m_NameSkel;						//Хранит имя объекта

public:
	INameSkel() {}
	INameSkel(const string& Name):m_NameSkel(Name)
	{}

	string ToString() const					//Дебагерское получение имени (не особо отличается)
	{
		return "Name=\t"+m_NameSkel+"\t";
	}
	string ToCode() const
	{
		return  m_NameSkel;
	}
	void SetName(const string& NewName)		//Установить имя напрямую
	{
		m_NameSkel = NewName;
	}	
	string GetName() const					//Изменить имя напрямую
	{
		return m_NameSkel;
	}
};
/*
Обертка вокруг NameSkel
*/
class IName 
{
protected:
	INameSkel m_Name;							//Хранит контейнер имени
public:
	IName(const string& Name) :m_Name(Name)
	{}
	void SetName(const string& NewName)			//Установка имени объекта
	{
		m_Name.SetName(NewName);
	}		
	string GetName() const						//Получение имени объекта
	{
		return m_Name.GetName();
	}
};
/*
Класс для предоставления параметра {public,private,protected}
*/
class IPublicSkel
{

public:
	IPublicSkel(const string& Public)
	{
		if (Public == "public")
			this->m_PublicSkel = publ;
		else if (Public == "private")
			this->m_PublicSkel = privat;
		else if (Public == "protected")
			this->m_PublicSkel = protec;
	}
	string EnumToString() const	
	{
		switch (m_PublicSkel)
		{
			case publ:		return "public";
			case privat:	return "private";
			case protec:	return "protected";
			default:		return "";
		}
	}
	string ToString() const
	{
		return "Public=\t" + EnumToString() + "\t";
	}
	string ToCode() const
	{
		return EnumToString() + ':';
	}
private:
	enum
	{
		publ,   //public
		privat, //private
		protec, //public
	}m_PublicSkel;		//Хранит тип доступности объекта
	/*UML имеет еще 4 состояние - Package, так как классы глобальные объекты 
	  public обеспечивает видимость во всей программе*/
};
/*
Обертка вокруг IPublicSkel для дальнейшего наследования
*/
class IPublic
{
public:
	IPublic(const string& Public):m_Public(Public){}
protected:
	IPublicSkel m_Public;
};