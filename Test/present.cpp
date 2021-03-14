#include "present.h"
IdMapType  IdMap::m_ids;
unsigned long IdMap::m_counter=1;
map<const string, const string>IdMap::IdName;

const string RightFormat(const string& Input)//ќбрезание типа до нужно дл€ ипользовани€ в дальнейшем
{//Ћибо перевод в числовой формат не стандартного типа
	string Typer = "";
	size_t pos = Input.find("EAJava_");
	if (pos != -1)
	{

		Typer = Input.substr(strlen("EAJava_"));
		string Example = Format(Typer);
		if (Example == "boolean") Example = "bool";//«десь строить услови€ дл€ особых типов, которые в c++ называютс€ по другому
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
const string Format(const string& Input)
{
	string Typer = Input;
	while (Typer.find('_') != string::npos)
		{
			size_t Left = Typer.find_first_of("_");
			size_t Right = Typer.find_last_of("_");
			string isNumber;

			if (Left !=  Right && Right-Left>1)
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