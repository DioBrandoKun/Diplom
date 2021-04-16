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
			//����, ���������� �� ���� ������, ��������� �� e.m_target ��-���� ���� decigion
			for (tie(ei, edge_end) = out_edges(e.m_target, g); ei != edge_end; ++ei	)				//�� �����  decigion ������� ��� �����
			{
				size_t targ = target(*ei, g);
				string body = get(edge_all, g)[*ei]->GetBogy();
				if (body != "")
				{
					in_loop = targ;																	//����� � ������� ������� � ����
				}
				else if (targ != e.m_target) out_loop = targ;										//����� ��� ������ ������� �� �����
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
			//����, ���������� �� ���� ������, ��������� �� e.m_source ��-���� ���� decigion
			for (tie(ei, edge_end) = out_edges(e.m_source, g); ei != edge_end; ++ei)
			{
				size_t point = target(*ei, g);
				//���� ������ ����� �� ����� �������, ������ ��� �� ���� �����
				if (get(edge_all, g)[*ei]->GetBogy() == "")
				{
					out_loop = point;
					break;
				}
			}
			graph_traits<Graph>::in_edge_iterator in, inge_end;
			Graph::vertex_descriptor start_loop;
			//����, ���������� �� ���� ������, �������� � e.m_source ��-���� ���� decigion
			for (tie(in, inge_end) = in_edges(e.m_source, g); in != inge_end; ++in)
			{
				//� ��� ���� ������ ������� ������ ���� �����
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
			//����� ���������� ��������� ������ �����, ���� ������� ����� ����� ������
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
			//�������� ���� ����� ������� � ������  fork
			if ((out_degree(v, g) == 0) ||
				find(m_forkPoints.begin(), m_forkPoints.end(), v) != m_forkPoints.end()
				)
			{
				if (m_forkFor)
				{
					//������� ����� �� ���������� thread.join
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
			//�������� �� ������ ���� �� ��-�� ����� fork �������� ����� ���� �����
			for (int i = 0; i < m_whileInfo.size(); i++)
			{
				if (v == m_whileInfo[i].start_loop)
					m_currentThread = m_whileInfo[i].creat_loop;

			}
			//������������ ������ ����
			boost::format fmt("%s" + (m_elementTable[m_currentThread] % "").str());
			fmt % (g[v]->GetBody() + "\n");
			//�������� ����� �� �������� ������� �������������� �������
			m_elementTable.erase(m_currentThread);
			m_currentThread = v;
			//���������� ������
			m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, boost::format("%s" + fmt.str())});
		}

		if (!m_currentBlocks.empty() && (m_currentBlocks.top() == DoWhile))
		{
			blockConst circle;
			bool find_circle = false;										//������� �������� ������ ��������� ���� �����
			//������� ����� ���� ������������ ������ ������ ������� (���� ����� � ��� ����������, ��� ������ ���� � ������� �����)
			//��� ������ ������� �������� ������ �����
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
				//��������� do{} while{} ��� ������ ��� �������� �� ������� back_edge
				boost::format fmt("do\n{\n%s\n}\nwhile(%s);\n%s");
				const std::string Loop = (m_elementTable[circle.end_loop] % "").str();
				const std::string linkBody = get(edge_all, g)[edge(circle.block_loop, v, g).first]->GetBogy();
				const std::string outLoop = (m_elementTable[circle.out_loop] % "").str();
				(fmt % Loop % linkBody % outLoop);
				//�� �������� ������� ������ �����
				m_elementTable.erase(circle.end_loop);
				m_elementTable.erase(circle.out_loop);
				m_currentThread = v;
				m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, "%s" + fmt.str()});
				//�������� ���������� �������
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
				bool find_while = false;	//�� ����� ����� ����
				//������� ����� ���� ������������ ������, ��� ������ ������� ������������� �����  decigion
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
					//���� ���� ������ ������ DoWhile
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
				//������� �� ����� ��� ����-������� ��������� disigion (��� �������� ���� �� ��� - ��� currentThread)
				for (tie(ei, edge_end) = out_edges(v, g); ei != edge_end; ++ei)
				{
					size_t point=target(*ei,g);
					if (point != v && point != m_currentThread)
					{
						taget = point;
						break;
					}
				}
				//��� ����� �� ������� � �������� ������ ������
				const std::string Thread1 = (m_elementTable[m_currentThread] % "").str();
				const std::string Thread2 = (m_elementTable[taget] % "").str();
				const string linkBody1 = get(edge_all, g)[edge(v, m_currentThread, g).first]->GetBogy();
				const string linkBody2 = get(edge_all, g)[edge(v, taget, g).first]->GetBogy();
				format fmt("if (%s)\n{\n%s}\nelse if (%s)\n{\n%s}\n%s");
				//������� ���� ������ ��� ��������� ���� ifelse
				size_t back_pos = m_ifelseBack.top();
				const std::string Block = "%s" + (fmt % linkBody1 % Thread1 % linkBody2 % Thread2 % (m_elementTable[back_pos] %  "").str()).str();
				//�������� ������ ������
				m_elementTable.erase(back_pos);
				m_elementTable.erase(taget);
				m_elementTable.erase(m_currentThread);
				m_currentThread = v;
				m_elementTable.insert(std::pair<Graph::vertex_descriptor, boost::format>{v, Block});
				//������� ����� �������� � �������
				m_ifelseBack.pop();
				m_currentBlocks.pop();
			}
			else if (!m_currentBlocks.empty() && (m_currentBlocks.top() == If))
			{
				//���� ��� ����� � ������ currentThread ��� ��������� ��� � IfElse
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
				//������� ����� �� �������������� 
				m_forkForever.push_back(m_currentThread);
				m_forkFor = false;
			}
			static unsigned thread_name = 0;	//����� ��� �������
			graph_traits<Graph>::out_edge_iterator ei, edge_end;
			string Block = "";
			size_t main = -1;			//���������� �������� ���
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
			//������ ������ ������ � �������
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
				bool ifblock = false;//���� ��������� ������ ������ if
				for (tie(ei, edge_end) = in_edges(v, g); ei != edge_end; ++ei)
				{
					if (g[source(*ei, g)]->GetType() == ActivityTrans::ActivityType::decigion)
						ifblock = !ifblock;
				}
				if (!ifblock)
				{
					//��������� ����� ����� ������ � ������� �������
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
		size_t end_loop;													//���� ����� �����
		size_t out_loop;													//���� ������ �� �����
		size_t block_loop;													//���� ��������� ����
		size_t creat_loop;													//����� �������� �����
		size_t start_loop;													//���� ������ �����
	};
	std::stack<BlockType>								m_currentBlocks;	//������ ���� �������
	std::stack<Graph::vertex_descriptor>				m_ifelseBack;			//������ ���� ����������� ��� ����� if
	size_t												m_creator = -2;		//������� ��� ��� �������
	std::map<Graph::vertex_descriptor, boost::format>	m_elementTable;		//������� ������, ������ ������� ��� ��� ������ ������
	Graph::vertex_descriptor							m_currentThread;		//������� ����� 
	std::vector < blockConst>							m_whileInfo;			//������ ���������� � ������, ������ � ���������� 
	std::vector<size_t>									m_forkBack;			//������ ���� ����������� ��� ����� fork
	std::vector<int>									m_forkForever;		//������ �����, ������� �� �������������� � ���� ��������� 
	bool												m_forkFor = false;	//���������� �������� �� ������� ����� ��� �������������
	std::vector<int>									m_forkPoints;			//������ ������ ������, ������� ����������� ������ fork
	std::string&										m_output;			//�����
};


