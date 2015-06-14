#ifndef CALENDAR_h
#define CALENDAR_h

#include<string>
#include<iostream>
#include<typeinfo>
#include<QObject>
#include<QFile>
#include<QXmlStreamWriter>
#include "timing.h"

using namespace std;
using namespace TIME;


/*! \class CalendarException
        \brief Classe permettant de gérer les exceptions du projet non liées au namespace TIME
*/
class CalendarException{
public:
    CalendarException(const string& message):info(message){} //! Constructeur à partir d'un string
    string getInfo() const { return info; } //! Retourne le message d'erreur
private:
    string info;
};

class Projet;

/*! \class ProjetManager
        \brief Classe composite des Projets
        La classe applique le designe pattern singleton et possède un iterateur sur le tableau de pointeurs de Projets.
*/
class ProjetManager {
private:
    Projet** projets;//! Tableau de pointeurs de Projet
    unsigned int nb;//! Nombre de Projets dans le tableau
    unsigned int nbMax;//! Taille allouée au tableau
    void addItem(Projet* p);//! Ajoute le projet* p au tableau projets
    Projet* trouverProjet(const string& id) const;//! Renvoie le projet correspondant à id s'il est présent dans le tableau projets
    ProjetManager():projets(0),nb(0),nbMax(0){}//! Constructeur de ProjetManager, initialise le tableau et sa taille à 0
    ~ProjetManager(){for(unsigned int i=0; i<nb; i++) delete projets[i]; delete[] projets;}//! Destructeur de ProjetManager, détruit les Projets composants puis le tableau projets
    ProjetManager(const Projet& um);//! Constructeur par recopie interdit
    ProjetManager& operator=(const Projet& um);//! Recopie par operator= interdite
    struct Handler{
        ProjetManager* instance;
        Handler():instance(0){}
        ~Handler(){ if (instance) delete instance; }
    };
    static Handler handler;//! Handler permettant de rendre ProjetManager singleton
    string genererId();//! Générateur d'identificateurs pour Projet
public:
    static ProjetManager& getInstance();//! Renvoie la seule instance de ProjetManager
    static void libererInstance();//! Libère l'instance de ProjetManager
    Projet& ajouterProjet(const string& nom, const string& file, const Date& dispo);//! Crée un Projet et garde son pointeur dans le tableau projets, à partir d'n nom, d'un nom de fichier et d'une date de disponibilité
    Projet& getProjet(const string& id);//! Renvoie une référence vers le projet correspondant à id s'il existe dans le tableau projets
    const Projet& getProjet(const string& code) const;//! Renvoie une référence const vers le projet correspondant à id s'il existe dans le tableau projets
    class Iterator{
        friend class ProjetManager;
        private:
            Projet** tab;
            int nb;
            int indice_projet;
            Iterator(Projet** t, int n):tab(t), nb(n), indice_projet(0){}
        public:
            Projet& current() const {
                if(indice_projet>=nb)
                    throw "incrementation d'un iterateur en fin de sequence";
                return *tab[indice_projet];
            }
            bool isDone() const { return indice_projet==nb; }
            void next(){ ++indice_projet; }
            void first(){ indice_projet=0; }
        };
    Iterator getIterator(){ return Iterator(projets,nb); }//! Renvoie un itérateur sur le tableau projets
};

class Tache;
class Unitaire;
class Composite;

