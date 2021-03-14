#include "CodeGen.h"
void CodeGen::AcivInit()
{
	for (auto& key : activTable)
	{
		if (key.second->GetType() == ActivityTrans::ActivityType::init)
		{
			size_t count = key.second->GetInCount();
			if (!count)
			{
				head.push_back(key.first);
			}
			else
			{
				key.second->SetFin();
			}
		}
	}
}
void CodeGen::Inside(const unsigned long start, vector<unsigned long>& out)
{
	auto headp=activTable[start];
	auto targetVec=headp->GetOut();
	out.push_back(start);
	for (auto elem : targetVec)
	{
		Inside(elem,out);
	}
}
void CodeGen::InsideEnd(const unsigned long start, bool& wayFin)//Проверяем есть ли путь от данного элемента до элемента конца
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
void CodeGen::Gen(const unsigned long start)
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
int CodeGen::Connect(const unsigned long dis, unsigned long& elem_out)//1 if else
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