#include "Calendar.h"
#include <fstream>

ostream& operator<<(ostream& fout, const Tache& t){
    fout<<t.getId()<<"\n";
    fout<<t.getTitre()<<"\n";
    fout<<t.getDateDisponibilite()<<"\n";
    fout<<t.getDateEcheance()<<"\n";
    return fout;
}

ostream& operator<<(ostream& f, const Programmation& p);

//TACHE
void Tache::addItem(Tache* t){
    if (nbPred==maxPred){
        Tache** newtab=new Tache*[maxPred+10];
        for(unsigned int i=0; i<nbPred; i++) newtab[i]=prec[i];
        // ou memcpy(newtab,prec,nb*sizeof(Tache*));
        maxPred+=10;
        Tache** old=prec;
        prec=newtab;
        delete[] old;
    }
    prec[nbPred++]=t;
}

Tache* Tache::trouverTache(const string& id)const{
    for(unsigned int i=0; i<nbPred; i++)
        if (id==prec[i]->getId()) return prec[i];
    return 0;
}

Tache& Tache::getPrecedence(const string& id){
    Tache* t=trouverTache(id);
    if (!t) throw CalendarException("erreur, Projet, tache inexistante");
    return *t;
}

const Tache& Tache::getPrecedence(const string& id)const{
    return const_cast<Tache*>(this)->getPrecedence(id);
}

//COMPOSITE
void Composite::addCompo(Tache* t){
    if (nbCompo==nbMaxCompo){
        Tache** newtab=new Tache*[nbMaxCompo+10];
        for(unsigned int i=0; i<nbCompo; i++) newtab[i]=composition[i];
        // ou memcpy(newtab,taches,nb*sizeof(Tache*));
        nbMaxCompo+=10;
        Tache** old=composition;
        composition=newtab;
        delete[] old;
    }
    composition[nbCompo++]=t;
}

Tache* Composite::trouverCompo(const string& id)const{
    for(unsigned int i=0; i<nbCompo; i++)
        if (id==composition[i]->getId()) return composition[i];
    return 0;
}

Tache& Composite::getCompo(const string& id){
    Tache* t=trouverCompo(id);
    if (!t) throw CalendarException("erreur, Projet, tache inexistante");
    return *t;
}

const Tache& Composite::getCompo(const string& id)const{
    return const_cast<Composite*>(this)->getCompo(id);
}

//PROJET
Projet::~Projet(){
    if (file!="") save(file);
    for(unsigned int i=0; i<nb; i++) delete taches[i];
    delete[] taches;
    file="";
}

void Projet::addItem(Tache* t){
    if (nb==nbMax){
        Tache** newtab=new Tache*[nbMax+10];
        for(unsigned int i=0; i<nb; i++) newtab[i]=taches[i];
        // ou memcpy(newtab,taches,nb*sizeof(Tache*));
        nbMax+=10;
        Tache** old=taches;
        taches=newtab;
        delete[] old;
    }
    taches[nb++]=t;
}

Tache* Projet::trouverTache(const string& id)const{
    for(unsigned int i=0; i<nb; i++)
        if (id==taches[i]->getId()) return taches[i];
    return 0;
}

Tache& Projet::ajouterUnitaire(const string& id, const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool preemp){
    if (trouverTache(id)) throw CalendarException("erreur, Projet, tache deja existante");
    Unitaire* newt=new Unitaire(dispo,deadline,id,t,duree,preemp);
    addItem(newt);
    return *newt;
}

Tache& Projet::ajouterComposite(const string& id, const string& t, const Date& dispo, const Date& deadline){
    if (trouverTache(id)) throw CalendarException("erreur, Projet, tache deja existante");
    Composite* newt=new Composite(id,t,dispo,deadline);
    addItem(newt);
    return *newt;
}

Date Projet::getEcheance(){
    Iterator it = getIterator();
    Date echeance = it.current().getDateEcheance();
    for( ; !it.isDone() ; it.next()){
        if(echeance<it.current().getDateEcheance()){
            echeance = it.current().getDateEcheance();
        }
    }
    return echeance;
}

Tache& Projet::getTache(const string& id){
    Tache* t=trouverTache(id);
    if (!t) throw CalendarException("erreur, Projet, tache inexistante");
    return *t;
}

const Tache& Projet::getTache(const string& id)const{
    return const_cast<Projet*>(this)->getTache(id);
}

Projet::Projet(const Projet& um):nb(um.nb),nbMax(um.nbMax), taches(new Tache*[um.nb]){
    for(unsigned int i=0; i<nb; i++) taches[i]=new Tache(*um.taches[i]);
}

