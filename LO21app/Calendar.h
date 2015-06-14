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
    //! Constructeur à partir d'un string
    CalendarException(const string& message):info(message){} 
    //! Retourne le message d'erreur
    string getInfo() const { return info; } 
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
    //! Tableau de pointeurs de Projet
    Projet** projets;
    //! Nombre de Projets dans le tableau
    unsigned int nb;
    //! Taille allouée au tableau
    unsigned int nbMax;
    //! Ajoute le projet* p au tableau projets
    void addItem(Projet* p);
    //! Renvoie le projet correspondant à id s'il est présent dans le tableau projets
    Projet* trouverProjet(const string& id) const;
    //! Constructeur de ProjetManager, initialise le tableau et sa taille à 0
    ProjetManager():projets(0),nb(0),nbMax(0){}
    //! Destructeur de ProjetManager, détruit les Projets composants puis le tableau projets
    ~ProjetManager(){for(unsigned int i=0; i<nb; i++) delete projets[i]; delete[] projets;}
    //! Constructeur par recopie interdit
    ProjetManager(const Projet& um);
    //! Recopie par operator= interdite
    ProjetManager& operator=(const Projet& um);
    struct Handler{
        ProjetManager* instance;
        Handler():instance(0){}
        ~Handler(){ if (instance) delete instance; }
    };
    //! Handler permettant de rendre ProjetManager singleton
    static Handler handler;
    //! Générateur d'identificateurs pour Projet
    string genererId();
public:
    //! Renvoie la seule instance de ProjetManager
    static ProjetManager& getInstance();
    //! Libère l'instance de ProjetManager
    static void libererInstance();
    //! Crée un Projet et garde son pointeur dans le tableau projets, à partir d'n nom, d'un nom de fichier et d'une date de disponibilité
    Projet& ajouterProjet(const string& nom, const string& file, const Date& dispo);
    //! Renvoie une référence vers le projet correspondant à id s'il existe dans le tableau projets
    Projet& getProjet(const string& id);
    //! Renvoie une référence const vers le projet correspondant à id s'il existe dans le tableau projets
    const Projet& getProjet(const string& code) const;
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
    //! Renvoie un itérateur sur le tableau projets
    Iterator getIterator(){ return Iterator(projets,nb); }
};

class Tache;
class Unitaire;
class Composite;

