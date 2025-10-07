#include <string>
#include <vector>
#include <map>

class DOMNode {
public:
    std::string tag_name;
    std::map<std::string, std::string> attributes;
    std::vector<DOMNode> children;
};
