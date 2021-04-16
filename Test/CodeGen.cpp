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
		m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{-1, boost::format("%s")});
		m_currentThread = -1;
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
	}

	void examine_edge(const Graph::edge_descriptor& e, const Graph& g) 
	{
		std::cout << "Examine edge: " << e << std::endl;
		if (g[e.m_target]->GetType() == ActivityTrans::ActivityType::join)
		{
			m_forkPoints.push_back(e.m_source);
			auto iter = find(m_forkBack.begin(), m_forkBack.end(), e.m_target);
			if(iter == m_forkBack.end())
				m_forkBack.push_back(e.m_target);
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
			Graph::vertex_descriptor in_loop;
			Graph::vertex_descriptor out_loop;
			//Цикл, проходящий по всем ребрам, исходящим из e.m_target то-есть блок decigion
			for (tie(ei, edge_end) = out_edges(e.m_target, g); ei != edge_end; ++ei	)				//Из блока  decigion выходят две ветви
			{
				size_t targ = target(*ei, g);
				string body = get(edge_all, g)[*ei]->GetBogy();
				if (body != "")
				{
					in_loop = targ;																	//Ветвь с текстом заходит в цикл
				}
				else if (targ != e.m_target) out_loop = targ;										//Ветвь без текста выводит из цикла
			}


			blockConst whiledata;
			whiledata.block_loop = e.m_target;
			whiledata.out_loop = out_loop;
			whiledata.end_loop = in_loop;
			whiledata.start_loop = e.m_source;
			m_creator--;
			m_currentThread = m_creator;
			whiledata.creat_loop = m_creator;
			m_whileInfo.push_back(whiledata);
			m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{m_creator, fmt});

		}
		else if (g[e.m_source]->GetType() == ActivityTrans::ActivityType::decigion)
		{
			m_currentBlocks.push(DoWhile);
			graph_traits<Graph>::out_edge_iterator ei, edge_end;
			Graph::vertex_descriptor out_loop;
			//Цикл, проходящий по всем ребрам, исходящим из e.m_source то-есть блок decigion
			for (tie(ei, edge_end) = out_edges(e.m_source, g); ei != edge_end; ++ei)
			{
				size_t point = target(*ei, g);
				//Если данное ребро не имело условия, значит это не блок цикла
				if (get(edge_all, g)[*ei]->GetBogy() == "")
				{
					out_loop = point;
					break;
				}
			}
			graph_traits<Graph>::in_edge_iterator in, inge_end;
			Graph::vertex_descriptor start_loop;
			//Цикл, проходящий по всем ребрам, входящим в e.m_source то-есть блок decigion
			for (tie(in, inge_end) = in_edges(e.m_source, g); in != inge_end; ++in)
			{
				//В наш блок должно входить только одно ребро
				size_t circleSt = source(*in, g);
				if (circleSt != e.m_source)
				{
					start_loop = circleSt;
					break;
				}
			}
			blockConst whiledata;
			whiledata.block_loop = e.m_source;
			whiledata.out_loop = out_loop;
			whiledata.end_loop = e.m_target;
			whiledata.start_loop = start_loop;
			boost::format fmt("%s");
			m_creator--;
			whiledata.creat_loop = m_creator;
			m_currentThread = m_creator;
			m_whileInfo.push_back(whiledata);
			m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{m_creator, fmt});
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
			//Здесь начинается обработка второй ветви, надо создать новый поток данных
			boost::format fmt("%s");
			m_creator--;
			m_currentThread = m_creator;
			m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{m_creator, fmt});
		}
	}

	void finish_vertex(const Graph::vertex_descriptor& v, const Graph& g)
	{
		using namespace boost;
		std::cout << "Finish vertex: " << v << std::endl;
		if (g[v]->GetType() == ActivityTrans::ActivityType::action)
		{
			//Проверка если ветвь связана с блоком  fork
			if ((out_degree(v, g) == 0) ||
				find(m_forkPoints.begin(), m_forkPoints.end(), v) != m_forkPoints.end()
				)
			{
				if (m_forkFor)
				{
					//Прошлая ветвь не использует thread.join
					m_forkForever.push_back(m_currentThread);
					m_forkFor = false;
				}
				if (out_degree(v, g) == 0) 
					m_forkFor = true;
				boost::format fmt("%s");
				m_creator--;
				m_currentThread = m_creator;
				m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{m_creator, fmt});
			}
			//Проверка на случай если мы из-за блока fork потеряли ветвь тела цикла
			for (int i = 0; i < m_whileInfo.size(); i++)
			{
				if (v == m_whileInfo[i].start_loop)
					m_currentThread = m_whileInfo[i].creat_loop;

			}
			//Формирование нового кода
			boost::format fmt("%s" + (m_elementTable[m_currentThread] % "").str());
			fmt % (g[v]->GetBody() + "\n");
			//Удаление чтобы не засорять таблицу промежуточными ветвями
			m_elementTable.erase(m_currentThread);
			m_currentThread = v;
			//Сохранение записи
			m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, boost::format("%s" + fmt.str())});
		}

		if (!m_currentBlocks.empty() && (m_currentBlocks.top() == DoWhile))
		{
			blockConst circle;
			bool find_circle = false;										//Элемент является концом обработки тела цикла
			//Смотрим среди всех существующих циклов данный элемент (Хотя можно и для последнего, они должны быть в формате стека)
			//Где данный элемент является концом цикла
			for (auto i = m_whileInfo.begin(); i != m_whileInfo.end(); i++)
			{
				if (i->end_loop == v)
				{
					circle = *i;
					m_whileInfo.erase(i);
					find_circle = true;
					break;
				}
			}
			if (find_circle)
			{
				//Формируем do{} while{} все номера нам известны из функции back_edge
				boost::format fmt("do\n{\n%s\n}\nwhile(%s);\n%s");
				const std::string Loop = (m_elementTable[circle.end_loop] % "").str();
				const std::string linkBody = get(edge_all, g)[edge(circle.block_loop, v, g).first]->GetBogy();
				const std::string outLoop = (m_elementTable[circle.out_loop] % "").str();
				(fmt % Loop % linkBody % outLoop);
				//Не забываем удалить лишние ветви
				m_elementTable.erase(circle.end_loop);
				m_elementTable.erase(circle.out_loop);
				m_currentThread = v;
				m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, "%s" + fmt.str()});
				//Действие обработано удаляем
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
			m_elementTable.erase(-1);
			m_currentThread = v;
			m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, body});

		}
		else if (g[v]->GetType() == ActivityTrans::ActivityType::init)
		{
			if (in_degree(v, g))
				throw(new std::runtime_error("InitialNode must have no input edges!"));
			string body = g[v]->GetBody() + "\n{\n";
			m_output = (m_elementTable[m_currentThread] % body).str();
			return;
		}
		else if (g[v]->GetType() == ActivityTrans::ActivityType::decigion)
		{
			const size_t CountOfOutEdges = out_degree(v, g);
			if (CountOfOutEdges != 2)
				throw(new std::runtime_error("More than two output edges is not supported"));

			if (!m_currentBlocks.empty() && (m_currentBlocks.top() == While))
			{
				blockConst circle;
				bool find_while = false;	//Не нашли такой цикл
				//Смотрим среди всех существующих циклов, где данный элемент соответствует блоку  decigion
				for (auto i= m_whileInfo.begin();i!= m_whileInfo.end();i++)
				{
					if (i->block_loop == v)
					{
						circle = *i;
						find_while = true;
						m_whileInfo.erase(i);
						break;
					}
				}
				if (find_while) 
				{
					//Если цикл найден смотри DoWhile
					const std::string Loop = (m_elementTable[circle.end_loop] % "").str();
					const std::string linkBody = get(edge_all, g)[edge(v, circle.end_loop, g).first]->GetBogy();
					const std::string outLoop = (m_elementTable[circle.out_loop] % "").str();

					format fmtloop("while(%s)\n{\n%s}\n%s");
					(fmtloop % linkBody % Loop % outLoop);
					m_elementTable.erase(circle.end_loop);
					m_elementTable.erase(circle.out_loop);
					m_currentThread = v;
					m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, "%s" + fmtloop.str()});
					m_currentBlocks.pop();
					return;
				}
			}
			else if (!m_currentBlocks.empty() && (m_currentBlocks.top() == IfElse))
			{
				graph_traits<Graph>::out_edge_iterator ei, edge_end;
				size_t taget;
				//Находим на какой еще блок-вершину указывает disigion (нам известен один из них - это currentThread)
				for (tie(ei, edge_end) = out_edges(v, g); ei != edge_end; ++ei)
				{
					size_t point=target(*ei,g);
					if (point != v && point != m_currentThread)
					{
						taget = point;
						break;
					}
				}
				//Код берем из таблицы с номерами данных вершин
				const std::string Thread1 = (m_elementTable[m_currentThread] % "").str();
				const std::string Thread2 = (m_elementTable[taget] % "").str();
				const string linkBody1 = get(edge_all, g)[edge(v, m_currentThread, g).first]->GetBogy();
				const string linkBody2 = get(edge_all, g)[edge(v, taget, g).first]->GetBogy();
				format fmt("if (%s)\n{\n%s}\nelse if (%s)\n{\n%s}\n%s");
				//Смотрим куда должен был вернуться блок ifelse
				size_t back_pos = m_ifelseBack.top();
				const std::string Block = "%s" + (fmt % linkBody1 % Thread1 % linkBody2 % Thread2 % (m_elementTable[back_pos] %  "").str()).str();
				//Затираем лишние записи
				m_elementTable.erase(back_pos);
				m_elementTable.erase(taget);
				m_elementTable.erase(m_currentThread);
				m_currentThread = v;
				m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, Block});
				//Удаляем точку возврата и событие
				m_ifelseBack.pop();
				m_currentBlocks.pop();
			}
			else if (!m_currentBlocks.empty() && (m_currentBlocks.top() == If))
			{
				//Весь код лежит в ячейке currentThread все остальное как в IfElse
				const std::string Thread1 = (m_elementTable[m_currentThread] % "").str();
				const string linkBody1 = get(edge_all, g)[edge(v, m_currentThread, g).first]->GetBogy();
				format fmt("if (%s)\n{\n%s}\n%s");
				size_t back_pos = m_ifelseBack.top();
				const std::string Block = "%s" + (fmt % linkBody1 % Thread1 % (m_elementTable[back_pos] % "").str()).str();
				m_elementTable.erase(back_pos);
				m_elementTable.erase(m_currentThread);
				m_currentThread = v;
				m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, Block});
				m_ifelseBack.pop();
				m_currentBlocks.pop();
			}
		}
		if (g[v]->GetType() == ActivityTrans::ActivityType::fork && in_degree(v, g) == 1 && g[v]->GetName() != "end")
		{
			if (m_forkFor)
			{
				//Текущий поток не присоединяется 
				m_forkForever.push_back(m_currentThread);
				m_forkFor = false;
			}
			static unsigned thread_name = 0;	//Имена для потоков
			graph_traits<Graph>::out_edge_iterator ei, edge_end;
			string Block = "";
			size_t main = -1;			//Определяет основной код
			vector<string> wait;
			for (tie(ei, edge_end) = out_edges(v, g); ei != edge_end; ++ei)
			{
				size_t point = target(*ei, g);
				if (point != v)
				{
					string linkBody = "";
					string body = "";
					auto iter_forever = std::find(m_forkForever.begin(), m_forkForever.end(), point);
					format fmt("std::thread thread%s([%s]()\n{\n%s});\n");
					format waitfmt("thread%s.join();\n");
					body = (m_elementTable[point] % "").str();
					linkBody = get(edge_all, g)[edge(v, point, g).first]->GetBogy();
					if ((linkBody == "main"))
					{
						main = point;
						continue;
					}
					thread_name++;
					if (iter_forever == m_forkForever.end())
					{
						wait.push_back((waitfmt % thread_name).str());
					}
					else m_forkForever.erase(iter_forever);
					Block += (fmt % thread_name % linkBody % body).str();
					m_elementTable.erase(point);

				}
			}
			Block += (m_elementTable[main] % "").str();
			m_elementTable.erase(main);
			for (auto iter = wait.begin(); iter != wait.end(); iter++)
				Block += *iter;
			m_currentThread = v;
			auto back= m_forkBack.begin()+ m_forkBack.size()-1;
			Block += (m_elementTable[*back] % "").str();
			m_elementTable.erase(*back);
			auto pair = std::pair<size_t,format>(v, boost::format("%s"+Block));
			m_elementTable.insert(pair);
		}	
		if (g[v]->GetType() == ActivityTrans::ActivityType::join)
		{
			//Просто меняем запись в таблице
			format fmt = m_elementTable[m_currentThread];
			m_elementTable.erase(m_currentThread);
			m_currentThread = v;
			m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, fmt});
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
					m_ifelseBack.push(v);
					m_currentBlocks.push(IfElse);
				}
				else
				{
					m_ifelseBack.push(v);
					m_currentBlocks.push(If);
				}
				m_creator--;
				boost::format fmt("%s");
				m_currentThread = m_creator;
				m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{m_creator, fmt});
			}
		}
	}	
