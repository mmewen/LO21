#include "Calendar.h"
#include <fstream>



ostream& operator<<(ostream& f, const Programmation& p);

//TACHE
ostream& operator<<(ostream& fout, Tache& t){
    t.afficher(fout);
    return fout;
}

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

//UNITAIRE
void Unitaire::afficher(ostream& f) {
    f<<"unitaire \n";
    f<<"id"<<getId()<<"\n";
    f<<"titre"<<getTitre()<<"\n";
    f<<"date dispo"<<getDateDisponibilite()<<"\n";
    f<<"date echeance"<<getDateEcheance()<<"\n";
    f<<"duree"<<getDuree()<<"\n";
    f<<"preemptive"<<isPreemp()<<"\n";
    f<<"precedence"<<"\n";
    Unitaire::Iterator it = getIterator();
    for(it.first(); !it.isDone(); it.next()){
        f<<it.current().getId()<<"\n";
    }
}

//COMPOSITE
void Composite::afficher(ostream& f) {
    f<<"composite \n";
    f<<"id"<<getId()<<"\n";
    f<<"titre"<<getTitre()<<"\n";
    f<<"date dispo"<<getDateDisponibilite()<<"\n";
    f<<"date echeance"<<getDateEcheance()<<"\n";
    f<<"precedence"<<"\n";
    Composite::Iterator it = getIterator();
    for(it.first(); !it.isDone(); it.next()){
        f<<it.current().getId()<<"\n";
    }
    f<<"composition"<<"\n";
    Composite::CompoIterator it2 = getCompoIterator();
    for(it2.first(); !it2.isDone(); it2.next()){
        f<<it2.current().getId()<<"\n";
    }
}

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
    // On ajoute la tache au tableau des taches du projet
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

    // On ajoute la tâche au tableau des taches qui n'ont pas de parents
    addItemNC(t);
}

void Projet::addItemNC(Tache* t){
    if (nbNC==nbMaxNC){
        Tache** newtab=new Tache*[nbMaxNC+10];
        for(unsigned int i=0; i<nbNC; i++) newtab[i]=tachesNonComposantes[i];
        nbMaxNC+=10;
        Tache** old=tachesNonComposantes;
        tachesNonComposantes=newtab;
        delete[] old;
    }

    tachesNonComposantes[nbNC++]=t;
}

Tache* Projet::trouverTache(const string& id)const{
    for(unsigned int i=0; i<nb; i++)
        if (id==taches[i]->getId()) return taches[i];
    return 0;
}

