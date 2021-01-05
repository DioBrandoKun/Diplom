#pragma once
#include <string>
#include <map>
#include <set>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;
typedef map<string, unsigned long> IdMapType;
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
	static const IdMapType::value_type& Insert(const string& id)//���������� ������ Id
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
	static const void InputName(const string& id,const string& Name)//���������� ������ �����
	{
		auto it =Insert(id);
		IdName.insert(make_pair(to_string(it.second), Name));
	}
	static unsigned long GetLocalId(const string& id)
	{
		return m_ids.at(id);
	}
	static const void Print()//����� ������� ����
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
	const IdMapType::value_type& m_valuePair;//Id �� XML ����� - Id ��������
};
/*
������� ������ Id
*/
class INumerableElement //Enum
{
public:
	INumerableElement(const string& id)
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
������� ������ IStaticSkel
*/
class IStatic 
{
public:
	IStatic(const string& StaticType) :Elem_static(StaticType)
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
	mutable string elem_type;

public:
	ITypeSkel() {}
	ITypeSkel(const string& type)
	{
		elem_type = RightFormat(type);
	}
	void SetType(const string Type) const
	{
		elem_type = Type;
	}
	string GetType() const
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
������� ������ ITypeSkel
*/
class IType
{
protected: 
	ITypeSkel Elem_type;
public:
	IType(const string& type) :Elem_type(type) {}
};
/*
����� ��� ������������� ����� ��������
*/
class NameSkel
{
private:
	 mutable string Name;

public:
	NameSkel() {}
	NameSkel(const string& Name):Name(Name)
	{}

	string ToString() const
	{
		return "Name=\t"+Name+"\t";
	}
	string ToCode() const
	{
		return  Name;
	}
	void SetName(string NewName) const //���������� ���
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
	IName(const string& Name) :Name(Name)
	{}
};
/*
����� ��� �������������� ��������� {public,private,protected}
*/
class IPublicSkel
{
public:
	IPublicSkel(const string& Public) :Public(Public) {}
	string ToString() const
	{
		return "Public=\t" + Public + "\t";
	}
	string ToCode() const
	{
		return Public + ':';
	}
private:
	 string Public;
};
/*
������� ������ IPublicSkel ��� ����������� ������������
*/
class IPublic
{
public:
	IPublic(const string& Public):Public(Public){}
protected:
	IPublicSkel Public;
};
/*
����� ��� ������������� �����-������
*/
class ClassValueTrans:public INumerableElement, public IStatic,public IType,public IName,public IPublic
{
public:
	ClassValueTrans(const string& id, const string& Name, const string& StaticType, const string& ElemType, const string& DefaulValue, const string& Public) :INumerableElement(id),
	IStatic(StaticType), IType(ElemType), DefaultVal(DefaulValue), IName(Name), IPublic(Public)
	{
		IdMap::InputName(id, Name);
		(DefaulValue == "")?NotStated = true:NotStated = false;			
	}

