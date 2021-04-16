#pragma once
#include "present.h"
class LinkTrans :public INumerableElement
{
public:
	LinkTrans(const string& id, const string& From, const string& To)
		:INumerableElement(id), m_source(IdMap::Insert(From).second), m_target(IdMap::Insert(To).second)
	{

	}
	void AddBody(const string& body) const					//�������� ��� �����
	{
		this->m_body = body;
	}
	string ToString() const
	{
		return "Link: " + m_id.ToString() + " "
			"Body:" + m_body + "\n";
	}
	string GetBogy() const									//�������� ��� �����
	{
		return m_body;
	}
	bool operator<(const LinkTrans& _Right) const {	
		return this->GetId() < _Right.GetId();
	}
	unsigned long GetTarget() const							//�������� ����� ������ ���������
	{
		return m_target;
	}
	unsigned long GetSource() const							//�������� ����� ������� ��������		
	{
		return m_source;
	}
private:
	const unsigned long		m_source;						//������� �� ������� ������� �����
	const unsigned long		m_target;						//������� ���� ���������� �����
	mutable string			m_body;							//���� �����
};
class ActivityTrans :public INumerableElement, public IName
{
public:

	enum class ActivityType
	{
		action,
		fork,
		init,
		decigion,
		fin,
		join
	};

	ActivityTrans(const string& id, const int& Type, const string& Name)
		:INumerableElement(id), IName(Name)
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
	void AddBody(const string& Body)			//�������� ����� ���
	{
		this->m_Body = Body;
	}
	string GetBody() const						//�������� ��� �����
	{
		return m_Body;
	}
	string ToString() const						//����� ��� ������
	{
		string Output;
		Output = "Activity:\n" + m_id.ToString() + "\n";
		for (unsigned i = 0; i < m_outLinks.size(); i++)
			Output += m_outLinks[i].ToString();
		return Output +
			"Body:" + m_Body + "\n";
	}

	void AddOutgoing(const LinkTrans& Linker)	//�������� ���������
	{
		m_outLinks.push_back(Linker);
	}
	void AddIngoing(const LinkTrans& Linker)	//�������� �������� �����
	{
		m_inLinks.push_back(Linker);
	}

	vector<unsigned> GetOut() const				//���������� ������ ������, ����������� � ������ ������� 
	{
		vector<unsigned> out;
		for (auto link : m_outLinks)
			out.push_back(link.GetTarget());
		return out;
	}
	vector<LinkTrans> GetInLinks() const		//����������� �������� ����� ��� �������� �����
	{
		return m_inLinks;
	}


	ActivityType GetType() const				//��������� ����
	{
		return  m_type;
	}
	void SetType(ActivityType newType)			//��� �������� ����� join
	{
		m_type = newType;
	}


private:

	ActivityType		m_type;					//��� ����� ����������
	vector<LinkTrans>	m_inLinks;				//�������� �����
	vector<LinkTrans>	m_outLinks;				//��������� �����
	string				m_Body;					//��� �����
};