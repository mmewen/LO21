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
    //! le modèle d'items du TreeView
    QStandardItemModel modele; 

    //! le tableau qui lie un projet à un item
    CoupleItemProjet* tabItemsProjets; 
    int tipNb, tipNbMax;

    //! le tableau qui lie chaque tâche à un item
    CoupleItemTache* tabItemsTaches; 
    int titNb, titNbMax;

    //! les icones de l'arbre
    QIcon *icone0, *icone1, *icone2; 

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
    //! Affiche le contenu d'une tache composite récursivement
    void printBranch(QStandardItem* , Tache* );
    //! Ajoute le couple (projet item) au tableau tabItemsProjets
    void addProjetItem(QStandardItem* item, Projet* projet);
    //! Ajoute le couple (tache item) au tableau tabItemsTaches
    void addTacheItem(QStandardItem* item, Tache* tache);
    //! Met l'icone qui va bien sur chaque item
    void setIcon(QStandardItem* item, int statut);
public:
    //! Retourne l'instance de la classe
    static TreeViewModel& getInstance();
    static void libererInstance();
    //! Retourne une référence sur le modèle du TreeView
    QStandardItemModel* getModele() { return &modele; } 
    //! Affiche l'arbre complet au lancement de l'application
    void printTree(); 
    //! Ajoute un projet à la racine (virtuelle) de l'arbre à l'ajout d'un projet
    void addProjet(Projet* nouveauProjet);
    //! Ajoute une tache à un projet
    void addTache(Projet* projet, Tache* tache);  
    //! Ajoute une tache à une tache mère (composite)
    void addTache(Tache* tacheMere, Tache* tache);  

    //! Retourne une tache à partir d'un item de l'arbre
    Tache* getTacheFromItem(QStandardItem* item);  
    //! Retourne l'item de l'arbre lié à une tache
    QStandardItem* getItemFromTache(Tache* tache);  

    //! Retourne un projet à partir d'un item de l'arbre
    Projet* getProjetFromItem(QStandardItem* item);  
    //! Retourne l'item associé à un projet
    QStandardItem* getItemFromProjet(Projet* projet);  

    //! Met à jour les icones de la tache t et de ses parents (quand une tache est programmée)
    void updateIcons(Tache* t);  

public slots:
    //! Met à jour le nom d'un projet
    void updateName(Projet* projet);  
    //! Met à jour le nom d'une tache unitaire
    void updateName(Unitaire* tache);  
    //! Met à jour le nom d'une tache composite
    void updateName(Composite* tache);  
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
    //! Fonction virtuelle pure dédiée à la sauvegarde de la tache ou du projet en cours d'édition
    virtual void slotSave() = 0; 
    //! Fonction virtuelle pure remettant à zéro le formulaire d'édition
    virtual void slotReload() = 0;  
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
    //! Exporte le projet sous forme d'un fichier XML
    void slotExporter();  
    void slotReload();
signals:
    //! Le projet passé en paramètre a été modifié
    void projetUpdated(Projet* projet);  
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
    //! Affiche la fin du formulaire d'édition de tache, qui est commun aux unitaires et composites
    void printFinForm(Tache* t);  
    virtual ~EditeurTache() {}
public slots:
    //! Active le widget
    void slotEnable();  
    virtual void slotReload() = 0;
signals:
    //! Demande d'actualisation du widget d'édition
    void reloadAll();  
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
    //! Edite les prédécesseurs de la tache unitaire actuelle
    void slotEditionPredecesseurs();  
signals:
    //! Une tache unitaire vient d'etre éditée
    void tacheUpdated(Unitaire* tache);  
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
    //! Edite les préférences de la tache composite actuelle
    void slotEditionPredecesseurs();  
signals:
    //! Une tache composite vient d'etre éditée
    void tacheUpdated(Composite* tache);  
};

/*! \class EditeurPrecedence
    \brief Editeur des précédences de la tache appelante.
*/
class EditeurPrecedence : public QDialog
{
    Q_OBJECT
public:
    EditeurPrecedence(Tache *t);
    //! Retourne l'id de la tache choisie dans la Combobox
    string& getTacheIdFromIndex( int index );  
    //! Retourne true s'il n'y a pas de précédence possible (entraine la non ouverture du dialogue)
    bool empty() const { return nbIndexes==0; }  
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
    //! Termine l'édition des précédences et ferme
    void slotAnnulation();  
    //! Ajoute la précédence et ferme le dialogue
    void slotAjout();  
signals:
    //! L'édition des précédences est terminée (validée ou annulée)
    void editionPrecedenceEnd();  
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
    //! Une tache a été programmée
    void tacheProgrammee();  
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
    //! Une activité a été programmée
    void activiteProgrammee();  
};

#endif // UICLASSES_H
