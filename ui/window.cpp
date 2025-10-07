#include "window.h"
#include <iostream>

void BrowserWindow::show() {
    std::cout << "=== " << title_ << " ===\n";
    std::cout << content_ << "\n";
    std::cout << "====================\n";
}

void BrowserWindow::setContent(const std::string& content) {
    content_ = content;
}

void BrowserWindow::setTitle(const std::string& title) {
    title_ = title;
}