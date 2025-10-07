#ifndef WINDOW_H
#define WINDOW_H

#include <string>

class BrowserWindow {
public:
    void show();
    void setContent(const std::string& content);
    void setTitle(const std::string& title);
    
private:
    std::string content_;
    std::string title_;
};

#endif