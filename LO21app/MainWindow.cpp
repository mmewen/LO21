#include "MainWindow.h"

//! initialisation de la fenêtre principale
MainWindow::MainWindow():
        zoneCentrale(new QWidget),
        tabs(new QTabWidget(zoneCentrale)),
        planningTab(new QWidget()),
        agendaLayout(new QHBoxLayout()),
        agenda(new AgendaView()),
        tachesTab(new QWidget()),
        tachesLayout(new QHBoxLayout),
        edition(0),
        editionScroll(new QScrollArea),
        tachesView(new QVBoxLayout),
        boutonsTreeView(new QFormLayout),
        vue(new QTreeView),
        treeView(TreeViewModel::getInstance())
    {
    this->setWindowTitle(QString::fromUtf8("Pistache"));
    this->setFixedSize(W_WIDTH, W_HEIGHT);

    // Onglets
    QTabWidget *tabs = new QTabWidget(zoneCentrale);
    tabs->setFixedSize(W_WIDTH, W_HEIGHT);

    tabs->addTab(tachesTab,"Édition");
    tabs->addTab(planningTab,"Agenda");

    // Onglet Agenda :
    agendaLayout->addWidget(agenda);
    planningTab->setLayout(agendaLayout);

    // Tree onglet Taches (modèle/vue)
    tachesLayout->addLayout(tachesView);
    tachesView->addWidget(vue);
    vue->setHeaderHidden(true);
    vue->setFixedSize(300,0.65*W_HEIGHT);
    vue->setModel(treeView.getModele());
    connect(vue, SIGNAL(clicked(QModelIndex)), this, SLOT(treeViewClicked(QModelIndex)));

    tachesView->addLayout(boutonsTreeView);
    QPushButton *buttonProjet = new QPushButton("+");
    QPushButton *buttonTU = new QPushButton("+");
    QPushButton *buttonTC = new QPushButton("+");
    QPushButton *buttonPU = new QPushButton("Programmer");
    QPushButton *buttonPA = new QPushButton("Programmer");
    boutonsTreeView->addRow("Projet", buttonProjet);
    boutonsTreeView->addRow("Tâche unitaire", buttonTU);
    boutonsTreeView->addRow("Tâche composite", buttonTC);
    boutonsTreeView->addRow("Tâche unitaire", buttonPU);
    boutonsTreeView->addRow("Activité", buttonPA);
    connect(buttonProjet, SIGNAL(clicked()), this, SLOT(slotAjouterProjet()));
    connect(buttonTU, SIGNAL(clicked()), this, SLOT(slotAjouterTU()));
    connect(buttonTC, SIGNAL(clicked()), this, SLOT(slotAjouterTC()));
    connect(buttonPU, SIGNAL(clicked()), this, SLOT(slotProgrammerTU()));
    connect(buttonPA, SIGNAL(clicked()), this, SLOT(slotProgrammerActivite()));

    tachesLayout->addWidget(editionScroll);
    QLabel *message = new QLabel("Cliquez sur une tache ou un projet pour pouvoir l'éditer");
    editionScroll->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    editionScroll->setWidget(message);

    connect( &ProgrammationManager::getInstance() , SIGNAL(programmationsChanged()), agenda, SLOT(slotEventsChanged()));


    QMenu *menuFichier = menuBar()->addMenu("&Fichier");
//    QMenu *menuEdition = menuBar()->addMenu("&Edition");
    QAction *actionQuitter = new QAction("&Quitter", this);
    menuFichier->addAction(actionQuitter);
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    tachesTab->setLayout(tachesLayout);
    this->setCentralWidget(zoneCentrale);

    treeView.printTree();
}

void MainWindow::showUnitaire(Unitaire& t){
    edition = new EditeurTU(&t);
    this->t = &t;
    editionScroll->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    editionScroll->setWidget(edition);
    editionScroll->setContentsMargins(11, 11, 11, 20);
    connect(edition, SIGNAL(tacheUpdated(Unitaire*)), &treeView, SLOT(updateName(Unitaire*)));
    connect(edition, SIGNAL(reloadAll()), this, SLOT(slotShowTU()));
}


void MainWindow::showComposite(Composite& t){
    edition = new EditeurTC(&t);
    this->t = &t;
    editionScroll->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    editionScroll->setWidget(edition);
    editionScroll->setContentsMargins(11, 11, 11, 20);
    connect(edition, SIGNAL(tacheUpdated(Composite*)), &treeView, SLOT(updateName(Composite*)));
    connect(edition, SIGNAL(reloadAll()), this, SLOT(slotShowTC()));
}