/*! \class Projet
        \brief Classe de projets composite des Taches et composant ProjetManager
*/
class Projet {
private:
    Tache** taches;//! Tableau de pointeurs de taches
    Tache** tachesNonComposantes;//! Tableau de pointeurs de taches pour les taches n'étant pas pointées par une Composite
    unsigned int nb;//! Nombre d'éléments dans taches
    unsigned int nbMax;//! Taille allouée à taches
    unsigned int nbNC;//! Nombre d'éléments dans tachesNonComposantes
    unsigned int nbMaxNC;//! Taille allouée à tachesNonComposantes
    string identificateur;//! Identificateur du projet
    string nom;//! Nom du projet
    string file;//! Nom du fichier servant à exporter le projet
    Date dispo;//! Date de disponibilité du projet
    void addItem(Tache* t);//! Ajoute une tache à taches
    void addItemNC(Tache* t);//! Ajoute une tache à tachesNonComposantes
    void deleteTacheNC(Tache* t);//! Retire une tache de tachesNonComposantes
    Projet(const string& id, const string& nom, const string& file, const Date& d):
        taches(0), tachesNonComposantes(0), nb(0),nbMax(0), nbNC(0),nbMaxNC(0), identificateur(id), nom(nom),file(file),dispo(d){}//! Constructeur de projet à partir du nom, du nom de fichier et de la date de disponibilité
    Projet(const Projet& um);//! Constructeur par recopie interdit
    Projet& operator=(const Projet& um);//! Recopie par operator= interdite
    friend Projet& ProjetManager::ajouterProjet(const string& nom, const string& file, const Date& dispo);
    string genererId();//! Génére un identificateur pour une tache
public:
    Tache* trouverTache(const string& id) const;//! Renvoie la tache correspondant à id si elle existe dans taches
    bool estComposante(Tache* tache);//! Retourne TRUE si la taches n'est pas dans tachesNonComposantes
    ~Projet();//! Destructeur de projet
    Unitaire& ajouterUnitaire(const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool premp);//! Crée une tache unitaire et ajoute son pointeur dans taches et dans tachesNonComposantes, à partir d'un titre, de dates de disponibilité et d'échéance, d'un durée et de la préemptabilité
    Composite& ajouterComposite(const string& t, const Date& dispo, const Date& deadline);//! Crée une tache composite et ajoute son pointeur dans taches et dans tachesNonComposantes, à partir d'un titre et de dates de disponibilité et d'échéance
    Tache& getTache(const string& id);//! Retourne la référence vers la tache désignée par id si elle existe
    const Tache& getTache(const string& code) const;//! Retourne la const référence vers la tache désignée par id si elle existe
    string getId() const { return identificateur; }//! Retourne l'identificateur du projet
    string getNom() const { return nom; }//! Retourne le nom du projet
    string getFile() const { return file; }//! Retourne le nom du fichier du projet
    Date getDispo() const { return dispo; }//! Retourne la date de disponibilité du projet
    unsigned int getNbTaches() const { return nb; }//! Retourne le nombre d'élément dans taches
    Date getEcheance();//! Retourne la date d'échéance du projet à partir des dates d'échéances de toutes les taches du projet
    int getStatut(); //! Retourne le statut du projet : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    void moveTacheTo(Tache* tMere, Tache* tFille);//! Ajoute une tache du projet (fille) en composition d'une autre tache (mère) et la retire de tachesNonComposantes
    void save(const string& f);//! Exporte le projet sous le fichier f
    void update(const string& nom, const Date& d);//! Met à jour le nom et la date de disponibilité du projet à partir des paramètres donnés
    class Iterator{
        friend class Projet;
        private:
            Tache** tab;
            int nb;
            int indice_tache;
            bool allowSuppr;
            Iterator(Tache** t, int n, bool s):tab(t), nb(n), indice_tache(0), allowSuppr(s){ }
        public:
            Tache& current() const {
                if(indice_tache>=nb){
                    throw CalendarException("incrementation d'un iterateur en fin de sequence");
                }
                return *tab[indice_tache];
            }
            bool isDone() const { return indice_tache==nb; }
            void next(){ ++indice_tache; }
            void first(){ indice_tache=0; }
            void suppr();
        };
    Iterator getIterator(){ return Iterator(taches,nb, false); }//! Retourne un itérateur sur taches
    Iterator getNCIterator(){ return Iterator(tachesNonComposantes,nbNC, true); }//! Retourne un itérateur sur tachesNonComposantes
};

