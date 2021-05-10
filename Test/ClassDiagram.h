#pragma once
#include "present.h"
/*
����� ��� ������������� �����-������
*/
class ClassValueTrans :public INumerableElement, public IStatic, public IType, public IName, public IPublic
{
public:
	ClassValueTrans(const string& id, const string& Name, const string& StaticType, const string& ElemType, const string& DefaulValue, const string& Public, const string& constType) :INumerableElement(id),
		IStatic(StaticType), IType(ElemType), m_DefaultVal(DefaulValue), IName(Name), IPublic(Public), m_isConst(constType == "true" ? "const " : "")
	{
		IdMap::InputName(id, Name);								//����� � ���������� ����� ������ ����� � IName
		(DefaulValue == "") ? m_NotStated = true : m_NotStated = false;
	}
	string ToString() const					//����� ��� ������
	{
		return
			"Value:\n" + m_id.ToString() +
			m_Elem_type.ToString() +
			m_Elem_static.ToString() +
			m_Name.ToString() +
			m_Public.ToString() +
			"Default=\t" + m_DefaultVal + "\n";
	}
	string ToCode() const					//�������� ����
	{
		string Value = "";
		if (!m_NotStated)					//���� �� �������� �� ���������
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
	string AfterToCode(const string& ClassName) const//����� ����� ��� ������� �������� ����������� � �� ������ ����� ������ ������� ��� ��� ���������� ����������
	{//�������� �� �������� �������� ����� ����� �� ToCode � AfterToCode ����� �� �������������
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
	void SetNum() const									//��������� �������� �������� � ��������
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
	ClassValueTrans& operator=(const ClassValueTrans& Buff)//�������� ��� ������ .erase
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
	bool			m_NotStated;		//���� �� ��������� ��������
	string			m_DefaultVal;		//�������� ���������� ��������
	const string	m_isConst;			//����������� �� ��� ������, � ����������, ���� UMl �������� ��������� ������ ������������ �������� ���������� ��������� ����� Static
	//�� Static-Const

};
/*
����� ��� ������������� ����-������� ������ uml
*/
class ClassOperTrans :public INumerableElement, public IStatic, public IType, public IName, public IPublic
{
public:
	ClassOperTrans(const string& id, const string& Name, const string& StaticType, const string& ElemType, const string& Public)
		:INumerableElement(id), IStatic(StaticType), IType(ElemType), IName(Name), IPublic(Public)
	{
		IdMap::InputName(id, Name);			//����� � ���������� ����� ������ ����� � IName
	}
	string ToString() const									//����� ��� ������
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
	string ToCode() const									//�������� ����
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
	void placeVirtual(const string& VirtualType) const			//������������� �������������
	{
		if (VirtualType == "true") m_Virtual = true;
		else m_Virtual = false;
	}
	void SetVirtual() const									//������� ��� �������� ������� ���������� � �����������
	{
		m_Virtual = true;
	}
	void AddElem(const string& Name, const string& Type)					//�������� ���� �������
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
	void AddElems(const vector<string>& Names, const vector<string>& Types)	//�������� ������ ���������
	{
		for (unsigned i = 0; i < Names.size(); i++)
		{
			AddElem(Names[i], Types[i]);
			m_Size++;
		}
	}
	void SetNum()										//��������� �������� �������� � ��������
	{
		string Type = GetType();						//������ ��� �������
		int J = atoi(Type.c_str());						//���� ��� �����
		if (J != 0)
		{
			auto ThisType = IdMap::m_IdName.find(Type);
			SetType(ThisType->second);
		}
		for (unsigned long i = 0; i < m_Size; i++)
		{
			string ElemType = m_Elems[i].second;
			J = atoi(ElemType.c_str());					//��� ����������
			if (J != 0)
			{
				auto ThisType = IdMap::m_IdName.find(ElemType);
				if (ThisType == IdMap::m_IdName.end()) return;
				m_Elems[i].second = ThisType->second;
			}
			else continue;
		}
	}
	set<unsigned long>				m_NeedRealize;			//����������� ������ ��� dataType ��� ������, ������� ������������
private:
	mutable bool					m_Virtual = false;		//����� �����������
	unsigned long					m_Size = 0;				//���-�� ��������� ������
	vector<pair<string, string>>	m_Elems;				//��������� ��������� ������
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
		:INumerableElement(id), IName(Name), m_Type(Type)
	{
		m_TargetId.push_back(IdMap::Insert(Target).second);
		m_SourceId.push_back(IdMap::Insert(Source).second);
		m_Counter = 1;
	}
	vector <unsigned long> GetSource() const	//�������� ������ ������ ����������
	{ 
		return m_SourceId; 
	}
	vector <unsigned long> GetTarget() const	//�������� ����� ����������� ����������
	{ 
		return m_TargetId; 
	}
	void AddSource(const string& Source)		//�������� ����� �� �������� ������� ����������
	{
		m_SourceId.push_back(IdMap::Insert(Source).second);
		m_Counter++;
	}
	void AddTarget(const string& Target)		//�������� ����� ������� ��������� ����������
	{
		m_TargetId.push_back(IdMap::Insert(Target).second);
		m_Counter++;
	}
	string m_Type = "";							//��� ����������
private:
	unsigned m_Counter = 0;						//���������� ���� �������, ������������ ����������
	vector<unsigned long> m_SourceId;			//��������� �������� ������� ����� ������������ ��� ������������� 
	vector<unsigned long> m_TargetId;			//��������� ������� ������� ����� ������������ �������������
};
/*
����� ��� ������������� uml realization, �� ����� ���� �������� ������������� �������
*/
class Realization :public INumerableElement
{
public:
	Realization(const string& id, const string& supplier, const string& client)
		:INumerableElement(id), m_Supplier(to_string(IdMap::Insert(supplier).second)),
		m_�lient(to_string(IdMap::Insert(client).second))
	{
	}
	string GetSupplier() const {			//�������� ������
		return m_Supplier;
	}
	string Get�lient() const {				//�������� ����������
		return m_�lient;
	}
private:
	const string m_Supplier;				//����� ������� �������� �������
	const string m_�lient;					//����� ����������� ����������
};
/*
����� ��� ������������� "�������" �� uml, ����� ���� ������������ ����������
� ������ �������������� �����.
*/
class ClassTrans :public INumerableElement, public IName
{
public:
	ClassTrans(const string& id, const string& Name)
		:INumerableElement(id), IName(Name)
	{
		IdMap::InputName(id, Name);				//����� � ���������� ����� ������ ����� � IName
	}
	void AddValue(const ClassValueTrans& Value)		//���������� ����-������
	{
		int Type = atoi(Value.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		m_Values.push_back(Value);
	}
	void Inher(const string& Inher)					//���������� ������
	{
		unsigned long Number = IdMap::Insert(Inher).second;
		Realize(Number);
		m_Inherit.push_back(to_string(Number));
	}
	void Realize(const int& Number)					//���������� ������ ������ ��� ����������
	{
		if (Number == GetLocalId()) return;
		if (m_NeedRealize.find(Number) == m_NeedRealize.end())
			m_NeedRealize.insert(Number);

	}
	string ToString() const							//����� � ������, �� ���������
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
	string ToCode() const						//����� ���� ������
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
	void AddCompos(const Assos& Assosiation)			//���������� ���������� ��� ������ ������
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
					if (Assosiation.m_Type == "composite")//����������, �� ������ ���������
					{
						m_Values.erase(it);
					}
					else if (Assosiation.m_Type == "shared")//���������, �� ������ ���������
					{
						m_Values.erase(it);
					}
				}
				else break;
			} while (true);
		}
	}
	void AddRealiz(Realization& NewRealiz)			//���������� ���������� �� ����� ���� �������� �������������
	{
		Realize(stoi(NewRealiz.GetSupplier()));
		m_Inherit.push_back(NewRealiz.GetSupplier());
	}
	void AddOperation(const ClassOperTrans& Newbie)	//���������� �������� � �����
	{
		m_Operations.push_back(Newbie);
		int Type = atoi(Newbie.GetType().c_str());
		if (Type != 0)
		{
			Realize(Type);
		}
		for (auto needrealId : Newbie.m_NeedRealize)	//���������� ������ ������ ��� ������� � ��� ������ ������ � ���������, ���� �����
		{
			if ((m_NeedRealize.find(needrealId) == m_NeedRealize.end()) && (needrealId != m_id.GetLocalId()))
				m_NeedRealize.insert(needrealId);
		}
	}
	void SetInterface() const						//��������� ��� ����� � ���������					
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
		for (int i = 0; i < m_Inherit.size(); i++)//��� ������� �� ������� �� ��������� 
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
	set<unsigned long>* GetRealize()					//�������� ���������, �� ������ ������� ����� �����������
	{
		return &m_NeedRealize;
	}
private:
	mutable bool			m_Interface = false;		//����� �������� ����������� (����������� �������)
	set<unsigned long>		m_NeedRealize;				//������ ������� ����� ��� ������������� ������ ������(�� ������ ������), C++ ����� � ������������������
	vector<ClassOperTrans>	m_Operations;				//������ ��������
	vector<ClassValueTrans> m_Values;					//������ �����-������
	vector<string>			m_Inherit;				//������ ������� - �� ������
};
