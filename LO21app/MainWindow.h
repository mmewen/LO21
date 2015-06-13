#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Includes
#include "Calendar.h"
#include "UIClasses.h"
#include "agendaview.h"

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
#define W_WIDTH 1100
#define W_HEIGHT 700



/*! \class MainWindow
 \brief Conserve les pointeurs vers les principaux widgets de la fenêtre. S'occupe des actions générales sur ceux-ci.

 Cette classe fait appel aux fonctions d'affichage de UIClasses.h
*/
class MainWindow: public QMainWindow{
    Q_OBJECT
    // Widgets généraux
    QWidget *zoneCentrale; //! zone contenant les onglets
    QTabWidget *tabs; //! Widget gérant les onglets
    Tache* t; //! Tache en cours d'édition

    // Widgets de l'agenda
    QWidget *planningTab; //! Onglet avec le planning
    QHBoxLayout *agendaLayout; //! Layout global de l'onglet
    AgendaView *agenda; //! Widget global de l'onglet


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
    void showUnitaire(Unitaire& t); //! Affiche l'édition d'une tache unitaire
    void saveUnitaire(); //! Enregistre une tache unitaire
    void showComposite(Composite& t); //! Affiche l'édition d' tache composite
    void showProjet(Projet& p); //! Affiche l'édition d'un projet
public slots:
    void treeViewClicked(const QModelIndex &index); //! On traite le clic sur l'item index de la vue TreeView
    void slotAjouterProjet(); //! Ajoute un projet dans le système et sur l'arbre
    void slotAjouterTU(); //! Ajoute une tache unitaire dans le système et sur l'arbre
    void slotAjouterTC(); //! Ajoute une tache composite dans le système et sur l'arbre
    void slotShowTU(); //! Lance l'affichage de l'édition de la tache t
    void slotShowTC(); //! Lance l'affichage de l'édition de la tache t
    void slotProgrammerTU(); //! Lance le dialogue pour programmer une tache unitaire
    void slotProgrammerActivite(); //! Lance le dialogue pour programmer une activité

};

#endif // MAINWINDOW_H