/*! \class Evenement
        \brief Classe abstraite d'éléments pouvant être programmés
*/
class Evenement {
    Duree duree;//! Durée de l'événement
public:
    virtual ~Evenement(){}//! Destructeur de l'événement
    Evenement(Duree dur):duree(dur){}//! Constructeur de l'événement à partir d'une durée
    Duree getDuree() const { return duree; }//! Renvoie la durée de l'événement
    QString getNom() const;//! Renvoie le nom et l'éventuel lieu de l'événement en QString
    void setDuree(const Duree& dur){ duree=dur; }//! Modifie la durée de l'événement à partir du paramètre "dur"
    virtual void afficher(ostream& f)=0;//! Méthode virtuelle pure d'affichage de l'événement
};

/*! \class Tache
        \brief Classe abstraite de taches composant Projet
*/
class Tache {
    Tache** prec;//! Tableau de pointeurs de taches, contraintes de précédence de la tache
    unsigned int nbPred;//! Nombre d'éléments de prec
    unsigned int maxPred;//! Taille allouée à prec
    string identificateur;//! Identificateur de la tache
    string titre;//! Titre de la tache
    Date disponibilite;//! Date de disponibilité de la tache
    Date echeance;//! Date d'échéance de la tache
    Projet* projetParent;//! Pointeur vers le projet contenant la tache
    Tache* trouverTache(const string& id) const;//! Renvoie un pointeur vers la tache désignée par id si elle existe dans prec
    Tache(const Tache& t);//! Constructeur par recopie interdit
    Tache& operator=(const Tache& t);//! Recopie par operator= interdite
public:
    unsigned int getNbPred(){return nbPred; }//! Renvoie le nombre d'éléments de prec
    Projet* getProjet() const { return projetParent; }//! Renvoie un pointeur vers le projet parent de la tache
    virtual ~Tache(){}//! Desctructeur de la tache
    Tache(const string& id, const string& t, const Date& dispo, const Date& deadline, Projet* p):
            prec(0), nbPred(0), maxPred(0), identificateur(id),titre(t),disponibilite(dispo),echeance(deadline), projetParent(p){}//! Constructeur de la tache à partir de l'id, le nom, les dates de disponibilité et échéance et le pointeur du projet parent
    virtual void addItem(Tache* t);//! Ajoute le pointeur de tache dans prec comme nouvelle contrainte de précédence
    Tache& getPrecedence(const string& id);//!  Renvoie une référence vers la tache désignée par id si elle existe dans prec
    const Tache& getPrecedence(const string& code) const;//!  Renvoie une const référence vers la tache désignée par id si elle existe dans prec
    bool isPrecedence() const { return nbPred>0?true:false; }//! Indique par booléen si la tache a des précédences ou non
    virtual bool isPrecedencePotentielle( const string& id );//! Indique si la tache désignée par id est une précédence potentielle ou non pour la tache
    string getId() const { return identificateur; }//! Renvoie l'identificateur de la tache
    string getTitre() const { return titre; }//! Renvoie le titre de la tache
    Date getDateDisponibilite() const {  return disponibilite; }//! Renvoie la date de disponibilité de la tache
    Date getDateEcheance() const {  return echeance; }//! Renvoie la date d'échéance de la tache
    Projet* getProjet() { return projetParent; }//! Renvoie le pointeur vers le projet parent
    virtual int getStatut() = 0; //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    virtual int isFinished(const Date& d, const Horaire& h)=0;//! Renvoie 1 si la tache a été terminée à la date d et l'horaire h, 0 sinon
    void setId(const string& id) { identificateur=id; }//! Modifie l'identificateur
    void setTitre(const string& t) { titre=t; }//! Modifie le titre
    void setDatesDisponibiliteEcheance(const Date& disp, const Date& e) {
        if (e<disp) throw CalendarException("erreur Tâche : date echéance < date disponibilité");
        disponibilite=disp; echeance=e;
    }//! Modifie les dates de disponibilité et d'échéance de la tache si elles sont compatibles
    class Iterator{
        friend class Tache;
        private:
            Tache** tab;
            int nb;
            int indice_tache;
            Iterator(Tache** t, int n):tab(t), nb(n), indice_tache(0){}
        public:
            Tache& current() const {
                if(indice_tache>=nb)
                    throw "incrementation d'un iterateur en fin de sequence";
                return *tab[indice_tache];
            }
            bool isDone() const { return indice_tache==nb; }
            void next(){ ++indice_tache; }
            void first(){ indice_tache=0; }
        };
    Iterator getIterator(){ return Iterator(prec,nbPred); }//! Renvoie un itérateur sur prec
    virtual void afficher(ostream& f) =0;//! Méthode virtuelle pure d'affichage de la tache
};