/*! \class Projet
        \brief Classe de projets composite des Taches et composant ProjetManager
*/
class Projet {
private:
    //! Tableau de pointeurs de taches
    Tache** taches;
    //! Tableau de pointeurs de taches pour les taches n'étant pas pointées par une Composite
    Tache** tachesNonComposantes;
    //! Nombre d'éléments dans taches
    unsigned int nb;
    //! Taille allouée à taches
    unsigned int nbMax;
    //! Nombre d'éléments dans tachesNonComposantes
    unsigned int nbNC;
    //! Taille allouée à tachesNonComposantes
    unsigned int nbMaxNC;
    //! Identificateur du projet
    string identificateur;
    //! Nom du projet
    string nom;
    //! Nom du fichier servant à exporter le projet
    string file;
    //! Date de disponibilité du projet
    Date dispo;
    //! Ajoute une tache à taches
    void addItem(Tache* t);
    //! Ajoute une tache à tachesNonComposantes
    void addItemNC(Tache* t);
    //! Retire une tache de tachesNonComposantes
    void deleteTacheNC(Tache* t);
    Projet(const string& id, const string& nom, const string& file, const Date& d):
        //! Constructeur de projet à partir du nom, du nom de fichier et de la date de disponibilité
        taches(0), tachesNonComposantes(0), nb(0),nbMax(0), nbNC(0),nbMaxNC(0), identificateur(id), nom(nom),file(file),dispo(d){}
    //! Constructeur par recopie interdit
    Projet(const Projet& um);
    //! Recopie par operator= interdite
    Projet& operator=(const Projet& um);
    friend Projet& ProjetManager::ajouterProjet(const string& nom, const string& file, const Date& dispo);
    //! Génére un identificateur pour une tache
    string genererId();
public:
    //! Renvoie la tache correspondant à id si elle existe dans taches
    Tache* trouverTache(const string& id) const;
    //! Retourne TRUE si la taches n'est pas dans tachesNonComposantes
    bool estComposante(Tache* tache);
    //! Destructeur de projet
    ~Projet();
    //! Crée une tache unitaire et ajoute son pointeur dans taches et dans tachesNonComposantes, à partir d'un titre, de dates de disponibilité et d'échéance, d'un durée et de la préemptabilité
    Unitaire& ajouterUnitaire(const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool premp);
    //! Crée une tache composite et ajoute son pointeur dans taches et dans tachesNonComposantes, à partir d'un titre et de dates de disponibilité et d'échéance
    Composite& ajouterComposite(const string& t, const Date& dispo, const Date& deadline);
    //! Retourne la référence vers la tache désignée par id si elle existe
    Tache& getTache(const string& id);
    //! Retourne la const référence vers la tache désignée par id si elle existe
    const Tache& getTache(const string& code) const;
    //! Retourne l'identificateur du projet
    string getId() const { return identificateur; }
    //! Retourne le nom du projet
    string getNom() const { return nom; }
    //! Retourne le nom du fichier du projet
    string getFile() const { return file; }
    //! Retourne la date de disponibilité du projet
    Date getDispo() const { return dispo; }
    //! Retourne le nombre d'élément dans taches
    unsigned int getNbTaches() const { return nb; }
    //! Retourne la date d'échéance du projet à partir des dates d'échéances de toutes les taches du projet
    Date getEcheance();
    //! Retourne le statut du projet : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    int getStatut(); 
    //! Ajoute une tache du projet (fille) en composition d'une autre tache (mère) et la retire de tachesNonComposantes
    void moveTacheTo(Tache* tMere, Tache* tFille);
    //! Exporte le projet sous le fichier f
    void save(const string& f);
    //! Met à jour le nom et la date de disponibilité du projet à partir des paramètres donnés
    void update(const string& nom, const Date& d);
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
    //! Retourne un itérateur sur taches
    Iterator getIterator(){ return Iterator(taches,nb, false); }
    //! Retourne un itérateur sur tachesNonComposantes
    Iterator getNCIterator(){ return Iterator(tachesNonComposantes,nbNC, true); }
};

/*! \class Evenement
        \brief Classe abstraite d'éléments pouvant être programmés
*/
class Evenement {
    //! Durée de l'événement
    Duree duree;
public:
    //! Destructeur de l'événement
    virtual ~Evenement(){}
    //! Constructeur de l'événement à partir d'une durée
    Evenement(Duree dur):duree(dur){}
    //! Renvoie la durée de l'événement
    Duree getDuree() const { return duree; }
    //! Renvoie le nom et l'éventuel lieu de l'événement en QString
    QString getNom() const;
    //! Modifie la durée de l'événement à partir du paramètre "dur"
    void setDuree(const Duree& dur){ duree=dur; }
    //! Méthode virtuelle pure d'affichage de l'événement
    virtual void afficher(ostream& f)=0;
};