Projet& Projet::operator=(const Projet& um){
    if (this==&um) return *this;
    this->~Projet();
    for(unsigned int i=0; i<um.nb; i++) addItem(new Tache(*um.taches[i]));
    return *this;
}

void Projet::load(const string& f){
    if (file!=f) this->~Projet();
    file=f;
    ifstream fin(file.c_str()); // open file
    if (!fin) throw CalendarException("erreur, lors de l'ouverture du fichier de taches");
    char tmp[256];
    while (!fin.eof()&&fin.good()){
        fin.getline(tmp,256); // get code
        if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture identificateur tache");
        string id=tmp;
        fin.getline(tmp,256); // get titre
        if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture titre tache");
        string titre=tmp;

        Duree duree;
        fin>>duree;
        if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture duree tache");

        Date dispo;
        fin>>dispo;
        if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture date disponibilite");

        Date echeance;
        fin>>echeance;
        if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture date echeance");

        bool preemp;
        fin>>preemp;
        if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture bool preemp");
        while (fin.peek()=='\r') fin.ignore();
        while (fin.peek()=='\n') fin.ignore();
        std::cout<<"LOAD "<<id<<"-"<<titre<<"-"<<duree<<"-"<<dispo<<"-"<<echeance<<"\n";
        ajouterUnitaire(id,titre,dispo,echeance,duree,preemp);
    }
    fin.close(); // close file
}

void  Projet::save(const string& f){
    file=f;
    ofstream fout(f.c_str(),ofstream::trunc); // toutes les taches existantes sont écrasées
    for(unsigned int i=0; i<nb; i++){
        fout<<*taches[i];
    }
    fout.close();
}

//PROJET MANAGER
void ProjetManager::addItem(Projet* t){
    if (nb==nbMax){
        Projet** newtab=new Projet*[nbMax+10];
        for(unsigned int i=0; i<nb; i++) newtab[i]=projets[i];
        // ou memcpy(newtab,taches,nb*sizeof(Tache*));
        nbMax+=10;
        Projet** old=projets;
        projets=newtab;
        delete[] old;
    }
    projets[nb++]=t;
}

Projet* ProjetManager::trouverProjet(const string& id)const{
    for(unsigned int i=0; i<nb; i++)
        if (id==projets[i]->getId()) return projets[i];
    return 0;
}

Projet& ProjetManager::getProjet(const string& id){
    Projet* t=trouverProjet(id);
    if (!t) throw CalendarException("erreur, ProjetManager, projet inexistant");
    return *t;
}

Projet& ProjetManager::ajouterProjet(const string& id, const string& nom, const string& file, const Date& dispo){
    if (trouverProjet(id)) throw CalendarException("erreur, ProjetMangager, projet deja existant");
    Projet* newt=new Projet(id,nom,file,dispo);
    addItem(newt);
    return *newt;
}


//PROGRAMMATION MANAGER
void ProgrammationManager::addItem(Programmation* t){
    if (nb==nbMax){
        Programmation** newtab=new Programmation*[nbMax+10];
        for(unsigned int i=0; i<nb; i++) newtab[i]=programmations[i];
        // ou memcpy(newtab,Programmations,nb*sizeof(Programmation*));
        nbMax+=10;
        Programmation** old=programmations;
        programmations=newtab;
        delete[] old;
    }
    programmations[nb++]=t;
}

Programmation* ProgrammationManager::trouverProgrammation(const Evenement& e)const{
    for(unsigned int i=0; i<nb; i++)
        if (&e==&programmations[i]->getEvenement()) return programmations[i];
    return 0;
}

void ProgrammationManager::ajouterProgrammation(const Evenement& e, const Date& d, const Horaire& h){
    if (trouverProgrammation(e)) throw CalendarException("erreur, ProgrammationManager, Programmation deja existante");
    Programmation* newt=new Programmation(e,d,h);
    addItem(newt);
}

ProgrammationManager::~ProgrammationManager(){
    for(unsigned int i=0; i<nb; i++) delete programmations[i];
    delete[] programmations;
}

ProgrammationManager::ProgrammationManager(const ProgrammationManager& um):nb(um.nb),nbMax(um.nbMax), programmations(new Programmation*[um.nb]){
    for(unsigned int i=0; i<nb; i++) programmations[i]=new Programmation(*um.programmations[i]);
}

ProgrammationManager& ProgrammationManager::operator=(const ProgrammationManager& um){
    if (this==&um) return *this;
    this->~ProgrammationManager();
    for(unsigned int i=0; i<um.nb; i++) addItem(new Programmation(*um.programmations[i]));
    return *this;
}
