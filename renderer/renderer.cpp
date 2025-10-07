#include "../renderer/renderer.h"
#include <iostream>

std::string Renderer::render(const DOMNode& node) {
    return renderNode(node);
}

std::string Renderer::renderNode(const DOMNode& node, int depth) {
    std::string result;
    
    if (node.isTextNode()) {
        if (!node.text_content.empty()) {
            // Добавление отступов, чтобы можно было лучше читать
            result.append(depth * 2, ' ');
            result += node.text_content + "\n";
        }
    } else {
        // Визуализация открывающего тега с атрибутами
        result.append(depth * 2, ' ');
        result += "<" + node.tag_name;
        
        for (const auto& attr : node.attributes) {
            result += " " + attr.first + "=\"" + attr.second + "\"";
        }
        
        result += ">\n";
        
        for (const auto& child : node.children) {
            result += renderNode(child, depth + 1);
        }
        
        // Рендер закрывающего тега
        if (!node.children.empty() || node.tag_name != "br") {
            result.append(depth * 2, ' ');
            result += "</" + node.tag_name + ">\n";
        }
    }
    
    return result;
}