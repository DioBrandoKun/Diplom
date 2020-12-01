#include "present.h"
IdMapType  IdMap::m_ids;
unsigned long IdMap::m_counter=0;
map<string, string>IdMap::IdName;

string RightFormat(string Input)//ќбрезание типа до нужно дл€ ипользовани€ в дальнейшем
{//Ћибо перевод в числовой формат не стандартного типа
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
XMI формат не позвол€ет различать символы: <,>,*,[,] 
все они представлены как '_'
это мешает нам вводить произвольные C++ типы
поэтому чтобы отлечить один тип от другого
мы смотрим их количество, кроме того между символами [,] должно находитьс€ число
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
				if (Num > 0)//≈сли между символами находитс€ число это скобочки массива
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