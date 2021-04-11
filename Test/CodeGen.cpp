#include "CodeGen.h"

template <class T>
T PopFromStack(std::stack<T>& stack)
{
	const T topElement = stack.top();
	stack.pop();
	return topElement;
}
class DfsCodeGenerator : public boost::default_dfs_visitor
{
public:
	DfsCodeGenerator(std::string& outputString) : m_output(outputString)
	{
		elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{-1, boost::format("%s")});
		currentThread = -1;
	}

	void initialize_vertex(const Graph::vertex_descriptor& s, const Graph& g) const
	{
		std::cout << "Initialize: " << s << std::endl;
	}

	void start_vertex(const Graph::vertex_descriptor& s, const Graph& g) const
	{
		std::cout << "Start: " << s << std::endl;
	}

	void discover_vertex(const Graph::vertex_descriptor& v, const Graph& g)
	{
		using namespace boost;
		std::cout << "Discover: " << v << std::endl;
		m_history.push_front(v);
	}

	void examine_edge(const Graph::edge_descriptor& e, const Graph& g) 
	{
		std::cout << "Examine edge: " << e << std::endl;
		if (g[e.m_target]->GetType() == ActivityTrans::ActivityType::fork && g[e.m_target]->GetName()=="end")
		{
			forkPoints.push_back(e.m_source);
			auto iter = find(forkBack.begin(), forkBack.end(), e.m_target);
			if(iter == forkBack.end())
				forkBack.push_back(e.m_target);
		}

	}

	void tree_edge(const Graph::edge_descriptor& e, const Graph& g) const
	{
		std::cout << "Tree edge: " << e << std::endl;

	}

	void back_edge(const Graph::edge_descriptor& e, const Graph& g)
	{
		using namespace boost;
		std::cout << "Back edge: " << e << std::endl;

		if (g[e.m_target]->GetType() == ActivityTrans::ActivityType::decigion)
		{
			m_currentBlocks.push(While);
			boost::format fmt("%s");
			graph_traits<Graph>::out_edge_iterator ei, edge_end;
			Graph::vertex_descriptor out;
			Graph::vertex_descriptor in;
			for (tie(ei, edge_end) = out_edges(e.m_target, g); ei != edge_end; ++ei)
			{
				size_t targ = target(*ei, g);
				string body = get(edge_all, g)[edge(e.m_target, targ , g).first]->GetBogy();
				if (body != "")
				{
					out = targ;
				}
				else if (targ != e.m_target) in = targ;
			}

			Graph::vertex_descriptor start;
			graph_traits<Graph>::in_edge_iterator in_start, in_end;
			for (tie(in_start, in_end) = in_edges(e.m_target, g); in_start != in_end; ++in_start)
			{
				size_t circleSt = source(*in_start, g);
				if (circleSt != e.m_target && get(edge_all, g)[edge(circleSt,e.m_target, g).first]->GetBogy()=="back")
				{
					start = circleSt;
					break;
				}
			}
			blockConst whiledata;
			whiledata.block = e.m_target;
			whiledata.out_loop = in;
			whiledata.loop = out;
			whiledata.loopstart = start;
			creator--;
			currentThread = creator;
			whiledata.creat = creator;
			whileInfo.push_back(whiledata);


			elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{creator, fmt});

		}
		else if (g[e.m_source]->GetType() == ActivityTrans::ActivityType::decigion)
		{
			m_currentBlocks.push(DoWhile);
			graph_traits<Graph>::out_edge_iterator ei, edge_end;
			Graph::vertex_descriptor out;
			for (tie(ei, edge_end) = out_edges(e.m_source, g); ei != edge_end; ++ei)
			{
				if (get(edge_all, g)[edge(e.m_source, target(*ei, g), g).first]->GetBogy() == "")
				{
					out = target(*ei, g);
					break;
				}
			}
			graph_traits<Graph>::in_edge_iterator in, inge_end;
			Graph::vertex_descriptor start;
			for (tie(in, inge_end) = in_edges(e.m_source, g); in != inge_end; ++in)
			{
				size_t circleSt = source(*in, g);
				if (circleSt != e.m_source)
				{
					start = circleSt;
					break;
				}
			}
			blockConst whiledata;
			whiledata.block = e.m_source;
			whiledata.out_loop = out;
			whiledata.loop = e.m_target;
			whiledata.loopstart = start;
			boost::format fmt("%s");
			creator--;
			whiledata.creat = creator;
			currentThread = creator;
			whileInfo.push_back(whiledata);
			elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{creator, fmt});
		}
		else
		{
			throw(new std::runtime_error("Back edge without DecisionNode!!!"));
		}
	}

	void forward_or_cross_edge(const Graph::edge_descriptor& e, const Graph& g)
	{
		std::cout << "Forward or cross edge: " << e << std::endl;
		if (m_currentBlocks.empty()) return;
		if (m_currentBlocks.top() == IfElse)
		{
			boost::format fmt("%s");
			creator--;
			currentThread = creator;
			elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{creator, fmt});
			}

	}

	void finish_vertex(const Graph::vertex_descriptor& v, const Graph& g)
	{
		using namespace boost;
		std::cout << "Finish vertex: " << v << std::endl;
		if (g[v]->GetType() == ActivityTrans::ActivityType::action)
		{
			if ((out_degree(v, g) == 0) ||
				find(forkPoints.begin(), forkPoints.end(), v) != forkPoints.end()
				)
			{
				if (forkFor)
				{
					forkForever.push_back(currentThread);
					forkFor = false;
				}
				if (out_degree(v, g) == 0) 
					forkFor = true;
				boost::format fmt("%s");
				creator--;
				currentThread = creator;
				elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{creator, fmt});
			}
			for (int i = 0; i < whileInfo.size(); i++)
			{
				if (v == whileInfo[i].loopstart)
					currentThread = whileInfo[i].creat;

			}
			boost::format fmt("%s" + (elementTable[currentThread] % "").str());
			fmt % (g[v]->GetBody() + "\n");
			elementTable.erase(currentThread);
			cout << fmt.str();
			currentThread = v;
			elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, boost::format("%s" + fmt.str())});
		}

		if (!m_currentBlocks.empty() && (m_currentBlocks.top() == DoWhile || m_currentBlocks.top() == DoWhileOther))
		{
			blockConst circle;
			bool find_circle = false;
			for (auto i = whileInfo.begin(); i != whileInfo.end(); i++)
			{
				if (i->loop == v)
				{
					circle = *i;
					whileInfo.erase(i);
					find_circle = true;
					break;
				}
			}
			if (find_circle)
			{
				boost::format fmt("do\n{\n%s\n}\nwhile(%s);\n%s");
				const std::string Loop = (elementTable[circle.loop] % "").str();
				const std::string linkBody = get(edge_all, g)[edge(circle.block, v, g).first]->GetBogy();
				const std::string outLoop = (elementTable[circle.out_loop] % "").str();
				(fmt % Loop % linkBody % outLoop);
				elementTable.erase(circle.loop);
				elementTable.erase(circle.out_loop);
				currentThread = v;
				elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, "%s" + fmt.str()});
				m_currentBlocks.pop();
				return;
			}
		}

		if (g[v]->GetType() == ActivityTrans::ActivityType::fin)
		{
			if (out_degree(v, g))
				throw(new std::runtime_error("ActivityFinalNode must have no output edges!"));
			string body;
		    body = "%s" + g[v]->GetBody() + "\n}\n";
			elementTable.erase(-1);
			currentThread = v;
			elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, body});

		}
		else if (g[v]->GetType() == ActivityTrans::ActivityType::init)
		{
			if (in_degree(v, g))
				throw(new std::runtime_error("InitialNode must have no input edges!"));
			string body = g[v]->GetBody() + "\n{\n";
			m_output = (elementTable[currentThread] % body).str();
			return;
		}
		else if (g[v]->GetType() == ActivityTrans::ActivityType::decigion)
		{
			const size_t CountOfOutEdges = out_degree(v, g);
			if (CountOfOutEdges != 2)
				throw(new std::runtime_error("More than two output edges is not supported"));

			if (CountOfOutEdges == 1)
				throw(new std::runtime_error("Decision node must have at least two output edges!"));

			if (!m_currentBlocks.empty() && (m_currentBlocks.top() == While))
			{
				blockConst circle;
				bool find_while = false;
				for (auto i= whileInfo.begin();i!= whileInfo.end();i++)
				{
					if (i->block == v)
					{
						circle = *i;
						find_while = true;
						whileInfo.erase(i);
						break;
					}
				}
				if (find_while) 
				{
					const std::string Loop = (elementTable[circle.loop] % "").str();
					const std::string linkBody = get(edge_all, g)[edge(v, circle.loop, g).first]->GetBogy();
					const std::string outLoop = (elementTable[circle.out_loop] % "").str();

					format fmtloop("while(%s)\n{\n%s}\n%s");
					(fmtloop % linkBody % Loop % outLoop);
					elementTable.erase(circle.loop);
					elementTable.erase(circle.out_loop);
					currentThread = v;
					elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, "%s" + fmtloop.str()});
					m_currentBlocks.pop();
					return;
				}
			}
			else if (!m_currentBlocks.empty() && (m_currentBlocks.top() == IfElse))
			{
				graph_traits<Graph>::out_edge_iterator ei, edge_end;
				size_t taget;
				for (tie(ei, edge_end) = out_edges(v, g); ei != edge_end; ++ei)
				{
					size_t point=target(*ei,g);
					if (point != v && point != currentThread)
					{
						taget = point;
						break;
					}
				}
				const std::string Thread1 = (elementTable[currentThread] % "").str();
				const std::string Thread2 = (elementTable[taget] % "").str();
				const string linkBody1 = get(edge_all, g)[edge(v, currentThread, g).first]->GetBogy();
				const string linkBody2 = get(edge_all, g)[edge(v, taget, g).first]->GetBogy();
				format fmt("if (%s)\n{\n%s}\nelse if (%s)\n{\n%s}\n%s");
				size_t back_pos = ifelseBack.top();
				const std::string Block = "%s" + (fmt % linkBody1 % Thread1 % linkBody2 % Thread2 % (elementTable[back_pos] %  "").str()).str();
				elementTable.erase(back_pos);
				elementTable.erase(taget);
				elementTable.erase(currentThread);
				currentThread = v;
				elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, Block});
				ifelseBack.pop();
				m_currentBlocks.pop();
			}
			else if (!m_currentBlocks.empty() && (m_currentBlocks.top() == If))
			{
				const std::string Thread1 = (elementTable[currentThread] % "").str();
				const string linkBody1 = get(edge_all, g)[edge(v, currentThread, g).first]->GetBogy();
				format fmt("if (%s)\n{\n%s}\n%s");
				size_t back_pos = ifelseBack.top();
				const std::string Block = "%s" + (fmt % linkBody1 % Thread1 % (elementTable[back_pos] % "").str()).str();
				elementTable.erase(back_pos);
				elementTable.erase(currentThread);
				currentThread = v;
				elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, Block});
				ifelseBack.pop();
				m_currentBlocks.pop();
			}
		}
		if (g[v]->GetType() == ActivityTrans::ActivityType::fork && in_degree(v, g) == 1 && g[v]->GetName() != "end")
		{
			if (forkFor)
			{
				forkForever.push_back(currentThread);
				forkFor = false;
			}
			graph_traits<Graph>::out_edge_iterator ei, edge_end;
			string Block = "";
			size_t main = -1;
			vector<string> wait;
			for (tie(ei, edge_end) = out_edges(v, g); ei != edge_end; ++ei)
			{
				size_t point = target(*ei, g);
				if (point != v)
				{
					string linkBody = "";
					string body = "";
					auto iter_forever = std::find(forkForever.begin(), forkForever.end(), point);
					format fmt("std::thread thread%s([%s]()\n{\n%s});\n");
					format waitfmt("thread%s.join();\n");
					body = (elementTable[point] % "").str();
					linkBody = get(edge_all, g)[edge(v, point, g).first]->GetBogy();
					if ((linkBody == "main"))
					{
						main = point;
						continue;
					}
					thread_name++;
					if (iter_forever == forkForever.end())
					{
						wait.push_back((waitfmt % thread_name).str());
					}
					else forkForever.erase(iter_forever);
					Block += (fmt % thread_name % linkBody % body).str();
					elementTable.erase(point);

				}
			}
			Block += (elementTable[main] % "").str();
			elementTable.erase(main);
			for (auto iter = wait.begin(); iter != wait.end(); iter++)
				Block += *iter;
			currentThread = v;
			auto back= forkBack.begin()+ forkBack.size()-1;
			Block += (elementTable[*back] % "").str();
			elementTable.erase(*back);
			auto pair = std::pair<size_t,format>(v, boost::format("%s"+Block));
			elementTable.insert(pair);

		}	
		if (g[v]->GetType() == ActivityTrans::ActivityType::fork)
		{
			format fmt = elementTable[currentThread];
			elementTable.erase(currentThread);
			currentThread = v;
			elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, fmt});
			return;
		}
		if (in_degree(v, g) >= 2)
		{
			if (g[v]->GetType() != ActivityTrans::ActivityType::fork)
			{
				graph_traits<Graph>::in_edge_iterator ei, edge_end;
				bool ifblock = false;//Блок являеться только блоком if
				for (tie(ei, edge_end) = in_edges(v, g); ei != edge_end; ++ei)
				{
					if (g[source(*ei, g)]->GetType() == ActivityTrans::ActivityType::decigion)
						ifblock = !ifblock;
				}
				if (!ifblock)
				{
					//запоминаю номер блока потока в который вернусь
					ifelseBack.push(v);
					creator--;
					m_currentBlocks.push(IfElse);
					boost::format fmt("%s");
					currentThread = creator;
					elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{creator, fmt});
				}
				else
				{
					ifelseBack.push(v);
					creator--;
					m_currentBlocks.push(If);
					boost::format fmt("%s");
					currentThread = creator;
					elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{creator, fmt});
				}
			}
		}
	}	
