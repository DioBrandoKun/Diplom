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
	static const IdMapType::value_type& Insert(const string& id)//Добавление нового Id
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
		IdName.insert(make_pair(to_string(it.second), Name));
	}
	static unsigned long GetLocalId(const string& id)
	{
		return m_ids.at(id);
	}
	static void Print()//Вывод таблицы имен
	{
		for (auto i: IdName)
		{		
			cout << i.first << " " << i.second << endl;
		}
	}
	static map<const string, const string>IdName;//Таблица имен
private:
	IdMap() {}

	static IdMapType m_ids;//Таблица id
	static unsigned long m_counter;//Счетчик
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
class INumerableElement //Enum
{
public:
	INumerableElement(const string& id)
		: m_id(id)
	{}
	const string& GetId() const
	{
		return m_id.GetId();
	}
	unsigned int GetLocalId() const
	{
		return m_id.GetLocalId();
	}
	//virtual 
protected:
	const Id m_id;
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
			elem_static = true;
	}
	string ToString() const
	{
		string IsStatic = (elem_static) ? "static\t" : "not static\t";
		return "Static=\t"+ IsStatic;
	}
	string ToCode() const
	{
		string IsStatic = (elem_static) ? "static\t" : "";
		return IsStatic;
	}
	bool GetStatic() const
	{
		return elem_static;
	}
private:
	 bool elem_static=false;
};
/*
Обертка вокруг IStaticSkel
*/
class IStatic 
{
public:
	IStatic(const string& StaticType) :Elem_static(StaticType)
	{}
	bool GetStatic() const
	{
		return Elem_static.GetStatic();
	}
protected:
	IStaticSkel Elem_static;
};	
/*
Класс для представления типа элементов
*/
class ITypeSkel //: public virtual IElement
{
private:
	mutable string elem_type;

public:
	ITypeSkel() {}
	ITypeSkel(const string& type)
	{
		elem_type = RightFormat(type);
	}
	void SetType(const string& Type) const
	{
		elem_type = Type;
	}
	string& GetType() const
	{
		return elem_type;
	}
	string ToString() const
	{
		return "Type=\t"+elem_type + "\t";
	}
	string ToCode() const
	{
		return elem_type+'\t';
	}
};
/*
Обертка вокруг ITypeSkel
*/
class IType
{
protected: 
	ITypeSkel Elem_type;
public:
	string GetType() const 
	{
		return Elem_type.GetType();
	}
	void SetType(const string& Type) const
	{
		Elem_type.SetType(Type);
	}
	IType(const string& type) :Elem_type(type) {}
};
/*
Класс для представления имени объектов
*/
class INameSkel
{
private:
	 string Name;

public:
	INameSkel() {}
	INameSkel(const string& Name):Name(Name)
	{}

	string ToString() const
	{
		return "Name=\t"+Name+"\t";
	}
	string ToCode() const
	{
		return  Name;
	}
	void SetName(const string& NewName)  //Установить имя
	{
		Name = NewName;
	}
	string GetName() const
	{
		return Name;
	}
};
/*
Обертка вокруг NameSkel
*/
class IName 
{
protected:
	INameSkel Name;
public:
	IName(const string& Name) :Name(Name)
	{}
	void SetName(const string& NewName) //Установить имя
	{
		Name.SetName(NewName);
	}
	string GetName() const
	{
		return Name.GetName();
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
			this->Public = publ;
		else if (Public == "private")
			this->Public = privat;
		else if (Public == "protected")
			this->Public = protec;
	}
	string EnumToString() const
	{
		switch (Public)
		{
			case publ:   return "public";
			case privat:   return "private";
			case protec: return "protected";
			default:      return "";
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
	}Public;
	/*UML имеет еще 4 состояние - Package, так как классы глобальные объекты 
	  public обеспечивает видимость во всей программе*/
};
/*
Обертка вокруг IPublicSkel для дальнейшего наследования
*/
class IPublic
{
public:
	IPublic(const string& Public):Public(Public){}
protected:
	IPublicSkel Public;
};
/*
Класс для представления класс-членов
*/
class ClassValueTrans:public INumerableElement, public IStatic,public IType,public IName,public IPublic
{
public:
	ClassValueTrans(const string& id, const string& Name, const string& StaticType, const string& ElemType, const string& DefaulValue, const string& Public, const string& constType) :INumerableElement(id),
	IStatic(StaticType), IType(ElemType), DefaultVal(DefaulValue), IName(Name), IPublic(Public), isConst(constType == "true" ? "const " : "")
	{
		IdMap::InputName(id, Name);
		(DefaulValue == "")?NotStated = true:NotStated = false;			
	}