	const string ToString() const
	{
		return
			"Value:\n" + m_id.ToString()+
			 Elem_type.ToString()+
			 Elem_static.ToString()+
			 Name.ToString()+
			 Public.ToString()+
			"Default=\t"+ DefaultVal+"\n";
	}
	const string ToCode() const//�������������
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
	const string AfterToCode(string ClassName) const//����� ����� ��� ������� �������� ����������� � �� ������ ����� ������ ������� ��� ��� ���������� ����������
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
	void SetName(string NewName) const//���������� ���
	{
		Name.SetName(NewName);
	}
	string GetType() const//�������� ��� �������
	{
		return this->Elem_type.GetType();
	}
	void SetNum() const//�������� �� ���� � ����� ����� �������
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
	ClassOperTrans(const string& id, const string& Name, const string& StaticType, const string& ElemType, const string& Public)
		:INumerableElement(id),IStatic(StaticType), IType(ElemType), IName(Name), IPublic(Public)
	{
		IdMap::InputName(id, Name);
	}
	const string ToString() const
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
	const string ToCode() const//�������������
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
	void IsVirtual(string VirtualType) const
	{ 
		if (VirtualType == "true") Virtual = true;
		else Virtual = false;
	}
	void SetVirtual() const //����� ��� �����������
	{
		 Virtual = true;
	}
	string GetType() const//�������� ���
	{
		return this->Elem_type.GetType();
	}
	void AddElem(string Name, string Type) //�������� �������
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
	void SetNum()  //��������� �������� �������� � ��������
	{
		string Type = Elem_type.GetType();//������ ��� �������
		int J = atoi(Type.c_str());//���� ��� �����
		if (J != 0)
		{
			auto ThisType=IdMap::IdName.find(Type);
			Elem_type.SetType(ThisType->second);
		}
		for (unsigned long i = 0; i < Size; i++)
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
	mutable bool Virtual = false;
	unsigned long Size = 0;//���-�� ���������
	vector<pair<string, string>> Elems;//�������� � �� ���
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
	string GetName() const { return Name.ToCode(); }
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
	Realization(const string& id, const string& supplier, const string& client)
		:INumerableElement(id)
	{
		Supplier = to_string(IdMap::Insert(supplier).second);
		�lient = to_string(IdMap::Insert(client).second);
	}
	string GetSupplier() const {
		return Supplier;
	}
	string Get�lient() const {
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
	ClassTrans(const string& id, const string& Name)
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
	void Inher(const string& Inher)//������, ����������
	{
		unsigned long Number = IdMap::Insert(Inher).second;
		Realize(Number);
		Inherit.push_back(to_string(Number));
	}
	void Realize(int Number)//���������� ������ ������ ��� ����������
	{
		if (Number == m_id.GetLocalId()) return;
		if (NeedRealize.find(Number) == NeedRealize.end())
			NeedRealize.insert(Number);

	}
	string ToString() const//����� � ������, �� ���������
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
			for (auto i = Values.cbegin(); i != Values.cend(); i++)
			{
				if (i->GetLocalId() == AssosElemName)
				{
					if (Assosiation.Type == "composite")//����������
					{
						Values.erase(i);
					}
					else if (Assosiation.Type == "shared")//���������
					{
						Values.erase(i);
					}
				}
			}
		}
	}
	string GetId() const
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
	void SetInterface() const
	{ 
		Interface = true;
		for (auto Start = Operations.cbegin(); Start != Operations.cend(); Start++)
			Start->SetVirtual();
	}//��� ����� ���������
	ClassOperTrans GetOperation(const string id) const//��������� �������� �� id
	{
		for (auto i = Operations.cbegin(); i != Operations.cend(); i++)
		{
			if (to_string(i->GetLocalId()) == id)
				return *i;
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
	mutable bool Interface=false;
	set<unsigned long> NeedRealize;//������ ������� ����� ��� ������������� ������ ������(�� ������ ������), C++ ����� � ������������������
	vector<ClassOperTrans> Operations;//������ ��������
	vector<ClassValueTrans> Values;//������ �����-������
	vector<string> Inherit;//������ ������� - �� ������
};
class LinkTrans :public INumerableElement
{
public:
	LinkTrans(const string& id, const string& From, const string& To)
		:INumerableElement(id), From(IdMap::Insert(From).second),To(IdMap::Insert(To).second)
	{
		//IdMap::InputName(id, Name);
	}
	void AddBody(string Body) const
	{
		this->Body = Body;
	}
	string GetId() const
	{
		return m_id.GetId();
	}
	string ToString() const
	{
		return "Link: " + m_id.ToString() + " "
			"Body:" + Body + "\n";
	}
	bool operator<(const LinkTrans& _Right) const {
		return this->m_id.GetId() < _Right.m_id.GetId();
	}
private:
	const unsigned long From;
	const unsigned long To;
	mutable string Body;
};
class ActivityTrans :public INumerableElement
{
public:
	ActivityTrans(const string& id, const string& Type)
		:INumerableElement(id),m_type(Type)
	{}
	void AddBody(string Body) const
	{
		this->Body = Body;
	}
	string ToString() const
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
	string GetId() const
	{
		return m_id.GetId();
	}
	void AddOutgoing(LinkTrans Linker)
	{
		Links.push_back(Linker);
	}
private:
	const string m_type;
	vector<LinkTrans> Links;
	mutable string Body;
};
