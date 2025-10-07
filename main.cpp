#include <iostream>
#include <string>
#include "network/http_client.h"
#include "parser/html_parser.h"
#include "renderer/renderer.h"
#include "ui/window.h"

int main() {
    try {
        std::string url;
        std::cout << "Enter URL: ";
        std::getline(std::cin, url);
        
        if (url.empty()) {
            url = "http://example.com";
        }
        
        // Извлечение HTML-содержимого
        std::cout << "Fetching " << url << "...\n";
        std::string html = fetch_url(url);
        
        if (html.empty()) {
            std::cerr << "Failed to fetch URL or empty response\n";
            return 1;
        }
        
        // Парсинг HTML
        std::cout << "Parsing HTML...\n";
        HTMLParser parser;
        DOMNode root = parser.parse(html);
        
        // Рендеринг через консоль
        std::cout << "Rendering...\n";
        Renderer renderer;
        std::string rendered_content = renderer.render(root);
        
        // Отображение в окне
        BrowserWindow window;
        window.setTitle("Simple Browser - " + url);
        window.setContent(rendered_content);
        window.show();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}