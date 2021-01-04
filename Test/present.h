#pragma once
#include <string>
#include <map>
#include <set>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>
typedef std::map<std::string, unsigned long> IdMapType;
using namespace std;
string RightFormat(string Input);
string Format(string Input);
/*
����� ������������� �� �������� ������� Id
������ ��������
XML id : id ���������� ���������

��� �� ������ ������� Id - ��� 
������ ��������
id ���������� ��������� : ��� �� �������
*/
class IdMap
{
public:
	static const IdMapType::value_type& Insert(const std::string& id)//���������� ������ Id
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
	static void InputName(const std::string& id,const std::string& Name)//���������� ������ �����
	{
		auto it =Insert(id);
		IdName.insert(make_pair(to_string(it.second), Name));
	}
	static unsigned long GetLocalId(const std::string& id)
	{
		return m_ids.at(id);
	}
	static void Print()//����� ������� ����
	{
		for (auto i = IdName.cbegin(); i != IdName.cend(); i++)
		{
			
			cout << i->first << " " << i->second << endl;
		}
	}
	static map<string, string>IdName;//������� ����
private:
	IdMap() {}

	static IdMapType m_ids;//������� id
	static unsigned long m_counter;//�������
};
/*
������������� Id �������
*/
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
	const IdMapType::value_type& m_valuePair;//Id �� XML ����� - Id ��������
};
/*
������� ������ Id
*/
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

/*
����� ��� ������������� ����������� ������
�������� � ������� true/false
*/
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
		std::string IsStatic = (elem_static) ? "static\t" : "";
		return IsStatic;
	}
	bool GetStatic() const
	{
		return elem_static;
	}
private:
	bool elem_static;
};
/*
������� ������ IStaticSkel
*/
class IStatic 
{
public:
	IStatic(const std::string& StaticType) :Elem_static(StaticType)
	{}
protected:
	IStaticSkel Elem_static;
};
/*
����� ��� ������������� ���� ���������
*/
class ITypeSkel //: public virtual IElement
{
private:
	std::string elem_type;

public:
	ITypeSkel() {}
	ITypeSkel(const std::string& type)
	{
		elem_type = RightFormat(type);
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
/*
������� ������ ITypeSkel
*/
class IType
{
protected: 
	ITypeSkel Elem_type;
public:
	IType(const std::string& type) :Elem_type(type) {}
};
/*
����� ��� ������������� ����� ��������
*/
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
	void SetName(string NewName)//���������� ���
	{
		Name = NewName;
	}
};
/*
������� ������ NameSkel
*/
class IName 
{
protected:
	NameSkel Name;
public:
	IName(const std::string& Name) :Name(Name)
	{}
};
/*
����� ��� �������������� ��������� {public,private,protected}
*/
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
/*
������� ������ IPublicSkel ��� ����������� ������������
*/
class IPublic
{
public:
	IPublic(const std::string& Public):Public(Public){}
protected:
	IPublicSkel Public;
};
/*
����� ��� ������������� �����-������
*/
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
	}
	const std::string ToCode() const//�������������
	{
		string Value="";
		if (!NotStated)//���� �� �������� �� ���������
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
	}
	const std::string AfterToCode(string ClassName) const//����� ����� ��� ������� �������� ����������� � �� ������ ����� ������ ������� ��� ��� ���������� ����������
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
	bool GetStatic() const//����������� ��� ���
	{
		return this->Elem_static.GetStatic();
	}
	void SetName(string NewName)//���������� ���
	{
		Name.SetName(NewName);
	}
	string GetType()//�������� ��� �������
	{
		return this->Elem_type.GetType();
	}
	void SetNum()//�������� �� ���� � ����� ����� �������
	{
		string Type = Elem_type.GetType();
		int J = atoi(Type.c_str());
		if (J != 0)
		{
			auto ThisType = IdMap::IdName.find(Type);
			if (ThisType == IdMap::IdName.end()) return;
			Elem_type.SetType(ThisType->second);
		}
	}
	ClassValueTrans& operator=(const ClassValueTrans& Buff)//������� ������ ��� ������ erase
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
	bool NotStated;//���� �� ��������� ��������
	string DefaultVal;//�������� ���������� ��������


};
/*
����� ��� ������������� ����-������� ������ uml
*/
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
	}
	const std::string ToCode() const//�������������
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
	void IsVirtual(string VirtualType)
	{ 
		if (VirtualType == "true") Virtual = true;
	}
	void SetVirtual()
	{
		 Virtual = true;
	}
	string GetType()//�������� ���
	{
		return this->Elem_type.GetType();
	}
	void AddElem(string Name, string Type)//�������� �������
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
	void AddElems(vector<string> Names, vector<string> Types)//�������� ��������
	{
		for (unsigned i = 0; i < Names.size(); i++)
		{
			AddElem(Names[i], Types[i]);
			Size++;
		}
	}
	void SetNum()//��������� �������� �������� � ��������
	{
		string Type = Elem_type.GetType();//������ ��� �������
		int J = atoi(Type.c_str());//���� ��� �����
		if (J != 0)
		{
			auto ThisType=IdMap::IdName.find(Type);
			Elem_type.SetType(ThisType->second);
		}
		for (int i = 0; i < Size; i++)
		{
			string ElemType = Elems[i].second;
			J = atoi(ElemType.c_str());//��� ����������
			if (J != 0)
			{
				auto ThisType = IdMap::IdName.find(ElemType);
				if (ThisType == IdMap::IdName.end()) return;
				Elems[i].second=ThisType->second;
			}
			else continue;
		}
	}

	set<unsigned long> NeedRealize;
