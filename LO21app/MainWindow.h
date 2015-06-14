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
    //! zone contenant les onglets
    QWidget *zoneCentrale; 
    //! Widget gérant les onglets
    QTabWidget *tabs; 
    //! Tache en cours d'édition
    Tache* t; 

    // Widgets de l'agenda
    //! Onglet avec le planning
    QWidget *planningTab; 
    //! Layout global de l'onglet
    QHBoxLayout *agendaLayout; 
    //! Widget global de l'onglet
    AgendaView *agenda; 


    // Widgets de l'onglet d'édition
    //! Onglet avec l'édition des tâches
    QWidget *tachesTab; 
    //! Layout horizontal contenant le layout du treeview à gauche et celui du formulaire à droite
    QHBoxLayout *tachesLayout; 
    Editeur* edition;

    // ... pour le formulaire d'édition
    //! Formulaire d'édition d'une tâche ou d'un projet
    QFormLayout *tachesFormLayout; 
    //! Zone de scroll contenant l'édition d'une tache/d'un projet
    QScrollArea *editionScroll; 

    // ... pour le treeview
    //! Layout vertical contenant le treeView et les boutons liés
    QVBoxLayout *tachesView; 
    //! Boutons d'ajout/supression de projet/tache
    QFormLayout *boutonsTreeView; 
    //! Partie vue du TreeView
    QTreeView *vue; 
    //! Partie modèle du Treeview : arborescence des projets et tâches
    TreeViewModel& treeView; 

//    void clearTFL();
public:
    ~MainWindow(){}
    MainWindow();
    //! Affiche l'édition d'une tache unitaire
    void showUnitaire(Unitaire& t); 
    //! Enregistre une tache unitaire
    void saveUnitaire(); 
    //! Affiche l'édition d' tache composite
    void showComposite(Composite& t); 
    //! Affiche l'édition d'un projet
    void showProjet(Projet& p); 
public slots:
    //! On traite le clic sur l'item index de la vue TreeView
    void treeViewClicked(const QModelIndex &index); 
    //! Ajoute un projet dans le système et sur l'arbre
    void slotAjouterProjet(); 
    //! Ajoute une tache unitaire dans le système et sur l'arbre
    void slotAjouterTU(); 
    //! Ajoute une tache composite dans le système et sur l'arbre
    void slotAjouterTC(); 
    //! Lance l'affichage de l'édition de la tache t
    void slotShowTU(); 
    //! Lance l'affichage de l'édition de la tache t
    void slotShowTC(); 
    //! Lance le dialogue pour programmer une tache unitaire
    void slotProgrammerTU(); 
    //! Lance le dialogue pour programmer une activité
    void slotProgrammerActivite(); 

};

#endif // MAINWINDOW_H
