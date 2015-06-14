#include "Calendar.h"
#include <fstream>



//TACHE
ostream& operator<<(ostream& fout, Tache& t){
    t.afficher(fout);
    return fout;
}

void Tache::addItem(Tache* t){
    if(getId()==t->getId())
        throw CalendarException("erreur, Tache, precedence reflexive");
    if(t->trouverTache(getId()))
        throw CalendarException("erreur, Tache, precedence recursive");
    if(getDateEcheance() < t->getDateDisponibilite())
        throw CalendarException("erreur, Tache, dates de précédence non correspondantes");
    if(trouverTache(t->getId()))
        throw CalendarException("erreur, Tache, précédence déjà existante");
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
    if (!t)
        throw CalendarException("erreur, Projet, tache inexistante");
    return *t;
}

const Tache& Tache::getPrecedence(const string& id)const{
    return const_cast<Tache*>(this)->getPrecedence(id);
}

bool Tache::isPrecedencePotentielle( const string& id ) {
    // Si la tâche n'existe pas, ce qui serait étrange
    if ( projetParent->trouverTache(id) == 0  ){
        throw CalendarException("Erreur étrange numéro XDFGVCF58214586");
        return false;
    }

    // Si c'est la tâche actuelle
    if ( id == identificateur )
        return false;

    // Si c'est déjà une tâche prédécesseur
    if(this->trouverTache(id))
        return false;

    // Si la tâche commence trop tard
    if(this->getDateEcheance() < projetParent->trouverTache(id)->getDateDisponibilite())
        return false;

    // Si la tâche est successeur
    if(projetParent->trouverTache(id)->trouverTache(getId()))
        return false;



    return true;
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
    f<<getNbPred()<<"\n";
    Unitaire::Iterator it = getIterator();
    for(it.first(); !it.isDone(); it.next()){
        f<<it.current().getId()<<"\n";
    }
}

int Unitaire::getStatut(){
    // Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée

    if (getDateEcheance() < Date::toTimingDate(QDate::currentDate())) // si la deadline est passée
        return 2;

    if ( getRestant().isNull() ){
        return 2;
    }
    if ( getFait().isNull() ){
        return 0;
    }

    return 1;
}

int Unitaire::isFinished(const Date& d, const Horaire& h){
    // Retourne le statut de la tache par rapport à la date d : 0 = pas encore faite, 1 = terminée
    if ( !getRestant().isNull() ){
        return 0;
    }
    ProgrammationManager& pm = ProgrammationManager::getInstance();
    if(!pm.trouverProgrammation(dynamic_cast<Evenement&>(*this)))
        return 0;
    if(d<pm.trouverProgrammation(dynamic_cast<Evenement&>(*this))->getDate()
            || (d==pm.trouverProgrammation(dynamic_cast<Evenement&>(*this))->getDate()
                && h<Horaire(pm.trouverProgrammation(dynamic_cast<Evenement&>(*this))->getHoraire().getHeure(),
                           pm.trouverProgrammation(dynamic_cast<Evenement&>(*this))->getHoraire().getMinute()
                           + pm.trouverProgrammation(dynamic_cast<Evenement&>(*this))->getDuree().getDureeEnMinutes())))
        return 0;
    return 1;
}


//COMPOSITE
void Composite::afficher(ostream& f) {
    f<<"composite \n";
    f<<"id"<<getId()<<"\n";
    f<<"titre"<<getTitre()<<"\n";
    f<<"date dispo"<<getDateDisponibilite()<<"\n";
    f<<"date echeance"<<getDateEcheance()<<"\n";
    f<<"precedence"<<"\n";
    f<<getNbPred()<<"\n";
    Composite::Iterator it = getIterator();
    for(it.first(); !it.isDone(); it.next()){
        f<<it.current().getId()<<"\n";
    }
    f<<"composition"<<"\n";
    f<<getNbCompo()<<"\n";
    Composite::CompoIterator it2 = getCompoIterator();
    for(it2.first(); !it2.isDone(); it2.next()){
        f<<it2.current().getId()<<"\n";
    }
}

void Composite::addItem(Tache* t){
    Tache::addItem(t);
}

void Composite::addCompo(Tache* t){
    if(getId()==t->getId())
        throw CalendarException("erreur, Composite, composition reflexive");
    if(trouverCompo(t->getId()))
        throw CalendarException("erreur, Composite, tache ajoutée déjà composante");
    if(t->getDateDisponibilite() < getDateDisponibilite())
        throw CalendarException("erreur, Composite, disponibilite de composition non correspondante");
    if(getDateEcheance() < t->getDateEcheance())
        throw CalendarException("erreur, Composite, echeance de composition non correspondante");
    if (nbCompo==nbMaxCompo){
        Tache** newtab=new Tache*[nbMaxCompo+10];
        for(unsigned int i=0; i<nbCompo; i++) newtab[i]=composition[i];
        // ou memcpy(newtab,taches,nb*sizeof(Tache*));
        nbMaxCompo+=10;
        Tache** old=composition;
        composition=newtab;
        delete[] old;
    }

    Tache* prec = 0;
    Tache::Iterator it = getIterator();
    for(it.first();!it.isDone();it.next()){
        prec = &(it.current());
        t->addItem(prec);
    }
    composition[nbCompo++]=t;
    this->addItem(t);
}

