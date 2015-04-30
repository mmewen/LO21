#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QtXml>
#include <QString>
#include <QFileDialog>
#include <QWidget>
#include <QTabWidget>
#include <QMainWindow>
#include <QTreeWidget>
#include <QList>
#include <QTreeWidgetItem>

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    QMainWindow *window = new QMainWindow();

    window->setWindowTitle(QString::fromUtf8("LO21"));
    window->setFixedSize(700, 500);

    QWidget *centralWidget = new QWidget(window);
    QTabWidget *tabs = new QTabWidget(centralWidget);

    tabs->setFixedSize(700, 500);
    QWidget *planningTab = new QWidget();
    QWidget *tachesTab = new QWidget();
    QWidget *trucTab = new QWidget();
    tabs->addTab(planningTab,"Planning");
    tabs->addTab(tachesTab,"Tache");
    tabs->addTab(trucTab,"Truc");

//    QPushButton quit("Quitter", window);
//    quit.show();
//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));

    QTreeWidget tree(tachesTab);

    tree.setFixedSize(200, 500);
    tree.setColumnCount(1);
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < 50; ++i)
        items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
    tree.insertTopLevelItems(0, items);
    tree.show();

//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));


    window->setCentralWidget(centralWidget);
    window->show();

    return app.exec();
}
