#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "../renderer/dom.h"
#include <string>
#include <vector>

class HTMLParser {
public:
    DOMNode parse(const std::string& html);
    
private:
    std::string preprocessHTML(const std::string& html);
    DOMNode parseElement(const std::string& html, size_t& pos);
    std::string parseTagName(const std::string& html, size_t& pos);
    std::string parseText(const std::string& html, size_t& pos);
    void parseAttributes(const std::string& html, size_t& pos, DOMNode& node);
    void skipWhitespace(const std::string& html, size_t& pos);
};

#endif