Tache* Composite::trouverCompo(const string& id)const{
    for(unsigned int i=0; i<nbCompo; i++)
        if (id==composition[i]->getId()) return composition[i];
    return 0;
}

Tache& Composite::getCompo(const string& id){
    Tache* t=trouverCompo(id);
    if (!t)
        throw CalendarException("erreur, Projet, tache inexistante");
    return *t;
}

const Tache& Composite::getCompo(const string& id)const{
    return const_cast<Composite*>(this)->getCompo(id);
}

bool Composite::isPrecedencePotentielle( const string& id ) {
    if (!Tache::isPrecedencePotentielle( id ))
        return false;

    // Si c'est une des tâches filles
    for( CompoIterator it = getCompoIterator() ; !it.isDone() ; it.next() ){
        if ( it.current().getId() == id ){
            return false;
        }

        if (!it.current().isPrecedencePotentielle( id ))
            return false;
    }
    cout<<endl;

    return true;
}


int Composite::getStatut(){
    // Retourne le statut de la tache : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée

    if (getDateEcheance() < Date::toTimingDate(QDate::currentDate())) // si la deadline est passée
        return 2;

    if (getNbCompo() != 0) {
        int statutTemp;
        bool fini = true, commence = false;
        for( Composite::CompoIterator it = getCompoIterator() ; !it.isDone() ;it.next() ){
            statutTemp = it.current().getStatut();

            if (statutTemp != 2){
                fini = false;
            }

            if (statutTemp != 0){
                commence = true;
            }
        }

        if ( commence && !fini){
            return 1; // en cours
        }

        if ( !commence ){
            return 0; // pas commencé
        }

        if ( fini ){
            return 2; // fini
        }

    } else {
        return 0;
    }
    return -1;
}

int Composite::isFinished(const Date& d, const Horaire& h){
    // Retourne le statut de la tache par rapport à la date d : 0 = pas encore faite, 1 = terminée
    if (getNbCompo() != 0) {
        int statutTemp;
        bool fini = true;
        for( Composite::CompoIterator it = getCompoIterator() ; !it.isDone() ;it.next() ){
            statutTemp = it.current().isFinished(d,h);
            if (statutTemp != 1){
                fini = false;
            }
        }
        if ( fini ){
            return 1; // fini
        }
        else
            return 0; // pas fini

    } else {
        return 1;
    }
    return -1;
}


//ACTIVITE
void Activite::afficher(ostream& f) {
    f<<"activite \n";
    f<<"nom "<<getNom().toStdString()<<"\n";
    f<<"duree "<<getDuree()<<"\n";
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
    if(dispo < getDispo())
        throw CalendarException("erreur, Projet, disponibilite de tache precede disponibilité de projet");
    string id = genererId();
    Unitaire* newt=new Unitaire(dispo,deadline, id, t,duree,preemp, this);
    addItem(newt);
    return *newt;
}

Composite& Projet::ajouterComposite(const string& t, const Date& dispo, const Date& deadline){
    if(dispo < getDispo())
        throw CalendarException("erreur, Projet, disponibilite de tache precede disponibilité de projet");
    string id = genererId();
    Composite* newt=new Composite(id, t,dispo,deadline, this);
    addItem(newt);
    return *newt;
}

Date Projet::getEcheance(){
    Date echeance;
    if (getNbTaches() > 0 ){
        Iterator it = getIterator();
        echeance = it.current().getDateEcheance();
        for(it.first() ; !it.isDone() ; it.next()){
            if(echeance<it.current().getDateEcheance()){
                echeance = it.current().getDateEcheance();
            }
        }
    } else {
        echeance = Date(1,1,3000);
    }
    return echeance;
}

Tache& Projet::getTache(const string& id){
    Tache* t=trouverTache(id);
    if (!t)
        throw CalendarException("erreur, Projet, tache inexistante");
    return *t;
}

const Tache& Projet::getTache(const string& id)const{
    return const_cast<Projet*>(this)->getTache(id);
}

