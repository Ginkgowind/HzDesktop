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

        // ���������λ�ü���Ŀ����ӣ�����ͼ��ķ����߼�
        // ����ֻ�Ǽ����λ����Ϣ
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