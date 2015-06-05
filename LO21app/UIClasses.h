#ifndef UICLASSES_H
#define UICLASSES_H

#include<QStandardItemModel>
#include <QStandardItem>
#include <typeinfo>
#include "Calendar.h"

class TreeViewModel {
    // Structures :
    struct CoupleItemProjet {
        Projet* projet;
        QStandardItem* item;
    };

    struct CoupleItemTache {
        Tache* tache;
        QStandardItem* item;
    };

    // Variables :
    QStandardItemModel modele; // le modèle

    CoupleItemProjet* tabItemsProjets; // le tableau qui lie un projet à un item
    int tipNb, tipNbMax;

    CoupleItemTache* tabItemsTaches; // le tableau qui lie chaque tâche à un item
    int titNb, titNbMax;




    // Méthodes privées générales :
    TreeViewModel(): tabItemsProjets(0), tipNb(0), tipNbMax(0), tabItemsTaches(0), titNb(0), titNbMax(0) {}
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
    void printBranch(QStandardItem* , Tache* );
    void addProjetItem(QStandardItem* item, Projet* projet);
    void addTacheItem(QStandardItem* item, Tache* tache);
public:
    static TreeViewModel& getInstance();
    static void libererInstance();
    void printTree();
    QStandardItemModel* getModele() { return &modele; }

    Tache* getTacheFromItem(QStandardItem* item);
    QStandardItem* getItemFromTache(Tache* tache);

    Projet* getProjetFromItem(QStandardItem* item);
    QStandardItem* getItemFromProjet(Projet* projet);

    // add projet (projet)
    // remove projet (projet)
    // idem pour les 2 types de taches
public slots:


};

// Penser à :
// mettre des const
// mettre des try catch
// insert : beginInsertRows() and endInsertRows()

#endif // UICLASSES_H