	string ToString() const
	{
		return
			"Value:\n" + m_id.ToString()+
			 Elem_type.ToString()+
			 Elem_static.ToString()+
			 Name.ToString()+
			 Public.ToString()+
			"Default=\t"+ DefaultVal+"\n";
	}

	string ToCode() const//Кодогенерация
	{
		string Value="";
		if (!NotStated)//Есть ли значение по умолчанию
		{
			if(!this->GetStatic())
			Value="="+DefaultVal;
		}
		return
			Public.ToCode() +
			isConst +
			Elem_static.ToCode() +
			Elem_type.ToCode() +
			Name.ToCode() +
			Value + ";\n";
	}
	string AfterToCode(const string& ClassName) const//Метод когда наш элемент является статическим и мы должны после класса указать его как глобальную переменную
	{//Хотелось бы частично выделить общую часть из ToCode и AfterToCode чтобы не дублироваться
		string Value = "";
		
		if (!NotStated)
		{
			Value = "=" + DefaultVal;
		}
		return
			isConst+
			Elem_type.ToCode() +
			ClassName+"::"+
			Name.ToCode()+Value+";\n";
	}
	void SetNum() const//Параметы из цифр в слова через словарь
	{
		string Type = GetType();
		int J = atoi(Type.c_str());
		if (J != 0)
		{
			auto ThisType = IdMap::IdName.find(Type);
			if (ThisType == IdMap::IdName.end()) return;
			SetType(ThisType->second);
		}
	}
	ClassValueTrans& operator=(const ClassValueTrans& Buff)//Функция нужная для работы erase
	{
		this->DefaultVal = Buff.DefaultVal;
		this->NotStated = Buff.NotStated;
		this->Name = Buff.Name;
		this->Public = Buff.Public;
		this->Elem_static = Buff.Elem_static;
		this->Elem_type = Buff.Elem_type;
		return *this;
	}
private:
	bool NotStated;//Есть ли дефолтное значение
	string DefaultVal;//Значение дефолтного значения
	const string isConst; //Константный ли тип данных, в дальнейшем, если UMl редактор позволяет делать константными операции желательно расширить класс Static
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
		IdMap::InputName(id, Name);
	}
	string ToString() const
	{
		string ReturnValue="";
		for (unsigned i = 0; i < Elems.size(); i++)
		{
			ReturnValue+="Name="+Elems[i].first+" Type="+ Elems[i].second+"\t";
		}
		return
			"Value:\n" + m_id.ToString() +
			Elem_type.ToString() +
			Elem_static.ToString() +
			Name.ToString() +
			ReturnValue+
			Public.ToString() + "\n";
	}
	string ToCode() const//Кодогенерация
	{
		string ReturnValue = "";
		for (unsigned i = 0; i < Elems.size(); i++)
		{
			ReturnValue = ReturnValue + Elems[i].second+" " + Elems[i].first;
			if (i != Elems.size() - 1) ReturnValue += ',';
		}
		string Virt="";
		if (Virtual) Virt = "virtual\t";
		return
			Public.ToCode() +
			Virt+
			Elem_static.ToCode() +
			Elem_type.ToCode() +
			Name.ToCode() +"("+
			ReturnValue+")";
	}
	void IsVirtual(const string& VirtualType) const
	{ 
		if (VirtualType == "true") Virtual = true;
		else Virtual = false;
	}
	void SetVirtual() const //метод для интерфейсов
	{
		 Virtual = true;
	}
	void AddElem(const string& Name, const string& Type) //Добавить элемент
	{
		string Typer = RightFormat(Type);
		Elems.push_back(pair<string, string>(Name, Typer));
		int TypeInt = atoi(Typer.c_str());
		if (TypeInt != 0)
		{
			if (NeedRealize.find(TypeInt) == NeedRealize.end())
				NeedRealize.insert(TypeInt);
		}
	}
	void AddElems(const vector<string>& Names,const vector<string>& Types)//Добавить элементы
	{
		for (unsigned i = 0; i < Names.size(); i++)
		{
			AddElem(Names[i], Types[i]);
			Size++;
		}
	}
	void SetNum()  //Переводим цифровые значения в названия
	{
		string Type = GetType();//узнаем тип функции
		int J = atoi(Type.c_str());//если это число
		if (J != 0)
		{
			auto ThisType=IdMap::IdName.find(Type);
			SetType(ThisType->second);
		}
		for (unsigned long i = 0; i < Size; i++)
		{
			string ElemType = Elems[i].second;
			J = atoi(ElemType.c_str());//тип параметров
			if (J != 0)
			{
				auto ThisType = IdMap::IdName.find(ElemType);
				if (ThisType == IdMap::IdName.end()) return;
				Elems[i].second=ThisType->second;
			}
			else continue;
		}
	}
	set<unsigned long> NeedRealize;//Необходимые классы или dataType
private:
	mutable bool Virtual = false;
	unsigned long Size = 0;//Кол-во элементов
	vector<pair<string, string>> Elems;//Элементы и их тип
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
		:INumerableElement(id), IName(Name),Type(Type)
	{
		TargetId.push_back(IdMap::Insert(Target).second);
		SourceId.push_back(IdMap::Insert(Source).second);
		Counter = 1;
	}
	vector <unsigned long> GetSource() const { return SourceId; }
	vector <unsigned long> GetTarget() const { return TargetId; }
	void AddSource(const string& Source) 
	{
		SourceId.push_back(IdMap::Insert(Source).second); 
		Counter++;
	}
	void AddTarget(const string& Target)
	{ 
		TargetId.push_back(IdMap::Insert(Target).second); 
		Counter++;
	}
	string Type = "";
private:
	unsigned Counter = 0;
	vector<unsigned long> SourceId;//Множество объектов которые будут использованы для представления 
	vector<unsigned long> TargetId;//Множество классов которые будут использовать представления
};
/*
Класс для представления uml realization, на самом деле является наследованием объекта
*/
class Realization:public INumerableElement
{
public:
	Realization(const string& id, const string& supplier, const string& client)
		:INumerableElement(id), Supplier(to_string(IdMap::Insert(supplier).second)),
		Сlient(to_string(IdMap::Insert(client).second))
	{
	}
	string GetSupplier() const {
		return Supplier;
	}
	string GetСlient() const {
		return Сlient;
	}
private:
	const string Supplier;//Предоставляет функцию
	const string Сlient;//Получает/наследует функцию
};
/*
Класс для представления "классов" из uml, кроме того представляет интерфейсы
и другие классоподобные формы.
*/
class ClassTrans :public INumerableElement, public IName
{
public:
	ClassTrans(const string& id, const string& Name)
		:INumerableElement(id),  IName(Name)
	{
		IdMap::InputName(id, Name);
	}
	void AddValue(const ClassValueTrans& Value)//Добавление член-класса
	{
		int Type = atoi(Value.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		Values.push_back(Value);
	}
	void Inher(const string& Inher)//Предки, добавление
	{
		unsigned long Number = IdMap::Insert(Inher).second;
		Realize(Number);
		Inherit.push_back(to_string(Number));
	}
	void Realize(const int& Number)//Пополнение списка нужных для реализации
	{
		if (Number == GetLocalId()) return;
		if (NeedRealize.find(Number) == NeedRealize.end())
			NeedRealize.insert(Number);

	}
	string ToString() const//Вывод в строку, не формально
	{
		string ReturnOper = "";
		for (unsigned i = 0; i < Operations.size(); i++)
		{
			ReturnOper+= Operations[i].ToString();
		}
		ReturnOper += '\n';
		string ReturnValue = "";
		for (unsigned i = 0; i < Values.size(); i++)
		{
			ReturnValue += Values[i].ToString();
		}
		ReturnValue += '\n';
		string ReturnInher = "Ancient:";
		for (unsigned i = 0; i < Inherit.size(); i++)
		{
			ReturnInher += Inherit[i]+"\t";
		}
		return
			"Value:\n" + m_id.ToString() +
			Name.ToString() +
			ReturnValue +
			ReturnOper +
			ReturnInher+
			 "\n";
	}
	string ToCode() const//к представлению в код
	{
		string Return = "class "+ Name.ToCode();
		for (unsigned i = 0; i < Inherit.size(); i++)
		{
			if (i == 0)Return = Return + ":";
			Return +="public "+Inherit[i];
			if (i != Inherit.size() - 1) Return += ',';
		}
		Return += "\n{\n";
		for (unsigned i = 0; i < Operations.size(); i++)
		{
			Return +='\t'+ Operations[i].ToCode();
			if (Interface) Return += +" = 0 ;\n";
			else  Return += +";\n";
		}
		for (unsigned i = 0; i < Values.size(); i++)
		{
			Return +='\t'+ Values[i].ToCode();
		}
		Return += "};\n";
		for (unsigned i = 0; i < Values.size(); i++)
		{
			if (Values[i].GetStatic())
				Return += Values[i].AfterToCode(Name.ToCode());
		}
		//for (auto i =NeedRealize.cbegin(); i != NeedRealize.cend(); i++)
		//{
		//		Return += to_string(*i)+" ";
		//}
		return
			Return;
	}
	void AddCompos(const Assos& Assosiation)//Добавление композиции для нашего класса
	{
		vector<unsigned long> AssosList = Assosiation.GetTarget();
		vector<unsigned long> SourceList = Assosiation.GetSource();
		for (auto num :AssosList)
		{
			do {
				auto it = std::find_if(Values.begin(), Values.end(), [num](const ClassValueTrans& valueTrans)
					{ return valueTrans.GetLocalId() == num; });
				if (it != Values.end())
				{
					if (Assosiation.Type == "composite")//Композиция, на случай обработки
					{
						Values.erase(it);
					}
					else if (Assosiation.Type == "shared")//Агрегация, на случай обработки
					{
						Values.erase(it);
					}
				}
				else break;
			}
			while (true);
			//for (auto i = 0; i < Values.size(); ++i)
			//{
			//	if (Values[i].GetLocalId() == num)
			//	{
			//		if (Assosiation.Type == "composite")//Композиция
			//		{
			//			Values.erase(Values.cbegin()+i);
			//			i--;
			//			
			//		}
			//		else if (Assosiation.Type == "shared")//Агрегация
			//		{
			//			Values.erase(Values.cbegin() + i);
			//			i--;
			//		}
			//	}
			//}
		}
	}
	void AddRealiz(Realization NewRealiz)//Реализация по своей сути является наследованием
	{
		//Inher(NewRealiz.GetSupplier());
		Realize(stoi(NewRealiz.GetSupplier()));
		Inherit.push_back(NewRealiz.GetSupplier());
	}
	void AddOperation(const ClassOperTrans& Newbie)//Добавление операции в класс
	{
		Operations.push_back(Newbie);
		int Type = atoi(Newbie.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		for (auto needrealId : Newbie.NeedRealize)//Сравниваем нужные классы для функции и для нашего класса и дополняем, если нужно
		{
			if((NeedRealize.find(needrealId) == NeedRealize.end())&&(needrealId!=m_id.GetLocalId()))
				NeedRealize.insert(needrealId);
		}
	}
	void SetInterface() const
	{ 
		Interface = true;
		for (auto& Start : Operations)
			Start.SetVirtual();
	}//Наш класс интерфейс
	ClassOperTrans GetOperation(const string& id) const//Получение операции по id
	{
		return *std::find_if(Operations.begin(), Operations.end(), [id](const ClassOperTrans& valueTrans)
				{ return valueTrans.GetLocalId() == stoi(id); });
		//for (auto i = Operations.cbegin(); i != Operations.cend(); i++)
		//{
		//	if (to_string(i->GetLocalId()) == id)
		//		return *i;
		//}
	}
	void SetNum()//Функция для перехода от цифр к обозначениям из IdMap::IdName
	{
		for (unsigned i = 0; i < Operations.size(); i++)
		{
			Operations[i].SetNum();
		}
		for (unsigned i = 0; i < Values.size(); i++)
		{
			Values[i].SetNum();
		}
		for (int i = 0; i < Inherit.size(); i++)//Для классов от которых мы наследуем 
		{
			int J = atoi(Inherit[i].c_str());
			if (J != 0)
			{
				auto ThisType = IdMap::IdName.find(Inherit[i]);
				if (ThisType == IdMap::IdName.end()) return;
				Inherit[i]= ThisType->second;
			}
			else continue;
		}
	}
	set<unsigned long> GetRealize() const
	{
		return NeedRealize;
	}
	void  SetRealize(set<unsigned long> ChangedRealize) 
	{
		NeedRealize = ChangedRealize;
	}
private:
	mutable bool Interface=false;
	set<unsigned long> NeedRealize;//Классы которые нужны для использования нашего класса(не только предки), C++ строк к последовательности
	vector<ClassOperTrans> Operations;//Список операций
	vector<ClassValueTrans> Values;//Список класс-членов
	vector<string> Inherit;//Список предков - их номера
};
class LinkTrans :public INumerableElement
{
public:
	LinkTrans(const string& id, const string& From, const string& To)
		:INumerableElement(id), source(IdMap::Insert(From).second), target(IdMap::Insert(To).second)
	{
		//IdMap::InputName(id, Name);
	}
	void AddBody(const string& body) const
	{
		this->body = body;
	}
	string ToString() const
	{
		return "Link: " + m_id.ToString() + " "
			"Body:" + body + "\n";
	}
	bool operator<(const LinkTrans& _Right) const {
		return this->GetId() < _Right.GetId();
	}
	unsigned long GetTarget() const
	{
		return target;
	}
	unsigned long GetSource() const
	{
		return source;
	}
private:
	const unsigned long source;
	const unsigned long target;
	mutable string body;
};
class ActivityTrans :public INumerableElement,public IName
{
public:	
	ActivityTrans(const string& id, const int& Type, const string& Name)
		:INumerableElement(id),IName(Name)
	{
		switch (Type)
		{
			case 1:
			{
				m_type = ActivityType::action;
				break;
			}
			case 2:
			{
				m_type = ActivityType::fork;
				break;
			}
			case 3:
			{
				m_type = ActivityType::init;
				break;
			}
			case 4:
			{
				m_type = ActivityType::decigion;
				break;
			}
			case 5:
			{
				m_type = ActivityType::fin;
				break;
			}
			default:
			{
				cout << "Wrong format activity\n";
				break;
			}
		}
	}
	void AddBody(const string& Body)
	{
		this->Body = Body;
	}
	string ToString() const
	{
		string Output;
		Output = "Activity:\n" + m_id.ToString()+"\n";
		for (unsigned i = 0; i < outLinks.size(); i++)
			Output += outLinks[i].ToString();
		return Output +
			"Body:" + Body + "\n";
	}
	void AddLinkBody(const string& LinkId, const string& LinkBody)
	{
		auto linkIt=find_if(outLinks.begin(), outLinks.end(), [LinkId](const LinkTrans& link)
			{ return link.GetId() == LinkId; });
		linkIt->AddBody(LinkBody);
	}
	void AddOutgoing(const LinkTrans& Linker)
	{
		outLinks.push_back(Linker);
	}
	void AddIngoing(const LinkTrans& Linker)
	{
		inLinks.push_back(Linker);
	}
	vector<LinkTrans> GetOutLinks() const
	{
		return outLinks;
	}
	size_t GetOutCount() const
	{
		return outLinks.size();
	}
	vector<unsigned> GetOut() const
	{
		vector<unsigned> out;
		for (auto link : outLinks)
			out.push_back(link.GetTarget());
		return out;
	}
	vector<LinkTrans> GetInLinks() const
	{
		return inLinks;
	}
	vector<unsigned> GetIn() const
	{
		vector<unsigned> out;
		for (auto link : inLinks)
			out.push_back(link.GetSource());
		return out;
	}
	size_t GetInCount() const
	{
		return inLinks.size();
	}
	enum class ActivityType
	{
		action,
		fork,
		init,
		decigion,
		fin
	};
	void SetFin()
	{
		m_type = ActivityType::fin;
	}
	ActivityType GetType() const
	{
		return  m_type;
	}
private:

	ActivityType m_type;
	vector<LinkTrans> inLinks;
	vector<LinkTrans> outLinks;
	string Body;

};