void GraphGen::AcivInit()
{   //����� ������� � ����� ActivityType::init 
	for (auto& elem : m_activTable)
	{
		if(elem.second->GetType() == ActivityTrans::ActivityType::init)
			m_head.push_back(elem.first);
	}
}
void GraphGen::InsideFork(const unsigned long start, vector<unsigned long>& out)
{	//����� ������ �������� � �������� join ������
	vector<unsigned long> forkPoints;
	Inside(start, out, forkPoints);
	for (size_t j = forkPoints.size() / 2; j < forkPoints.size(); j++)
		//�� �� ������� ��� fork ���� ��������������� ���� �� ������
		m_activTable[forkPoints[j]]->SetType(ActivityTrans::ActivityType::join);
}
void GraphGen::Inside(const unsigned long start, vector<unsigned long>& out, vector<unsigned long>&fork)
{	//����� ����� ��������, ��� ������ ���� �������������� ������
	auto headp=m_activTable[start];
	auto targetVec=headp->GetOut();
	auto out_iter=find_if(out.begin(), out.end(), 
		[start](unsigned long& input)
		{
			return start == input; //����� �� ����������� � ������ �����
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
	size_t initialNode;//��������� ������� �����
	std::unordered_map<unsigned long, size_t> idMap; //������� ��������� id � ������� ������ �����

	/*BOOST_FOREACH(const ActivityTrans & vertex, std::get<0>(m_activity))*/
	int i = 0;
	for(auto activity : activ_points)
	{
		//��������� ������� � �����
		const size_t VertexDescriptor = boost::add_vertex(std::make_shared<ActivityTrans>(*m_activTable[activity]), m_graph);
		if(!i)	//��������� ������� ������ �������
			initialNode = VertexDescriptor;
		i++;
		//���������� ����� ������ ��� ������� ����� ����������
		idMap[m_activTable[activity]->GetLocalId()] = VertexDescriptor;
	}
	for (auto activity : activ_points)
	{	//�� ����� ������ �����
		auto targets=m_activTable[activity]->GetOut();//������� � ����� ������� ��������� ������� �������
		for (auto one_targ : targets)
		{	//������ ���� ����� ����� - LinkTrans
			auto links=m_activTable[one_targ]->GetInLinks();//�������� ��� �����
			auto linkIter = find_if(links.begin(), links.end(), [activity](LinkTrans& inLink)
				{	//������� �����, � �������� �������� activity
					return (inLink.GetSource() == activity);
				}); //��������� � �����, ��� ��� ����� ����� ������� ������������ �� ��������� �� links.end()
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