/*! \class Unitaire
        \brief Classe de taches programmables, héritant de Tachet et d'Evenement
        La classe de taches pouvant être programmées, en plusieurs fois si préemptive, en une seule fois sinon.
        La classe est forcément préemptive si sa durée dépasse 12h.
*/
class Unitaire : public Tache, public Evenement {
    Duree dureeFaite;//! Durée totale programmée de la tache
    bool preemptable;//! Préemptabilité de la tache
    Unitaire(const Unitaire& t);//! Constructeur par recopie interdit
    Unitaire& operator=(const Unitaire& t);//! Recopie par operator= interdite
    Unitaire(Date d, Date e, string id, string t, Duree dur, bool p, Projet *pro):Tache(id, t, d, e, pro),Evenement(dur),dureeFaite(0),preemptable(p){
        if(dur.getDureeEnHeures()>=12){
            preemptable=true;
        }
    }//! Constructeur de tache unitaire à partir de la date de disponibilité, date d'échéance, durée, préemptabilité et pointeur du projet parent. Le constructeur rend la tache préemptable si la durée > 12h.
    friend Unitaire& Projet::ajouterUnitaire(const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool premp);
public:
    virtual void addItem(Tache* t){Tache::addItem(t);}//! Ajoute la tache pointée aux tableau de précédences prec
    bool isPreemp() const { return preemptable; }//! Indique si la tache est préemptable ou non
    Duree getFait() const { return dureeFaite; }//! Retourne la durée faite de la tache
    Duree getRestant() const { Duree dR = Duree(getDuree().getDureeEnMinutes() - getFait().getDureeEnMinutes()); return dR; }//! Retourne la durée restante à programmer de la tache, calculée en soustrayant la durée totale avec la dutée faite
    int getStatut(); //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    virtual int isFinished(const Date& d, const Horaire& h);//! Renvoie 1 si la tache a été terminée à la date d et l'horaire h, 0 sinon
    void setFait(const Duree& f){ dureeFaite=f; }//! Modifie la durée faite de la tache
    void setPreemp(){ preemptable=true; }//! Rend la tache préemptable
    void setNonPreemp(){ if(getDuree().getDureeEnHeures()>12) preemptable=true; else preemptable=false; }//! Rend, si possible, la tache preemptable
    virtual void afficher(ostream& f) ;//! Affiche les attributs de la tache
    void update(string id, string t, Date d, Date e, Duree dur, Duree df, bool p){
        if(getRestant().getDureeEnMinutes()==0)
            throw CalendarException("La tache a été entièrement programmée et n'est plus modifiable");
        setId(id); setTitre(t); setDatesDisponibiliteEcheance(d,e); setDuree(dur); setFait(df);
        if(dur.getDureeEnHeures()>=12)
            setPreemp();
        else{
            if(p)setPreemp();
            else setNonPreemp();
        }
    }//! Met à jour la tache en modifiant ses attributs à partir des paramètres donnés : id, titre, disponibilité, échéance, durée, durée faite et préemptabilité
    void update(string t, Date d, Date e, Duree dur, bool p){
        setTitre(t); setDatesDisponibiliteEcheance(d,e); setDuree(dur);
        if(dur.getDureeEnHeures()>=12)
            setPreemp();
        else{
            if(p)setPreemp();
            else setNonPreemp();
        }
    }//! Met à jour la tache en modifiant ses attributs à partir des paramètres donnés : id, titre, disponibilité, échéance, durée et préemptabilité
};

