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
	static const IdMapType::value_type& Insert(const string& id)//���������� ������ Id ��� ����� �������������
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
	static void InputName(const string& id,const string& Name)//���������� ������ �����
	{
		auto it =Insert(id);
		m_IdName.insert(make_pair(to_string(it.second), Name));
	}
	static unsigned long GetLocalId(const string& id)
	{
		return m_ids.at(id);
	}
	static void Print()										//����� ���������� ������� ����
	{
		for (auto i: m_IdName)
		{		
			cout << i.first << " " << i.second << endl;
		}
	}
	static map<const string, const string>	m_IdName;		//������� id - ��� ��������
private:
	IdMap() {}

	static IdMapType						m_ids;			//������� id - xmi id ��������
	static unsigned long					m_counter;		//������� ��������
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
�������������� �������� ���������
*/
class INumerableElement 
{
public:
	INumerableElement(const string& id)
		: m_id(id)
	{}
	const string& GetId() const			//���������� ������ �� id xmi-������� �������
	{
		return m_id.GetId();
	}
	unsigned int GetLocalId() const		//���������� ��������� �������� id
	{
		return m_id.GetLocalId();
	}
protected:
	const Id m_id;						//������ ��������� id �������
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
	bool GetStatic() const						//��������� ���������� � �����������
	{
		return m_elem_staticSkel;
	}
private:
	 bool m_elem_staticSkel=false;				//������ ���������� � ����������� �������
};
/*
������� ������ IStaticSkel
*/
class IStatic 
{
public:
	IStatic(const string& StaticType) :m_Elem_static(StaticType)
	{}
	bool GetStatic() const						//��������� ���������� � ����������� ��������
	{
		return m_Elem_static.GetStatic();
	}
protected:
	IStaticSkel m_Elem_static;					//������ �������� ����������� �������
};	
/*
����� ��� ������������� ���� ���������
*/
class ITypeSkel //: public virtual IElement
{
private:
	mutable string m_elem_typeSkel;				//������ ��� �������

public:
	ITypeSkel() {}
	ITypeSkel(const string& type)
	{
		m_elem_typeSkel = RightFormat(type);
	}
	void SetType(const string& Type) const		//��������� ����
	{
		m_elem_typeSkel = Type;
	}
	string& GetType() const						//��������� ����
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
������� ������ ITypeSkel
*/
class IType
{
protected: 
	ITypeSkel m_Elem_type;						//������ ��������� ���� �������
public:
	string GetType() const						//��������� � ���� ������
	{
		return m_Elem_type.GetType();
	}
	void SetType(const string& Type) const		//��������� ���� �������
	{
		m_Elem_type.SetType(Type);
	}
	IType(const string& type) :m_Elem_type(type) {}
};
/*
����� ��� ������������� ����� ��������
*/
class INameSkel
{
private:
	 string m_NameSkel;						//������ ��� �������

public:
	INameSkel() {}
	INameSkel(const string& Name):m_NameSkel(Name)
	{}

	string ToString() const					//����������� ��������� ����� (�� ����� ����������)
	{
		return "Name=\t"+m_NameSkel+"\t";
	}
	string ToCode() const
	{
		return  m_NameSkel;
	}
	void SetName(const string& NewName)		//���������� ��� ��������
	{
		m_NameSkel = NewName;
	}	
	string GetName() const					//�������� ��� ��������
	{
		return m_NameSkel;
	}
};
/*
������� ������ NameSkel
*/
class IName 
{
protected:
	INameSkel m_Name;							//������ ��������� �����
public:
	IName(const string& Name) :m_Name(Name)
	{}
	void SetName(const string& NewName)			//��������� ����� �������
	{
		m_Name.SetName(NewName);
	}		
	string GetName() const						//��������� ����� �������
	{
		return m_Name.GetName();
	}
};
/*
����� ��� �������������� ��������� {public,private,protected}
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
	}m_PublicSkel;		//������ ��� ����������� �������
	/*UML ����� ��� 4 ��������� - Package, ��� ��� ������ ���������� ������� 
	  public ������������ ��������� �� ���� ���������*/
};
/*
������� ������ IPublicSkel ��� ����������� ������������
*/
class IPublic
{
public:
	IPublic(const string& Public):m_Public(Public){}
protected:
	IPublicSkel m_Public;
};