private:
	enum BlockType
	{
		DoWhile,
		While,
		IfElse,
		Fork,
		If
	};
	struct blockConst
	{
		size_t end_loop;													//Блок конца цикла
		size_t out_loop;													//Блок выхода из цикла
		size_t block_loop;													//Блок создающий цикл
		size_t creat_loop;													//Поток создания цикла
		size_t start_loop;													//Блок начала цикла
	};
	std::stack<BlockType>								m_currentBlocks;	//Хранит стек событий
	std::stack<Graph::vertex_descriptor>				m_ifelseBack;			//Хранит стек возвращения для блоко if
	size_t												m_creator = -2;		//Счетчик для доп потоков
	std::map<Graph::vertex_descriptor, boost::format>	m_elementTable;		//Таблица обхода, хранит текущих код для разных ветвей
	Graph::vertex_descriptor							m_currentThread;		//Текущая ветвь 
	std::vector < blockConst>							m_whileInfo;			//Хранит информацию о циклах, начале и завершении 
	std::vector<size_t>									m_forkBack;			//хранит стек возвращения для блока fork
	std::vector<int>									m_forkForever;		//Хранит ветви, которые не присоединяются к коду программы 
	bool												m_forkFor = false;	//Определяет является ли текущая ветвь без присоединения
	std::vector<int>									m_forkPoints;			//Хранит номера ветвей, которые разделяются блоков fork
	std::string&										m_output;			//Вывод
};


