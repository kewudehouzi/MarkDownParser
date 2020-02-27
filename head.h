#include <string>


std::string Get_Head(){
    std::string head = "<!DOCTYPE html>\n\t<html>\n\t<head>\
    \n\t\t<meta charset=\"UTF-8\">\n\t\t<title></title>\
    \n\t\t<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/gh/Microsoft/vscode/extensions/markdown-language-features/media/markdown.css\">\n\t\t\
    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/gh/Microsoft/vscode/extensions/markdown-language-features/media/highlight.css\">\n\t\
    </head>\n\t<body class=\"vscode-light\">\n";

    return head;
}

std::string Get_End(){
    std:: string end= "</body></html>";
    return end;
}
