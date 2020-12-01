#include "present.h"
IdMapType  IdMap::m_ids;
unsigned long IdMap::m_counter=0;
map<string, string>IdMap::IdName;

string RightFormat(string Input)//��������� ���� �� ����� ��� ������������ � ����������
{//���� ������� � �������� ������ �� ������������ ����
	string Typer = "";
	int pos = Input.find("EAJava_");
	if (pos != -1)
	{

		Typer = Input.substr(strlen("EAJava_"));
		string Example = Format(Typer);
		return Example;
	}
	else
	{
		Typer = std::to_string(IdMap::Insert(Input).second);
	}
	return Typer;
}
/*
XMI ������ �� ��������� ��������� �������: <,>,*,[,] 
��� ��� ������������ ��� '_'
��� ������ ��� ������� ������������ C++ ����
������� ����� �������� ���� ��� �� �������
�� ������� �� ����������, ����� ���� ����� ��������� [,] ������ ���������� �����
*/
string Format(string Input)
{
	string Typer = Input;
	while (Typer.find('_') != string::npos)
		{
			unsigned Left = Typer.find_first_of("_");
			unsigned Right = Typer.find_last_of("_");
			string isNumber;
			if (Left !=  Right)
			{
				isNumber = Typer.substr(Left + 1,Right-Left);
				int Num = atoi(isNumber.c_str());
				if (Num > 0)//���� ����� ��������� ��������� ����� ��� �������� �������
				{
					Typer[Left] = '[';
					Typer[Right] = ']';
				}
				else
				{
					Typer[Left] = '<';
					Typer[Right] = '>';
				}
			}
			else
			{
				Typer[Left] = '*';
			}
		}
	return Typer;
}