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
    Projet** projets;
    unsigned int nb;
    unsigned int nbMax;
    void addItem(Projet* p);
    Projet* trouverProjet(const string& id) const;
    ProjetManager():projets(0),nb(0),nbMax(0){}
    ~ProjetManager(){for(unsigned int i=0; i<nb; i++) delete projets[i]; delete[] projets;}
    ProjetManager(const Projet& um);
    ProjetManager& operator=(const Projet& um);
    struct Handler{
        ProjetManager* instance;
        Handler():instance(0){}
        // destructeur appelé à la fin du programme
        ~Handler(){ if (instance) delete instance; }
    };
    static Handler handler;
    string genererId();
public:
    static ProjetManager& getInstance();
    static void libererInstance();
    Projet& ajouterProjet(const string& nom, const string& file, const Date& dispo);
    Projet& getProjet(const string& id);
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
    Tache** taches;
    Tache** tachesNonComposantes;
    unsigned int nb;
    unsigned int nbMax;
    unsigned int nbNC;
    unsigned int nbMaxNC;
    string identificateur;
    string nom;
    string file;
    Date dispo;
    void addItem(Tache* t);
    void addItemNC(Tache* t);
    void deleteTacheNC(Tache* t);
    Projet(const string& id, const string& nom, const string& file, const Date& d):
        taches(0), tachesNonComposantes(0), nb(0),nbMax(0), nbNC(0),nbMaxNC(0), identificateur(id), nom(nom),file(file),dispo(d){}
    Projet(const Projet& um);
    Projet& operator=(const Projet& um);
    friend Projet& ProjetManager::ajouterProjet(const string& nom, const string& file, const Date& dispo);
    string genererId();
public:
    Tache* trouverTache(const string& id) const;
    bool estComposante(Tache* tache);
    ~Projet();
    Unitaire& ajouterUnitaire(const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool premp);
    Composite& ajouterComposite(const string& t, const Date& dispo, const Date& deadline);
    Tache& getTache(const string& id);
    string getId() const { return identificateur; }
    string getNom() const { return nom; }
    string getFile() const { return file; }
    Date getDispo() const { return dispo; }
    unsigned int getNbTaches() const { return nb; }
    Date getEcheance();
    int getStatut(); //! Retourne le statut du projet : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    void moveTacheTo(Tache* tMere, Tache* tFille);
    //void load(const string& f);
    void save(const string& f);
    void update(const string& nom, const Date& d);
    const Tache& getTache(const string& code) const;
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
    Iterator getIterator(){ return Iterator(taches,nb, false); }
    Iterator getNCIterator(){ return Iterator(tachesNonComposantes,nbNC, true); }
};

/*! \class Evenement
        \brief Classe abstraite d'éléments pouvant être programmés
*/
class Evenement {
    Duree duree;
public:
    virtual ~Evenement(){}
    Evenement(Duree dur):duree(dur){}
    Duree getDuree() const { return duree; }
    QString getNom() const;
    void setDuree(const Duree& dur){ duree=dur; }
    virtual void afficher(ostream& f)=0;
};

/*! \class Tache
        \brief Classe abstraite de taches composant Projet
*/
class Tache {
    Tache** prec;
    unsigned int nbPred;
    unsigned int maxPred;
    string identificateur;
    string titre;
    Date disponibilite;
    Date echeance;
    Projet* projetParent;
    Tache* trouverTache(const string& id) const;
    Tache(const Tache& t);
    Tache& operator=(const Tache& t);
public:
    unsigned int getNbPred(){return nbPred; }
    Projet* getProjet() const { return projetParent; }
    virtual ~Tache(){}
    Tache(const string& id, const string& t, const Date& dispo, const Date& deadline, Projet* p):
            prec(0), nbPred(0), maxPred(0), identificateur(id),titre(t),disponibilite(dispo),echeance(deadline), projetParent(p){}
    virtual void addItem(Tache* t);
    Tache& getPrecedence(const string& id);
    const Tache& getPrecedence(const string& code) const;
    bool isPrecedence() const { return nbPred>0?true:false; }
    virtual bool isPrecedencePotentielle( const string& id );
    string getId() const { return identificateur; }
    string getTitre() const { return titre; }
    Date getDateDisponibilite() const {  return disponibilite; }
    Date getDateEcheance() const {  return echeance; }
    Projet* getProjet() { return projetParent; }
    virtual int getStatut() = 0; //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    void setId(const string& id) { identificateur=id; }
    void setTitre(const string& t) { titre=t; }
    void setDatesDisponibiliteEcheance(const Date& disp, const Date& e) {
        if (e<disp) throw CalendarException("erreur Tâche : date echéance < date disponibilité");
        disponibilite=disp; echeance=e;
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
    Iterator getIterator(){ return Iterator(prec,nbPred); }
    virtual void afficher(ostream& f) =0;
};

/*! \class Unitaire
        \brief Classe de taches programmables, héritant de Tachet et d'Evenement
        La classe de taches pouvant être programmées, en plusieurs fois si préemptive, en une seule fois sinon.
        La classe est forcément préemptive si sa durée dépasse 12h.
*/
class Unitaire : public Tache, public Evenement {
    Duree dureeFaite;
    bool preemptable;
    Unitaire(const Unitaire& t);
    Unitaire& operator=(const Unitaire& t);
    Unitaire(Date d, Date e, string id, string t, Duree dur, bool p, Projet *pro):Tache(id, t, d, e, pro),Evenement(dur),dureeFaite(0),preemptable(p){
        if(dur.getDureeEnHeures()>=12){
            preemptable=true;
        }
    }
    friend Unitaire& Projet::ajouterUnitaire(const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool premp);
public:
    virtual void addItem(Tache* t){Tache::addItem(t);}
    bool isPreemp() const { return preemptable; }
    Duree getFait() const { return dureeFaite; }
    Duree getRestant() const { Duree dR = Duree(getDuree().getDureeEnMinutes() - getFait().getDureeEnMinutes()); return dR; }
    int getStatut(); //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    void setFait(const Duree& f){ dureeFaite=f; }
    void setPreemp(){ preemptable=true; }
    void setNonPreemp(){ preemptable=false; }
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
    }
    void update(string t, Date d, Date e, Duree dur, bool p){
        setTitre(t); setDatesDisponibiliteEcheance(d,e); setDuree(dur);
        if(dur.getDureeEnHeures()>=12)
            setPreemp();
        else{
            if(p)setPreemp();
            else setNonPreemp();
        }
    }
};