void  Projet::save(const string& f){
    file=f;
    QFile newfile( QString::fromStdString(file));
    if (!newfile.open(QIODevice::WriteOnly | QIODevice::Text))
        throw CalendarException("erreur sauvegarde tâches : ouverture fichier xml");
    QXmlStreamWriter stream(&newfile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("Projet");
    stream.writeTextElement("identificateur",QString::fromStdString(getId()));
    stream.writeTextElement("titre",QString::fromStdString(getNom()));
    stream.writeTextElement("disponibilite",getDispo().getQDate().toString(Qt::ISODate));
    stream.writeStartElement("taches");
    Projet::Iterator it = getIterator();
    for(it.first();!it.isDone();it.next()){
        if(typeid(it.current())==typeid(Unitaire&)){
            stream.writeStartElement("tache_unitaire");
            stream.writeAttribute("preemptive", (dynamic_cast<Unitaire&>(it.current()).isPreemp())?"true":"false");
            stream.writeTextElement("identificateur",QString::fromStdString(it.current().getId()));
            stream.writeTextElement("titre",QString::fromStdString(it.current().getTitre()));
            stream.writeTextElement("disponibilite",it.current().getDateDisponibilite().getQDate().toString(Qt::ISODate));
            stream.writeTextElement("echeance",it.current().getDateEcheance().getQDate().toString(Qt::ISODate));
            QString str;
            str.setNum(dynamic_cast<Unitaire&>(it.current()).getDuree().getDureeEnMinutes());
            stream.writeTextElement("duree",str);
            stream.writeStartElement("precedences");
            Tache::Iterator it2 = it.current().getIterator();
            for(it2.first();!it2.isDone();it2.next()){
                stream.writeStartElement("tache");
                stream.writeTextElement("identificateur",QString::fromStdString(it2.current().getId()));
                stream.writeEndElement();
            }
            stream.writeEndElement();
            stream.writeEndElement();
        }
        else
        if(typeid(it.current())==typeid(Composite&)){
            stream.writeStartElement("tache_composite");
            stream.writeTextElement("identificateur",QString::fromStdString(it.current().getId()));
            stream.writeTextElement("titre",QString::fromStdString(it.current().getTitre()));
            stream.writeTextElement("disponibilite",it.current().getDateDisponibilite().getQDate().toString(Qt::ISODate));
            stream.writeTextElement("echeance",it.current().getDateEcheance().getQDate().toString(Qt::ISODate));
            stream.writeStartElement("precedences");
            Tache::Iterator it2 = it.current().getIterator();
            for(it2.first();!it2.isDone();it2.next()){
                stream.writeStartElement("tache");
                stream.writeTextElement("identificateur",QString::fromStdString(it2.current().getId()));
                stream.writeEndElement();
            }
            stream.writeEndElement();
            stream.writeStartElement("composition");
            Composite::CompoIterator it3 = dynamic_cast<Composite&>(it.current()).getCompoIterator();
            for(it3.first();!it3.isDone();it3.next()){
                stream.writeStartElement("tache");
                stream.writeTextElement("identificateur",QString::fromStdString(it3.current().getId()));
                stream.writeEndElement();
            }
            stream.writeEndElement();
            stream.writeEndElement();
        }
    }
    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndDocument();
    newfile.close();
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
    Projet::Iterator it = getIterator();
    for(it.first();!it.isDone();it.next()){
        if(it.current().getDateDisponibilite() < d)
            throw CalendarException("erreur, Projet, date de disponibilité > dates disponibilité Taches");
    }
    this->dispo = d;
    cout<<"saved:"<<this->nom<<endl;
}

int Projet::getStatut(){
    // Retourne le statut du projet : 0 = rien n'est fait, 1 = en cours, 2 = terminé/deadline passée

    if (getEcheance() < Date::toTimingDate(QDate::currentDate())) // si la deadline est passée
        return 2;

    if (getNbTaches() != 0) {
        int statutTemp;
        bool fini = true, commence = false;
        for( Projet::Iterator it = getIterator() ; !it.isDone() ;it.next() ){
            statutTemp = it.current().getStatut();

            if (statutTemp != 2){
                fini = false;
            }

            if (statutTemp != 0){
                commence = true;
            }
        }

        if ( commence && !fini){
            return 1; // en cours
        }

        if ( !commence ){
            return 0; // pas commencé
        }

        if ( fini ){
            return 2; // fini
        }


    } else {
        return 0;
    }
    return -1;
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
    emit programmationsChanged();
}

Programmation* ProgrammationManager::trouverProgrammation(Evenement& e)const{
    for(unsigned int i=0; i<nb; i++)
        if (&e==&programmations[i]->getEvenement()) return programmations[i];
    return 0;
}

Programmation* ProgrammationManager::trouverProgrammation(const Evenement& e)const{
    for(unsigned int i=0; i<nb; i++)
        if (&e==&programmations[i]->getEvenement()) return programmations[i];
    return 0;
}

void ProgrammationManager::ajouterProgrammation(Unitaire& e, const Date& d, const Horaire& h, Duree dur){
    if(dur.getDureeEnMinutes()==0)
        throw CalendarException("erreur, ProgrammationManager, programmation d'une durée nulle");
    if(e.getRestant().getDureeEnMinutes()<dur.getDureeEnMinutes())
        throw CalendarException("erreur, ProgrammationManager, programmation d'une durée trop grande");
    if(12*60<dur.getDureeEnMinutes())
        throw CalendarException("erreur, ProgrammationManager, programmation d'une durée supérieure à 12 heures. Faites des pauses !");
    if(!e.isPreemp())
        if (trouverProgrammation(e))
            throw CalendarException("erreur, ProgrammationManager, Programmation deja existante");
    ProgrammationManager& um = ProgrammationManager::getInstance();
    ProgrammationManager::Iterator it = um.getIterator();
    for(it.first();!it.isDone();it.next()){
        Horaire horairefin = Horaire(it.current().getHoraire().getHeure(),
                                     it.current().getHoraire().getMinute()+it.current().getDuree().getDureeEnMinutes());
        if(it.current().getDate()==d
                && it.current().getHoraire() < h
                && h < horairefin) // si les evenements sont le meme jours et que le nouveau commence avant la fin de l'ancien
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(it.current().getDate().demain()==d
                && horairefin < it.current().getHoraire()
                && h < horairefin) // si les evenements sont à un jour d'écart et que l'ancien se termine après ou pendant le nouveau
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        Horaire newhorairefin = Horaire(h.getHeure(), h.getMinute()+dur.getDureeEnMinutes());
        if(it.current().getDate()==d
                && h < it.current().getHoraire()
                && it.current().getHoraire() < newhorairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(d.demain()==it.current().getDate()
                && newhorairefin < h
                && h < horairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
    }
    if(d < e.getDateDisponibilite())
        throw CalendarException("erreur, ProgrammationManager, programmation avant disponibilite");
    if(e.getDateEcheance() < d)
        throw CalendarException("erreur, ProgrammationManager, programmation après echeance");
    Tache::Iterator it2 = e.getIterator();
    for(it2.first();!it2.isDone();it2.next()){
        if(!it2.current().isFinished(d,h))
            throw CalendarException("erreur, ProgrammationManager, précédence non respectée");
    }
    e.setFait(Duree(e.getFait().getDureeEnMinutes()+dur.getDureeEnMinutes()));
    Programmation* newt=new Programmation(e,d,h,dur);
    addItem(newt);
}

void ProgrammationManager::ajouterProgrammation(Activite& e, const Date& d, const Horaire& h, const Duree& dur){
    if(dur.getDureeEnMinutes()==0)
        throw CalendarException("erreur, ProgrammationManager, programmation d'une durée nulle");
    if(12*60<dur.getDureeEnMinutes())
        throw CalendarException("erreur, ProgrammationManager, programmation d'une durée supérieure à 12 heures. Faites des pauses !");
    if (trouverProgrammation(e)) throw CalendarException("erreur, ProgrammationManager, Programmation deja existante");
    ProgrammationManager& um = ProgrammationManager::getInstance();
    ProgrammationManager::Iterator it = um.getIterator();
    for(it.first();!it.isDone();it.next()){
        Horaire horairefin = Horaire(it.current().getHoraire().getHeure(),
                                     it.current().getHoraire().getMinute()+it.current().getDuree().getDureeEnMinutes());
        if(it.current().getDate()==d
                && it.current().getHoraire() < h
                && h < horairefin) // si les evenements sont le meme jours et que le nouveau commence avant la fin de l'ancien
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(it.current().getDate().demain()==d
                && horairefin < it.current().getHoraire()
                && h < horairefin) // si les evenements sont à un jour d'écart et que l'ancien se termine après ou pendant le nouveau
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        Horaire newhorairefin = Horaire(h.getHeure(), h.getMinute()+dur.getDureeEnMinutes());
        if(it.current().getDate()==d
                && h < it.current().getHoraire()
                && it.current().getHoraire() < newhorairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
        if(d.demain()==it.current().getDate()
                && newhorairefin < h
                && h < horairefin)
            throw CalendarException("erreur, ProgrammationManager, horaires chevauchant");
    }
    Programmation* newt=new Programmation(e,d,h,dur);
    addItem(newt);
}

ProgrammationManager::~ProgrammationManager(){
    for(unsigned int i=0; i<nb; i++) delete programmations[i];
    delete[] programmations;
}


QString Evenement::getNom() const{
    if ( typeid(*this) == typeid(Unitaire) ){
        return QString::fromStdString( dynamic_cast<const Unitaire*>(this)->getTitre());
    } else if ( typeid(*this) == typeid(Activite) ){
        return QString::fromStdString( dynamic_cast<const Activite*>(this)->getTitre() + "\n" + dynamic_cast<const Activite*>(this)->getLieu());
    } else {
        throw CalendarException("Erreur de type d'évènement...");
    }
}
