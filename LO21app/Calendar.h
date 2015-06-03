#ifndef CALENDAR_h
#define CALENDAR_h
#include<string>
#include<iostream>
#include "timing.h"
using namespace std;
using namespace TIME;


class CalendarException{
public:
    CalendarException(const string& message):info(message){}
    string getInfo() const { return info; }
private:
    string info;
};

class Evenement {
    Duree duree;
public:
    Evenement(Duree dur):duree(dur){}
    Duree getDuree() const { return duree; }
};

class Tache {
    Tache** prec;
    unsigned int nbPred;
    unsigned int maxPred;
    string identificateur;
    string titre;
    Date disponibilite;
    Date echeance;
    Tache* trouverTache(const string& id) const;
public:
    Tache(const string& id, const string& t, const Date& dispo, const Date& deadline):
            prec(0),identificateur(id),titre(t),disponibilite(dispo),echeance(deadline){}
    void addItem(Tache* t);
    Tache& getPrecedence(const string& id);
    const Tache& getPrecedence(const string& code) const;
    string getId() const { return identificateur; }
    string getTitre() const { return titre; }
    Date getDateDisponibilite() const {  return disponibilite; }
    Date getDateEcheance() const {  return echeance; }
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
};

class Unitaire : public Tache, public Evenement {
    //Duree duree; //si non-preemtable -> duree<=12h
    Duree dureeRestante;
    Duree dureeFaite;
    bool preemptable;
public:
    Unitaire(Date d, Date e, string id, string t, Duree dur, bool p):Tache(id,t,d,e),Evenement(dur),dureeRestante(dur),dureeFaite(0),preemptable(p){
        if(dur.getDureeEnHeures()>=12){
            this->preemptable=false;
        }
    }
    //Duree getDuree() const { return duree; }
    bool isPreemp() const { return preemptable; }
    Duree getFait() const { return dureeFaite; }
    Duree getRestant() const;
};

class Composite : public Tache {
private:
    Tache** composition;
    unsigned int nbCompo;
    unsigned int nbMaxCompo;
    Tache* trouverCompo(const string& id) const;
public:
    Composite(string id, string t, Date d, Date e):Tache(id,t,d,e),composition(0),nbCompo(0),nbMaxCompo(0){}
    void addCompo(Tache* t);
    Tache& getCompo(const string& id);
    const Tache& getCompo(const string& code) const;
    friend class iterator;
    class iterator{
    private:
        Tache** tab;
        int nb;
        int indice_tache;
        friend class Composite;
        iterator(Tache** t, int n):tab(t), nb(n), indice_tache(0){}
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
    iterator getiterator(){ return iterator(composition,nbCompo); }
};

class Activite : public Evenement {
private:
    string titre;
    string lieu;
    //Duree duree;
public:
    Activite(const string& t, const string& l, Duree dur): titre(t), lieu(l), Evenement(dur) {}
    string getTitre() const { return titre; }
    string getLieu() const { return lieu; }
    //Duree getDuree() const { return duree; }
};

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
    Tache* trouverTache(const string& id) const;
public:
    Projet(const string& id, const string& nom, const string& file, const Date& d):identificateur(id),nom(nom),file(file),dispo(d),taches(0),nb(0),nbMax(0){}
    ~Projet();
    Projet(const Projet& um);
    Projet& operator=(const Projet& um);
    Tache& ajouterUnitaire(const string& id, const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool premp);
    Tache& ajouterComposite(const string& id, const string& t, const Date& dispo, const Date& deadline);
    Tache& getTache(const string& id);
    string getId() const { return identificateur; }
    string getNom() const { return nom; }
    string getFile() const { return file; }
    Date getDispo() const { return dispo; }
    Date getEcheance();
    void load(const string& f);
    void save(const string& f);
    const Tache& getTache(const string& code) const;
    class Iterator{
        friend class Projet;
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
    Iterator getIterator(){ return Iterator(taches,nb); }
};

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
public:
    Projet& ajouterProjet(const string& id, const string& nom, const string& file, const Date& dispo);
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

class Programmation {
    const Evenement* evt;
    Date date;
    Horaire horaire;
public:
    Programmation(const Evenement& e, const Date& d, const Horaire& h):evt(&e), date(d), horaire(h){}
    const Evenement& getEvenement() const { return *evt; }
    Date getDate() const { return date; }
    Horaire getHoraire() const { return horaire; }
};

class ProgrammationManager {
private:
    Programmation** programmations;
    unsigned int nb;
    unsigned int nbMax;
    void addItem(Programmation* t);
    Programmation* trouverProgrammation(const Evenement& e) const;
    ProgrammationManager();
    ~ProgrammationManager();
    ProgrammationManager(const ProgrammationManager& um);
    ProgrammationManager& operator=(const ProgrammationManager& um);
public:
    void ajouterProgrammation(const Evenement& e, const Date& d, const Horaire& h);
};

ostream& operator<<(ostream& f, const Tache& t);
ostream& operator<<(ostream& f, const Programmation& p);

#endif
