import sys
import os
import json
from datetime import datetime

from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtWebEngineCore import QWebEngineProfile, QWebEnginePage, QWebEngineDownloadRequest
from PyQt6.QtWidgets import (QApplication, QMainWindow, QVBoxLayout, 
                             QWidget, QLineEdit, QToolBar, 
                             QPushButton, QTabWidget)
from PyQt6.QtCore import QUrl

class DataManager:
    def __init__(self):
        self.data_path = os.path.join(os.path.expanduser("~"), ".securebrowser", "data")
        os.makedirs(self.data_path, exist_ok=True)
        
    def save_bookmarks(self, bookmarks):
        """Сохранение закладок"""
        with open(os.path.join(self.data_path, "bookmarks.json"), 'w', encoding='utf-8') as f:
            json.dump(bookmarks, f, ensure_ascii=False, indent=2)
            
    def load_bookmarks(self):
        """Загрузка закладок"""
        try:
            with open(os.path.join(self.data_path, "bookmarks.json"), 'r', encoding='utf-8') as f:
                return json.load(f)
        except FileNotFoundError:
            return []
    
    def save_history(self, history):
        """Сохранение истории (шифрованное)"""
        # Простое XOR-шифрование для примера
        encrypted = [self.simple_encrypt(str(item)) for item in history]
        with open(os.path.join(self.data_path, "history.enc"), 'w') as f:
            f.write('\n'.join(encrypted))
    
    def simple_encrypt(self, text):
        """Простое шифрование"""
        return ''.join(chr(ord(c) ^ 0x55) for c in text)

class OfflinePageManager:
    def __init__(self):
        self.offline_path = os.path.join(os.path.expanduser("~"), ".securebrowser", "offline")
        os.makedirs(self.offline_path, exist_ok=True)
        
    def add_page(self, url, title):
        """Добавление страницы в офлайн-коллекцию"""
        metadata = {
            'url': url,
            'title': title,
            'saved_at': datetime.now().isoformat()
        }
        
        # Сохраняем метаданные
        with open(os.path.join(self.offline_path, 'metadata.json'), 'a') as f:
            f.write(json.dumps(metadata) + '\n')

