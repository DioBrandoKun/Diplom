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
	DfsCodeGenerator(std::string& outputString) : m_output(outputString), m_currentThread(0)
	{
		std::stack<std::pair<boost::format, Graph::vertex_descriptor>> stack;
		auto pair = std::pair<boost::format, Graph::vertex_descriptor>(boost::format("%s"), -1);
		stack.push(pair);
		m_threads.push_back(stack);
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
		/*if (out_degree(v, g) == 2 && g[v]->GetType()!= ActivityTrans::ActivityType::fork)
		{
			if (m_blockDescriptors.empty() || m_blockDescriptors.top().first != v)
			{
				m_currentBlocks.push(IfElse);

				boost::format fmt("%s");
				m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, v));
				m_blockDescriptors.push(std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>(v, v));
			}
		}*/

	}

	void examine_edge(const Graph::edge_descriptor& e, const Graph& g) 
	{
		std::cout << "Examine edge: " << e << std::endl;
		if (g[e.m_target]->GetType() == ActivityTrans::ActivityType::fork && g[e.m_target]->GetName()=="end")
		{
			forkPoints.push_back(e.m_source);
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
			m_currentThread++;
			if (m_threads.size() <= m_currentThread)
			{
				std::stack<std::pair<boost::format, Graph::vertex_descriptor>> stack;
				m_threads.push_back(stack);	
		}

			m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, e.m_source));

			auto findIter = std::find_if(m_history.begin(), m_history.end(),
				[&](Graph::vertex_descriptor& v)
				{
					graph_traits<Graph>::out_edge_iterator ei, edge_end;
					for (tie(ei, edge_end) = out_edges(e.m_target, g); ei != edge_end; ++ei)
						if (v == target(*ei, g))
							return 1;
					return 0;
				}
			);
			//*findIter ?? e.m_source
			m_blockDescriptors.push(std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>(e.m_target, *findIter));
		}
		else if (g[e.m_source]->GetType() == ActivityTrans::ActivityType::decigion)
		{
			m_currentBlocks.push(DoWhile);
			boost::format fmt("%s");
			m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, e.m_source));
			m_blockDescriptors.push(std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>(e.m_target, e.m_source));
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
		//if (m_currentBlocks.top() != IfElse && m_currentBlocks.top() != Fork)
			//	throw(new std::runtime_error("Forward or cross edge without IfElse or Fork!"));
		/*if (g[e.m_source]->GetType() == ActivityTrans::ActivityType::fork)
		{
			boost::format fmt("%s");
			m_currentThread++;
			if (m_threads.size() <= m_currentThread)
			{
				std::stack<std::pair<boost::format, Graph::vertex_descriptor>> stack;
				m_threads.push_back(stack);
			}

			m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, e.m_target));
		}*/
		if (m_currentBlocks.top() == IfElse)
		{
			boost::format fmt("%s");
			m_currentThread++;
			if (m_threads.size() <= m_currentThread)
			{
				std::stack<std::pair<boost::format, Graph::vertex_descriptor>> stack;
				m_threads.push_back(stack);
			}

			m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, e.m_target));
		}

	}

	void finish_vertex(const Graph::vertex_descriptor& v, const Graph& g)
	{
		using namespace boost;
		std::cout << "Finish vertex: " << v << std::endl;

		if (g[v]->GetType() == ActivityTrans::ActivityType::action)
		{
			if ((out_degree(v, g) == 0)||
				find(forkPoints.begin(), forkPoints.end(), v) != forkPoints.end()) //значит мы в потоке
			{
				int new_size = 1;
				if (forkSize.size() > 0)
				{
					new_size = forkSize.top() + 1;
					forkSize.pop();
				}
				forkSize.push(new_size);
				m_currentThread++;
				if (m_threads.size() <= m_currentThread)
				{
					std::stack<std::pair<boost::format, Graph::vertex_descriptor>> stack;
					m_threads.push_back(stack);
				}
				if((out_degree(v, g) == 0)) forkForever.push_back(m_currentThread);
				boost::format fmt("%s");
				m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, v));
			}
			
			boost::format fmt("%s" + (m_threads[m_currentThread].top().first % "").str());
			fmt % (g[v]->GetBody() + "\n");
			const auto pair = std::pair<boost::format, Graph::vertex_descriptor>(boost::format("%s" + fmt.str()), v);
			m_threads[m_currentThread].top() = pair;
			/*BOOST_FOREACH(const Effect & effect, g[v]->GetEffects())
			{
				boost::format fmt("%s" + (m_threads[m_currentThread].top().first % "").str());
				fmt % (effect.GetBody() + "\n");

				const auto pair = std::pair<boost::format, Graph::vertex_descriptor>(boost::format("%s" + fmt.str()), v);

				m_threads[m_currentThread].top() = pair;
			}*/
		}

		if (!m_currentBlocks.empty() && (m_currentBlocks.top() == DoWhile))
		{
			if (m_blockDescriptors.top().first == v)
			{
				// Do {} while ($guard)
				boost::format fmt("do\n{\n%s}\nwhile(%s);\n%s");
				string link_body = get(edge_all, g)[edge(m_blockDescriptors.top().second, v, g).first]->GetBogy();
				const std::string loopBody = (PopFromStack(m_threads[m_currentThread]).first % "").str();
				const std::string loop = "%s" +
					(fmt % loopBody % link_body % (PopFromStack(m_threads[m_currentThread]).first % "").str()).str();
				const auto pair = std::pair<boost::format, Graph::vertex_descriptor>(boost::format(loop), v);

				m_threads[m_currentThread].push(pair);
				m_blockDescriptors.pop();
				m_currentBlocks.pop();
				return;
			}
		}
		else if (!m_currentBlocks.empty() && (m_currentBlocks.top() == While))
		{
			if (m_blockDescriptors.top().second == v)
			{
				// While ($guard) { }
				boost::format fmt("while(%s)\n{\n%s}\n");
				string link_body = get(edge_all, g)[edge(m_blockDescriptors.top().first, v, g).first]->GetBogy();
				const std::string loop = (fmt % link_body % (PopFromStack(m_threads[m_currentThread]).first % "").str()).str() + "%s";

				const auto pair = std::pair<boost::format, Graph::vertex_descriptor>(boost::format(loop), v);

				m_threads[m_currentThread].push(pair);
			}
		}

		if (g[v]->GetType() == ActivityTrans::ActivityType::fin)
		{
			if (out_degree(v, g))
				throw(new std::runtime_error("ActivityFinalNode must have no output edges!"));
			string body;
		    body = "%s" + g[v]->GetBody() + "\n}\n";
			format fmt((m_threads[m_currentThread].top().first % body).str());
			auto pair = std::pair<format, size_t>(fmt, v);
			m_threads[m_currentThread].top() = pair;

		}
		else if (g[v]->GetType() == ActivityTrans::ActivityType::init)
		{
			if (in_degree(v, g))
				throw(new std::runtime_error("InitialNode must have no input edges!"));
			string body = g[v]->GetBody() + "\n{\n";
			m_output = (m_threads[m_currentThread].top().first % body).str();
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
				if (!m_blockDescriptors.empty() && (m_threads[m_currentThread].top().second == m_blockDescriptors.top().first))
				{
					const std::string notLoop = (m_threads[m_currentThread - 1].top().first % "").str();
					format fmt("%s" + (m_threads[m_currentThread].top().first % notLoop).str());
					cout << notLoop;
					m_threads.erase(m_threads.begin()+m_currentThread);
					m_currentThread--;

					auto pair = std::pair<format, size_t>(fmt, v);
					m_threads[m_currentThread].top() = pair;
					m_blockDescriptors.pop();
					m_currentBlocks.pop();
					return;
				}
				else if (!m_blockDescriptors.empty() && (m_threads[m_currentThread - 1].top().second == m_blockDescriptors.top().first))
				{
					const std::string notLoop = (m_threads[m_currentThread].top().first % "").str();
					format fmt("%s" + (m_threads[m_currentThread - 1].top().first % notLoop).str());
					m_threads.erase(m_threads.begin() + m_currentThread);
					m_currentThread--;
					auto pair = std::pair<format, size_t>(fmt, v);
					m_threads[m_currentThread].top() = pair;
					m_blockDescriptors.pop();
					m_currentBlocks.pop();
					return;
				}
			}
			else if (!m_currentBlocks.empty() && (m_currentBlocks.top() == IfElse))
			{
				const std::string Thread1 = (m_threads[m_currentThread].top().first % "").str();
				const string linkBody = get(edge_all, g)[edge(v, m_threads[m_currentThread].top().second, g).first]->GetBogy();
				

				const std::string Thread2 = (m_threads[(m_currentThread - 1)>0? m_currentThread - 1:0].top().first % "").str();
				cout << edge(v, m_threads[m_currentThread - 1].top().second, g).first;
				const string linkBody2 = get(edge_all, g)[edge(v, m_threads[m_currentThread - 1].top().second, g).first]->GetBogy();
				
				format fmt("if (%s)\n{\n%s}\nelse if (%s)\n{\n%s}\n%s");
				m_threads[m_currentThread - 1].pop();
				const std::string Block = "%s" + (fmt % linkBody % Thread1 % linkBody2 % Thread2 % (m_threads[m_currentThread - 1].top().first % "").str()).str();
				m_threads[m_currentThread].pop();
				m_currentThread--;

				auto pair = std::pair<format, size_t>(boost::format(Block), v);
				m_threads[m_currentThread].top() = pair;
				m_blockDescriptors.pop();
				m_currentBlocks.pop();
			}
		}
		if (g[v]->GetType() == ActivityTrans::ActivityType::fork && in_degree(v, g) == 1)
		{
			auto fork_size = forkSize.top();
			auto main = 0;
			string Block = "";
			vector<string> wait;
			//vector<string> forkBody
			int end = m_currentThread - fork_size;
			for (int i = m_currentThread; i > end; i--)
			{
				string linkBody="";
				string body="";
				if(i!= main)
				if (i >= 0)
				{
					bool join=true;
					auto iter_fork=std::find(forkForever.begin(), forkForever.end(), i);
					for (int j = 0; j < forkForever.size(); j++)
					{
						if (forkForever[j] == i)
						{
							join = false;
							break;
						}
					}
					//format fmt("if (%s)\n{\n%s}\nelse if (%s)\n{\n%s}\n%s");
					format fmt("std::thread thread%s([%s]()\n{\n%s});\n");
					format waitfmt("thread%s.join();\n");
					body = (m_threads[i].top().first % "").str();
					//cout << m_threads[i].top().first << m_threads[i].top().second;
					linkBody = get(edge_all, g)[edge(v, m_threads[i].top().second, g).first]->GetBogy();
					if ((linkBody == "main"))
					{
						main = i;
						continue;
					}
					thread_name++;
					if(join)
						wait.push_back((waitfmt% thread_name).str());
					Block += (fmt % thread_name % linkBody % body).str();
					
				}
			}

			Block += (m_threads[main].top().first % "").str();
			for (auto iter = wait.begin(); iter != wait.end(); iter++)
				Block += *iter;
			for (int i = m_currentThread; i > end; i--)
				m_threads[i].pop();
			if (m_currentThread < 0) m_currentThread = 0;

			Block+= "%s";
			m_currentThread = end;
			Block += (m_threads[m_currentThread].top().first % "").str();
			auto pair = std::pair<format, size_t>(boost::format(Block), v);
			m_threads[m_currentThread].top()=pair;
			forkSize.pop();
		}
		if (in_degree(v, g) >= 2)
		{
			if (g[v]->GetType() != ActivityTrans::ActivityType::fork)
			{
				if (m_blockDescriptors.empty() || m_blockDescriptors.top().first != v)
				{
					m_currentBlocks.push(IfElse);

					boost::format fmt("%s");
					m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, v));
					m_blockDescriptors.push(std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>(v, v));
				}
			}
			/*else if(m_blockDescriptors.empty() || m_blockDescriptors.top().first != v)
			{
				m_currentThread++;
				m_currentBlocks.push(Fork);
				forkSize.push(in_degree(v, g)-1);
				boost::format fmt("%s");
				if (m_threads.size() <= m_currentThread)
				{
					std::stack<std::pair<boost::format, Graph::vertex_descriptor>> stack;
					m_threads.push_back(stack);
				}
				m_threads[m_currentThread].push(std::pair<boost::format, Graph::vertex_descriptor>(fmt, v));
			}*/
		}
	}

private:
	enum BlockType
	{
		DoWhile,
		While,
		IfElse,
		Fork
	};

	std::stack<BlockType> m_currentBlocks;
	std::stack<std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>> m_blockDescriptors;
	std::stack<int> forkSize;
	std::vector<int> forkForever;
	unsigned thread_name = 0;
	int m_currentThread;
	std::vector<std::stack<std::pair<boost::format, Graph::vertex_descriptor>>> m_threads;
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

