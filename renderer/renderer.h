#ifndef RENDERER_H
#define RENDERER_H

#include "../renderer/dom.h"
#include <string>

class Renderer {
public:
    std::string render(const DOMNode& node);
    
private:
    std::string renderNode(const DOMNode& node, int depth = 0);
};

#endif