private:
	enum BlockType
	{
		DoWhile,
		DoWhileOther,
		While,
		IfElse,
		Fork,
		If
	};
	struct blockConst
	{
		size_t loop;
		size_t out_loop;
		size_t block;
		size_t creat;
		size_t loopstart;
	};
	std::stack<BlockType> m_currentBlocks;	//здесь хранить пару тип, номер блока 
	//так как if else могут быть только в двух разных потоках?, но как их найти
	//одна часть будет лежать в текущем элемене на обработке, вторую можно найти
	//для if else будет стек потока возврата
	std::stack<Graph::vertex_descriptor> ifelseBack;
	size_t creator = -2;//счетчик для доп потоков
	std::map<Graph::vertex_descriptor, boost::format> elementTable;
	Graph::vertex_descriptor currentThread;
	std::vector < blockConst> whileInfo;//Хранит информацию о циклах, начале и завершении 
	std::vector<size_t> forkBack;

	//хранить в стеке структуру типа блок левая часть (цикл) правая часть (выход из цикла)
	std::stack<int> forkSize;
	std::vector<int> forkForever;
	bool forkFor = false;
	unsigned thread_name = 0;


	std::vector<int> forkPoints;
	std::list<Graph::vertex_descriptor> m_history;

	std::string& m_output;
};
void GraphGen::AcivInit()
{   //Поиск вершины с типом ActivityType::init
	for (auto& elem : activTable)
	{
		if(elem.second->GetType() == ActivityTrans::ActivityType::init)
			head.push_back(elem.first);
	}
	/*auto head_iter=find_if(activTable.begin(), activTable.end(),
		[](auto& imp) 
		{
			return imp.second->GetType() == ActivityTrans::ActivityType::init; 
		});
	if(head_iter!= activTable.end())
		head.push_back(head_iter->first);*/
}
void GraphGen::Inside(const unsigned long start, vector<unsigned long>& out)
{	//Обход графа вглубину, для поиска всех использующихся вершин
	auto headp=activTable[start];
	auto targetVec=headp->GetOut();
	auto out_iter=find_if(out.begin(), out.end(), 
		[start](unsigned long& input)
		{
			return start == input; //Чтобы не повторяться в случае цикла
		});
	if(out_iter == out.end())
		out.push_back(start);
	for (auto elem : targetVec)
	{
		if(find(out.begin(), out.end(),elem)==out.end())
		Inside(elem,out);
	}
}
void GraphGen::MakeGraph(vector<unsigned long>& activ_points)
{
	size_t initialNode;//начальная вершина графа
	std::unordered_map<unsigned long, size_t> idMap; //таблица локальных id и номеров вершин графа

	/*BOOST_FOREACH(const ActivityTrans & vertex, std::get<0>(m_activity))*/
	int i = 0;
	for(auto activity : activ_points)
	{
		//Добавляем вершины к графу
		const size_t VertexDescriptor = boost::add_vertex(std::make_shared<ActivityTrans>(*activTable[activity]), m_graph);
		if(!i)	//Начальная вершина только нулевая
			initialNode = VertexDescriptor;
		i++;
		//Запоминаем номер вешины для данного блока активности
		idMap[activTable[activity]->GetLocalId()] = VertexDescriptor;
	}
	for (auto activity : activ_points)
	{	//На графе ставим ребра
		auto targets=activTable[activity]->GetOut();//Смотрим в какие вершины переходит текущая вершина
		for (auto one_targ : targets)
		{	//Теперь надо найти ребро - LinkTrans
			auto links=activTable[one_targ]->GetInLinks();//Получаем все ребра
			auto linkIter = find_if(links.begin(), links.end(), [activity](LinkTrans& inLink)
				{	//Находим ребро, у которого источник activity
					return (inLink.GetSource() == activity);
				}); //Добавляем к графу, так как такое ребро обязано существовать не проверяем на links.end()
			boost::add_edge(idMap[activity], idMap[one_targ], std::make_shared<LinkTrans>(*linkIter), m_graph);
		}
	}
	boost::write_graphviz(std::cout, m_graph);
	std::string code;
	DfsCodeGenerator visitor(code);
	boost::depth_first_search(m_graph, boost::visitor(visitor).root_vertex(initialNode));
	cout << code;

	bool sh=true;
	/*
	BOOST_FOREACH(const Edge & edge, std::get<1>(m_activity))
	{
		const std::unordered_map<size_t, unsigned long>::const_iterator itSource = std::find_if(
			idMap.begin(), idMap.end(), boost::bind(&std::unordered_map<size_t, unsigned long>::value_type::second, _1) == edge.GetSource().GetLocalId()
		);
		const std::unordered_map<size_t, unsigned long>::const_iterator itTarget = std::find_if(
			idMap.begin(), idMap.end(), boost::bind(&std::unordered_map<size_t, unsigned long>::value_type::second, _1) == edge.GetTarget().GetLocalId()
		);

		if ((itSource != idMap.end()) && (itTarget != idMap.end()))
			boost::add_edge(itSource->first, itTarget->first, std::make_shared<Edge>(edge), m_graph);
	}

	const char* labels[] = { "DecisionNode", "ActivityFinalNode","InitialNode", "Action", "Action" };
	//, boost::make_label_writer(labels));*/

	//std::string code;
	//DfsCodeGenerator visitor(code);
	//boost::depth_first_search(m_graph, boost::visitor(visitor).root_vertex(initialNode));

	//std::cout << code;
}
void GraphGen::InsideEnd(const unsigned long start, bool& wayFin)//Проверяем есть ли путь от данного элемента до элемента конца
{
	auto headp = activTable[start];
	auto targetVec = headp->GetOut();
	if (headp->GetType() == ActivityTrans::ActivityType::fin)
	{
		wayFin = true;
		return;
	}
	for (auto elem : targetVec)
	{
		InsideEnd(elem, wayFin);
	}
}
void GraphGen::Gen(const unsigned long start)
{
	auto headp = activTable[start];
	auto count_in = headp->GetInCount();
	if (headp->GetType() == ActivityTrans::ActivityType::action && count_in > 1)//цикл do while
	{
		cout << "do";
		dowhile.push_back(headp->GetLocalId());
	}
	if (headp->GetType() == ActivityTrans::ActivityType::decigion)
	{
		unsigned long elem;//элемент, который разделяет if/else конструкцию
		auto result = Connect(headp->GetLocalId(), elem);
		if (result==1)
		{
			cout << "if/else";
		}
		else if (result == 2)
		{
			cout << "if/left";
		}
		else if (result == 3)
		{
			cout << "right";
		}
	}
	auto targetVec = headp->GetOut();
	for (auto elem : targetVec)
	{
		Gen(elem);
	}
}
int GraphGen::Connect(const unsigned long dis, unsigned long& elem_out)//1 if else
//2 if генер левую ветку
//3 if генер правую ветку
{
	elem_out = -1;
	auto headp = activTable[dis];
	auto outways = headp->GetOut();
	vector<unsigned long> first;
	vector<unsigned long> second;
	Inside(outways[0], first);
	Inside(outways[1], second);
	for (size_t i=0;i< first.size();i++)
	{
		auto found=find(second.begin(), second.end(), first[i]);
		if (found != second.end())
		{
			if(elem_out<0)
				elem_out = first[i];
			second.erase(found);
			first.erase(first.begin() + i);
		}
	}
	if (elem_out > 0)
	{
		if ((second.size() > 0) && (first.size() > 0))
			return 1;
		else if (first.size() > 0)
			return 2;
		else return 3;
	}
	else return -1;
}

