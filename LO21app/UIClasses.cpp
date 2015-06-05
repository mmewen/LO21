#include "UIClasses.h"

TreeViewModel::Handler TreeViewModel::handler=TreeViewModel::Handler();

TreeViewModel& TreeViewModel::getInstance(){
    if (handler.instance==0) handler.instance=new TreeViewModel();
    return *(handler.instance);
}

void TreeViewModel::libererInstance(){
    if (handler.instance!=0) delete handler.instance;
    handler.instance=0;
}

TreeViewModel::~TreeViewModel(){
    modele.clear();
}

void TreeViewModel::addProjetItem(QStandardItem* item, Projet* projet){
    if (tipNb==tipNbMax){
        CoupleItemProjet* newtab = new CoupleItemProjet[tipNbMax+10];
        for(unsigned int i=0; i<tipNb; i++) newtab[i]=tabItemsProjets[i];
        tipNbMax+=10;
        CoupleItemProjet* old = tabItemsProjets;
        tabItemsProjets=newtab;
        delete[] old;
    }

    tabItemsProjets[tipNb].projet=projet;
    tabItemsProjets[tipNb++].item=item;
}

void TreeViewModel::addTacheItem(QStandardItem* item, Tache* tache){
    if (titNb==titNbMax){
        CoupleItemTache* newtab = new CoupleItemTache[titNbMax+10];
        for(unsigned int i=0; i<titNb; i++) newtab[i]=tabItemsTaches[i];
        titNbMax+=10;
        CoupleItemTache* old = tabItemsTaches;
        tabItemsTaches=newtab;
        delete[] old;
    }

    tabItemsTaches[titNb].tache=tache;
    tabItemsTaches[titNb++].item=item;
}

void TreeViewModel::printTree(){
    ProjetManager& pjm = ProjetManager::getInstance();
    QStandardItem *parentItem = modele.invisibleRootItem();

    // Affiche les projets
    for(ProjetManager::Iterator it = pjm.getIterator(); !it.isDone(); it.next()){

        // On ajoute la ligne
        QStandardItem* itemTemp = new QStandardItem(QString::fromStdString(it.current().getNom()));
        itemTemp->setEditable(false);
        parentItem->appendRow(itemTemp);

        // On ajoute le couple (item, projet) au tableau qui va bien
        addProjetItem(itemTemp, &(it.current()));

        for(Projet::Iterator pit = it.current().getNCIterator(); !pit.isDone(); pit.next()){
            // Pour chaque projet, on affiche les taches sans mère et leurs filles
            printBranch(itemTemp, &pit.current());
        }

    }

}

void TreeViewModel::printBranch(QStandardItem* parentItem, Tache* tache){

    // On ajoute l'item
    QStandardItem* tacheItemTemp = new QStandardItem(QString::fromStdString(tache->getTitre()));
    tacheItemTemp->setEditable(false);
    parentItem->appendRow(tacheItemTemp);

    // On ajoute le couple (item, tache) au tableau qui va bien aussi
    addTacheItem(tacheItemTemp, tache);

    // On fait la même pour toutes les tâches filles
    if (typeid(*tache) == typeid(Composite)){
        for(Composite::CompoIterator tit = dynamic_cast<Composite*>(tache)->getCompoIterator(); !tit.isDone(); tit.next()){
            printBranch(tacheItemTemp, &tit.current());
        }
    }
}


Tache* TreeViewModel::getTacheFromItem(QStandardItem* item){
    for(int i=0; i<titNb; i++){
        if (tabItemsTaches[i].item == item){
            return tabItemsTaches[i].tache;
        }
    }
    throw CalendarException("Pas de tache trouvée en lien avec cet item");
}

Projet* TreeViewModel::getProjetFromItem(QStandardItem* item){
    for(int i=0; i<tipNb; i++){
        if (tabItemsProjets[i].item == item){
            return tabItemsProjets[i].projet;
        }
    }
    throw CalendarException("Pas de projet trouvé en lien avec cet item");
}

QStandardItem* TreeViewModel::getItemFromTache(Tache* tache){
    for(int i=0; i<titNb; i++){
        if (tabItemsTaches[i].tache == tache){
            return tabItemsTaches[i].item;
        }
    }
    throw CalendarException("Pas d'item trouvé en lien avec cette tache");
}

QStandardItem* TreeViewModel::getItemFromProjet(Projet* projet){
    for(int i=0; i<tipNb; i++){
        if (tabItemsProjets[i].projet == projet){
            return tabItemsProjets[i].item;
        }
    }
    throw CalendarException("Pas d'item trouvé en lien avec cette tache");
}