/*! \class Composite
        \brief Classe d'ensembles de taches, héritant de Tache
        La classe possède un pointeur de Taches qui représente les composants de la tache composite.
*/
class Composite : public Tache {
private:
    Tache** composition;
    unsigned int nbCompo;
    unsigned int nbMaxCompo;
    Tache* trouverCompo(const string& id) const;
    Composite(const Composite& t);
    Composite& operator=(const Composite& t);
    Composite(string id, string t, Date d, Date e, Projet* p):Tache(id,t,d,e,p),composition(0),nbCompo(0),nbMaxCompo(0){}
    friend Composite& Projet::ajouterComposite(const string& t, const Date& dispo, const Date& deadline);
public:
    unsigned int getNbCompo(){return nbCompo; }
    virtual void addItem(Tache* t);
    void addCompo(Tache* t);
    Tache& getCompo(const string& id);
    const Tache& getCompo(const string& code) const;
    int getStatut(); //! Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée
    bool isPrecedencePotentielle( const string& id );
    void update(string id, string t, Date d, Date e){
        setId(id); setTitre(t); setDatesDisponibiliteEcheance(d,e);
    }
    void update(string t, Date d, Date e){
        setTitre(t); setDatesDisponibiliteEcheance(d,e);
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
    CompoIterator getCompoIterator(){ return CompoIterator(composition,nbCompo); }
    virtual void afficher(ostream& f) ;
};

/*! \class Activite
        \brief Classe d'evenement basique programmable, héritant d'Evenement
        La classe possède un titre et un lieu.
*/
class Activite : public Evenement {
private:
    string titre;
    string lieu;
    //Duree duree;
public:
    Activite(const string& t, const string& l, Duree dur): Evenement(dur), titre(t), lieu(l) {}
    string getTitre() const { return titre; }
    string getLieu() const { return lieu; }
    void setTitre(const string& t) { titre=t; }
    void setLieu(const string& l) { lieu=l; }
    //Duree getDuree() const { return duree; }
    void update(string t, string l, Duree d){
        setTitre(t); setLieu(l); setDuree(d);
    }
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
    Programmation** programmations;
    unsigned int nb;
    unsigned int nbMax;
    void addItem(Programmation* t);
    Programmation* trouverProgrammation(Evenement& e) const;
    Programmation* trouverProgrammation(const Evenement& e) const;
    ProgrammationManager():programmations(0),nb(0),nbMax(0){}
    ~ProgrammationManager();
    ProgrammationManager(const ProgrammationManager& um);
    ProgrammationManager& operator=(const ProgrammationManager& um);
    struct Handler{
        ProgrammationManager* instance;
        Handler():instance(0){}
        // destructeur appelé à la fin du programme
        ~Handler(){ if (instance) delete instance; }
    };
    static Handler handler;
public:
    static ProgrammationManager& getInstance();
    static void libererInstance();
    void ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur);
    void ajouterProgrammation(Activite& e, const Date& d, const Horaire& h, const Duree& dur);
    void  save(const string& f);
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
    Iterator getIterator(){ return Iterator(programmations,nb); }
signals:
    void programmationsChanged();
};

/*! \class Programmation
        \brief Classe donnant une date, un horaire et une durée à une Evenement, classe composant ProgrammationManager
*/
class Programmation {
    const Evenement* evt;
    Date date;
    Horaire horaire;
    Duree duree;
    Programmation(const Programmation& um);
    Programmation& operator=(const Programmation& um);
    Programmation(const Evenement& e, const Date& d, const Horaire& h, const Duree& dur):evt(&e), date(d), horaire(h), duree(dur){}
    friend void ProgrammationManager::ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur);
    friend void ProgrammationManager::ajouterProgrammation(Activite& e, const Date& d, const Horaire& h, const Duree& dur);
public:
    const Evenement& getEvenement() const { return *evt; }
    Date getDate() const { return date; }
    Horaire getHoraire() const { return horaire; }
    Duree getDuree() const { return duree; }
};

ostream& operator<<(ostream& f, const Tache& t);
ostream& operator<<(ostream& f, const Programmation& p);

#endif
