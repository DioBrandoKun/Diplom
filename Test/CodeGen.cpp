#include "CodeGen.h"
void GraphGen::AcivInit()
{   //����� ������� � ����� ActivityType::init
	auto head_iter=find_if(activTable.begin(), activTable.end(),
		[](auto& imp) 
		{
			return imp.second->GetType() == ActivityTrans::ActivityType::init; 
		});
	if(head_iter!= activTable.end())
		head.push_back(head_iter->first);
}
void GraphGen::Inside(const unsigned long start, vector<unsigned long>& out)
{	//����� ����� ��������, ��� ������ ���� �������������� ������
	auto headp=activTable[start];
	auto targetVec=headp->GetOut();
	auto out_iter=find_if(out.begin(), out.end(), 
		[start](unsigned long& input)
		{
			return start == input; //����� �� ����������� � ������ �����
		});
	if(out_iter == out.end())
		out.push_back(start);
	for (auto elem : targetVec)
	{
		Inside(elem,out);
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
		const size_t VertexDescriptor = boost::add_vertex(std::make_shared<ActivityTrans>(*activTable[activity]), m_graph);
		if(!i)	//��������� ������� ������ �������
			initialNode = VertexDescriptor;
		i++;
		//���������� ����� ������ ��� ������� ����� ����������
		idMap[activTable[activity]->GetLocalId()] = VertexDescriptor;
	}
	for (auto activity : activ_points)
	{	//�� ����� ������ �����
		auto targets=activTable[activity]->GetOut();//������� � ����� ������� ��������� ������� �������
		for (auto one_targ : targets)
		{	//������ ���� ����� ����� - LinkTrans
			auto links=activTable[one_targ]->GetInLinks();//�������� ��� �����
			auto linkIter = find_if(links.begin(), links.end(), [activity](LinkTrans& inLink)
				{	//������� �����, � �������� �������� activity
					return (inLink.GetSource() == activity);
				}); //��������� � �����, ��� ��� ����� ����� ������� ������������ �� ��������� �� links.end()
			boost::add_edge(idMap[activity], idMap[one_targ], std::make_shared<LinkTrans>(*linkIter), m_graph);
		}
	}
	boost::write_graphviz(std::cout, m_graph);
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
void GraphGen::InsideEnd(const unsigned long start, bool& wayFin)//��������� ���� �� ���� �� ������� �������� �� �������� �����
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
	if (headp->GetType() == ActivityTrans::ActivityType::action && count_in > 1)//���� do while
	{
		cout << "do";
		dowhile.push_back(headp->GetLocalId());
	}
	if (headp->GetType() == ActivityTrans::ActivityType::decigion)
	{
		unsigned long elem;//�������, ������� ��������� if/else �����������
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
//2 if ����� ����� �����
//3 if ����� ������ �����
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