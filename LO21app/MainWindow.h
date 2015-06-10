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
#include <QVBoxLayout>
#include <QDate>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QSignalMapper>
#include <QScrollArea>
//#include <QTextEdit>
//#include <QtXml>
//#include <QFileDialog>
//#include <QList>

// Constantes
#define W_WIDTH 900
#define W_HEIGHT 700



/// \class MainWindow
/// \brief Conserve les pointeurs vers les principaux widgets de la fenêtre. S'occupe des actions générales sur ceux-ci.
///
class MainWindow: public QMainWindow{
    Q_OBJECT
    // Widgets généraux
    QWidget *zoneCentrale; //! zone contenant les onglets
    QTabWidget *tabs; //! Widget gérant les onglets
    Tache* t;

    // Widgets de l'agenda
    QWidget *planningTab; //! Onglet avec le planning


    // Widgets de l'onglet d'édition
    QWidget *tachesTab; //! Onglet avec l'édition des tâches
    QHBoxLayout *tachesLayout; //! Layout horizontal contenant le layout du treeview à gauche et celui du formulaire à droite
    Editeur* edition;

    // ... pour le formulaire d'édition
    QFormLayout *tachesFormLayout; //! Formulaire d'édition d'une tâche ou d'un projet
    QScrollArea *editionScroll; //! Zone de scroll contenant l'édition d'une tache/d'un projet

    // ... pour le treeview
    QVBoxLayout *tachesView; //! Layout vertical contenant le treeView et les boutons liés
    QFormLayout *boutonsTreeView; //! Boutons d'ajout/supression de projet/tache
    QTreeView *vue; //! Partie vue du TreeView
    TreeViewModel& treeView; //! Partie modèle du Treeview : arborescence des projets et tâches

//    void clearTFL();
public:
    ~MainWindow(){}
    MainWindow();
    void showUnitaire(Unitaire& t);
    void saveUnitaire();
    void showComposite(Composite& t);
    void showProjet(Projet& p);
public slots:
    void treeViewClicked(const QModelIndex &index);
    void slotAjouterProjet();
    void slotAjouterTU();
    void slotAjouterTC();
    void slotShowTU();
    void slotShowTC();
    void slotProgrammerTU();
    void slotProgrammerActivite();
//    quit();

};

#endif // MAINWINDOW_H

// TODO :
/*
 * faire des fonctions Show plus jolies
 */
