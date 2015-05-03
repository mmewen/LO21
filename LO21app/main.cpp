#include "MainWindow.h"
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
#include <QStandardItemModel>
#include <QStandardItem>

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


    // Mode normal
//    QTreeWidget tree(tachesTab);
//    tree.setFixedSize(200, 500);
//    tree.setColumnCount(1);
//    tree.setHeaderHidden(true);
//    QList<QTreeWidgetItem *> items;
//    for (int i = 0; i < 50; ++i)
//        items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
//    tree.insertTopLevelItems(0, items);
//    tree.show();

    // Mode modèle/vue
    QStandardItemModel *modele = new QStandardItemModel;
    QStandardItem *item = new QStandardItem("Truc");
    modele->appendRow(item);
    item->appendRow(new QStandardItem("Sous truc"));
    QStandardItem *item2 = new QStandardItem("Plep");
    modele->appendRow(item2);
    item2->appendRow(new QStandardItem("Sous plep"));
    QTreeView *vue = new QTreeView(tachesTab);
    vue->setHeaderHidden(true);
    vue->setFixedSize(200,500);
    vue->setModel(modele);

    // Une fois qu'on aura une classe d'affichage
//    QTreeView::connect(vue,  SIGNAL(clicked()), &app, SLOT(quit()));


//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));


    window->setCentralWidget(centralWidget);
    window->show();

    return app.exec();
}