void MainWindow::showProjet(Projet& p){
    edition = new EditeurProjet(&p);
    editionScroll->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    editionScroll->setWidget(edition);
    editionScroll->setContentsMargins(11, 11, 11, 20);
    connect(edition, SIGNAL(projetUpdated(Projet*)), &treeView, SLOT(updateName(Projet*)));
}

void MainWindow::treeViewClicked(const QModelIndex &index)
{
    QStandardItem *item = treeView.getModele()->itemFromIndex(index);
//    cout<<"Row : "<<item->row()<<", parent : "<<item->parent()<<endl;

    if(item->parent() == 0){
        // C'est un projet, on l'affiche
        Projet& p = *treeView.getProjetFromItem(item);

        showProjet(p);
    } else {
        // C'est une tache, on fait pareil
        Tache& t = *treeView.getTacheFromItem(item);

        // On détecte le type :
        if (typeid(t) == typeid(Composite)){
            showComposite(dynamic_cast<Composite&>(t));
        } else {
            showUnitaire(dynamic_cast<Unitaire&>(t));
        }
    }

}


//void MainWindow::clearTFL(){
//    QLayoutItem *child;

//    if (tachesFormLayout != 0){
//        int count = tachesFormLayout->count();
//        while ( count-- != 0) {
//            child = tachesFormLayout->takeAt(0);
//            child->widget()->setParent(NULL);
//            tachesFormLayout->removeItem(child);
//        }
//        tachesFormLayout->deleteLater();
//    }
//}



void MainWindow::slotAjouterProjet(){
    // Création du projet
    ProjetManager& pjm = ProjetManager::getInstance();
    Projet& nouveauProjet = pjm.ajouterProjet("Nouveau projet", "pj5.xml", Date(4,6,2015)); // XXX /!\ nom de fichier !

    // Ajout à la vue
    treeView.addProjet(&nouveauProjet);
}