void GraphGen::AcivInit()
{   //Поиск вершины с типом ActivityType::init 
	for (auto& elem : m_activTable)
	{
		if(elem.second->GetType() == ActivityTrans::ActivityType::init)
			m_head.push_back(elem.first);
	}
}
void GraphGen::InsideFork(const unsigned long start, vector<unsigned long>& out)
{	//Вызов обхода вглубину и создание join вершин
	vector<unsigned long> forkPoints;
	Inside(start, out, forkPoints);
	for (size_t j = forkPoints.size() / 2; j < forkPoints.size(); j++)
		//Мы не считаем что fork идут последовательно друг за другом
		m_activTable[forkPoints[j]]->SetType(ActivityTrans::ActivityType::join);
}
void GraphGen::Inside(const unsigned long start, vector<unsigned long>& out, vector<unsigned long>&fork)
{	//Обход графа вглубину, для поиска всех использующихся вершин
	auto headp=m_activTable[start];
	auto targetVec=headp->GetOut();
	auto out_iter=find_if(out.begin(), out.end(), 
		[start](unsigned long& input)
		{
			return start == input; //Чтобы не повторяться в случае цикла
		});
	if (headp->GetType() == ActivityTrans::ActivityType::fork) fork.push_back(headp->GetLocalId());
	if(out_iter == out.end())
		out.push_back(start);
	for (auto elem : targetVec)
	{
		if(find(out.begin(), out.end(),elem)==out.end())
		Inside(elem,out,fork);
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
		const size_t VertexDescriptor = boost::add_vertex(std::make_shared<ActivityTrans>(*m_activTable[activity]), m_graph);
		if(!i)	//Начальная вершина только нулевая
			initialNode = VertexDescriptor;
		i++;
		//Запоминаем номер вешины для данного блока активности
		idMap[m_activTable[activity]->GetLocalId()] = VertexDescriptor;
	}
	for (auto activity : activ_points)
	{	//На графе ставим ребра
		auto targets=m_activTable[activity]->GetOut();//Смотрим в какие вершины переходит текущая вершина
		for (auto one_targ : targets)
		{	//Теперь надо найти ребро - LinkTrans
			auto links=m_activTable[one_targ]->GetInLinks();//Получаем все ребра
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