/*! \class Composite
        \brief Classe d'ensembles de taches, héritant de Tache
        La classe possède un pointeur de Taches qui représente les composants de la tache composite.
*/
class Composite : public Tache {
private:
    Tache** composition;//! Tableau de pointeurs de Taches composantes
    unsigned int nbCompo;//! Nombre d'éléments dans composition
    unsigned int nbMaxCompo;//! Taille allouée à composition
    Tache* trouverCompo(const string& id) const;//! Renvoie le pointeur vers la tache correspondant à id si elle existe dans composition
    Composite(const Composite& t);//! Constructeur par recopie interdit
    Composite& operator=(const Composite& t);//! Recopie par operator= interdite
    Composite(string id, string t, Date d, Date e, Projet* p):Tache(id,t,d,e,p),composition(0),nbCompo(0),nbMaxCompo(0){}//! Constructeur de composite avec un identificateur, un titre, date de disponibilité, date d'échéance et pointeur vers projet parent
    friend Composite& Projet::ajouterComposite(const string& t, const Date& dispo, const Date& deadline);
public:
    unsigned int getNbCompo(){return nbCompo; }//! Retourne le nombre de taches composantes
    virtual void addItem(Tache* t);//! Ajoute le pointeur de tache dans le tableau de précédence
    void addCompo(Tache* t);//! Ajoute la pointeur de tache dans composition
    Tache& getCompo(const string& id);//! Renvoie la référence vers la tache correspondant à id si elle existe dans composition
    const Tache& getCompo(const string& code) const;//! Renvoie la const référence vers la tache correspondant à id si elle existe dans composition
    int getStatut(); //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    virtual int isFinished(const Date& d, const Horaire& h);//! Renvoie 1 si la tache a été terminée à la date d et l'horaire h, 0 sinon
    bool isPrecedencePotentielle( const string& id );//! Indique si la tache correspondant à id est une précédence potentielle ou non
    void update(string id, string t, Date d, Date e){
        setId(id); setTitre(t); setDatesDisponibiliteEcheance(d,e);
    }//! Met à jour les attributs identificateur, titre, dates de Composite
    void update(string t, Date d, Date e){
        setTitre(t); setDatesDisponibiliteEcheance(d,e);
    }//! Met à jour les attributes titre et dates de composite
    friend class CompoIterator;
    class CompoIterator{
    private:
        Tache** tab;
        int nb;
        int indice_tache;
        friend class Composite;
        CompoIterator(Tache** t, int n):tab(t), nb(n), indice_tache(0){}
    public:
        Tache& current() const {
            if(indice_tache>=nb)
                throw "incrementation d'un iterateur en fin de sequence";
            return *tab[indice_tache];
        }
        bool isDone() const { return indice_tache==nb; }
        void next(){ ++indice_tache; }
        void first(){ indice_tache=0; }
    };
    CompoIterator getCompoIterator(){ return CompoIterator(composition,nbCompo); }//! Retourne un iterateur sur le tableau de composition
    virtual void afficher(ostream& f) ;//! Affiche les attributs de la tache composite
};

/*! \class Activite
        \brief Classe d'evenement basique programmable, héritant d'Evenement
        La classe possède un titre et un lieu.
*/
class Activite : public Evenement {
private:
    string titre;//! Titre de l'activité
    string lieu;//! Lieu de l'activité
public:
    Activite(const string& t, const string& l, Duree dur): Evenement(dur), titre(t), lieu(l) {}//! Constructeur d'activité
    string getTitre() const { return titre; }//! Retourne le titre
    string getLieu() const { return lieu; }//! Retourne le lieu
    void setTitre(const string& t) { titre=t; }//! Modifie le titre
    void setLieu(const string& l) { lieu=l; }//! Modifie le lieu
    void update(string t, string l, Duree d){
        setTitre(t); setLieu(l); setDuree(d);
    }//! Met à jour le titre, le lieu et la durée de l'activité
    virtual void afficher(ostream& f) ;//! Affiche les attributs de l'activité
};

class Programmation;