/*! \class Tache
        \brief Classe abstraite de taches composant Projet
*/
class Tache {
    //! Tableau de pointeurs de taches, contraintes de précédence de la tache
    Tache** prec;
    //! Nombre d'éléments de prec
    unsigned int nbPred;
    //! Taille allouée à prec
    unsigned int maxPred;
    //! Identificateur de la tache
    string identificateur;
    //! Titre de la tache
    string titre;
    //! Date de disponibilité de la tache
    Date disponibilite;
    //! Date d'échéance de la tache
    Date echeance;
    //! Pointeur vers le projet contenant la tache
    Projet* projetParent;
    //! Renvoie un pointeur vers la tache désignée par id si elle existe dans prec
    Tache* trouverTache(const string& id) const;
    //! Constructeur par recopie interdit
    Tache(const Tache& t);
    //! Recopie par operator= interdite
    Tache& operator=(const Tache& t);
public:
    //! Renvoie le nombre d'éléments de prec
    unsigned int getNbPred(){return nbPred; }
    //! Renvoie un pointeur vers le projet parent de la tache
    Projet* getProjet() const { return projetParent; }
    //! Desctructeur de la tache
    virtual ~Tache(){}
    Tache(const string& id, const string& t, const Date& dispo, const Date& deadline, Projet* p):
            //! Constructeur de la tache à partir de l'id, le nom, les dates de disponibilité et échéance et le pointeur du projet parent
            prec(0), nbPred(0), maxPred(0), identificateur(id),titre(t),disponibilite(dispo),echeance(deadline), projetParent(p){}
    //! Ajoute le pointeur de tache dans prec comme nouvelle contrainte de précédence
    virtual void addItem(Tache* t);
    //!  Renvoie une référence vers la tache désignée par id si elle existe dans prec
    Tache& getPrecedence(const string& id);
    //!  Renvoie une const référence vers la tache désignée par id si elle existe dans prec
    const Tache& getPrecedence(const string& code) const;
    //! Indique par booléen si la tache a des précédences ou non
    bool isPrecedence() const { return nbPred>0?true:false; }
    //! Indique si la tache désignée par id est une précédence potentielle ou non pour la tache
    virtual bool isPrecedencePotentielle( const string& id );
    //! Renvoie l'identificateur de la tache
    string getId() const { return identificateur; }
    //! Renvoie le titre de la tache
    string getTitre() const { return titre; }
    //! Renvoie la date de disponibilité de la tache
    Date getDateDisponibilite() const {  return disponibilite; }
    //! Renvoie la date d'échéance de la tache
    Date getDateEcheance() const {  return echeance; }
    //! Renvoie le pointeur vers le projet parent
    Projet* getProjet() { return projetParent; }
    //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    virtual int getStatut() = 0; 
    //! Renvoie 1 si la tache a été terminée à la date d et l'horaire h, 0 sinon
    virtual int isFinished(const Date& d, const Horaire& h)=0;
    //! Modifie l'identificateur
    void setId(const string& id) { identificateur=id; }
    //! Modifie le titre
    void setTitre(const string& t) { titre=t; }
    void setDatesDisponibiliteEcheance(const Date& disp, const Date& e) {
        if (e<disp) throw CalendarException("erreur Tâche : date echéance < date disponibilité");
        disponibilite=disp; echeance=e;
    //! Modifie les dates de disponibilité et d'échéance de la tache si elles sont compatibles
    }
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
    //! Renvoie un itérateur sur prec
    Iterator getIterator(){ return Iterator(prec,nbPred); }
    //! Méthode virtuelle pure d'affichage de la tache
    virtual void afficher(ostream& f) =0;
};

/*! \class Unitaire
        \brief Classe de taches programmables, héritant de Tachet et d'Evenement
        La classe de taches pouvant être programmées, en plusieurs fois si préemptive, en une seule fois sinon.
        La classe est forcément préemptive si sa durée dépasse 12h.
*/
class Unitaire : public Tache, public Evenement {
    //! Durée totale programmée de la tache
    Duree dureeFaite;
    //! Préemptabilité de la tache
    bool preemptable;
    //! Constructeur par recopie interdit
    Unitaire(const Unitaire& t);
    //! Recopie par operator= interdite
    Unitaire& operator=(const Unitaire& t);
    Unitaire(Date d, Date e, string id, string t, Duree dur, bool p, Projet *pro):Tache(id, t, d, e, pro),Evenement(dur),dureeFaite(0),preemptable(p){
        if(dur.getDureeEnHeures()>=12){
            preemptable=true;
        }
    //! Constructeur de tache unitaire à partir de la date de disponibilité, date d'échéance, durée, préemptabilité et pointeur du projet parent. Le constructeur rend la tache préemptable si la durée > 12h.
    }
    friend Unitaire& Projet::ajouterUnitaire(const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool premp);
public:
    //! Ajoute la tache pointée aux tableau de précédences prec
    virtual void addItem(Tache* t){Tache::addItem(t);}
    //! Indique si la tache est préemptable ou non
    bool isPreemp() const { return preemptable; }
    //! Retourne la durée faite de la tache
    Duree getFait() const { return dureeFaite; }
    //! Retourne la durée restante à programmer de la tache, calculée en soustrayant la durée totale avec la dutée faite
    Duree getRestant() const { Duree dR = Duree(getDuree().getDureeEnMinutes() - getFait().getDureeEnMinutes()); return dR; }
    //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    int getStatut(); 
    //! Renvoie 1 si la tache a été terminée à la date d et l'horaire h, 0 sinon
    virtual int isFinished(const Date& d, const Horaire& h);
    //! Modifie la durée faite de la tache
    void setFait(const Duree& f){ dureeFaite=f; }
    //! Rend la tache préemptable
    void setPreemp(){ preemptable=true; }
    //! Rend, si possible, la tache preemptable
    void setNonPreemp(){ if(getDuree().getDureeEnHeures()>12) preemptable=true; else preemptable=false; }
    //! Affiche les attributs de la tache
    virtual void afficher(ostream& f) ;
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
    //! Met à jour la tache en modifiant ses attributs à partir des paramètres donnés : id, titre, disponibilité, échéance, durée, durée faite et préemptabilité
    }
    void update(string t, Date d, Date e, Duree dur, bool p){
        setTitre(t); setDatesDisponibiliteEcheance(d,e); setDuree(dur);
        if(dur.getDureeEnHeures()>=12)
            setPreemp();
        else{
            if(p)setPreemp();
            else setNonPreemp();
        }
    //! Met à jour la tache en modifiant ses attributs à partir des paramètres donnés : id, titre, disponibilité, échéance, durée et préemptabilité
    }
};

