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

    // On fait la même pour toutes les tâches filles
    if (typeid(*tache) == typeid(Composite)){
        for(Composite::CompoIterator tit = dynamic_cast<Composite*>(tache)->getCompoIterator(); !tit.isDone(); tit.next()){
            printBranch(tacheItemTemp, &tit.current());
        }
    }

}
