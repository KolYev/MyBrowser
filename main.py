import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget
from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtCore import QUrl

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("MyBrowser")
        self.setGeometry(500, 150, 500, 500)
        
        widget = QWidget()
        self.setCentralWidget(widget)
        
        layout = QVBoxLayout(widget)
        
        self.view = QWebEngineView()
        layout.addWidget(self.view)
        
        self.view.setUrl(QUrl("https://www.google.com/"))
        
def main():
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    
    sys.exit(app.exec())


if __name__=="__main__":
    main()