string Projet::genererId(){
    string chaine;
    char buffer[6];
    char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    do{
        for (int i = 0; i < 5; ++i) {
            buffer[i] = '0';
            buffer[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        buffer[5] = 0;
        chaine.assign(buffer, buffer + 5);
    } while (trouverTache(chaine));

    return chaine;
}

Unitaire& Projet::ajouterUnitaire(const string& t, const Date& dispo, const Date& deadline, const Duree& duree, const bool preemp){
    string id = genererId();
    Unitaire* newt=new Unitaire(dispo,deadline, id, t,duree,preemp);
    addItem(newt);
    return *newt;
}

Composite& Projet::ajouterComposite(const string& t, const Date& dispo, const Date& deadline){
    string id = genererId();
    Composite* newt=new Composite(id, t,dispo,deadline);
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

/*
Projet::Projet(const Projet& um):nb(um.nb),nbMax(um.nbMax), taches(new Tache*[um.nb]){
    for(unsigned int i=0; i<nb; i++) taches[i]=new Tache(*um.taches[i]);
}

Projet& Projet::operator=(const Projet& um){
    if (this==&um) return *this;
    this->~Projet();
    for(unsigned int i=0; i<um.nb; i++) addItem(new Tache(*um.taches[i]));
    return *this;
}
*/

void Projet::load(const string& f){
    if (file!=f) this->~Projet();
    file=f;
    ifstream fin(file.c_str()); // open file
    if (!fin) throw CalendarException("erreur, lors de l'ouverture du fichier de taches");
    char tmp[256];
    while (!fin.eof()&&fin.good()){
        fin.getline(tmp,256); // get code
        if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture identificateur tache");
        string type=tmp;
        if(type=="unitaire"){
            fin.getline(tmp,256); // get code
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture identificateur tache");
            string id=tmp;
            fin.getline(tmp,256); // get titre
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture titre tache");
            string titre=tmp;

            Date dispo;
            fin>>dispo;
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture date disponibilite");

            Date echeance;
            fin>>echeance;
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture date echeance");

            Duree duree;
            fin>>duree;
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture duree tache");

            bool preemp;
            fin>>preemp;
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture bool preemp");
            while (fin.peek()=='\r') fin.ignore();
            while (fin.peek()=='\n') fin.ignore();
            std::cout<<"LOAD unitaire"<<id<<"-"<<titre<<"-"<<dispo<<"-"<<echeance<<"-"<<duree<<"-"<<preemp<<"\n";
            ajouterUnitaire(titre,dispo,echeance,duree,preemp);
        }
        if(type=="composite"){
            fin.getline(tmp,256); // get code
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture identificateur tache");
            string id=tmp;
            fin.getline(tmp,256); // get titre
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture titre tache");
            string titre=tmp;

            Date dispo;
            fin>>dispo;
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture date disponibilite");

            Date echeance;
            fin>>echeance;
            if (fin.bad()) throw CalendarException("erreur, fichier taches, lecture date echeance");

            while (fin.peek()=='\r') fin.ignore();
            while (fin.peek()=='\n') fin.ignore();
            std::cout<<"LOAD composite"<<id<<"-"<<titre<<"-"<<dispo<<"-"<<echeance<<"\n";
            ajouterComposite(titre,dispo,echeance);
        }
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

void Projet::moveTacheTo(Tache* tMere, Tache* tFille){
    // On vérifie que la classe mère est bien de type composite
    if (typeid(*tMere) == typeid(Composite)){

        // On vérifie que la compo est possible
        if (!estComposante(tFille)){
            // Si c'est possible, on l'ajoute à la liste des compos
            dynamic_cast<Composite*>(tMere)->addCompo(tFille);
            deleteTacheNC(tFille);
        } else {
            throw CalendarException("Composition impossible car la tache fille est déjà composante");
        }
    } else {
        throw CalendarException("Composition impossible car la tache mère n'est pas composite");
    }
}

void Projet::deleteTacheNC(Tache* t){
    for(Iterator it = getNCIterator(); !it.isDone(); it.next()){
        if( it.current().getId() == t->getId() ){
            it.suppr();
            --nbNC;
            return;
        }
    }
    throw CalendarException("Suppression impossible : tache non trouvée");
}

bool Projet::estComposante(Tache* t){
    for(Iterator it = getNCIterator(); !it.isDone(); it.next()){
        if( it.current().getId() == t->getId() ){
            return false;
        }
    }
    return true;
}

void Projet::Iterator::suppr(){
    if(allowSuppr && (nb>0) && (nb != indice_tache)){
        tab[indice_tache] = tab[nb];
        --indice_tache;
        --nb;
    } else throw ("Suppression de tache impossible");
}

void Projet::update(const string& nom, const Date& d){
    this->nom = nom;
    this->dispo = d;
    cout<<"saved:"<<this->nom<<endl;
}




//PROJET MANAGER
ProjetManager::Handler ProjetManager::handler=ProjetManager::Handler();

ProjetManager& ProjetManager::getInstance(){
    if (handler.instance==0) handler.instance=new ProjetManager;
    return *(handler.instance);
}

void ProjetManager::libererInstance(){
    if (handler.instance!=0) delete handler.instance;
    handler.instance=0;
}

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

string ProjetManager::genererId(){
    string chaine;
    char buffer[6];
    char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    do{
        for (int i = 0; i < 5; ++i) {
            buffer[i] = '0';
            buffer[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        buffer[5] = 0;
        chaine.assign(buffer, buffer + 5);
    } while (trouverProjet(chaine));

    return chaine;
}

Projet& ProjetManager::ajouterProjet(const string& nom, const string& file, const Date& dispo){
    string id = genererId();
    Projet* newt=new Projet(id,nom,file,dispo);
    addItem(newt);
    return *newt;
}


//PROGRAMMATION MANAGER
ProgrammationManager::Handler ProgrammationManager::handler=ProgrammationManager::Handler();

ProgrammationManager& ProgrammationManager::getInstance(){
    if (handler.instance==0) handler.instance=new ProgrammationManager;
    return *(handler.instance);
}

void ProgrammationManager::libererInstance(){
    if (handler.instance!=0) delete handler.instance;
    handler.instance=0;
}

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

void ProgrammationManager::ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur){
    if (trouverProgrammation(e)) throw CalendarException("erreur, ProgrammationManager, Programmation deja existante");
    ProgrammationManager& um = ProgrammationManager::getInstance();
    ProgrammationManager::Iterator it = um.getIterator();
    for(it.first();!it.isDone();it.next()){
        Horaire horairefin = Horaire(it.current().getHoraire().getHeure()+it.current().getEvenement().getDuree().getDureeEnHeuresInt(),
                                     it.current().getHoraire().getMinute()+(it.current().getEvenement().getDuree().getDureeEnMinutes())%60);
        if(it.current().getDate()==d
                && it.current().getHoraire() < h
                && h < horairefin) // si les evenements sont le meme jours et que le nouveau commence avant la fin de l'ancien
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(it.current().getDate().demain()==d
                && horairefin < it.current().getHoraire()
                && h < horairefin) // si les evenements sont à un jour d'écart et que l'ancien se termine après ou pendant le nouveau
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        Horaire newhorairefin = Horaire(h.getHeure()+e.getDuree().getDureeEnHeuresInt(),
                                        h.getMinute()+(e.getDuree().getDureeEnMinutes())%60);
        if(it.current().getDate()==d
                && h < it.current().getHoraire()
                && it.current().getHoraire() < newhorairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(d.demain()==it.current().getDate()
                && newhorairefin < h
                && h < horairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
    }
    if(e.isPreemp()){
        if(e.getRestant().getDureeEnMinutes() < dur.getDureeEnMinutes()){
            e.setRestant(Duree(0));
            e.setFait(e.getDuree());
        }
        else{
            e.setRestant(Duree(e.getRestant().getDureeEnMinutes()-dur.getDureeEnMinutes()));
            e.setFait(Duree(e.getFait().getDureeEnMinutes()+dur.getDureeEnMinutes()));
        }
    }
    Programmation* newt=new Programmation(e,d,h);
    addItem(newt);
}

void ProgrammationManager::ajouterProgrammation(Activite& e, const Date& d, const Horaire& h){
    if (trouverProgrammation(e)) throw CalendarException("erreur, ProgrammationManager, Programmation deja existante");
    ProgrammationManager& um = ProgrammationManager::getInstance();
    ProgrammationManager::Iterator it = um.getIterator();
    for(it.first();!it.isDone();it.next()){
        Horaire horairefin = Horaire(it.current().getHoraire().getHeure()+it.current().getEvenement().getDuree().getDureeEnHeuresInt(),
                                     it.current().getHoraire().getMinute()+(it.current().getEvenement().getDuree().getDureeEnMinutes())%60);
        if(it.current().getDate()==d
                && it.current().getHoraire() < h
                && h < horairefin) // si les evenements sont le meme jours et que le nouveau commence avant la fin de l'ancien
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(it.current().getDate().demain()==d
                && horairefin < it.current().getHoraire()
                && h < horairefin) // si les evenements sont à un jour d'écart et que l'ancien se termine après ou pendant le nouveau
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        Horaire newhorairefin = Horaire(h.getHeure()+e.getDuree().getDureeEnHeuresInt(),
                                        h.getMinute()+(e.getDuree().getDureeEnMinutes())%60);
        if(it.current().getDate()==d
                && h < it.current().getHoraire()
                && it.current().getHoraire() < newhorairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(d.demain()==it.current().getDate()
                && newhorairefin < h
                && h < horairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
    }
    Programmation* newt=new Programmation(e,d,h);
    addItem(newt);
}

ProgrammationManager::~ProgrammationManager(){
    for(unsigned int i=0; i<nb; i++) delete programmations[i];
    delete[] programmations;
}

/*
ProgrammationManager::ProgrammationManager(const ProgrammationManager& um):nb(um.nb),nbMax(um.nbMax), programmations(new Programmation*[um.nb]){
    for(unsigned int i=0; i<nb; i++) programmations[i]=new Programmation(*um.programmations[i]);
}

ProgrammationManager& ProgrammationManager::operator=(const ProgrammationManager& um){
    if (this==&um) return *this;
    this->~ProgrammationManager();
    for(unsigned int i=0; i<um.nb; i++) addItem(new Programmation(*um.programmations[i]));
    return *this;
}
*/
