#ifndef DOM_H
#define DOM_H

#include <string>
#include <vector>
#include <map>

struct DOMNode {
    std::string tag_name;
    std::map<std::string, std::string> attributes;
    std::vector<DOMNode> children;
    std::string text_content;
    
    // Вспомогательные методы
    bool isTextNode() const { return tag_name == "#text"; }
    bool hasAttribute(const std::string& attr) const {
        return attributes.find(attr) != attributes.end();
    }
    std::string getAttribute(const std::string& attr) const {
        auto it = attributes.find(attr);
        return it != attributes.end() ? it->second : "";
    }
};

#endif