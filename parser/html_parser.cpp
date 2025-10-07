#include "../parser/html_parser.h"
#include <iostream>
#include <cctype>

DOMNode HTMLParser::parse(const std::string& html) {
    std::string processed_html = preprocessHTML(html);
    DOMNode root;
    root.tag_name = "root";
    
    size_t pos = 0;
    while (pos < processed_html.length()) {
        skipWhitespace(processed_html, pos);
        if (pos >= processed_html.length()) break;
        
        if (processed_html[pos] == '<') {
            DOMNode element = parseElement(processed_html, pos);
            if (!element.tag_name.empty()) {
                root.children.push_back(element);
            }
        } else {
            std::string text = parseText(processed_html, pos);
            if (!text.empty()) {
                DOMNode text_node;
                text_node.tag_name = "#text";
                text_node.text_content = text;
                root.children.push_back(text_node);
            }
        }
    }
    
    return root;
}

std::string HTMLParser::preprocessHTML(const std::string& html) {
    std::string result;
    bool in_script = false;
    bool in_style = false;
    
    for (size_t i = 0; i < html.length(); i++) {
        if (html.substr(i, 7) == "<script") {
            in_script = true;
        } else if (html.substr(i, 9) == "</script>") {
            in_script = false;
            i += 8;
        } else if (html.substr(i, 6) == "<style") {
            in_style = true;
        } else if (html.substr(i, 8) == "</style>") {
            in_style = false;
            i += 7;
        }
        
        if (!in_script && !in_style) {
            result += html[i];
        }
    }
    
    return result;
}

DOMNode HTMLParser::parseElement(const std::string& html, size_t& pos) {
    DOMNode node;
    
    if (html[pos] != '<') return node;
    pos++;
    
    // Проверка наличия закрывающего тега
    if (pos < html.length() && html[pos] == '/') {
        pos++;
        std::string tag_name = parseTagName(html, pos);
        while (pos < html.length() && html[pos] != '>') pos++;
        if (pos < html.length()) pos++; // Skip '>'
        return node; // Вернуть пустой узел для закрывающих тегов
    }
    
    node.tag_name = parseTagName(html, pos);
    parseAttributes(html, pos, node);
    
    // Самозакрывающиеся теги
    if (pos < html.length() && html.substr(pos, 2) == "/>") {
        pos += 2;
        return node;
    }
    
    if (pos < html.length() && html[pos] == '>') {
        pos++;
    }
    
    while (pos < html.length()) {
        skipWhitespace(html, pos);
        if (pos >= html.length()) break;
        
        // Проверка наличия закрывающего тега
        if (html[pos] == '<' && pos + 1 < html.length() && html[pos + 1] == '/') {
            std::string closing_tag = "</" + node.tag_name + ">";
            if (html.substr(pos, closing_tag.length()) == closing_tag) {
                pos += closing_tag.length();
                break;
            }
        }
        
        if (html[pos] == '<') {
            DOMNode child = parseElement(html, pos);
            if (!child.tag_name.empty()) {
                node.children.push_back(child);
            }
        } else {
            std::string text = parseText(html, pos);
            if (!text.empty()) {
                DOMNode text_node;
                text_node.tag_name = "#text";
                text_node.text_content = text;
                node.children.push_back(text_node);
            }
        }
    }
    
    return node;
}

std::string HTMLParser::parseTagName(const std::string& html, size_t& pos) {
    std::string tag_name;
    while (pos < html.length() && !std::isspace(html[pos]) && html[pos] != '>' && html[pos] != '/') {
        tag_name += html[pos];
        pos++;
    }
    return tag_name;
}

std::string HTMLParser::parseText(const std::string& html, size_t& pos) {
    std::string text;
    while (pos < html.length() && html[pos] != '<') {
        text += html[pos];
        pos++;
    }
    
    // Обрезка текста
    size_t start = 0;
    while (start < text.length() && std::isspace(text[start])) start++;
    size_t end = text.length();
    while (end > start && std::isspace(text[end - 1])) end--;
    
    if (start < end) {
        return text.substr(start, end - start);
    }
    return "";
}

void HTMLParser::parseAttributes(const std::string& html, size_t& pos, DOMNode& node) {
    skipWhitespace(html, pos);
    
    while (pos < html.length() && html[pos] != '>' && html[pos] != '/') {
        std::string attr_name;
        std::string attr_value;
        
        while (pos < html.length() && !std::isspace(html[pos]) && html[pos] != '=' && 
               html[pos] != '>' && html[pos] != '/') {
            attr_name += html[pos];
            pos++;
        }
        
        skipWhitespace(html, pos);
        
        if (pos < html.length() && html[pos] == '=') {
            pos++;
            skipWhitespace(html, pos);
            
            if (pos < html.length() && (html[pos] == '"' || html[pos] == '\'')) {
                char quote = html[pos];
                pos++;
                
                while (pos < html.length() && html[pos] != quote) {
                    attr_value += html[pos];
                    pos++;
                }
                
                if (pos < html.length() && html[pos] == quote) {
                    pos++;
                }
            }
        }
        
        if (!attr_name.empty()) {
            node.attributes[attr_name] = attr_value;
        }
        
        skipWhitespace(html, pos);
    }
}

void HTMLParser::skipWhitespace(const std::string& html, size_t& pos) {
    while (pos < html.length() && std::isspace(html[pos])) {
        pos++;
    }
}