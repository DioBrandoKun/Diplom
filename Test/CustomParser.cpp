#include "CustomParser.h"
std::map <const string, int> mapPac =  //для парсинга хештега упакованных элементов
{ {"uml:Class",1},{"uml:Realization",2},{"uml:Association",3},{"uml:Interface",4},{ "uml:Enumeration",5},
	{ "uml:DataType",7},{"uml:Package",8}, {"uml:Activity",8},{"uml:ControlFlow",9},{"uml:Action",10},{"uml:ForkNode",10},{"uml:ActivityFinalNode",10},
	{"uml:InitialNode",10},{"uml:DecisionNode",10},{"uml:Comment",11} };
map<const string, int> ActivType = { {"uml:Action",1},{"uml:ForkNode",2},{"uml:InitialNode",3},{"uml:DecisionNode",4},{"uml:ActivityFinalNode",5} };
void CustomParser::Parse( const ptree& Pack,const bool& OtherTree)
{
	ptree PackageTree;
	try//Проверка на правильный формат XMI файла
	{
		if (OtherTree)
			PackageTree = Pack;//Если вложенный Package
		else
		{
			PackageTree = m_root.get_child("xmi:XMI.uml:Model.packagedElement");
		}
		//кроме того странное разделение по тегам для множества Package
	}
	catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>)
	{
		cout << "Wrong XMI format\n";
		return;
	}
	BOOST_FOREACH(const ptree::value_type & diagramTree, PackageTree.get_child(""))
	{
		if (diagramTree.first != "<xmlattr>")
		{
			auto Type = diagramTree.second.get<std::string>("<xmlattr>.xmi:type");
			switch (mapPac[Type])
			{
			case 1:
			{
				cout << "uml:Class\n";
				Class(diagramTree.second);
				break;
			}
			case 2:
			{
				cout << "uml:Realization\n";
				Realizat(diagramTree.second);
				break;
			}
			case 3:
			{
				cout << "uml:Association\n";
				Assosiation(diagramTree.second);
				break;
			}
			case 4:
			{
				cout << "uml:Interface\n";
				Interface(diagramTree.second);
				break;
			}
			case 5:
			{
				cout << "uml:Enumeration\n";
				Enum(diagramTree.second);
				break;
			}
			case 7:
			{
				cout << "uml:DataType\n";
				string id = diagramTree.second.get<std::string>("<xmlattr>.xmi:id");
				string Name = diagramTree.second.get<std::string>("<xmlattr>.name");
				cout << Name;
				m_Realized.insert(IdMap::Insert(id).second);
				IdMap::InputName(id, Name);
				break;
			}
			case 8:
			{
				Parse(diagramTree.second, true); //вложенный Package
				break;
			}
			case 9:
			{
				cout << "uml:ControlFlow\n";
				LinkParse(diagramTree.second);
				break;
			}
			case 10://Разбирает все блоки диаграммы активности
			{
				cout << "Uml action"<< endl;
				Activity(diagramTree.second);
				break;
			}
			case 11:
			{
				cout << "uml:Comment" << endl;
				string body= diagramTree.second.get<string>("<xmlattr>.body");
				
				string id_block= diagramTree.second.get<string>("annotatedElement.<xmlattr>.xmi:idref");
				unsigned long id_long = IdMap::Insert(id_block).second;
				m_comments.push_back(pair<unsigned long, string>(id_long, body));
				break;
			}
			default:
			{
				cout << "Wrong Elem type:" << Type << endl;
			}
			}
		}
	}
}
map<unsigned long, ActivityTrans*> CustomParser::Normalize()//Функция для работы с классами и ассоциациями
{
	IdMap::Print();
	SetLinks();
	SetComments();
	for (auto&  assosItem: m_AllAssos)//Восстанавливаем композиции
	{
		vector<unsigned long> ClassToAssos = assosItem.GetSource();
		
		for (auto& classItem: ClassToAssos)//AllClass
		{
			auto iter = find_if(m_AllClass.begin(), m_AllClass.end(), [classItem](ClassTrans& elem)
				{
					return elem.GetLocalId() == classItem;
				});
			if(iter!= m_AllClass.end())		//Находим класс, в которой направлена ассоциация и записываем её
				iter->AddCompos(assosItem);
		}
	}
	for (auto& realizItem : m_AllRealiz)//Установка реализаций
	{
		string client = realizItem.GetСlient();
		auto classReal=find_if(m_AllClass.begin(), m_AllClass.end(), [client](auto& classItem)
			{
				return to_string(classItem.GetLocalId()) == client;
			});
		classReal->AddRealiz(realizItem);//Реализация всегда для одного класса, поэтому мы сразу идем дальше
	}
	for (auto& classItem : m_AllClass)//Переход для всех классов от цифр к буквенным определениям
	{
		classItem.SetNum();
	}

	{	//Здесь стоит придумать выход из вечного цикла
		unsigned long j = 0;
		vector<unsigned long> realizedJ; //костыль чтобы не реализовывать копирование для класса ClassTrans
		//номера классов которые еще не были выведены
		for (unsigned long k = 0; k < m_AllClass.size(); k++)
		{
			realizedJ.push_back(k);
		}
		while (realizedJ.size()>0)//формируем порядок вывода классов
		{
			for (auto j = realizedJ.cbegin(); j != realizedJ.cend();)//в этом цикле смотрим какие классы нужно реализовать и выписываем класс, если он уже ни от кого не зависит
			{
				auto Num = *j;
				auto NeedRealize = m_AllClass[Num].GetRealize();//Получаем указатель на нужные классу вектор
				
				for (auto k = (*NeedRealize).cbegin(); k != (*NeedRealize).cend(); )
				{
					if (m_Realized.find(*k) != m_Realized.end())
					{
						k = (*NeedRealize).erase(k);
					}
					else k++;
				}
				if ((*NeedRealize).size() == 0) 
				{
					m_Realized.insert(m_AllClass[Num].GetLocalId());
					cout << m_AllClass[Num].ToCode();
					j = realizedJ.erase(j);
				}
				else
				{
					j++;
				}
			}
		}
	}
	cout << endl;
	return m_AllActivity;
}
void CustomParser::Class(const ptree& pt, const int& Interface)//1-для Интерфеса 2-для инумератора
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	string StrName ;
	try
	{
		StrName = pt.get<string>("<xmlattr>.name");
	}
	catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>)
	{
		return;
	}
	ClassTrans Example(Id, StrName);
	BOOST_FOREACH(const ptree::value_type & elem, pt.get_child(""))
	{
		if (elem.first != "<xmlattr>")
		{
			if (elem.first == "ownedAttribute")
			{
				Example.AddValue(ClassValue(elem.second));
			}
			if (elem.first == "ownedOperation")
			{
				Example.AddOperation(ClassOperations(elem.second, StrName));
			}
			if (elem.first == "generalization")
			{
				Example.Inher(Inhert(elem.second));
			}
		}
	}
	if (Interface == 1 or Interface == 2)Example.SetInterface();
	m_AllClass.push_back(move(Example));
}
string CustomParser::Inhert(const ptree& pt)//Наследование
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id"); 
	const std::string ParentId = pt.get<std::string>("<xmlattr>.general");
	return ParentId;
}
ClassValueTrans CustomParser::ClassValue(const ptree& pt)
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	const std::string constType = pt.get<std::string>("<xmlattr>.isReadOnly");
	string ElemName; 
	string ElemPrivate; 
	string ElemStatic="false";
	string AssociationId;
	string ElemType;
	string ElemValue="";
	BOOST_FOREACH(const ptree::value_type & elem, pt.get_child(""))
	{
		if (elem.first == "<xmlattr>")
		{
			BOOST_FOREACH(const ptree::value_type & Info, elem.second.get_child(""))
			{
				if (Info.first == "name") ElemName = Info.second.data();
				else if (Info.first == "visibility") ElemPrivate = Info.second.data();
				else if (Info.first == "isStatic") ElemStatic = Info.second.data();
			}
		}
		if (elem.first == "type") ElemType=elem.second.get<std::string>("<xmlattr>.xmi:idref");
		if (elem.first =="defaultValue") ElemValue= elem.second.get<std::string>("<xmlattr>.value");
	}
	ClassValueTrans Example(Id, ElemName, ElemStatic, ElemType, ElemValue, ElemPrivate, constType);
	return Example;
}
ClassOperTrans CustomParser::ClassOperations(const ptree& pt, const string& ClassName)
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	const string StrName = pt.get<string>("<xmlattr>.name");
	vector<string> InputName;
	vector<string> Return;
	string Static = "false";
	string Abstract = "false";
	BOOST_FOREACH(const ptree::value_type & Info, pt.get_child(""))//Параметры операции
	{		
		if(Info.first!="<xmlattr>")
		{
			BOOST_FOREACH(const ptree::value_type & elem, Info.second.get_child("<xmlattr>"))
			{
					if (elem.first == "name")
						InputName.push_back(elem.second.data());
					else if (elem.first == "type")
					{
						Return.push_back(elem.second.data());
					}
			}
		}
		else BOOST_FOREACH(const ptree::value_type & Data, Info.second.get_child(""))
		{
			if (Data.first == "isAbstract")
				Abstract = Data.second.data();
			if (Data.first == "isStatic")
				Static = Data.second.data();
		}
	}
	string type;
	if ((std::find(InputName.begin(), InputName.end(), "return") == InputName.end()) || (Return.size() == 0))//Если операция содержит неправильное колво элементов
	{
		type = "none";
	}
	else type = Return[Return.size() - 1];
	ClassOperTrans Example(Id,StrName,Static,type , StrVisibility);
	if (Return.size() > 0 && type!="none")
	{
		Return.pop_back();
		InputName.pop_back();
	}

	Example.placeVirtual(Abstract);
	Example.AddElems(InputName, Return);
	return Example;
}
void CustomParser::Realizat(const ptree& pt)
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	const std::string IdSup= pt.get<std::string>("<xmlattr>.supplier");
	const std::string IdClient = pt.get<std::string>("<xmlattr>.client");
	Realization NewRealiz(Id, IdSup, IdClient);
	m_AllRealiz.push_back(move(NewRealiz));
}
void CustomParser::Assosiation(const ptree& pt)
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	ptree ElementsPtree;
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	string Name="";
	vector<string> Member;
	string Target = "";
	string Source = "";
	string OwnedId="";
	string Type = "";
	
	BOOST_FOREACH(const ptree::value_type & Info, pt.get_child(""))//Параметры операции
	{
		if (Info.first != "<xmlattr>")
		{
			if ((Info.first == "memberEnd"))
			{
				Member.push_back(Info.second.get<std::string>("<xmlattr>.xmi:idref"));
			}
			else if ((Info.first == "ownedEnd"))
			{
				string OwnXmiId = Info.second.get<std::string>("<xmlattr>.xmi:id");
				OwnedId=Info.second.get<std::string>("type.<xmlattr>.xmi:idref");
				Type = Info.second.get<std::string>("<xmlattr>.aggregation");
				for (int i =0; i < Member.size(); i++)
				{
					//композиция и агригации являются одинаковыми элементами в uml модели
					//имеют отличную структуру		
					//У настоящей композиции две memberEnd ссылки являются ссылками на ownedEnd
					//а в ownedEnd лежат ссылки на класссы
					//у агрегации же одна memberEnd ссылка нужная нам
					//а вторую надо раскрыть через ownedEnd
					if (Member[i] == OwnXmiId)
					{
						Member[i] = OwnedId;
					}
				}
				if (Type == "none")//none представляет собой ассоциацию, она не влияет на классы в XMI и ее рассматривать не имеет смысла 
					return;
			}
		}
		else BOOST_FOREACH(const ptree::value_type & Elems, Info.second.get_child(""))
		{
			if (Elems.first == "name") Name = Elems.second.data();
		}
	}
	Assos Example(Id, Name, Member[1], Member[0],Type);
	m_AllAssos.push_back(move(Example));
}
void CustomParser::Interface(const ptree& pt)
{
	Class(pt,1);
}
void CustomParser::Enum(const ptree& pt)
{
	Class(pt, 2);
}
void CustomParser::Activity(const ptree& pt)
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	string Name;
	try
	{
		Name = pt.get<std::string>("<xmlattr>.name");
	}
	catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>)
	{
		Name = "Not stated";
	}
	const std::string type = pt.get<std::string>("<xmlattr>.xmi:type");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	ActivityTrans* Example=new ActivityTrans(Id, ActivType[type],Name);
	m_AllActivity.insert({ Example->GetLocalId(), Example });
}
void CustomParser::LinkParse(const ptree& pt)
{
	const std::string id = pt.get<std::string>("<xmlattr>.xmi:id");
	string source = pt.get<string>("<xmlattr>.source");
	string targer = pt.get<string>("<xmlattr>.target");
	string body = "";
	try
	{
		body = pt.get<std::string>("<xmlattr>.name");
	}
	catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>)
	{
	}
	LinkTrans linkXMI(id, source, targer);
	linkXMI.SetBody(body);
	m_allLink.push_back(linkXMI);
}
void CustomParser::SetLinks()
{
	for (auto& link : m_allLink)
	{
		m_AllActivity[link.GetSource()]->SetOutgoing(link);
		m_AllActivity[link.GetTarget()]->SetIngoing(link);
	}
}
void CustomParser::SetComments()
{
	for (auto& pair : m_comments)
	{
		m_AllActivity[pair.first]->SetBody(pair.second);
	}
}