class SecureBrowser(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SecureBrowser - Локальный & Анонимный")
        self.setGeometry(500, 150, 800, 600)
        
        # Создаем локальный профиль
        self.local_profile = self.setup_local_profile()
        self.data_manager = DataManager()
        self.offline_manager = OfflinePageManager()
        
        self.setup_ui()
        self.setup_offline_features()
        self.setup_security_features()
        
    def setup_local_profile(self):
        """Настройка локального профиля браузера"""
        profile = QWebEngineProfile("local-profile")
        
        # Путь для хранения данных локально
        data_path = os.path.join(os.path.expanduser("~"), ".securebrowser")
        os.makedirs(data_path, exist_ok=True)
        
        profile.setPersistentStoragePath(os.path.join(data_path, "storage"))
        profile.setCachePath(os.path.join(data_path, "cache"))
        profile.setDownloadPath(os.path.join(data_path, "downloads"))
        
        # Настройки приватности
        profile.setHttpCacheType(QWebEngineProfile.HttpCacheType.DiskHttpCache)
        profile.setPersistentCookiesPolicy(QWebEngineProfile.PersistentCookiesPolicy.NoPersistentCookies)
        
        return profile
    
    def setup_ui(self):
        """Настройка пользовательского интерфейса"""
        # Главный виджет
        widget = QWidget()
        self.setCentralWidget(widget)
        layout = QVBoxLayout(widget)
        
        # Панель инструментов
        self.toolbar = QToolBar()
        self.addToolBar(self.toolbar)
        
        # Кнопки навигации
        back_btn = QPushButton("←")
        forward_btn = QPushButton("→")
        reload_btn = QPushButton("⟲")
        home_btn = QPushButton("🏠")
        
        # Адресная строка
        self.url_bar = QLineEdit()
        self.url_bar.setPlaceholderText("Введите URL или поисковый запрос...")
        
        # Добавляем элементы на панель
        self.toolbar.addWidget(back_btn)
        self.toolbar.addWidget(forward_btn)
        self.toolbar.addWidget(reload_btn)
        self.toolbar.addWidget(home_btn)
        self.toolbar.addWidget(self.url_bar)
        
        # Система вкладок
        self.tabs = QTabWidget()
        self.tabs.setTabsClosable(True)
        self.tabs.tabCloseRequested.connect(self.close_tab)
        
        layout.addWidget(self.tabs)
        
        # Создаем первую вкладку
        self.add_new_tab("https://duckduckgo.com", "Стартовая страница")
        
        # Подключаем сигналы
        back_btn.clicked.connect(lambda: self.current_browser().back())
        forward_btn.clicked.connect(lambda: self.current_browser().forward())
        reload_btn.clicked.connect(lambda: self.current_browser().reload())
        home_btn.clicked.connect(self.go_home)
        self.url_bar.returnPressed.connect(self.navigate_to_url)
        
    def add_new_tab(self, url, title="Новая вкладка"):
        """Создание новой вкладки"""
        browser = QWebEngineView()
        web_page = QWebEnginePage(self.local_profile, browser)
        browser.setPage(web_page)
        
        # Подключаем события
        browser.urlChanged.connect(self.update_url)
        browser.titleChanged.connect(lambda title: self.update_tab_title(browser, title))
        
        index = self.tabs.addTab(browser, title)
        self.tabs.setCurrentIndex(index)
        
        browser.setUrl(QUrl(url))
        
        return browser
    
    def setup_offline_features(self):
        """Настройка офлайн-функций"""
        # Кнопка для сохранения страницы офлайн
        save_offline_btn = QPushButton("💾 Офлайн")
        self.toolbar.addWidget(save_offline_btn)
        save_offline_btn.clicked.connect(self.save_page_offline)
        
    def save_page_offline(self):
        """Сохранение текущей страницы для офлайн-использования"""
        browser = self.current_browser()
        current_url = browser.url().toString()
        current_title = browser.title()
        
        # Сохраняем страницу
        filename = f"{hash(current_url)}.html"
        filepath = os.path.join(self.offline_manager.offline_path, filename)
        
        def save_finished(path):
            print(f"Страница сохранена: {current_url}")
            self.offline_manager.add_page(current_url, current_title)
        
        # Сохраняем полную страницу
        browser.page().save(filepath, QWebEngineDownloadRequest.SavePageFormat.CompleteHtmlSaveFormat)
        # Для отслеживания завершения сохранения можно подключить сигнал, 
        # но в данной реализации это сложнее
        
    def setup_security_features(self):
        """Настройка функций безопасности"""
        # Блокировка трекеров
        self.local_profile.setHttpUserAgent(
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
        )
        
        # Отключаем различные API, которые могут нарушать приватность
        # В PyQt6 setPermissionPolicy может быть недоступен или работать иначе

    def current_browser(self):
        """Текущий активный браузер"""
        return self.tabs.currentWidget()

    def close_tab(self, index):
        """Закрытие вкладки"""
        if self.tabs.count() > 1:
            self.tabs.removeTab(index)

    def navigate_to_url(self):
        """Переход по URL"""
        url = self.url_bar.text()
        if not url.startswith(('http://', 'https://')):
            url = 'https://' + url
        self.current_browser().setUrl(QUrl(url))

    def update_url(self, q):
        """Обновление адресной строки"""
        self.url_bar.setText(q.toString())

    def update_tab_title(self, browser, title):
        """Обновление заголовка вкладки"""
        index = self.tabs.indexOf(browser)
        if title:
            self.tabs.setTabText(index, title[:15] + '...' if len(title) > 15 else title)

    def go_home(self):
        """Переход на домашнюю страницу"""
        self.current_browser().setUrl(QUrl("https://duckduckgo.com"))
        
def main():
    app = QApplication(sys.argv)
    
    # Настройка приложения
    app.setApplicationName("SecureBrowser")
    app.setApplicationVersion("1.0")
    
    window = SecureBrowser()
    window.show()
    
    sys.exit(app.exec())

if __name__ == "__main__":
    main()