#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Includes
#include <QString>
#include <QMainWindow>
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QTabWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QFormLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QDate>
//#include <QPushButton>
//#include <QLineEdit>
//#include <QTextEdit>
//#include <QVBoxLayout>
//#include <QtXml>
//#include <QFileDialog>
//#include <QTreeWidget>
//#include <QList>
//#include <QTreeWidgetItem>

// Constantes
#define W_WIDTH 700
#define W_HEIGHT 500



/// \class MainWindow
/// \brief Conserve les pointeurs vers les principaux widgets de la fenêtre. S'occupe des actions générales sur ceux-ci.
///
class MainWindow: public QMainWindow{
    QWidget *zoneCentrale;
    QTabWidget *tabs;
    QWidget *planningTab;
    QWidget *tachesTab;
    QHBoxLayout *tachesLayout;
    QFormLayout *tachesFormLayout;

    void showProject();
public:
    MainWindow();
    void showUnitaire();
    void saveUnitaire();
//public slots:
//    quit();
};

#endif // MAINWINDOW_H
