#pragma once
#include "present.h"
class LinkTrans :public INumerableElement
{
public:
	LinkTrans(const string& id, const string& From, const string& To)
		:INumerableElement(id), m_source(IdMap::Insert(From).second), m_target(IdMap::Insert(To).second)
	{

	}
	void AddBody(const string& body) const					//Добавить код ребра
	{
		this->m_body = body;
	}
	string ToString() const
	{
		return "Link: " + m_id.ToString() + " "
			"Body:" + m_body + "\n";
	}
	string GetBogy() const									//Получить код ребра
	{
		return m_body;
	}
	bool operator<(const LinkTrans& _Right) const {	
		return this->GetId() < _Right.GetId();
	}
	unsigned long GetTarget() const							//Получить номер вершин исходящей
	{
		return m_target;
	}
	unsigned long GetSource() const							//Получить номер вершниы входящей		
	{
		return m_source;
	}
private:
	const unsigned long		m_source;						//Вершина из которой исходит ребро
	const unsigned long		m_target;						//Вершина куда направлено ребро
	mutable string			m_body;							//Тело ребра
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
	void AddBody(const string& Body)			//Добавить блоку код
	{
		this->m_Body = Body;
	}
	string GetBody() const						//Получить код блока
	{
		return m_Body;
	}
	string ToString() const						//Вывод для дебага
	{
		string Output;
		Output = "Activity:\n" + m_id.ToString() + "\n";
		for (unsigned i = 0; i < m_outLinks.size(); i++)
			Output += m_outLinks[i].ToString();
		return Output +
			"Body:" + m_Body + "\n";
	}

	void AddOutgoing(const LinkTrans& Linker)	//Добавить исходящее
	{
		m_outLinks.push_back(Linker);
	}
	void AddIngoing(const LinkTrans& Linker)	//Добавить входящее ребро
	{
		m_inLinks.push_back(Linker);
	}

	vector<unsigned> GetOut() const				//Возвращает список вершин, переходящих в данную вершину 
	{
		vector<unsigned> out;
		for (auto link : m_outLinks)
			out.push_back(link.GetTarget());
		return out;
	}
	vector<LinkTrans> GetInLinks() const		//Возвращение входящих ребер при создании графа
	{
		return m_inLinks;
	}


	ActivityType GetType() const				//Получения типа
	{
		return  m_type;
	}
	void SetType(ActivityType newType)			//Для создания блока join
	{
		m_type = newType;
	}


private:

	ActivityType		m_type;					//Тип блока активности
	vector<LinkTrans>	m_inLinks;				//Входящие ребра
	vector<LinkTrans>	m_outLinks;				//Исходящие ребра
	string				m_Body;					//Код блока
};