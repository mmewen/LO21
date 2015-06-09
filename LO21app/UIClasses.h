#ifndef UICLASSES_H
#define UICLASSES_H

#include<QStandardItemModel>
#include <QStandardItem>
#include <QLineEdit>
#include <typeinfo>
#include <QFormLayout>
#include <QPushButton>
#include <QCalendarWidget>
#include <QSignalMapper>
#include "Calendar.h"
#include "timing.h"
#include <QCheckBox>
#include <QSpinBox>
#include <QDate>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>

class TreeViewModel : public QObject {
    Q_OBJECT
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
    ~TreeViewModel();
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
    QStandardItemModel* getModele() { return &modele; }

    void printTree();
    void addProjet(Projet* nouveauProjet);
    void addTache(Projet* projet, Tache* tache);
    void addTache(Tache* tacheMere, Tache* tache);

    Tache* getTacheFromItem(QStandardItem* item);
    QStandardItem* getItemFromTache(Tache* tache);

    Projet* getProjetFromItem(QStandardItem* item);
    QStandardItem* getItemFromProjet(Projet* projet);

public slots:
    void updateName(Projet* projet);
    void updateName(Unitaire* tache);
    void updateName(Composite* tache);

//    Projet* getProjetFromSelected();

    // add projet (projet)
    // remove projet (projet)
    // idem pour les 2 types de taches


};

class Editeur : public QWidget{
    Q_OBJECT
public:
    QFormLayout *formLayout;
    virtual ~Editeur(){}
    Editeur(): formLayout(new QFormLayout) {}
public slots:
    virtual void slotSave() = 0;
    virtual void slotReload() = 0;
};

class EditeurProjet : public Editeur{
    Q_OBJECT
    QLineEdit *titre;
    QCalendarWidget *dispo;
    QCalendarWidget *echeance;
    Projet* projet;
public:
    EditeurProjet(Projet* p);
    virtual ~EditeurProjet() {}
public slots:
    void slotSave();
    void slotReload();
signals:
    void projetUpdated(Projet* projet);
};

class EditeurTache : public Editeur{
    Q_OBJECT
public:
    QLineEdit *titre;
    QCalendarWidget *dispo;
    QCalendarWidget *echeance;
    QPushButton *annuler;
    QPushButton *sauver;
    QPushButton *predecesseurs;
    QListWidget *listePredecesseurs;
    QListWidgetItem** tabPredecesseurs;
    EditeurTache();
    void printFinForm(Tache* t);
    virtual ~EditeurTache() {}
};

class EditeurTU : public EditeurTache{
    Q_OBJECT
    QSpinBox *duree;
    QCheckBox *preemptible;
    Unitaire* tache;
public:
    virtual ~EditeurTU(){}
    EditeurTU(Unitaire *t);
public slots:
    void slotSave();
    void slotReload();
signals:
    void tacheUpdated(Unitaire* tache);
};

class EditeurTC : public EditeurTache{
    Q_OBJECT
    Composite* tache;
public:
    virtual ~EditeurTC(){}
    EditeurTC(Composite *t);
public slots:
    void slotSave();
    void slotReload();
signals:
    void tacheUpdated(Composite* tache);
};

class EditeurPrecedence : public QWidget
{
    Q_OBJECT
public:
    EditeurPrecedence(Tache *t);
private:
    Tache* tache;
    QComboBox *Precedences;
    QPushButton *Ajouter;
    QFormLayout *Donnee;
};

class ProgrammationTache : public QWidget
{
    Q_OBJECT
    Unitaire *tache;
    QPushButton *Sauver;
public:
    QLabel *titre;
    QSpinBox *hHoraire;
    QSpinBox *mHoraire;
    QSpinBox *hDuree;
    QSpinBox *mDuree;
    QCalendarWidget *date;
    QFormLayout *formLayout;
    ProgrammationTache(Unitaire* t);
public slots:
    virtual void slotSave();
signals:
    void tacheProgrammee(Unitaire* tache);
};










// TODO:
// - enregistrement changements
// - maj treeview à l'enregistrement
// - précédences
// - faire les free qui vont bien
// - suppr taches/projets ?

// Penser à :
// mettre des const
// mettre des try catch
// insert : beginInsertRows() and endInsertRows()

#endif // UICLASSES_H
