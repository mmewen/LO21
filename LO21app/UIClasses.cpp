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
        for(int i=0; i<tipNb; i++) newtab[i]=tabItemsProjets[i];
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
        for(int i=0; i<titNb; i++) newtab[i]=tabItemsTaches[i];
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

void TreeViewModel::addProjet(Projet* nouveauProjet){
    QStandardItem *parentItem = modele.invisibleRootItem();

    // On ajoute la ligne
    QStandardItem* itemTemp = new QStandardItem(QString::fromStdString(nouveauProjet->getNom()));
    itemTemp->setEditable(false);
    parentItem->appendRow(itemTemp);

    // On ajoute le couple (item, projet) au tableau qui va bien
    addProjetItem(itemTemp, nouveauProjet);
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

void TreeViewModel::addTache(Projet* projet, Tache* tache){
    // On ajoute l'item
    QStandardItem* tacheItemTemp = new QStandardItem(QString::fromStdString(tache->getTitre()));
    tacheItemTemp->setEditable(false);
    getItemFromProjet(projet)->appendRow(tacheItemTemp);

    // On ajoute le couple (item, tache) au tableau qui va bien
    addTacheItem(tacheItemTemp, tache);
}

void TreeViewModel::addTache(Tache* tacheMere, Tache* tache){
    // On ajoute l'item
    QStandardItem* tacheItemTemp = new QStandardItem(QString::fromStdString(tache->getTitre()));
    tacheItemTemp->setEditable(false);
    getItemFromTache(tacheMere)->appendRow(tacheItemTemp);

    // On ajoute le couple (item, tache) au tableau qui va bien
    addTacheItem(tacheItemTemp, tache);
}

void TreeViewModel::updateName(Projet* projet){
    getItemFromProjet(projet)->setText(QString::fromStdString(projet->getNom()));
}

void TreeViewModel::updateName(Unitaire* tache){
    getItemFromTache(tache)->setText(QString::fromStdString(tache->getTitre()));
}

void TreeViewModel::updateName(Composite* tache){
    getItemFromTache(tache)->setText(QString::fromStdString(tache->getTitre()));
}






EditeurProjet::EditeurProjet(Projet* p):
    Editeur(),
    titre (new QLineEdit),
    dispo (new QCalendarWidget),
    echeance (new QCalendarWidget),
    projet(p)
{
    dispo->setMinimumDate(QDate::currentDate());

    QPushButton *annuler = new QPushButton("Annuler les modifications");
    QPushButton *sauver = new QPushButton("Sauver les changements");

    // Remplissage des champs
    titre->setText(QString::fromStdString(p->getNom()));
    dispo->setSelectedDate(p->getDispo().getQDate());
    echeance->setSelectedDate(p->getEcheance().getQDate());

    formLayout->addRow("Titre de la tache", titre);
    formLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tard que les dates de début des tâches
    formLayout->addRow("Echéance", echeance);


    // afficher autres trucs !!

    // mettre des taches prédécesseurs
    formLayout->addRow("", annuler);
    formLayout->addRow("", sauver);

    this->setLayout(formLayout);
    this->setFixedWidth(550);

    connect(annuler, SIGNAL(clicked()), this, SLOT(slotReload()));
    connect(sauver, SIGNAL(clicked()), this, SLOT(slotSave()));
}

void EditeurProjet::slotReload(){
    titre->setText(QString::fromStdString(projet->getNom()));
    dispo->setSelectedDate(projet->getDispo().getQDate());
}

void EditeurProjet::slotSave(){
    projet->update( titre->text().toStdString(),
                    Date( dispo->selectedDate().day(), dispo->selectedDate().month(), dispo->selectedDate().year()) );
    emit projetUpdated(projet);
//    Date::toTimingDate(dispo->selectedDate()) // marche pas, pourquoi ?
}

EditeurTache::EditeurTache():
    titre(new QLineEdit),
    dispo(new QCalendarWidget),
    echeance(new QCalendarWidget),
    annuler(new QPushButton("Annuler les changements")),
    sauver(new QPushButton("Enregistrer les changements")),
    predecesseurs(new QPushButton("Ajouter des taches prédécesseurs"))
{}


EditeurTU::EditeurTU(Unitaire *t):
    EditeurTache(),
    duree (new QSpinBox),
    preemptible (new QCheckBox),
    tache(t)
{
    dispo->setMinimumDate(QDate::currentDate());
    echeance->setMinimumDate(QDate::currentDate());

    // Remplissage des champs
    titre->setText(QString::fromStdString(t->getTitre()));
    dispo->setSelectedDate(t->getDateDisponibilite().getQDate());
    echeance->setSelectedDate(t->getDateEcheance().getQDate());
    duree->setValue(t->getDuree().getDureeEnMinutes());
    preemptible->setChecked(t->isPreemp());

    formLayout = new QFormLayout;
    formLayout->addRow("Titre de la tache", titre);
    formLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tôt que le début du projet !
    formLayout->addRow("Échéance", echeance); // vérifier qu'elle est après la disponibilité
    formLayout->addRow("Durée (en minutes)", duree);
    formLayout->addRow("Préemptible", preemptible);


    // afficher durée faite
    //          + durée restante

    // mettre des taches prédécesseurs
    formLayout->addRow("", predecesseurs);
    formLayout->addRow("", annuler);
    formLayout->addRow("", sauver);

    this->setLayout(formLayout);
    this->setFixedWidth(550);

    connect(annuler, SIGNAL(clicked()), this, SLOT(slotReload()));
    connect(sauver, SIGNAL(clicked()), this, SLOT(slotSave()));
}


void EditeurTU::slotReload(){
    titre->setText(QString::fromStdString(tache->getTitre()));
    dispo->setSelectedDate(tache->getDateDisponibilite().getQDate());
    echeance->setSelectedDate(tache->getDateEcheance().getQDate());
    duree->setValue(tache->getDuree().getDureeEnMinutes());
    preemptible->setChecked(tache->isPreemp());
}

void EditeurTU::slotSave(){
    tache->update( titre->text().toStdString(),
                   Date( dispo->selectedDate().day(), dispo->selectedDate().month(), dispo->selectedDate().year()),
                   Date( echeance->selectedDate().day(), echeance->selectedDate().month(), echeance->selectedDate().year()),
                   Duree( duree->value() ),
                   preemptible->isChecked());
    emit tacheUpdated(tache);
    slotReload();
//    Date::toTimingDate(dispo->selectedDate()) // marche pas, pourquoi ?
}


EditeurTC::EditeurTC(Composite *t):
    EditeurTache(),
    tache(t)
{
    dispo->setMinimumDate(QDate::currentDate());
    echeance->setMinimumDate(QDate::currentDate());

    // Remplissage des champs
    titre->setText(QString::fromStdString(t->getTitre()));
    dispo->setSelectedDate(t->getDateDisponibilite().getQDate());
    echeance->setSelectedDate(t->getDateEcheance().getQDate());

    formLayout = new QFormLayout;
    formLayout->addRow("Titre de la tache", titre);
    formLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tôt que le début du projet !
    formLayout->addRow("Échéance", echeance); // vérifier qu'elle est après la disponibilité


    // afficher durée faite
    //          + durée restante

    // mettre des taches prédécesseurs
    formLayout->addRow("", predecesseurs);
    formLayout->addRow("", annuler);
    formLayout->addRow("", sauver);

    this->setLayout(formLayout);
    this->setFixedWidth(550);

    connect(annuler, SIGNAL(clicked()), this, SLOT(slotReload()));
    connect(sauver, SIGNAL(clicked()), this, SLOT(slotSave()));
}


void EditeurTC::slotReload(){
    titre->setText(QString::fromStdString(tache->getTitre()));
    dispo->setSelectedDate(tache->getDateDisponibilite().getQDate());
    echeance->setSelectedDate(tache->getDateEcheance().getQDate());
}

void EditeurTC::slotSave(){
    tache->update( titre->text().toStdString(),
                   Date( dispo->selectedDate().day(), dispo->selectedDate().month(), dispo->selectedDate().year()),
                   Date( echeance->selectedDate().day(), echeance->selectedDate().month(), echeance->selectedDate().year()) );
    emit tacheUpdated(tache);
    slotReload();
//    Date::toTimingDate(dispo->selectedDate()) // marche pas, pourquoi ?
}

EditeurPrecedence::EditeurPrecedence(Tache *t):tache(t),Precedences(new QComboBox){
    Tache::Iterator it = t->getIterator();
    for(it.first();!it.isDone();it.next()){
        Precedences->addItem(QString::fromStdString(it.current().getTitre()));
    }
}








