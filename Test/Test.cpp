// Test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>//Парсер XML

#include "CustomParser.h"
#include "CodeGen.h"
using boost::property_tree::ptree;//Не находит тип через hpp

int main()
{

    ptree pt;
    std::string Path = "B:/Torrent/UML/123456.xml"; //Путь к файлу
    read_xml(Path, pt);
    CustomParser Parser(pt);
    Parser.Parse();
    GraphGen(Parser.Normalize());
    
    

    /*ActivityTrans Example("2", 1, "Govono");
    ActivityTrans Example2("1", 1, "Test");
    typedef adjacency_list <vecS, vecS, boost::bidirectionalS, std::shared_ptr<ActivityTrans>> vector_graph_t;


    // creates a graph with 4 vertices
    vector_graph_t g;

    // fills the property 'vertex_name_t' of the vertices
    //boost::put(vertex_name_t(), g, 0, Example); // set the property of a vertex
    //indexes[Example] = boost::vertex(0, g);     // retrives the associated vertex descriptor
    //boost::put(vertex_name_t(), g, 1, Example2);
    //indexes[Example2] = boost::vertex(1, g);
    const size_t VertexDescriptor = boost::add_vertex(std::make_shared<ActivityTrans>(Example), g);
    const size_t VertexDescriptor2 = boost::add_vertex(std::make_shared<ActivityTrans>(Example2), g);
    // adds the edges
    // indexes[edges[0].first] maps "aaa" to the associated vertex index*/
    //boost::add_edge(indexes[Example], indexes[Example2], std::make_shared<ActivityTrans>(Example), g);
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
