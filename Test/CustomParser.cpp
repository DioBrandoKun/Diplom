#include "CustomParser.h"
std::map <const string, int> mapElements = { {"uml:Activity",0},{"uml:StateNode",1},//для парсинга хештега элементс
	{"uml:Decision",2},{"uml:Synchronization",3} };
std::map <const string, int> mapPac =  //для парсинга хештега упакованные элементы
{ {"uml:Class",0},{"uml:Realization",1},{"uml:Association",2},{"uml:Interface",3},{ "uml:Enumeration",4},{"uml:Activity",5},
	{ "uml:DataType",6},{"uml:Package",7} };
void CustomParser::Parse( const ptree& Pack,const bool& PackSearch,const bool& ElementsSearch,const bool& OtherTree)
{
	ptree PackageTree;
	ptree Nodes;
	try//Проверка на правильный формат XMI файла
	{
		if (OtherTree)
			PackageTree = Pack;//Если вложенный Package
		else
		{

			PackageTree = m_root.get_child("xmi:XMI.uml:Model.packagedElement");//Из этой области парсим классы, но здесь мало информации для элементов активности

		}
		//кроме того странное разделение по тегам для множества Package
		Nodes = m_root.get_child("xmi:XMI.xmi:Extension.elements");//Область для диаграммы активности
	}
	catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>)
	{
		cout << "Wrong XMI format\n";
		return;
	}
	if(ElementsSearch)
	BOOST_FOREACH(const ptree::value_type & elem, Nodes.get_child(""))
	{
		string Type;
		try//Первый элемент в элементс для основного пакета проекта не имеет типа, пропускаем его
		{
			Type = elem.second.get<std::string>("<xmlattr>.xmi:type");
		}
		catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>)
		{
			continue;
		}
		switch (mapElements[Type])//с помощью таблицы находим нужный тег
		{
			case 0:
			{
				cout << "uml:Activity" << endl;
				Activity(elem.second);
				cout << endl;
				break;
			}
			case 1:
			{
				cout << "uml:StateNode" << endl;
				StateNote(elem.second);
				cout << endl;
				break;
			}
			case 2:
			{
				cout << "uml:Decision" << endl;
				Decision(elem.second);
				cout << endl;
				break;
			}
			case 3:
			{
				cout << "uml:Synchronization" << endl;
				Fork(elem.second);
				cout << endl;
				break;
			}
			default:
				break;
		}
	}
	if(PackSearch)
	BOOST_FOREACH(const ptree::value_type & diagramTree, PackageTree.get_child(""))
	{
		if (diagramTree.first != "<xmlattr>")
		{
			
			if (diagramTree.first == "packagedElement")
			{

				auto Type = diagramTree.second.get<std::string>("<xmlattr>.xmi:type");
				if(Type!= "uml:Activity")
				std::cout << Type << std::endl;
				switch (mapPac[Type])
				{
				case 0:
					{
						Class(diagramTree.second);
						cout << endl;
						break;
					}
				case 1:
					{
						Realizat(diagramTree.second);
						cout << endl;
						break;
					}
				case 2:
					{
						Assosiation(diagramTree.second);
						cout << endl;
						break;
					}
				case 3:
					{
						Interface(diagramTree.second);
						cout << endl;
						break;
					}
				case 4:
					{
						Interface(diagramTree.second);
						cout << endl;
						break;
					}
				case 5:
					{
						Enum(diagramTree.second);
						cout << endl;
						break;
					}
				case 6:
					{
						BOOST_FOREACH(const ptree::value_type & elem, diagramTree.second.get_child(""))
							if (elem.first == "edge") EdgeCheck(elem.second);
						break;
					}
				case 7:
					{
						string id = diagramTree.second.get<std::string>("<xmlattr>.xmi:id");
						string Name = diagramTree.second.get<std::string>("<xmlattr>.name");
						cout << Name;
						Realized.insert(IdMap::Insert(id).second);
						IdMap::InputName(id, Name);
						break;
					}
				case 8:
					{
						Parse(diagramTree.second, true, false, true); //вложенный Package
						break;
					}
				default:
					{
						cout << "Wrong Elem type:" << Type;
						return;
					}
				}
			}
			if (diagramTree.first == "edge")
			{
				EdgeCheck(diagramTree.second);
			}
		}
	}
}
void CustomParser::Normalize()//Функция для работы с классами и ассоциациями
{
	IdMap::Print();
	for (unsigned i = 0; i < AllAssos.size(); i++)//Восстанавливаем ассоциации
	{
		vector<unsigned long> AssosTion = AllAssos[i].GetTarget();
		for (unsigned j = 0; j < AllClass.size(); j++)
		{
			if (AllClass[j].GetLocalId() == AssosTion[0])
			{
				AllClass[j].AddAssos(AllAssos[i]);
				break;
			}
		}
	}
	for (unsigned i = 0; i < AllAssos.size(); i++)//Восстанавливаем композиции
	{
		vector<unsigned long> ClassToAssos = AllAssos[i].GetTarget();
		for (unsigned j = 0; j < AllClass.size(); j++)
		{
			AllClass[j].AddCompos(AllAssos[i]);
		}
	}
	for (unsigned i = 0; i < AllRealiz.size(); i++)//Восстанавливаем а реализации
	{
		for (unsigned j = 0; j < AllClass.size(); j++)
		{
			if (to_string(AllClass[j].GetLocalId()) == AllRealiz[i].GetСlient())
			{
				AllClass[j].AddRealiz(AllRealiz[i]);
				break;//Реализация всегда для одного класса, поэтому мы сразу идем дальше				
			}
		}
	}
	for (unsigned i = 0; i < AllClass.size(); i++)//Переход для всех классов от цифр к буквенным определениям
	{
		AllClass[i].SetNum();
	}
	{
		unsigned long j = 0;
		vector<unsigned long> realizedJ; //костыль чтобы не реализовывать копирование для класса ClassTrans
		//номера классов которые еще не были выведены
		for (unsigned long k = 0; k < AllClass.size(); k++)
		{
			realizedJ.push_back(k);
		}
		while (realizedJ.size()>0)//формируем порядок вывода классов
		{
			for (auto j = realizedJ.cbegin(); j != realizedJ.cend();)//в этом цикле смотрим какие классы нужно реализовать и выписываем класс, если он уже ни от кого не зависит
			{
				auto Num = *j;
				auto NeedRealize = AllClass[Num].GetRealize();
				for (auto k = NeedRealize.cbegin(); k != NeedRealize.cend(); )
				{
					if (Realized.find(*k) != Realized.end())
					{
						k = NeedRealize.erase(k);
					}
					else k++;
				}
				if (NeedRealize.size() == 0)
				{
					Realized.insert(AllClass[Num].GetLocalId());
					cout << AllClass[Num].ToCode();
					j = realizedJ.erase(j);
				}
				else
				{
					AllClass[Num].SetRealize(NeedRealize);
					j++;
				}
			}
		}
	}
}
ClassTrans CustomParser::Class(const ptree& pt, const int& Interface)//1-для Интерфеса 2-для инумератора
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	const string StrName = pt.get<string>("<xmlattr>.name");
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
	if (Interface == 1)Example.SetInterface();
	AllClass.push_back(Example);
	return Example;
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
	string ElemName; //
	string ElemPrivate;// 
	string ElemStatic="false";// 
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
						cout << elem.second.data();
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
	if ((Return.size() != InputName.size()) || (Return.size() == 0))//Если операция содержит неправильное колво элементов
	{
		cout << "Wrong operation format\t" << ClassName << "\t" << StrName;
		//throw std::exception("Wrong operation format\t" + ClassName + "\t" + StrName);
	}
	ClassOperTrans Example(Id,StrName,Static, Return[Return.size()-1], StrVisibility);
	if (Return.size() > 0)
	{
		Return.pop_back();
		InputName.pop_back();
	}
	Example.IsVirtual(Abstract);
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
	AllRealiz.push_back(NewRealiz);
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
	
	cout << "Assosiation:";
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
	AllAssos.push_back(Example);
	cout << endl;
}
void CustomParser::Interface(const ptree& pt)
{
	Class(pt,1);
}
void CustomParser::Enum(const ptree& pt)
{
	Class(pt, 2);
}
void CustomParser::Activity(const ptree& pt,int ActivNum)//1:Final/Start-Node 2:Decision 3:Fork
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:idref");
	if(ActivNum!=3)
		const std::string Name = pt.get<std::string>("<xmlattr>.name");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.scope");
	ActivityTrans Example(Id, ActivType[ActivNum]);
	string Code;
	BOOST_FOREACH(const ptree::value_type & Info, pt.get_child(""))
	{
		if (Info.first == "properties")
			BOOST_FOREACH(const ptree::value_type & Elem, Info.second.get_child("<xmlattr>"))
		{
			if (Elem.first == "documentation") Code = Elem.second.data();
		}
		if (Info.first == "links")
		{
			BOOST_FOREACH(const ptree::value_type & Elem, Info.second.get_child(""))
			{

				string LinkId = Elem.second.get<string>("<xmlattr>.xmi:id");
				string Start = Elem.second.get<string>("<xmlattr>.start");
				string End = Elem.second.get<string>("<xmlattr>.end");
				LinkTrans LinkXMI(LinkId, Start, End);
				if (ActivNum == 3)
				{
					if(End==Id)
					if (Link.find(LinkId) == Link.end())
					{
						Link.insert(LinkId);
					}
				}
				if (Start == Id)
					Example.AddOutgoing(LinkXMI);
			}
		}
	}
	Example.AddBody(Code);
	AllActivity.push_back(Example);
	cout << "Activity:" << Id << " " << Code<<endl;
}
void CustomParser::StateNote(const ptree& pt)
{
	Activity(pt,1);
}
void CustomParser::Decision(const ptree& pt)
{
	Activity(pt, 2);
}
void CustomParser::Fork(const ptree& pt)
{
	Activity(pt, 3);
}
void CustomParser::EdgeCheck(const ptree& pt)//Поиск тела link
{//При использовании fork есть необходимость передавать параметры в потоки эти параметры мы и ищем
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	string Start = pt.get<string>("<xmlattr>.source");
	string Body;
	bool Work=false;
	if (Link.find(Id) != Link.end())
	{
		Link.erase(Link.find(Id));
		Work = true;
	}
	if (!Work) return;
	BOOST_FOREACH(const ptree::value_type & Elem, pt.get_child(""))
	{
		if (Elem.first == "guard")
			BOOST_FOREACH(const ptree::value_type & Info, Elem.second.get_child("<xmlattr>"))
		{
			if (Info.first == "body")
				Body = Info.second.data();
		}
	}
	for (unsigned i = 0; i < AllActivity.size(); i++)
	{
		if (AllActivity[i].GetId() == Start)
		{
			AllActivity[i].AddLinkBody(Id, Body);
			return;
		}
	}
}