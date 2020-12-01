#include "CustomParser.h"
void CustomParser::Parse()
{
	vector<string> DataType;//Типы данных, так как классы воспринимают другие типы как - нужные для реализации
	ptree PackageTree = m_root.get_child("xmi:XMI.uml:Model.packagedElement");//Из этой области парсим классы, но здесь мало информации для элементов активности
	//кроме того странное разделение по тегам для множества Package
	ptree Nodes = m_root.get_child("xmi:XMI.xmi:Extension.elements");//Область для диаграммы активности
	BOOST_FOREACH(const ptree::value_type & elem, Nodes.get_child(""))
	{
		string Type;
		try//Для Package без элементов может выдать ошибку
		{
			Type = elem.second.get<std::string>("<xmlattr>.xmi:type");
		}
		catch(boost::wrapexcept<boost::property_tree::ptree_bad_path>)//Если проект пустой, даже без пакета
		{
			cout << "Project is empty";
			return;
		}
		if (Type == "uml:Activity")
		{
			cout << "uml:Activity" << endl;
			Activity(elem.second);
			cout << endl;
		}
		if (Type == "uml:StateNode")
		{
			cout << "uml:StateNode" << endl;
			StateNote(elem.second);
			cout << endl;
		}
		if (Type == "uml:Decision")
		{
			cout << "uml:Decision" << endl;
			Decision(elem.second);
			cout << endl;
		}
		if (Type == "uml:Synchronization")
		{
			cout << "uml:Synchronization" << endl;
			Fork(elem.second);
			cout << endl;
		}
	}
	BOOST_FOREACH(const ptree::value_type & diagramTree, PackageTree.get_child(""))
	{
		if (diagramTree.first != "<xmlattr>")
		{
			
			if (diagramTree.first == "packagedElement")
			{

				auto Type = diagramTree.second.get<std::string>("<xmlattr>.xmi:type");
				if(Type!= "uml:Activity")
				std::cout << Type << std::endl;
				if (Type == "uml:Class")
				{
					Class(diagramTree.second);
					cout << endl;
				}
				else if (Type == "uml:Realization")
				{
					Realization(diagramTree.second);
					cout << endl;
				}
				else if (Type == "uml:Association")
				{
					Assosiation(diagramTree.second);
					cout << endl;
				}
				else if (Type == "uml:Interface")
				{
					Interface(diagramTree.second);
					cout << endl;
				}
				else if (Type == "uml:Enumeration")
				{
					Enum(diagramTree.second);
					cout << endl;
				}
				else if (Type == "uml:Activity")
				{
					BOOST_FOREACH(const ptree::value_type & elem, diagramTree.second.get_child(""))
						if (elem.first == "edge") EdgeCheck(elem.second);
				}
				else if (Type == "uml:DataType")
				{
					string id= diagramTree.second.get<std::string>("<xmlattr>.xmi:id");
					string Name = diagramTree.second.get<std::string>("<xmlattr>.name");
					DataType.push_back(to_string(IdMap::Insert(Name).second));
					IdMap::InputName(id, Name);
				}
				else
				{
					cout << "Wrong Elem type:" << Type;
					return;
				}
			}
			if (diagramTree.first == "edge")
			{
				EdgeCheck(diagramTree.second);
			}
		}
	}
	IdMap::Print();
	for (unsigned i = 0; i < AllActivity.size(); i++)
	{
		cout << AllActivity[i].ToString();
	}
	for (unsigned i = 0; i < AllAssos.size(); i++)//Восстанавливаем ассоциации
	{
		for (unsigned j = 0; j < AllClass.size(); j++)
		{
			if (AllAssos[i].GetTarget() == to_string(AllClass[j].GetLocalId()))
				AllClass[j].PutAssos(AllAssos[i]);
		}
	}
	for (unsigned i = 0; i < AllClass.size(); i++)//Переход для всех классов от цифр к буквенным определениям
	{
		AllClass[i].SetNum();
		std::cout << AllClass[i].ToCode();
	}
}
ClassTrans CustomParser::Class(const ptree& pt, int Interface)//1-для Интерфеса 2-для инумератора
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
				else if (Info.first == "association")
				{
					AssociationId = Info.second.data();
					string AssociationClass = pt.get<std::string>("type.<xmlattr>.xmi:idref");
					cout <<"Assos:"<< AssociationId << " " << AssociationClass << endl;
				}
			}
		}
		if (elem.first == "type") ElemType=elem.second.get<std::string>("<xmlattr>.xmi:idref");
		if (elem.first =="defaultValue") ElemValue= elem.second.get<std::string>("<xmlattr>.value");
	}
	ClassValueTrans Example(Id, ElemName, ElemStatic, ElemType, ElemValue, ElemPrivate);
	return Example;
}
ClassOperTrans CustomParser::ClassOperations(const ptree& pt,string ClassName)
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
			if (Data.first != "isAbstract")
				Static = Data.second.data();
			if (Data.first != "isStatic")
				Static = Data.second.data();
		}
	}
	if ((Return.size() != InputName.size()) || (Return.size() == 0))//Если операция содержит неправильное колво элементов
	{
		cout << "Wrong operation format\t" << ClassName << "\t" << StrName;
		//throw std::exception("Wrong operation format\t" + ClassName + "\t" + StrName);
	}
	ClassOperTrans Example(Id,StrName,Static, Return[Return.size()-1], StrVisibility);
	if (Return.size() > 1)
	{
		Return.pop_back();
		InputName.pop_back();
	}
	Example.AddElems(InputName, Return);
	return Example;
}
void CustomParser::Realization(const ptree& pt)
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	const std::string IdSup= pt.get<std::string>("<xmlattr>.supplier");
	const std::string IdClient = pt.get<std::string>("<xmlattr>.client");
	cout << "Realization:" << Id << " " << StrVisibility << " " << IdSup << " " << IdClient << endl;
}
void CustomParser::Assosiation(const ptree& pt)
{
	const std::string Id = pt.get<std::string>("<xmlattr>.xmi:id");
	const std::string StrVisibility = pt.get<std::string>("<xmlattr>.visibility");
	string Name="";
	string MemberEnd="";
	string OwnedId="";
	bool Member = true;
	bool Owned = true;
	cout << "Assosiation:";
	BOOST_FOREACH(const ptree::value_type & Info, pt.get_child(""))//Параметры операции
	{
		if (Info.first != "<xmlattr>")
		{
			if ((Info.first == "memberEnd")&&Member)
			{
				Member = false;
				MemberEnd = Info.second.get<std::string>("<xmlattr>.xmi:idref");
			}
			else if ((Info.first == "ownedEnd")&& Owned)
			{
				Owned = false;
				OwnedId=Info.second.get<std::string>("type.<xmlattr>.xmi:idref");
			}
		}
		else BOOST_FOREACH(const ptree::value_type & Elems, Info.second.get_child(""))
		{
			if (Elems.first == "name") Name = Elems.second.data();
		}
	}
	Assos Example(Id, Name, OwnedId, MemberEnd);
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
					//bool push = true;
					if(End==Id)
					if (Link.find(LinkId) == Link.end())
					{
						//push = false;
						Link.insert(LinkId);
					}
					/*
					for (int i = 0; i < Link.size(); i++)
						if (Id == Link[i])
						{
							push = false;
							break;
						}
						*/
					//if(push)Link.push_back(LinkId);
				}
				if (Start == Id)
					Example.AddOutgoing(LinkXMI);
				//cout << "Link:" << Id<<" "<<Start << " " << End << endl;
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