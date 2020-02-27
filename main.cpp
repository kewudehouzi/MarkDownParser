#include <iostream>
#include "MD_Parser_copy.h"
#include "head.h"
using namespace std;



int main(){
    MarkDownParser transformer("ForTest.md");

    std::string contents = transformer.getContents();

    std::string head = Get_Head(), end = Get_End();

    std::ofstream out;

    out.open("ForTest3.html");

    out << head << contents << end << endl;
}
