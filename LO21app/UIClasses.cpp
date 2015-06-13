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
    delete icone0, icone1, icone2;
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
        setIcon(itemTemp, it.current().getStatut());
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
    setIcon(tacheItemTemp, tache->getStatut());
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
    try{
    for(int i=0; i<titNb; i++){
        if (tabItemsTaches[i].item == item){
            return tabItemsTaches[i].tache;
        }
    }
    throw CalendarException("Pas de tache trouvée en lien avec cet item");
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
}


Projet* TreeViewModel::getProjetFromItem(QStandardItem* item){
    try{
    for(int i=0; i<tipNb; i++){
        if (tabItemsProjets[i].item == item){
            return tabItemsProjets[i].projet;
        }
    }
    throw CalendarException("Pas de projet trouvé en lien avec cet item");
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
}

QStandardItem* TreeViewModel::getItemFromTache(Tache* tache){
    try{
    for(int i=0; i<titNb; i++){
        if (tabItemsTaches[i].tache == tache){
            return tabItemsTaches[i].item;
        }
    }
    throw CalendarException("Pas d'item trouvé en lien avec cette tache");
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
}

QStandardItem* TreeViewModel::getItemFromProjet(Projet* projet){
    try{
    for(int i=0; i<tipNb; i++){
        if (tabItemsProjets[i].projet == projet){
            return tabItemsProjets[i].item;
        }
    }
    throw CalendarException("Pas d'item trouvé en lien avec cette tache");
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
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
    setIcon(tacheItemTemp, tache->getStatut());

    getItemFromTache(tacheMere)->appendRow(tacheItemTemp);

    // On ajoute le couple (item, tache) au tableau qui va bien
    addTacheItem(tacheItemTemp, tache);
}

void TreeViewModel::setIcon(QStandardItem* item, int statut){
    if(statut==2)
        item->setIcon(*icone2);
    else if(statut==1)
        item->setIcon(*icone1);
    else if(statut==0)
        item->setIcon(*icone0);
//    QIcon* icone0;
//    QIcon* icone1;
//    QIcon* icone2;
//    QIcon("dot0.svg")
//    item->setText("cdfb");
}

void TreeViewModel::updateName(Projet* projet){
    getItemFromProjet(projet)->setText(QString::fromStdString(projet->getNom()));
    setIcon(getItemFromProjet(projet), projet->getStatut());
}

void TreeViewModel::updateName(Unitaire* tache){
    getItemFromTache(tache)->setText(QString::fromStdString(tache->getTitre()));
    setIcon(getItemFromTache(tache), tache->getStatut());
}

void TreeViewModel::updateName(Composite* tache){
    getItemFromTache(tache)->setText(QString::fromStdString(tache->getTitre()));
    setIcon(getItemFromTache(tache), tache->getStatut());
}

void TreeViewModel::updateIcons(Tache* t){
    setIcon( getItemFromTache(t), t->getStatut() );

    QStandardItem* current = getItemFromTache(t);
    while ((current = current->parent())->parent() != 0){
        setIcon( current, getTacheFromItem(current)->getStatut() );
    }
    setIcon( current, getProjetFromItem(current)->getStatut() );
}






EditeurProjet::EditeurProjet(Projet* p):
    Editeur(),
    titre (new QLineEdit),
    dispo (new QCalendarWidget),
    echeance (new QCalendarWidget),
    projet(p)
{

    //dispo->setMinimumDate(QDate::currentDate());
    dispo->setMaximumDate(p->getEcheance().getQDate());
    Projet::Iterator it=p->getIterator();
    for(it.first();!it.isDone();it.next()){
        if(it.current().getDateDisponibilite().getQDate() < dispo->maximumDate())
            dispo->setMaximumDate(it.current().getDateDisponibilite().getQDate());
    }

    QPushButton *sauver = new QPushButton("Sauver les changements");
    QPushButton *annuler = new QPushButton("Annuler les modifications");
    QPushButton *exporter = new QPushButton("Exporter le projet en XML");

    // Remplissage des champs
    titre->setText(QString::fromStdString(p->getNom()));
    formLayout->addRow("Titre de la tache", titre);
    try{
        dispo->setSelectedDate(p->getDispo().getQDate());
        dispo->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
        echeance->setSelectedDate(p->getEcheance().getQDate());
        echeance->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
        echeance->setEnabled(false);

        formLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tard que les dates de début des tâches
        formLayout->addRow("Echéance", echeance);
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }


    // afficher autres trucs !!

    // mettre des taches prédécesseurs
    formLayout->addRow("", sauver);
    formLayout->addRow("", annuler);
    formLayout->addRow("", exporter);

    this->setLayout(formLayout);
    this->setFixedWidth(550);

    connect(annuler, SIGNAL(clicked()), this, SLOT(slotReload()));
    connect(sauver, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(exporter, SIGNAL(clicked()), this, SLOT(slotExporter()));
}

void EditeurProjet::slotReload(){
    titre->setText(QString::fromStdString(projet->getNom()));
    dispo->setSelectedDate(projet->getDispo().getQDate());
    dispo->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
}

void EditeurProjet::slotSave(){
    try{
    projet->update( titre->text().toStdString(), Date::toTimingDate(dispo->selectedDate()) );
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
    emit projetUpdated(projet);
}

void EditeurProjet::slotExporter(){
    try{
        QMessageBox msgBox;
        msgBox.setText("Dans un monde parfait, les projets s'enregistrent, MAINTENANT !");

        QString fileName = QFileDialog::getSaveFileName(this->parentWidget(),
                                                        QString::fromStdString("Exporter le projet "+this->projet->getNom()),
                                                        "export.xml",
                                                        "Fichier XML (*.xml)");

        this->projet->save( fileName.toStdString() );
        msgBox.exec();
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
}


EditeurTache::EditeurTache():
    titre(new QLineEdit),
    dispo(new QCalendarWidget),
    echeance(new QCalendarWidget),
    annuler(new QPushButton("Annuler les changements")),
    sauver(new QPushButton("Enregistrer les changements")),
    predecesseurs(new QPushButton("Ajouter des taches prédécesseurs")),
    listePredecesseurs(new QListWidget),
    tabPredecesseurs(new QListWidgetItem*[100]),
    nbPred(0)
{}

void EditeurTache::printFinForm(Tache* t){
    if (t->isPrecedence()){
        int i = 0;
        for (Unitaire::Iterator it = t->getIterator(); !it.isDone() && (i<100); it.next()){
            tabPredecesseurs[i] = new QListWidgetItem(QString::fromStdString(it.current().getTitre()), listePredecesseurs);
            i++;
        }
        nbPred = i;
        listePredecesseurs->setMaximumHeight(50);
        formLayout->addRow("Prédecesseurs", listePredecesseurs);
    } else {
        QLabel* truc = new QLabel("Cette tache n'a pas de prédécesseurs pour le moment !");
        formLayout->addRow("Prédecesseurs", truc);
    }

    // mettre des taches prédécesseurs
    formLayout->addRow("", predecesseurs);
    formLayout->addRow("", sauver);
    formLayout->addRow("", annuler);

    this->setLayout(formLayout);
    this->setFixedWidth(650);

    connect(predecesseurs, SIGNAL(clicked()), this, SLOT(slotEditionPredecesseurs()));
    connect(annuler, SIGNAL(clicked()), this, SLOT(slotReload()));
    connect(sauver, SIGNAL(clicked()), this, SLOT(slotSave()));
}

void EditeurTU::slotEditionPredecesseurs(){
    EditeurPrecedence *ep = new EditeurPrecedence(tache);
    connect(ep, SIGNAL(editionPrecedenceEnd()), this, SLOT(slotEnable()));
    if (!ep->empty()){
        this->setEnabled(false);
        ep->exec();
    }
}

void EditeurTC::slotEditionPredecesseurs(){
    EditeurPrecedence *ep = new EditeurPrecedence(tache);
    connect(ep, SIGNAL(editionPrecedenceEnd()), this, SLOT(slotEnable()));
    if (!ep->empty()){
        this->setEnabled(false);
        ep->exec();
    }
}

void EditeurTache::slotEnable(){
    this->setEnabled(true);
    emit reloadAll();
}



EditeurTU::EditeurTU(Unitaire *t):
    EditeurTache(),
    duree (new QSpinBox),
    preemptible (new QCheckBox),
    tache(t)
{
    dispo->setMinimumDate(t->getProjet()->getDispo().getQDate());
    //echeance->setMinimumDate(QDate::currentDate());
    echeance->setMinimumDate(t->getProjet()->getDispo().getQDate());
    dispo->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // Remplissage des champs
    titre->setText(QString::fromStdString(t->getTitre()));
    dispo->setSelectedDate(t->getDateDisponibilite().getQDate());
    dispo->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    echeance->setSelectedDate(t->getDateEcheance().getQDate());
    echeance->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    duree->setValue(t->getDuree().getDureeEnMinutes());
    duree->setMaximum(24*60);
    preemptible->setChecked(t->isPreemp());

    formLayout = new QFormLayout;
    formLayout->addRow("Titre de la tache", titre);
    formLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tôt que le début du projet !
    formLayout->addRow("Échéance", echeance); // vérifier qu'elle est après la disponibilité
    formLayout->addRow("Durée (en minutes)", duree);
    formLayout->addRow("Préemptible", preemptible);


    // afficher durée faite
    //          + durée restante
    printFinForm(tache);
}


void EditeurTU::slotReload(){
    titre->setText(QString::fromStdString(tache->getTitre()));
    dispo->setSelectedDate(tache->getDateDisponibilite().getQDate());
    dispo->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    echeance->setSelectedDate(tache->getDateEcheance().getQDate());
    echeance->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    duree->setValue(tache->getDuree().getDureeEnMinutes());
    preemptible->setChecked(tache->isPreemp());
}

void EditeurTU::slotSave(){
    try{
    tache->update( titre->text().toStdString(),
                   Date::toTimingDate(dispo->selectedDate()),
                   Date::toTimingDate(echeance->selectedDate()),
                   Duree( duree->value() ),
                   preemptible->isChecked());
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }

    emit tacheUpdated(tache);
    slotReload();
}


EditeurTC::EditeurTC(Composite *t):
    EditeurTache(),
    tache(t)
{
    dispo->setMaximumDate(t->getDateEcheance().getQDate());
    Composite::CompoIterator it=t->getCompoIterator();
    for(it.first();!it.isDone();it.next()){
        if(it.current().getDateDisponibilite().getQDate() < dispo->maximumDate())
            dispo->setMaximumDate(it.current().getDateDisponibilite().getQDate());
    }

    dispo->setMinimumDate(t->getProjet()->getDispo().getQDate());
    //echeance->setMinimumDate(QDate::currentDate());
    echeance->setMinimumDate(t->getProjet()->getDispo().getQDate());
    dispo->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // Remplissage des champs
    titre->setText(QString::fromStdString(t->getTitre()));
    dispo->setSelectedDate(t->getDateDisponibilite().getQDate());
    echeance->setSelectedDate(t->getDateEcheance().getQDate());
    echeance->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    formLayout = new QFormLayout;
    formLayout->addRow("Titre de la tache", titre);
    formLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tôt que le début du projet !
    formLayout->addRow("Échéance", echeance); // vérifier qu'elle est après la disponibilité


    // afficher durée faite
    //          + durée restante

    // mettre des taches prédécesseurs
    printFinForm(tache);
}


void EditeurTC::slotReload(){
    titre->setText(QString::fromStdString(tache->getTitre()));
    dispo->setSelectedDate(tache->getDateDisponibilite().getQDate());
    echeance->setSelectedDate(tache->getDateEcheance().getQDate());
    dispo->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    echeance->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
}

void EditeurTC::slotSave(){
    try{
    tache->update( titre->text().toStdString(), Date::toTimingDate(dispo->selectedDate()), Date::toTimingDate(echeance->selectedDate()) );
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
    emit tacheUpdated(tache);
    slotReload();
}

EditeurPrecedence::EditeurPrecedence(Tache *t):
        tache(t),
        QDialog(),
        precedencesPotentielles(new QComboBox),
        Donnee(new QFormLayout),
        annuler(new QPushButton("Annuler")),
        ajouter(new QPushButton("Ajouter")),
        nbIndexes(0)
{
    Projet::Iterator it = t->getProjet()->getIterator();
    for(it.first();!it.isDone();it.next()){
        try {
            if (t->isPrecedencePotentielle(it.current().getId())){
                precedencesPotentielles->addItem(QString::fromStdString(it.current().getTitre()));
                couples[nbIndexes].identifiant = it.current().getId();
                couples[nbIndexes++].index = precedencesPotentielles->count() - 1;

//                cout<<"Ligne "<<nbIndexes<<" "<<couples[nbIndexes-1].identifiant<<" "<<couples[nbIndexes-1].index<<endl;
            }
        } catch (CalendarException e){
            QMessageBox *erreur = new QMessageBox;
            erreur->setText(QString::fromStdString(e.getInfo()));
            erreur->exec();
        }
    }

    if(nbIndexes == 0){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText( "Pas de précédence potentielle !" );
        erreur->exec();
        emit editionPrecedenceEnd();
    } else {
        Donnee->addRow("Précédences", precedencesPotentielles);
        Donnee->addRow("", annuler);
        Donnee->addRow("", ajouter);
        connect(annuler, SIGNAL(clicked()), this, SLOT(slotAnnulation()));
        connect(ajouter, SIGNAL(clicked()), this, SLOT(slotAjout()));

        this->setLayout(Donnee);
        this->setWindowTitle(QString::fromUtf8("Ajout d'une précédence"));
        this->setModal(true);
    }

}

string& EditeurPrecedence::getTacheIdFromIndex( int index ){
    for (int i=0; (i<nbIndexes) && (i<100) ; i++){
        if (couples[i].index == index)
            return couples[i].identifiant;
    }
    throw CalendarException("Tache pas trouvée avec cet index !");
}

void EditeurPrecedence::slotAnnulation(){
//    emit editionPrecedenceEnd( "" );
    emit editionPrecedenceEnd();
    this->close();
}

void EditeurPrecedence::slotAjout(){
    try {
        tache->addItem( &(tache->getProjet()->getTache( getTacheIdFromIndex(precedencesPotentielles->currentIndex()) ) ) );
//        emit editionPrecedenceEnd( tache->getProjet()->getTache( getTacheIdFromIndex(precedencesPotentielles->currentIndex())).getTitre() );
        emit editionPrecedenceEnd();
    } catch (CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
    this->close();
}







ProgrammationTache::ProgrammationTache(Unitaire* t):
    QDialog(),
    formLayout(new QFormLayout),
    tache(t),
    titre(new QLabel),
    date(new QCalendarWidget),
    Sauver(new QPushButton("Enregistrer"))
{
    this->setWindowTitle(QString("Programmation de la tache ")+QString::fromStdString(tache->getTitre()));
    date->setMinimumDate(t->getDateDisponibilite().getQDate());
    date->setMaximumDate(t->getDateEcheance().getQDate());

    // Remplissage des champs
    titre->setText(QString::fromStdString(t->getTitre()));
    date->setSelectedDate(QDate::currentDate());

    hHoraire = new QSpinBox(this);
    mHoraire = new QSpinBox(this);
    hHoraire->setMinimum(0);
    hHoraire->setMaximum(23);
    hHoraire->setSuffix("heure(s)");
    hHoraire->setValue(12);
    mHoraire->setMinimum(0);
    mHoraire->setSuffix("minute(s)");
    mHoraire->setValue(0);

    if(!tache->isPreemp()){
        hDuree = new QSpinBox(this);
        mDuree = new QSpinBox(this);
        hDuree->setMinimum(tache->getDuree().getDureeEnHeuresInt());
        hDuree->setMaximum(tache->getDuree().getDureeEnHeuresInt());
        hDuree->setSuffix("heure(s)");
        hDuree->setValue(tache->getDuree().getDureeEnHeuresInt());
        mDuree->setMinimum((tache->getDuree().getDureeEnMinutes())%60);
        mDuree->setMaximum((tache->getDuree().getDureeEnMinutes())%60);
        mDuree->setSuffix("minute(s)");
        mDuree->setValue((tache->getDuree().getDureeEnMinutes())%60);
    }
    else{
        hDuree = new QSpinBox(this);
        mDuree = new QSpinBox(this);
        hDuree->setMinimum(0);
        if(tache->getDuree().getDureeEnMinutes()<12*60)
            hDuree->setMaximum(tache->getDuree().getDureeEnHeuresInt());
        else
            hDuree->setMaximum(12);
        hDuree->setSuffix("heure(s)");
        hDuree->setValue(tache->getRestant().getDureeEnHeuresInt());
        mDuree->setMinimum(0);
        mDuree->setMaximum(59);
        mDuree->setSuffix("minute(s)");
        mDuree->setValue((tache->getRestant().getDureeEnMinutes())%60);
    }

    formLayout->addRow("Titre", titre);
    formLayout->addRow("Date", date);
    formLayout->addRow("Horaire", hHoraire);
    formLayout->addRow("", mHoraire);
    formLayout->addRow("Durée", hDuree);
    formLayout->addRow("", mDuree);

    formLayout->addRow("", Sauver);

    this->setLayout(formLayout);
    connect(Sauver, SIGNAL(clicked()), this, SLOT(slotSave()));
}

void ProgrammationTache::slotSave(){
    try{
        ProgrammationManager::getInstance().ajouterProgrammation(
                    *tache,
                    Date(date->selectedDate().day(), date->selectedDate().month(), date->selectedDate().year()),
                    Horaire(hHoraire->value(),mHoraire->value()),
                    Duree(hDuree->value(),mDuree->value()));
        //les parametres de la tache sont mis à jour automatiquement par ajouterProgrammation
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
    emit tacheProgrammee();
    this->close();
}

ProgrammationActivite::ProgrammationActivite():
    QDialog(),
    formLayout(new QFormLayout),
    titre(new QLineEdit),
    lieu(new QLineEdit),
    date(new QCalendarWidget),
    Sauver(new QPushButton("Enregistrer"))
{
    this->setWindowTitle(QString("Programmation d'une activité"));
    titre->setText(QString("Nouvelle activité"));
    //date->setMinimumDate(QDate::currentDate());

    // Remplissage des champs
    titre->setText(QString("Nouvelle activité"));
    lieu->setText(QString(""));
    date->setSelectedDate(QDate::currentDate());

    hHoraire = new QSpinBox(this);
    mHoraire = new QSpinBox(this);
    hHoraire->setMinimum(0);
    hHoraire->setMaximum(23);
    hHoraire->setSuffix("heure(s)");
    hHoraire->setValue(12);
    mHoraire->setMinimum(0);
    mHoraire->setSuffix("minute(s)");
    mHoraire->setValue(0);

    hDuree = new QSpinBox(this);
    mDuree = new QSpinBox(this);
    hDuree->setMinimum(0);
    hDuree->setMaximum(12);
    hDuree->setSuffix("heure(s)");
    hDuree->setValue(0);
    mDuree->setMinimum(0);
    mDuree->setMaximum(59);
    mDuree->setSuffix("minute(s)");
    mDuree->setValue(0);

    formLayout->addRow("Titre", titre);
    formLayout->addRow("Lieu", lieu);
    formLayout->addRow("Date", date);
    formLayout->addRow("Horaire", hHoraire);
    formLayout->addRow("", mHoraire);
    formLayout->addRow("Durée", hDuree);
    formLayout->addRow("", mDuree);

    formLayout->addRow("", Sauver);

    this->setLayout(formLayout);
    connect(Sauver, SIGNAL(clicked()), this, SLOT(slotSave()));
}

void ProgrammationActivite::slotSave(){
    Activite* activite = new Activite(titre->text().toStdString(),
                                      lieu->text().toStdString(),
                                      Duree(hDuree->value(), mDuree->value()));
    try{
        ProgrammationManager::getInstance().ajouterProgrammation(
                    *activite,
                    Date(date->selectedDate().day(), date->selectedDate().month(), date->selectedDate().year()),
                    Horaire(hHoraire->value(),mHoraire->value()),
                    Duree(hDuree->value(),mDuree->value()));
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
    emit activiteProgrammee();
    this->close();
}
