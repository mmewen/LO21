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
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>

/*! \class TreeViewModel
        \brief Gestionnaire singloton du modèle du TreeView<br>

        Contient les différentes méthodes afin d'afficher l'arbre des projets et taches.<br>
        On connait ainsi les taches incluses dans un projet, et celles qui sont dans une tache composite.<br>
        On voit rapidement l'état des taches/projet grace à une petite icone.<br>
        Permet de retrouver un projet/tache à partir d'un item et inversement.<br>
*/
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

    QIcon* icone0;
    QIcon* icone1;
    QIcon* icone2;

    // Méthodes privées générales :
    TreeViewModel(): tabItemsProjets(0), tipNb(0), tipNbMax(0), tabItemsTaches(0), titNb(0), titNbMax(0),
         icone0(new QIcon("dot0.png")), icone1(new QIcon("dot1.png")), icone2(new QIcon("dot2.png")) {}
    ~TreeViewModel();
    TreeViewModel (const TreeViewModel&);
    struct Handler{
        TreeViewModel* instance;
        Handler():instance(0){}
        ~Handler(){ if (instance) delete instance; }
    };
    static Handler handler;

    // Méthodes privées d'affichage :
    void printBranch(QStandardItem* , Tache* );//! Affiche le contenu d'une tache composite récursivement
    void addProjetItem(QStandardItem* item, Projet* projet);//! Ajoute le couple (projet item) au tableau tabItemsProjets
    void addTacheItem(QStandardItem* item, Tache* tache);//! Ajoute le couple (tache item) au tableau tabItemsTaches
    void setIcon(QStandardItem* item, int statut);//! Met l'icone qui va bien sur chaque item
public:
    static TreeViewModel& getInstance();//! Retourne l'instance de la classe
    static void libererInstance();
    QStandardItemModel* getModele() { return &modele; } //! Retourne une référence sur le modèle du TreeView
    void printTree(); //! Affiche l'arbre complet au lancement de l'application
    void addProjet(Projet* nouveauProjet);//! Ajoute un projet à la racine (virtuelle) de l'arbre à l'ajout d'un projet
    void addTache(Projet* projet, Tache* tache);  //! Ajoute une tache à un projet
    void addTache(Tache* tacheMere, Tache* tache);  //! Ajoute une tache à une tache mère (composite)

    Tache* getTacheFromItem(QStandardItem* item);  //! Retourne une tache à partir d'un item de l'arbre
    QStandardItem* getItemFromTache(Tache* tache);  //! Retourne l'item de l'arbre lié à une tache

    Projet* getProjetFromItem(QStandardItem* item);  //! Retourne un projet à partir d'un item de l'arbre
    QStandardItem* getItemFromProjet(Projet* projet);  //! Retourne l'item associé à un projet

    void updateIcons(Tache* t);  //! Met à jour les icones de la tache t et de ses parents (quand une tache est programmée)

public slots:
    void updateName(Projet* projet);  //! Met à jour le nom d'un projet
    void updateName(Unitaire* tache);  //! Met à jour le nom d'une tache unitaire
    void updateName(Composite* tache);  //! Met à jour le nom d'une tache composite
};


/*! \class Editeur
    \brief Classe abstraite pour le widget d'édition (partie droite de l'onglet d'édition)

Contient le formLayout et des classes virtuelles pures d'enregistrement et de chargement des taches/projets.
*/
class Editeur : public QWidget{
    Q_OBJECT
public:
    QFormLayout *formLayout;
    virtual ~Editeur(){}
    Editeur(): formLayout(new QFormLayout) {}
public slots:
    virtual void slotSave() = 0;  //! Fonction virtuelle pure dédiée à la sauvegarde de la tache ou du projet en cours d'édition
    virtual void slotReload() = 0;  //! Fonction virtuelle pure remettant à zéro le formulaire d'édition
};

/*! \class EditeurProjet
    \brief Classe pour l'Editeur de projet

Remplit le formulaire, l'enregistre et l'exporte en XML
*/
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
    void slotExporter();  //! Exporte le projet sous forme d'un fichier XML
    void slotReload();
signals:
    void projetUpdated(Projet* projet);  //! Le projet passé en paramètre a été modifié
};

/*! \class EditeurTache
    \brief Classe abstraite pour l'Editeur de taches
*/
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
    int nbPred;
    EditeurTache();
    void printFinForm(Tache* t);  //! Affiche la fin du formulaire d'édition de tache, qui est commun aux unitaires et composites
    virtual ~EditeurTache() {}
public slots:
    void slotEnable();  //! Active le widget
    virtual void slotReload() = 0;
signals:
    void reloadAll();  //! Demande d'actualisation du widget d'édition
};

/*! \class EditeurTU
    \brief EditeurTache de taches unitaires
*/
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
    void slotEditionPredecesseurs();  //! Edite les prédécesseurs de la tache unitaire actuelle
signals:
    void tacheUpdated(Unitaire* tache);  //! Une tache unitaire vient d'etre éditée
};

/*! \class EditeurTC
    \brief EditeurTache de taches composite
*/
class EditeurTC : public EditeurTache{
    Q_OBJECT
    Composite* tache;
public:
    virtual ~EditeurTC(){}
    EditeurTC(Composite *t);
public slots:
    void slotSave();
    void slotReload();
    void slotEditionPredecesseurs();  //! Edite les préférences de la tache composite actuelle
signals:
    void tacheUpdated(Composite* tache);  //! Une tache composite vient d'etre éditée
};

/*! \class EditeurPrecedence
    \brief Editeur des précédences de la tache appelante.
*/
class EditeurPrecedence : public QDialog
{
    Q_OBJECT
public:
    EditeurPrecedence(Tache *t);
    string& getTacheIdFromIndex( int index );  //! Retourne l'id de la tache choisie dans la Combobox
    bool empty() const { return nbIndexes==0; }  //! Retourne true s'il n'y a pas de précédence possible (entraine la non ouverture du dialogue)
private:
    struct coupleItemId {
        string identifiant;
        int index;
    };
    Tache* tache;
    QComboBox *precedencesPotentielles;
    QPushButton *Ajouter;
    QFormLayout *Donnee;
    QPushButton *annuler;
    QPushButton *ajouter;
    coupleItemId couples[100];
    int nbIndexes;
public slots:
    void slotAnnulation();  //! Termine l'édition des précédences et ferme
    void slotAjout();  //! Ajoute la précédence et ferme le dialogue
signals:
    void editionPrecedenceEnd();  //! L'édition des précédences est terminée (validée ou annulée)
};

/*! \class ProgrammationTache
    \brief Programme la tache sélectionnée dans le tree view.

Vérifie qu'une programmation est possible et demande à l'utilisateur de la saisir
*/
class ProgrammationTache : public QDialog
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
    void tacheProgrammee();  //! Une tache a été programmée
};

/*! \class ProgrammationActivite
    \brief Dialogue de programmation d'une activité

Demande à l'utilisateur de saisir les caractéristiques d'une activité et la programme.
*/
class ProgrammationActivite : public QDialog
{
    Q_OBJECT
    QPushButton *Sauver;
public:
    QLineEdit *titre;
    QLineEdit *lieu;
    QSpinBox *hHoraire;
    QSpinBox *mHoraire;
    QSpinBox *hDuree;
    QSpinBox *mDuree;
    QCalendarWidget *date;
    QFormLayout *formLayout;
    ProgrammationActivite();
public slots:
    virtual void slotSave();
signals:
    void activiteProgrammee();  //! Une activité a été programmée
};

#endif // UICLASSES_H
