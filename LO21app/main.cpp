#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QtXml>
#include <QString>
#include <QFileDialog>

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);
    QPushButton quit("Quitter");
    quit.show();

    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));

    return app.exec();
}
