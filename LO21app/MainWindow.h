#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Includes
#include "Calendar.h"
#include "UIClasses.h"

#include <QString>
#include <QMainWindow>
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QTabWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include <QTreeView>
#include <QFormLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QDate>
#include <QCalendarWidget>
#include <QSpinBox>
#include <QPushButton>
//#include <QLineEdit>
//#include <QTextEdit>
//#include <QVBoxLayout>
//#include <QtXml>
//#include <QFileDialog>
//#include <QTreeWidget>
//#include <QList>
//#include <QTreeWidgetItem>

// Constantes
#define W_WIDTH 900
#define W_HEIGHT 700



/// \class MainWindow
/// \brief Conserve les pointeurs vers les principaux widgets de la fenêtre. S'occupe des actions générales sur ceux-ci.
///
class MainWindow: public QMainWindow{
    Q_OBJECT
    QWidget *zoneCentrale;
    QTabWidget *tabs;
    QWidget *planningTab;
    QWidget *tachesTab;
    QHBoxLayout *tachesLayout;
    QFormLayout *tachesFormLayout;
    TreeViewModel& treeView;

    void showProject();
    void clearTFL();
public:
    ~MainWindow(){};
    MainWindow();
    void showUnitaire(const Unitaire& t);
    void saveUnitaire();
    void showComposite(const Composite& t);
    void showProjet(const Projet& p);
public slots:
    void treeViewClicked(const QModelIndex &index);
//    quit();

};

#endif // MAINWINDOW_H

// TODO :
/*
 * faire des fonctions Show plus jolies
 */
