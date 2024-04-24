#include <QtWidgets>

class DesktopWidget : public QWidget {
public:
    DesktopWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setAcceptDrops(true);
    }

protected:
    void dragEnterEvent(QDragEnterEvent* event) override {
        if (event->mimeData()->hasFormat("text/plain")) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent* event) override {
        QString text = event->mimeData()->text();
        QPoint position = event->pos();

        // 在这里根据位置计算目标格子，处理图标的放置逻辑
        // 这里只是简单输出位置信息
        qDebug() << "Dropped at position:" << position;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    DesktopWidget desktop;
    desktop.setWindowTitle("Desktop Grid Example");
    desktop.resize(600, 400);
    desktop.show();

    return app.exec();
}