/*! \class ProgrammationManager
        \brief Classe composite de Programmation
        La classe applique le designe pattern singleton et possède un iterateur sur le tableau de pointeurs de Programmations.
*/
class ProgrammationManager : public QObject {
    Q_OBJECT
private:
    Programmation** programmations;//! Tableau de pointeurs des programmations
    unsigned int nb;//! Nombre de programmations
    unsigned int nbMax;//! Taille allouée au tableau programmations
    void addItem(Programmation* t);//! Ajoute le pointeur de programmation au tableau

    ProgrammationManager():programmations(0),nb(0),nbMax(0){}//! Constructeur de ProgrammationManager, initialise le tableau à 0
    ~ProgrammationManager();//! Destructeur de ProgrammationManager
    ProgrammationManager(const ProgrammationManager& um);//! Constructeur par recopie interdit
    ProgrammationManager& operator=(const ProgrammationManager& um);//! Recopie par operator= interdite
    struct Handler{
        ProgrammationManager* instance;
        Handler():instance(0){}
        // destructeur appelé à la fin du programme
        ~Handler(){ if (instance) delete instance; }
    };
    static Handler handler;//! Handler permettant de rendre ProgrammationManager singleton
public:
    Programmation* trouverProgrammation(Evenement& e) const;//! Renvoie le pointeur de la programmation de l'événement correspondant si elle existe dans le tableau
    Programmation* trouverProgrammation(const Evenement& e) const;//! Renvoie le pointeur de la programmation de l'événement const correspondant si elle existe dans le tableau
    static ProgrammationManager& getInstance();//! Renvoie la seule instance de ProgrammationManager
    static void libererInstance();//! Libère l'unique instance de ProgrammationManager
    void ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur);//! Crée une programmation d'une Tache Unitaire et stocke le pointeur dans le tableau programmations
    void ajouterProgrammation(Activite& e, const Date& d, const Horaire& h, const Duree& dur);//! Crée une programmation d'une Activité et stocke le pointeur dans le tableau programmations
    class Iterator{
        friend class ProgrammationManager;
        private:
            Programmation** tab;
            int nb;
            int indice_prog;
            Iterator(Programmation** t, int n):tab(t), nb(n), indice_prog(0){}
        public:
            Programmation& current() const {
                if(indice_prog>=nb)
                    throw "incrementation d'un iterateur en fin de sequence";
                return *tab[indice_prog];
            }
            bool isDone() const { return indice_prog==nb; }
            void next(){ ++indice_prog; }
            void first(){ indice_prog=0; }
        };
    Iterator getIterator(){ return Iterator(programmations,nb); }//! Renvoie un iterateur sur programmations
signals:
    void programmationsChanged();//! Signal de changement de programmation
};

/*! \class Programmation
        \brief Classe donnant une date, un horaire et une durée à une Evenement, classe composant ProgrammationManager
*/
class Programmation {
    const Evenement* evt;//! Pointeur vers l'événement programmé
    Date date;//! Date de la programmation
    Horaire horaire;//! Horaire de la programmation
    Duree duree;//! Durée de la programmation
    Programmation(const Programmation& um);//! Constructeur par recopie interdit
    Programmation& operator=(const Programmation& um);//! Recopie par operator= interdite
    Programmation(const Evenement& e, const Date& d, const Horaire& h, const Duree& dur):evt(&e), date(d), horaire(h), duree(dur){}//! Constructeur de programmation à partir d'une référence sur un Evenement, d'une date, d'un horaire et d'une durée
    friend void ProgrammationManager::ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur);
    friend void ProgrammationManager::ajouterProgrammation(Activite& e, const Date& d, const Horaire& h, const Duree& dur);
public:
    const Evenement& getEvenement() const { return *evt; }//! Renvoie la référence sur l'événement programmé
    Date getDate() const { return date; }//! Renvoie la date de la programmation
    Horaire getHoraire() const { return horaire; }//! Renvoie l'horaire de la programmation
    Duree getDuree() const { return duree; }//! Renvoie la durée de la programmation
};

ostream& operator<<(ostream& f, const Tache& t);//! Operator d'affichage de Tache sur un ostream

#endif
