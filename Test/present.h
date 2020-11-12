#pragma once
#include <string>
#include <map>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
typedef std::map<std::string, unsigned long> IdMapType;
using namespace std;
class IdMap
{
public:
	static const IdMapType::value_type& Insert(const std::string& id)
	{
		auto it = m_ids.find(id);
		if (it == m_ids.end())
		{
			return *(m_ids.insert(std::make_pair(id, m_counter++)).first);
		}
		else
		{
			return *it;
		}
	}
	static void InputName(const std::string& id,const std::string& Name)
	{
		auto it =Insert(id);
		IdName.insert(make_pair(to_string(it.second), Name));
	}
	static unsigned long GetLocalId(const std::string& id)
	{
		return m_ids.at(id);
	}
	static void Print()
	{
		for (auto i = IdName.cbegin(); i != IdName.cend(); i++)
		{
			
			cout << i->first << " " << i->second << endl;
		}
	}
	static map<string, string>IdName;
private:
	IdMap() {}

	static IdMapType m_ids;
	static unsigned long m_counter;
};


class Id
{
public:
	Id(const std::string& id) : m_valuePair(IdMap::Insert(id))
	{}

	unsigned long GetLocalId() const
	{
		return m_valuePair.second;
	}

	const std::string& GetId() const
	{
		return m_valuePair.first;
	}

	std::string ToString() const
	{
		return
			"xmi:id=\"" + m_valuePair.first + "\"; " +
			"localId=" + std::to_string(m_valuePair.second) + "\n";
	}

private:
	const IdMapType::value_type& m_valuePair;
};

class INumerableElement //Enum
{
public:
	INumerableElement(const std::string& id)
		: m_id(id)
	{}

	unsigned int GetLocalId() const
	{
		return m_id.GetLocalId();
	}

protected:
	const Id m_id;
};


class IStaticSkel
{
public:
	IStaticSkel(const std::string& StaticType)
	{
		if (StaticType == "false")
			elem_static = false;
		else if (StaticType == "true")
			elem_static = true;
	}
	std::string ToString() const
	{
		std::string IsStatic = (elem_static) ? "static\t" : "not static\t";
		return "Static=\t"+ IsStatic;
	}
	std::string ToCode() const
	{
		std::string IsStatic = (elem_static) ? "static " : "";
		return IsStatic;
	}
	bool GetStatic() const
	{
		return elem_static;
	}
private:
	bool elem_static;
};

class IStatic //Enum
{
public:
	IStatic(const std::string& StaticType) :Elem_static(StaticType)
	{}
protected:
	IStaticSkel Elem_static;
};

class ITypeSkel //: public virtual IElement
{
private:
	std::string elem_type;

public:
	ITypeSkel() {}
	ITypeSkel(const std::string& type)
	{
		int pos = type.find("EAJava_");
		if (pos != -1)
		{
			elem_type = type.substr(strlen("EAJava_"));
			while (elem_type.find('_') != string::npos)
			{
				cout << elem_type.find('_');
				elem_type[elem_type.find('_')] = '*';
			}
		}
		else
		{
			elem_type = std::to_string(IdMap::Insert(type).second);
		}
	}
	void SetType(string Type)
	{
		elem_type = Type;
	}
	string GetType()
	{
		return elem_type;
	}
	std::string ToString() const
	{
		return "Type=\t"+elem_type + "\t";
	}
	std::string ToCode() const
	{
		return elem_type+'\t';
	}
};

class IType
{
protected: 
	ITypeSkel Elem_type;
public:
	IType(const std::string& type) :Elem_type(type) {}
};

class NameSkel
{
private:
	 std::string Name;

public:
	NameSkel() {}
	NameSkel(const std::string& Name):Name(Name)
	{}

	std::string ToString() const
	{
		return "Name=\t"+Name+"\t";
	}
	std::string ToCode() const
	{
		return  Name;
	}
	void SetName(string NewName)
	{
		Name = NewName;
	}
};

class IName 
{
protected:
	NameSkel Name;
public:
	IName(const std::string& Name) :Name(Name)
	{}
};

class IPublicSkel
{
public:
	IPublicSkel(const std::string& Public) :Public(Public) {}
	std::string ToString() const
	{
		return "Public=\t" + Public + "\t";
	}
	std::string ToCode() const
	{
		return Public + ':';
	}
private:
	std::string Public;
};

class IPublic
{
public:
	IPublic(const std::string& Public):Public(Public){}
protected:
	IPublicSkel Public;
};

class ClassValueTrans:public INumerableElement, public IStatic,public IType,public IName,public IPublic
{
public:
	ClassValueTrans(const std::string& id, const std::string& Name, const std::string& StaticType, const std::string& ElemType, const std::string& DefaulValue, const std::string& Public) :INumerableElement(id),
	IStatic(StaticType), IType(ElemType), DefaultVal(DefaulValue), IName(Name), IPublic(Public)
	{
		IdMap::InputName(id, Name);
		(DefaulValue == "")?NotStated = true:NotStated = false;			
	}

