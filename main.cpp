#include <iostream>
#include "MD_Parser.h"
#include "head.h"
using namespace std;



int main(){
    MarkDownParser transformer("Test.md");

    std::string contents = transformer.getContents();

    std::string head = Get_Head(), end = Get_End();

    std::ofstream out;

    out.open("Test3.html");

    out << head << contents << end << endl;
}
