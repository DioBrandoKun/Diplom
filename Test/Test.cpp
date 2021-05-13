// Test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>//Парсер XML
#include "CustomParser.h"
#include "CodeGen.h"
using boost::property_tree::ptree;//Не находит тип через hpp
using namespace std;
int main(int args, char *argv[])
{
    string Path;
#ifndef DEBUG
    if (args != 2)
    {
        cout << "First programm\nSecond XMI\n";
        return -1;
    }
    else
    {
        Path = string(argv[1]);
        string check = Path.substr(Path.rfind(".") + 1);
        if (check != "xml")
        {
            cout << "XMI file must have xml format!";
            return -2;
        }
    }
#else
    Path = "B:/Torrent/UML/123456.xml";
#endif 
    ofstream fout; //Вывод для файла
    ptree pt;
    string name = Path.substr(Path.rfind("/")+1, Path.rfind(".") - Path.rfind("/"))+".cpp"; //Имя файла и формат
    fout.open(name);
    read_xml(Path, pt);
    string out;
    CustomParser Parser(pt);
    Parser.Parse();
    auto elements = Parser.Normalize();
    out = Parser.output;
    if (elements.size() > 0)
    {
        GraphGen graph(elements);
        out += graph.output;
    }
    fout << out;
    fout.close();
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