void MainWindow::slotAjouterTU(){
    // Récupération du projet et de la tâche mère
    QModelIndexList selectedIndexList = vue->selectionModel()->selectedIndexes();
    if (selectedIndexList.size() == 1){ // si exactement un truc est sélectionné
        QStandardItem *item, *parentProject;
        item = treeView.getModele()->itemFromIndex(selectedIndexList.at(0));

        parentProject = item;
        while (parentProject->parent() != 0){
            parentProject = parentProject->parent();
        }

        Projet* projetParent = treeView.getProjetFromItem(parentProject);

        // Création de la tache
        //Date aujourdhui;
        //aujourdhui.setDateAujourdhui();
        //Unitaire& tache = projetParent->ajouterUnitaire("Nouvelle tache unitaire", aujourdhui, aujourdhui,Duree(1,30), false);
        Date dispo;
        dispo=projetParent->getDispo();
        Unitaire& tache = projetParent->ajouterUnitaire("Nouvelle tache unitaire", dispo, dispo,Duree(1,30), false);

        // Déplacement de la tâche au bon endroit
        if (item->parent() != 0) { // si la tache est composante
            Tache* tacheParente = treeView.getTacheFromItem(item);
            if (typeid(*tacheParente) == typeid(Composite)){
//                cout<<"Tache déplacée"<<endl;
                    tache.setDatesDisponibiliteEcheance(tacheParente->getDateDisponibilite(),tacheParente->getDateEcheance());
                    projetParent->moveTacheTo(tacheParente, &tache);
                    treeView.addTache(tacheParente, &tache);

            } else { // La mère n'est pas une tache composite, on la crée "à coté" de la tache unitaire sélectionnée
//                cout<<"Tache parent unitaire"<<endl;
                if(item->parent()->parent() == 0){ // ie. on crée une tache dans un projet
//                    cout<<"Tache créée à la racine à coté de la TU sélectionnée"<<endl;
                    treeView.addTache(projetParent, &tache);
                } else {
                    Tache* tacheParente = treeView.getTacheFromItem(item->parent());
                        tache.setDatesDisponibiliteEcheance(tacheParente->getDateDisponibilite(),tacheParente->getDateEcheance());
                        projetParent->moveTacheTo(tacheParente, &tache);
                        treeView.addTache(tacheParente, &tache);

                }
            }
        } else {
//            cout<<"Tache créée à la racine"<<endl;
            treeView.addTache(projetParent, &tache);
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText("Veuillez sélectionner un projet dans lequel ajouter cette tache avant de cliquer, bisous <3");
        msgBox.exec();
    }

}

void MainWindow::slotAjouterTC(){
    // Récupération du projet et de la tâche mère
    QModelIndexList selectedIndexList = vue->selectionModel()->selectedIndexes();
    if (selectedIndexList.size() == 1){ // si exactement un truc est sélectionné
        QStandardItem *item, *parentProject;
        item = treeView.getModele()->itemFromIndex(selectedIndexList.at(0));

        parentProject = item;
        while (parentProject->parent() != 0){
            parentProject = parentProject->parent();
        }

        Projet* projetParent = treeView.getProjetFromItem(parentProject);

        // Création de la tache
        //Date aujourdhui;
        //aujourdhui.setDateAujourdhui();
        //Composite& tache = projetParent->ajouterComposite("Nouvelle tache composite", aujourdhui, aujourdhui);
        Date dispo;
        dispo=projetParent->getDispo();
        Composite& tache = projetParent->ajouterComposite("Nouvelle tache composite", dispo, dispo);

        // Déplacement de la tâche au bon endroit
        if (item->parent() != 0) { // si la tache est composante
            Tache* tacheParente = treeView.getTacheFromItem(item);
            if (typeid(*tacheParente) == typeid(Composite)){
//                cout<<"Tache déplacée"<<endl;
                    tache.setDatesDisponibiliteEcheance(tacheParente->getDateDisponibilite(),tacheParente->getDateEcheance());
                    projetParent->moveTacheTo(tacheParente, &tache);
                    treeView.addTache(tacheParente, &tache);

            } else { // La mère n'est pas une tache composite, on la crée "à coté" de la tache unitaire sélectionnée
//                cout<<"Tache parent unitaire"<<endl;
                if(item->parent()->parent() == 0){ // ie. on crée une tache dans un projet
//                    cout<<"Tache créée à la racine à coté de la TU sélectionnée"<<endl;
                    tache.setDatesDisponibiliteEcheance(projetParent->getDispo(),projetParent->getDispo());
                    treeView.addTache(projetParent, &tache);
                } else {
                    Tache* tacheParente = treeView.getTacheFromItem(item->parent());
                        tache.setDatesDisponibiliteEcheance(tacheParente->getDateDisponibilite(),tacheParente->getDateEcheance());
                        projetParent->moveTacheTo(tacheParente, &tache);
                        treeView.addTache(tacheParente, &tache);


                }
            }
        } else {
//            cout<<"Tache créée à la racine"<<endl;
            treeView.addTache(projetParent, &tache);
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText("Veuillez sélectionner un projet dans lequel ajouter cette tache !");
        msgBox.exec();
    }
}

void MainWindow::slotShowTU(){
    showUnitaire(*(dynamic_cast<Unitaire*>(t)));
}

void MainWindow::slotShowTC(){
    showComposite(*(dynamic_cast<Composite*>(t)));
}

void MainWindow::slotProgrammerTU(){
    // Récupération du projet et de la tâche mère
    QModelIndexList selectedIndexList = vue->selectionModel()->selectedIndexes();
    if (selectedIndexList.size() == 1){ // si exactement un truc est sélectionné
        QStandardItem *item, *parentProject;
        item = treeView.getModele()->itemFromIndex(selectedIndexList.at(0));

        parentProject = item;
        while (parentProject->parent() != 0){
            parentProject = parentProject->parent();
        }

        if (item->parent() != 0) {
            Tache* tache = treeView.getTacheFromItem(item);
            if (typeid(*tache) == typeid(Unitaire)){
                if(dynamic_cast<Unitaire*>(tache)->getRestant().getDureeEnMinutes()==0){
                    QMessageBox msgBox;
                    msgBox.setText("La tache a déjà été entièrement programmée");
                    msgBox.exec();
                }
                else{
                    edition->setEnabled(false);
                    ProgrammationTache *pt = new ProgrammationTache(dynamic_cast<Unitaire*>(tache));
                    connect(pt, SIGNAL(tacheProgrammee()), edition, SLOT(slotEnable()));
                    connect(pt, SIGNAL(rejected()), edition, SLOT(slotEnable()));
                    pt->exec();
                }
            }
            else{
                QMessageBox msgBox;
                msgBox.setText("La tache sélectionnée n'est pas unitaire");
                msgBox.exec();
            }
        }
        else{
            QMessageBox msgBox;
            msgBox.setText("Sélectionnez une tache unitaire à programmer");
            msgBox.exec();
        }
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("Sélectionnez une tache unitaire à programmer");
        msgBox.exec();
    }
}

void MainWindow::slotProgrammerActivite(){
    //edition->setEnabled(false);
    ProgrammationActivite *pa = new ProgrammationActivite();
    connect(pa, SIGNAL(activiteProgrammee()), edition, SLOT(slotEnable()));
    //connect(pa, SIGNAL(rejected()), edition, SLOT(slotEnable()));
    pa->exec();
}
