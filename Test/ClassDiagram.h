#pragma once
#include "present.h"
/*
Класс для представления класс-членов
*/
class ClassValueTrans :public INumerableElement, public IStatic, public IType, public IName, public IPublic
{
public:
	ClassValueTrans(const string& id, const string& Name, const string& StaticType, const string& ElemType, const string& DefaulValue, const string& Public, const string& constType) :INumerableElement(id),
		IStatic(StaticType), IType(ElemType), m_DefaultVal(DefaulValue), IName(Name), IPublic(Public), m_isConst(constType == "true" ? "const " : "")
	{
		IdMap::InputName(id, Name);								//Можно в дальнейшем вшить данный вызов в IName
		(DefaulValue == "") ? m_NotStated = true : m_NotStated = false;
	}
	string ToString() const					//Вывод для дебага
	{
		return
			"Value:\n" + m_id.ToString() +
			m_Elem_type.ToString() +
			m_Elem_static.ToString() +
			m_Name.ToString() +
			m_Public.ToString() +
			"Default=\t" + m_DefaultVal + "\n";
	}
	string ToCode() const					//Создание кода
	{
		string Value = "";
		if (!m_NotStated)					//Есть ли значение по умолчанию
		{
			if (!this->GetStatic())
				Value = "=" + m_DefaultVal;
		}
		return
			m_Public.ToCode() +
			m_isConst +
			m_Elem_static.ToCode() +
			m_Elem_type.ToCode() +
			m_Name.ToCode() +
			Value + ";\n";
	}
	string AfterToCode(const string& ClassName) const//Метод когда наш элемент является статическим и мы должны после класса указать его как глобальную переменную
	{//Хотелось бы частично выделить общую часть из ToCode и AfterToCode чтобы не дублироваться
		string Value = "";
		if (!m_NotStated)
		{
			Value = "=" + m_DefaultVal;
		}
		return
			m_isConst +
			m_Elem_type.ToCode() +
			ClassName + "::" +
			m_Name.ToCode() + Value + ";\n";
	}
	void SetNum() const									//Переводим цифровые значения в названия
	{
		string Type = GetType();
		int J = atoi(Type.c_str());
		if (J != 0)
		{
			auto ThisType = IdMap::m_IdName.find(Type);
			if (ThisType == IdMap::m_IdName.end()) return;
			SetType(ThisType->second);
		}
	}
	ClassValueTrans& operator=(const ClassValueTrans& Buff)//Оператор для работы .erase
	{
		this->m_DefaultVal = Buff.m_DefaultVal;
		this->m_NotStated = Buff.m_NotStated;
		this->m_Name = Buff.m_Name;
		this->m_Public = Buff.m_Public;
		this->m_Elem_static = Buff.m_Elem_static;
		this->m_Elem_type = Buff.m_Elem_type;
		return *this;
	}
private:
	bool			m_NotStated;		//Есть ли дефолтное значение
	string			m_DefaultVal;		//Значение дефолтного значения
	const string	m_isConst;			//Константный ли тип данных, в дальнейшем, если UMl редактор позволяет делать константными операции желательно расширить класс Static
	//до Static-Const

};
/*
Класс для представления член-методов класса uml
*/
class ClassOperTrans :public INumerableElement, public IStatic, public IType, public IName, public IPublic
{
public:
	ClassOperTrans(const string& id, const string& Name, const string& StaticType, const string& ElemType, const string& Public)
		:INumerableElement(id), IStatic(StaticType), IType(ElemType), IName(Name), IPublic(Public)
	{
		IdMap::InputName(id, Name);			//Можно в дальнейшем вшить данный вызов в IName
	}
	string ToString() const									//Вывод для дебага
	{
		string ReturnValue = "";
		for (unsigned i = 0; i < m_Elems.size(); i++)
		{
			ReturnValue += "Name=" + m_Elems[i].first + " Type=" + m_Elems[i].second + "\t";
		}
		return
			"Value:\n" + m_id.ToString() +
			m_Elem_type.ToString() +
			m_Elem_static.ToString() +
			m_Name.ToString() +
			ReturnValue +
			m_Public.ToString() + "\n";
	}
	string ToCode() const									//Создание кода
	{
		string ReturnValue = "";
		for (unsigned i = 0; i < m_Elems.size(); i++)
		{
			ReturnValue = ReturnValue + m_Elems[i].second + " " + m_Elems[i].first;
			if (i != m_Elems.size() - 1) ReturnValue += ',';
		}
		string Virt = "";
		if (m_Virtual) Virt = "virtual\t";
		return
			m_Public.ToCode() +
			Virt +
			m_Elem_static.ToCode() +
			m_Elem_type.ToCode() +
			m_Name.ToCode() + "(" +
			ReturnValue + ")";
	}
	void placeVirtual(const string& VirtualType) const			//Устанавливает виртуальность
	{
		if (VirtualType == "true") m_Virtual = true;
		else m_Virtual = false;
	}
	void SetVirtual() const									//Функция для перевода методов интерфейса в виртуальные
	{
		m_Virtual = true;
	}
	void AddElem(const string& Name, const string& Type)					//Добавить один элемент
	{
		string Typer = RightFormat(Type);
		m_Elems.push_back(pair<string, string>(Name, Typer));
		int TypeInt = atoi(Typer.c_str());
		if (TypeInt != 0)
		{
			if (m_NeedRealize.find(TypeInt) == m_NeedRealize.end())
				m_NeedRealize.insert(TypeInt);
		}
	}
	void AddElems(const vector<string>& Names, const vector<string>& Types)	//Добавить группу элементов
	{
		for (unsigned i = 0; i < Names.size(); i++)
		{
			AddElem(Names[i], Types[i]);
			m_Size++;
		}
	}
	void SetNum()										//Переводим цифровые значения в названия
	{
		string Type = GetType();						//узнаем тип функции
		int J = atoi(Type.c_str());						//если это число
		if (J != 0)
		{
			auto ThisType = IdMap::m_IdName.find(Type);
			SetType(ThisType->second);
		}
		for (unsigned long i = 0; i < m_Size; i++)
		{
			string ElemType = m_Elems[i].second;
			J = atoi(ElemType.c_str());					//тип параметров
			if (J != 0)
			{
				auto ThisType = IdMap::m_IdName.find(ElemType);
				if (ThisType == IdMap::m_IdName.end()) return;
				m_Elems[i].second = ThisType->second;
			}
			else continue;
		}
	}
	set<unsigned long>				m_NeedRealize;			//Необходимые классы или dataType для метода, которые используются
private:
	mutable bool					m_Virtual = false;		//Метод виртуальный
	unsigned long					m_Size = 0;				//Кол-во элементов метода
	vector<pair<string, string>>	m_Elems;				//Колеекция элементов метода
};
/*
Класс для представления отношения вида Ассоциация
так же как и композиция
хранит отправителя, точку назначения и имя
для композиции - точка назначения это поле данных класса
SourceId - id поля данных
для ассоциации -
SourceId - id класса
для мультиассоциации -
SourceId - группа id
*/
class Assos :public INumerableElement, public IName
{
public:
	Assos(const string& id, const string& Name, const string& Source, const string& Target, const string& Type)
		:INumerableElement(id), IName(Name), m_Type(Type)
	{
		m_TargetId.push_back(IdMap::Insert(Target).second);
		m_SourceId.push_back(IdMap::Insert(Source).second);
		m_Counter = 1;
	}
	vector <unsigned long> GetSource() const	//Получить классы дающие ассоциацию
	{ 
		return m_SourceId; 
	}
	vector <unsigned long> GetTarget() const	//Получить класс принимающие ассоциацию
	{ 
		return m_TargetId; 
	}
	void AddSource(const string& Source)		//Добавить класс из которого исходит ассоциация
	{
		m_SourceId.push_back(IdMap::Insert(Source).second);
		m_Counter++;
	}
	void AddTarget(const string& Target)		//Добавить класс который принимает ассоциацию
	{
		m_TargetId.push_back(IdMap::Insert(Target).second);
		m_Counter++;
	}
	string m_Type = "";							//Тип ассоциации
private:
	unsigned m_Counter = 0;						//Количество всех классов, использующих ассоциацию
	vector<unsigned long> m_SourceId;			//Множество объектов которые будут использованы для представления 
	vector<unsigned long> m_TargetId;			//Множество классов которые будут использовать представления
};
/*
Класс для представления uml realization, на самом деле является наследованием объекта
*/
class Realization :public INumerableElement
{
public:
	Realization(const string& id, const string& supplier, const string& client)
		:INumerableElement(id), m_Supplier(to_string(IdMap::Insert(supplier).second)),
		m_Сlient(to_string(IdMap::Insert(client).second))
	{
	}
	string GetSupplier() const {			//Получить предка
		return m_Supplier;
	}
	string GetСlient() const {				//Получить наследника
		return m_Сlient;
	}
private:
	const string m_Supplier;				//Класс которые передает функцию
	const string m_Сlient;					//Класс наследюущий реализацию
};
/*
Класс для представления "классов" из uml, кроме того представляет интерфейсы
и другие классоподобные формы.
*/
class ClassTrans :public INumerableElement, public IName
{
public:
	ClassTrans(const string& id, const string& Name)
		:INumerableElement(id), IName(Name)
	{
		IdMap::InputName(id, Name);				//Можно в дальнейшем вшить данный вызов в IName
	}
	void AddValue(const ClassValueTrans& Value)		//Добавление член-класса
	{
		int Type = atoi(Value.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		m_Values.push_back(Value);
	}
	void Inher(const string& Inher)					//Добавление предка
	{
		unsigned long Number = IdMap::Insert(Inher).second;
		Realize(Number);
		m_Inherit.push_back(to_string(Number));
	}
	void Realize(const int& Number)					//Пополнение списка нужных для реализации
	{
		if (Number == GetLocalId()) return;
		if (m_NeedRealize.find(Number) == m_NeedRealize.end())
			m_NeedRealize.insert(Number);

	}
	string ToString() const							//Вывод в строку, не формально
	{
		string ReturnOper = "";
		for (unsigned i = 0; i < m_Operations.size(); i++)
		{
			ReturnOper += m_Operations[i].ToString();
		}
		ReturnOper += '\n';
		string ReturnValue = "";
		for (unsigned i = 0; i < m_Values.size(); i++)
		{
			ReturnValue += m_Values[i].ToString();
		}
		ReturnValue += '\n';
		string ReturnInher = "Ancient:";
		for (unsigned i = 0; i < m_Inherit.size(); i++)
		{
			ReturnInher += m_Inherit[i] + "\t";
		}
		return
			"Value:\n" + m_id.ToString() +
			m_Name.ToString() +
			ReturnValue +
			ReturnOper +
			ReturnInher +
			"\n";
	}
	string ToCode() const						//Вывод кода класса
	{
		string Return = "class " + m_Name.ToCode();
		for (unsigned i = 0; i < m_Inherit.size(); i++)
		{
			if (i == 0)Return = Return + ":";
			Return += "public " + m_Inherit[i];
			if (i != m_Inherit.size() - 1) Return += ',';
		}
		Return += "\n{\n";
		for (unsigned i = 0; i < m_Operations.size(); i++)
		{
			Return += '\t' + m_Operations[i].ToCode();
			if (m_Interface) Return += +" = 0 ;\n";
			else  Return += +";\n";
		}
		for (unsigned i = 0; i < m_Values.size(); i++)
		{
			Return += '\t' + m_Values[i].ToCode();
		}
		Return += "};\n";
		for (unsigned i = 0; i < m_Values.size(); i++)
		{
			if (m_Values[i].GetStatic())
				Return += m_Values[i].AfterToCode(m_Name.ToCode());
		}
		return
			Return;
	}
	void AddCompos(const Assos& Assosiation)			//Добавление композиции для нашего класса
	{
		vector<unsigned long> AssosList = Assosiation.GetTarget();
		vector<unsigned long> SourceList = Assosiation.GetSource();
		for (auto num : AssosList)
		{
			do {
				auto it = std::find_if(m_Values.begin(), m_Values.end(), [num](const ClassValueTrans& valueTrans)
					{ return valueTrans.GetLocalId() == num; });
				if (it != m_Values.end())
				{
					if (Assosiation.m_Type == "composite")//Композиция, на случай обработки
					{
						m_Values.erase(it);
					}
					else if (Assosiation.m_Type == "shared")//Агрегация, на случай обработки
					{
						m_Values.erase(it);
					}
				}
				else break;
			} while (true);
		}
	}
	void AddRealiz(Realization& NewRealiz)			//Добавление реализации по своей сути является наследованием
	{
		Realize(stoi(NewRealiz.GetSupplier()));
		m_Inherit.push_back(NewRealiz.GetSupplier());
	}
	void AddOperation(const ClassOperTrans& Newbie)	//Добавление операции в класс
	{
		m_Operations.push_back(Newbie);
		int Type = atoi(Newbie.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		for (auto needrealId : Newbie.m_NeedRealize)	//Сравниваем нужные классы для функции и для нашего класса и дополняем, если нужно
		{
			if ((m_NeedRealize.find(needrealId) == m_NeedRealize.end()) && (needrealId != m_id.GetLocalId()))
				m_NeedRealize.insert(needrealId);
		}
	}
	void SetInterface() const						//Перевести наш класс в интерфейс					
	{
		m_Interface = true;
		for (auto& Start : m_Operations)
			Start.SetVirtual();
	}
	void SetNum()									
	{
		for (unsigned i = 0; i < m_Operations.size(); i++)
		{
			m_Operations[i].SetNum();
		}
		for (unsigned i = 0; i < m_Values.size(); i++)
		{
			m_Values[i].SetNum();
		}
		for (int i = 0; i < m_Inherit.size(); i++)//Для классов от которых мы наследуем 
		{
			int J = atoi(m_Inherit[i].c_str());
			if (J != 0)
			{
				auto ThisType = IdMap::m_IdName.find(m_Inherit[i]);
				if (ThisType == IdMap::m_IdName.end()) return;
				m_Inherit[i] = ThisType->second;
			}
			else continue;
		}
	}
	set<unsigned long>* GetRealize()					//Получить указатель, на классы которые нужно реализовать
	{
		return &m_NeedRealize;
	}
private:
	mutable bool			m_Interface = false;		//Класс является интерфейсом (абстрактным классом)
	set<unsigned long>		m_NeedRealize;				//Классы которые нужны для использования нашего класса(не только предки), C++ строк к последовательности
	vector<ClassOperTrans>	m_Operations;				//Список операций
	vector<ClassValueTrans> m_Values;					//Список класс-членов
	vector<string>			m_Inherit;				//Список предков - их номера
};