/*! \class Composite
        \brief Classe d'ensembles de taches, héritant de Tache
        La classe possède un pointeur de Taches qui représente les composants de la tache composite.
*/
class Composite : public Tache {
private:
    //! Tableau de pointeurs de Taches composantes
    Tache** composition;
    //! Nombre d'éléments dans composition
    unsigned int nbCompo;
    //! Taille allouée à composition
    unsigned int nbMaxCompo;
    //! Renvoie le pointeur vers la tache correspondant à id si elle existe dans composition
    Tache* trouverCompo(const string& id) const;
    //! Constructeur par recopie interdit
    Composite(const Composite& t);
    //! Recopie par operator= interdite
    Composite& operator=(const Composite& t);
    //! Constructeur de composite avec un identificateur, un titre, date de disponibilité, date d'échéance et pointeur vers projet parent
    Composite(string id, string t, Date d, Date e, Projet* p):Tache(id,t,d,e,p),composition(0),nbCompo(0),nbMaxCompo(0){}
    friend Composite& Projet::ajouterComposite(const string& t, const Date& dispo, const Date& deadline);
public:
    //! Retourne le nombre de taches composantes
    unsigned int getNbCompo(){return nbCompo; }
    //! Ajoute le pointeur de tache dans le tableau de précédence
    virtual void addItem(Tache* t);
    //! Ajoute la pointeur de tache dans composition
    void addCompo(Tache* t);
    //! Renvoie la référence vers la tache correspondant à id si elle existe dans composition
    Tache& getCompo(const string& id);
    //! Renvoie la const référence vers la tache correspondant à id si elle existe dans composition
    const Tache& getCompo(const string& code) const;
    //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    int getStatut(); 
    //! Renvoie 1 si la tache a été terminée à la date d et l'horaire h, 0 sinon
    virtual int isFinished(const Date& d, const Horaire& h);
    //! Indique si la tache correspondant à id est une précédence potentielle ou non
    bool isPrecedencePotentielle( const string& id );
    void update(string id, string t, Date d, Date e){
        setId(id); setTitre(t); setDatesDisponibiliteEcheance(d,e);
    //! Met à jour les attributs identificateur, titre, dates de Composite
    }
    void update(string t, Date d, Date e){
        setTitre(t); setDatesDisponibiliteEcheance(d,e);
    //! Met à jour les attributes titre et dates de composite
    }
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
    //! Retourne un iterateur sur le tableau de composition
    CompoIterator getCompoIterator(){ return CompoIterator(composition,nbCompo); }
    //! Affiche les attributs de la tache composite
    virtual void afficher(ostream& f) ;
};

/*! \class Activite
        \brief Classe d'evenement basique programmable, héritant d'Evenement
        La classe possède un titre et un lieu.
*/
class Activite : public Evenement {
private:
    //! Titre de l'activité
    string titre;
    //! Lieu de l'activité
    string lieu;
public:
    //! Constructeur d'activité
    Activite(const string& t, const string& l, Duree dur): Evenement(dur), titre(t), lieu(l) {}
    //! Retourne le titre
    string getTitre() const { return titre; }
    //! Retourne le lieu
    string getLieu() const { return lieu; }
    //! Modifie le titre
    void setTitre(const string& t) { titre=t; }
    //! Modifie le lieu
    void setLieu(const string& l) { lieu=l; }
    void update(string t, string l, Duree d){
        setTitre(t); setLieu(l); setDuree(d);
    //! Met à jour le titre, le lieu et la durée de l'activité
    }
    //! Affiche les attributs de l'activité
    virtual void afficher(ostream& f) ;
};