	const std::string ToString() const
	{
		return
			"Value:\n" + m_id.ToString()+
			 Elem_type.ToString()+
			 Elem_static.ToString()+
			 Name.ToString()+
			 Public.ToString()+
			"Default=\t"+ DefaultVal+"\n";
		/*"\t" + GetVisibilityString(m_visibility);*/
	}
	const std::string ToCode() const
	{
		string Value="";
		if (!NotStated)
		{
			if(!this->Elem_static.GetStatic())
			Value="="+DefaultVal;
		}
		return
			Public.ToCode() +
			Elem_static.ToCode() +
			Elem_type.ToCode() +
			Name.ToCode() +
			Value + ";\n";
		/*"\t" + GetVisibilityString(m_visibility);*/
	}
	const std::string AfterToCode(string ClassName) const
	{
		string Value = "";
		if (!NotStated)
		{
			Value = "=" + DefaultVal;
		}
		return
			Elem_type.ToCode() +
			ClassName+"::"+
			Name.ToCode()+Value+";\n";
	}
	bool GetStatic() const
	{
		return this->Elem_static.GetStatic();
	}
	void SetName(string NewName)
	{
		Name.SetName(NewName);
	}
	void SetNum()
	{
		string Type = Elem_type.GetType();
		int J = atoi(Type.c_str());
		if (J != 0)
		{
			auto ThisType = IdMap::IdName.find(Type);
			Elem_type.SetType(ThisType->second);
		}
	}
private:
	bool NotStated;
	const std::string DefaultVal;
};
class ClassOperTrans :public INumerableElement, public IStatic, public IType, public IName, public IPublic
{
public:
	ClassOperTrans(const std::string& id, const std::string& Name, const std::string& StaticType, const std::string& ElemType, const std::string& Public)
		:INumerableElement(id),IStatic(StaticType), IType(ElemType), IName(Name), IPublic(Public)
	{
		IdMap::InputName(id, Name);
	}
	const std::string ToString() const
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
		/*"\t" + GetVisibilityString(m_visibility);*/
	}
	const std::string ToCode() const
	{
		string ReturnValue = "";
		for (unsigned i = 0; i < Elems.size(); i++)
		{
			ReturnValue = ReturnValue + Elems[i].second+" " + Elems[i].first;
			if (i != Elems.size() - 1) ReturnValue += ',';
		}
		return
			Public.ToCode() +
			Elem_static.ToCode() +
			Elem_type.ToCode() +
			Name.ToCode() +"("+
			ReturnValue+")" + ";\n";
	}
	void AddElem(string Name, string Type)
	{
		string Typer;
		int pos = Type.find("EAJava_");
		if (pos != -1)
		{
			Typer = Type.substr(strlen("EAJava_"));
			while (Typer.find('_') != string::npos)
			{
				cout << Typer.find('_');
				Typer[Typer.find('_')] = '*';
			}
		}
		else
		{
			Typer = std::to_string(IdMap::Insert(Type).second);
		}
		Elems.push_back(pair<string, string>(Name, Typer));
	}
	void AddElems(vector<string> Names, vector<string> Types)
	{
		for (unsigned i = 0; i < Names.size(); i++)
		{
			AddElem(Names[i], Types[i]);
			Size++;
		}
	}
	void SetNum()
	{
		string Type = Elem_type.GetType();//узнаем тип функции
		int J = atoi(Type.c_str());//если это число
		if (J != 0)
		{
			auto ThisType=IdMap::IdName.find(Type);
			Elem_type.SetType(ThisType->second);
		}
		for (int i = 0; i < Size; i++)
		{
			string ElemType = Elems[i].second;
			J = atoi(ElemType.c_str());//тип параметров
			if (J != 0)
			{
				auto ThisType = IdMap::IdName.find(ElemType);
				Elems[i].second=ThisType->second;
			}
			else continue;
		}
	}
private:
	unsigned long Size = 0;
	std::vector<pair<string, string>> Elems;//Элементы и их тип
};

class Assos :public INumerableElement, public IName
{
public:
	Assos(const std::string& id, const std::string& Name, const std::string& Class, const std::string& Elem)
		:INumerableElement(id), IName(Name)
	{
		ElemId = std::to_string(IdMap::Insert(Elem).second);
		ClassId = std::to_string(IdMap::Insert(Class).second);
	}
	string GetClass() { return ClassId; }
	string GetElem() { return ElemId; }
	string GetName() { return Name.ToCode(); }
private:
	string ClassId;
	string ElemId;
};

class ClassTrans :public INumerableElement, public IName
{
public:
	ClassTrans(const std::string& id, const std::string& Name)
		:INumerableElement(id),  IName(Name)
	{
		IdMap::InputName(id, Name);
	}
	void AddOper(ClassOperTrans Oper)
	{
		Operations.push_back(Oper);
	}
	void AddValue(ClassValueTrans Value)
	{
		Values.push_back(Value);
	}
	void Inher(const std::string& Inher)//предки
	{
		string Number = std::to_string(IdMap::Insert(Inher).second);
		Inherit.push_back(Number);
	}
	const std::string ToString() const
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
	const std::string ToCode() const
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
		return
			Return;
		/*"\t" + GetVisibilityString(m_visibility);*/
	}
	void PutAssos(Assos Assosiation)
	{
		for (unsigned i = 0; i < Values.size(); i++)
		{
			if (to_string(Values[i].GetLocalId()) == Assosiation.GetElem())
			{
				Values[i].SetName("\t"+Assosiation.GetName());
			}
		}
	}
	void AddOperation(ClassOperTrans Newbie)
	{
		Operations.push_back(Newbie);
	}
	ClassOperTrans GetOperation(string id)
	{
		for (unsigned i = 0; i < Operations.size(); i++)
		{
			if (to_string(Operations[i].GetLocalId()) == id)
				return Operations[i];
		}
	}
	void SetNum()
	{
		for (unsigned i = 0; i < Operations.size(); i++)
		{
			Operations[i].SetNum();
		}
		for (unsigned i = 0; i < Values.size(); i++)
		{
			Values[i].SetNum();
		}
		for (int i = 0; i < Inherit.size(); i++)
		{
			int J = atoi(Inherit[i].c_str());
			if (J != 0)
			{
				auto ThisType = IdMap::IdName.find(Inherit[i]);
				Inherit[i]= ThisType->second;
			}
			else continue;
		}
	}
private:
	vector<ClassOperTrans> Operations;
	vector<ClassValueTrans> Values;
	vector<string> Inherit;
};