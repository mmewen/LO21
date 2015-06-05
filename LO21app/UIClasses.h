#ifndef UICLASSES_H
#define UICLASSES_H

#include<QStandardItemModel>
#include <QStandardItem>
#include <typeinfo>
#include "Calendar.h"

class TreeViewModel {
    // Structures :
    struct coupleItemProjet {
        Projet* projet;
        QStandardItem* item;
    };

    // Variables :
    QStandardItemModel modele; // le modèle

    coupleItemProjet tabItemsProjets[];// le tableau qui lie un projet à un item
    int tipNb, tipNbMax;

    // le tableau qui lie chaque tâche à un item


    // Méthodes privées générales :
    TreeViewModel(): tipNb(0), tipNbMax(0) {}
//    ~TreeViewModel(){for(unsigned int i=0; i<nb; i++) delete projets[i]; delete[] projets;}
    ~TreeViewModel(){}
    TreeViewModel(const TreeViewModel&);
    struct Handler{
        TreeViewModel* instance;
        Handler():instance(0){}
        ~Handler(){ if (instance) delete instance; }
    };
    static Handler handler;

    // Méthodes privées d'affichage :
public:
    static TreeViewModel& getInstance();
    static void libererInstance();
    void printTree();
    void printBranch(QStandardItem* , Tache* );
    QStandardItemModel* getModele() { return &modele; }
    // add projet (projet)
    // remove projet (projet)
    // idem pour les 2 types de taches
};

// insert : beginInsertRows() and endInsertRows()

#endif // UICLASSES_H