class Programmation;

/*! \class ProgrammationManager
        \brief Classe composite de Programmation
        La classe applique le designe pattern singleton et possède un iterateur sur le tableau de pointeurs de Programmations.
*/
class ProgrammationManager : public QObject {
    Q_OBJECT
private:
    //! Tableau de pointeurs des programmations
    Programmation** programmations;
    //! Nombre de programmations
    unsigned int nb;
    //! Taille allouée au tableau programmations
    unsigned int nbMax;
    //! Ajoute le pointeur de programmation au tableau
    void addItem(Programmation* t);

    //! Constructeur de ProgrammationManager, initialise le tableau à 0
    ProgrammationManager():programmations(0),nb(0),nbMax(0){}
    //! Destructeur de ProgrammationManager
    ~ProgrammationManager();
    //! Constructeur par recopie interdit
    ProgrammationManager(const ProgrammationManager& um);
    //! Recopie par operator= interdite
    ProgrammationManager& operator=(const ProgrammationManager& um);
    struct Handler{
        ProgrammationManager* instance;
        Handler():instance(0){}
        // destructeur appelé à la fin du programme
        ~Handler(){ if (instance) delete instance; }
    };
    //! Handler permettant de rendre ProgrammationManager singleton
    static Handler handler;
public:
    //! Renvoie le pointeur de la programmation de l'événement correspondant si elle existe dans le tableau
    Programmation* trouverProgrammation(Evenement& e) const;
    //! Renvoie le pointeur de la programmation de l'événement const correspondant si elle existe dans le tableau
    Programmation* trouverProgrammation(const Evenement& e) const;
    //! Renvoie la seule instance de ProgrammationManager
    static ProgrammationManager& getInstance();
    //! Libère l'unique instance de ProgrammationManager
    static void libererInstance();
    //! Crée une programmation d'une Tache Unitaire et stocke le pointeur dans le tableau programmations
    void ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur);
    //! Crée une programmation d'une Activité et stocke le pointeur dans le tableau programmations
    void ajouterProgrammation(Activite& e, const Date& d, const Horaire& h, const Duree& dur);
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
    //! Renvoie un iterateur sur programmations
    Iterator getIterator(){ return Iterator(programmations,nb); }
signals:
    //! Signal de changement de programmation
    void programmationsChanged();
};

/*! \class Programmation
        \brief Classe donnant une date, un horaire et une durée à une Evenement, classe composant ProgrammationManager
*/
class Programmation {
    //! Pointeur vers l'événement programmé
    const Evenement* evt;
    //! Date de la programmation
    Date date;
    //! Horaire de la programmation
    Horaire horaire;
    //! Durée de la programmation
    Duree duree;
    //! Constructeur par recopie interdit
    Programmation(const Programmation& um);
    //! Recopie par operator= interdite
    Programmation& operator=(const Programmation& um);
    //! Constructeur de programmation à partir d'une référence sur un Evenement, d'une date, d'un horaire et d'une durée
    Programmation(const Evenement& e, const Date& d, const Horaire& h, const Duree& dur):evt(&e), date(d), horaire(h), duree(dur){}
    friend void ProgrammationManager::ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur);
    friend void ProgrammationManager::ajouterProgrammation(Activite& e, const Date& d, const Horaire& h, const Duree& dur);
public:
    //! Renvoie la référence sur l'événement programmé
    const Evenement& getEvenement() const { return *evt; }
    //! Renvoie la date de la programmation
    Date getDate() const { return date; }
    //! Renvoie l'horaire de la programmation
    Horaire getHoraire() const { return horaire; }
    //! Renvoie la durée de la programmation
    Duree getDuree() const { return duree; }
};

//! Operator d'affichage de Tache sur un ostream
ostream& operator<<(ostream& f, const Tache& t);

#endif