private:
	bool Virtual = false;
	unsigned long Size = 0;//���-�� ���������
	std::vector<pair<string, string>> Elems;//�������� � �� ���
};
/*
����� ��� ������������� ��������� ���� ����������
��� �� ��� � ����������
������ �����������, ����� ���������� � ���
��� ���������� - ����� ���������� ��� ���� ������ ������
SourceId - id ���� ������
��� ���������� -
SourceId - id ������
��� ���������������� -
SourceId - ������ id
*/
class Assos :public INumerableElement, public IName
{
public:
	Assos(const std::string& id, const std::string& Name, const std::string& Source, const std::string& Target, const std::string& Type)
		:INumerableElement(id), IName(Name),Type(Type)
	{
		TargetId.push_back(IdMap::Insert(Target).second);
		SourceId.push_back(IdMap::Insert(Source).second);
		Counter = 1;
	}
	vector <unsigned long> GetSource() { return SourceId; }
	vector <unsigned long> GetTarget() { return TargetId; }
	void AddSource(const std::string& Source) 
	{
		SourceId.push_back(IdMap::Insert(Source).second); 
		Counter++;
	}
	void AddTarget(const std::string& Target)
	{ 
		TargetId.push_back(IdMap::Insert(Target).second); 
		Counter++;
	}
	bool IsMulti()//���������������� 
	{

	}
	string GetName() { return Name.ToCode(); }
	string Type = "";
private:
	unsigned Counter = 0;
	vector<unsigned long> SourceId;//��������� �������� ������� ����� ������������ ��� ������������� 
	vector<unsigned long> TargetId;//��������� ������� ������� ����� ������������ �������������
};
/*
����� ��� ������������� uml realization, �� ����� ���� �������� ������������� �������
*/
class Realization:public INumerableElement
{
public:
	Realization(const std::string& id, const std::string& supplier, const std::string& client)
		:INumerableElement(id)
	{
		Supplier = std::to_string(IdMap::Insert(supplier).second);
		�lient = std::to_string(IdMap::Insert(client).second);
	}
	string GetSupplier() {
		return Supplier;
	}
	string Get�lient() {
		return �lient;
	}
private:
	string Supplier;//������������� �������
	string �lient;//��������/��������� �������
};
/*
����� ��� ������������� "�������" �� uml, ����� ���� ������������ ����������
� ������ �������������� �����.
*/
class ClassTrans :public INumerableElement, public IName
{
public:
	ClassTrans(const std::string& id, const std::string& Name)
		:INumerableElement(id),  IName(Name)
	{
		IdMap::InputName(id, Name);
	}
	void AddValue(ClassValueTrans Value)//���������� ����-������
	{
		int Type = atoi(Value.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		Values.push_back(Value);
	}
	void Inher(const std::string& Inher)//������, ����������
	{
		unsigned long Number = IdMap::Insert(Inher).second;
		Realize(Number);
		Inherit.push_back(std::to_string(Number));

	}
	void Realize(int Number)//���������� ������ ������ ��� ����������
	{
		if (Number == m_id.GetLocalId()) return;
		if (NeedRealize.find(Number) == NeedRealize.end())
			NeedRealize.insert(Number);

	}
	const std::string ToString() const//����� � ������, �� ���������
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
	string ToCode() const//� ������������� � ���
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
		for (auto i =NeedRealize.cbegin(); i != NeedRealize.cend(); i++)
		{
				Return += to_string(*i)+" ";
		}
		return
			Return;
	}
	void AddCompos(Assos Assosiation)//���������� ���������� ��� ������ ������
	{
		vector<unsigned long> AssosList = Assosiation.GetTarget();
		vector<unsigned long> SourceList = Assosiation.GetSource();
		for (unsigned j = 0; j < AssosList.size(); j++)
		{
			unsigned long AssosElemName = AssosList[j];
			for (unsigned i = 0; i < Values.size(); i++)
			{
				if (Values[i].GetLocalId() == AssosElemName)
				{
					if (Assosiation.Type == "composite")//����������, ������ ��� ����������
					{
						Values.erase(Values.cbegin() + i);
						//Values[i].SetName(Assosiation.GetName());
						//for (auto Item = SourceList.cbegin(); Item != SourceList.cend(); Item++)
						//{
						//	Realize(boost::lexical_cast<int>(*Item));
						//}
					}
					else if (Assosiation.Type == "shared")//��������� ������� ���, ��� ������ ������� ��������� � ����� ������� ���������� ��� ���� ������
					{
						Values.erase(Values.cbegin()+i);
					}
				}
			}
		}
	}
	string GetId()
	{
		return m_id.GetId();
	}
	void AddRealiz(Realization NewRealiz)//���������� �� ����� ���� �������� �������������
	{
		Inherit.push_back(NewRealiz.GetSupplier());
	}
	void AddAssos(Assos Assosiation)//���������� ����������, ���� �������� � NeedRealize
	{
		vector<unsigned long> listNeed=Assosiation.GetSource();
		for(int i=0;i< listNeed.size();i++)
			NeedRealize.insert(listNeed[i]);
	}
	void AddOperation(ClassOperTrans Newbie)//���������� �������� � �����
	{
		Operations.push_back(Newbie);
		int Type = atoi(Newbie.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		for (auto i = Newbie.NeedRealize.cbegin(); i != Newbie.NeedRealize.cend(); i++)//���������� ������ ������ ��� ������� � ��� ������ ������ � ���������, ���� �����
		{
			if((NeedRealize.find(*i) == NeedRealize.end())&&(*i!=m_id.GetLocalId()))
				NeedRealize.insert(*i);
		}
	}
	void SetInterface() 
	{ 
		Interface = true;
		for (auto Start = Operations.begin(); Start != Operations.end(); Start++)
			Start->SetVirtual();
	}//��� ����� ���������
	ClassOperTrans GetOperation(string id)//��������� �������� �� id
	{
		for (unsigned i = 0; i < Operations.size(); i++)
		{
			if (to_string(Operations[i].GetLocalId()) == id)
				return Operations[i];
		}
	}
	void SetNum()//������� ��� �������� �� ���� � ������������ �� IdMap::IdName
	{
		for (unsigned i = 0; i < Operations.size(); i++)
		{
			Operations[i].SetNum();
		}
		for (unsigned i = 0; i < Values.size(); i++)
		{
			Values[i].SetNum();
		}
		for (int i = 0; i < Inherit.size(); i++)//��� ������� �� ������� �� ��������� 
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
private:
	bool Interface=false;
	set<unsigned long> NeedRealize;//������ ������� ����� ��� ������������� ������ ������(�� ������ ������), C++ ����� � ������������������
	vector<ClassOperTrans> Operations;//������ ��������
	vector<ClassValueTrans> Values;//������ �����-������
	vector<string> Inherit;//������ ������� - �� ������
};
class LinkTrans :public INumerableElement
{
public:
	LinkTrans(const std::string& id, const std::string& From, const std::string& To)
		:INumerableElement(id)
	{
		this->From = IdMap::Insert(From).second;
		this->To = IdMap::Insert(To).second;
		//IdMap::InputName(id, Name);
	}
	void AddBody(string Body)
	{
		this->Body = Body;
	}
	string GetId()
	{
		return m_id.GetId();
	}
	string ToString()
	{
		return "Link: " + m_id.ToString() + " "
			"Body:" + Body + "\n";
	}
	bool operator<(const LinkTrans& _Right) const {
		return this->m_id.GetId() < _Right.m_id.GetId();
	}
private:
	unsigned long From;
	unsigned long To;
	string Body;
};
class ActivityTrans :public INumerableElement
{
public:
	ActivityTrans(const std::string& id, const std::string& Type)
		:INumerableElement(id),m_type(Type)
	{}
	void AddBody(string Body)
	{
		this->Body = Body;
	}
	string ToString()
	{
		string Output;
		Output = "Activity:\n" + m_id.ToString()+"\n";
		for (unsigned i = 0; i < Links.size(); i++)
			Output += Links[i].ToString();
		return Output +
			"Body:" + Body + "\n";
	}
	void AddLinkBody(string LinkId,string LinkBody)
	{
		for (unsigned i = 0; i < Links.size(); i++)
			if (Links[i].GetId() == LinkId)
			{
				Links[i].AddBody(LinkBody);
				return;
			}
	}
	string GetId()
	{
		return m_id.GetId();
	}
	void AddOutgoing(LinkTrans Linker)
	{
		Links.push_back(Linker);
	}
private:
	const std::string m_type;
	vector<LinkTrans> Links;
	string Body;
};
