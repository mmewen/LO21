#include "MainWindow.h"

//! initialisation de la fenêtre principale
MainWindow::MainWindow():
        zoneCentrale(new QWidget),
        tabs(new QTabWidget(zoneCentrale)),
        planningTab(new QWidget()),
        tachesTab(new QWidget()),
        tachesLayout(new QHBoxLayout),
        tachesFormLayout(0),
        editionScroll(new QScrollArea),
        tachesView(new QVBoxLayout),
        boutonsTreeView(new QFormLayout),
        vue(new QTreeView),
        treeView(TreeViewModel::getInstance())
    {
    this->setWindowTitle(QString::fromUtf8("LO21"));
    this->setFixedSize(W_WIDTH, W_HEIGHT);

    // Onglets
    QTabWidget *tabs = new QTabWidget(zoneCentrale);
    tabs->setFixedSize(W_WIDTH, W_HEIGHT);

    tabs->addTab(tachesTab,"Édition");
    tabs->addTab(planningTab,"Agenda");

    // Tree onglet Taches (modèle/vue)
    tachesLayout->addLayout(tachesView);
    tachesView->addWidget(vue);
    vue->setHeaderHidden(true);
    vue->setFixedSize(300,0.7*W_HEIGHT);
    vue->setModel(treeView.getModele());
    connect(vue, SIGNAL(clicked(QModelIndex)), this, SLOT(treeViewClicked(QModelIndex)));

    tachesView->addLayout(boutonsTreeView);
    QPushButton *buttonProjet = new QPushButton("+");
    QPushButton *buttonTU = new QPushButton("+");
    QPushButton *buttonTC = new QPushButton("+");
    boutonsTreeView->addRow("Projet", buttonProjet);
    boutonsTreeView->addRow("Tâche unitaire", buttonTU);
    boutonsTreeView->addRow("Tâche composite", buttonTC);
    connect(buttonProjet, SIGNAL(clicked()), this, SLOT(slotAjouterProjet()));
    connect(buttonTU, SIGNAL(clicked()), this, SLOT(slotAjouterTU()));
    connect(buttonTC, SIGNAL(clicked()), this, SLOT(slotAjouterTC()));

    tachesLayout->addWidget(editionScroll);
    QLabel *message = new QLabel("Cliquez sur une tache ou un projet pour pouvoir l'éditer");
    editionScroll->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    editionScroll->setWidget(message);

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
    // XXXX faire fonction de ménage !
    Editeur* edition = new EditeurTU(&t);
    editionScroll->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    editionScroll->setWidget(edition);
    editionScroll->setContentsMargins(11, 11, 11, 20);
    connect(edition, SIGNAL(tacheUpdated(Unitaire*)), &treeView, SLOT(updateName(Unitaire*)));
}


void MainWindow::showComposite(Composite& t){
    // XXXX faire fonction de ménage !
    Editeur* edition = new EditeurTC(&t);
    editionScroll->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    editionScroll->setWidget(edition);
    editionScroll->setContentsMargins(11, 11, 11, 20);
    connect(edition, SIGNAL(tacheUpdated(Composite*)), &treeView, SLOT(updateName(Composite*)));
}

void MainWindow::showProjet(Projet& p){
//    clearTFL(); // XXXX faire fonction de ménage !
    Editeur* edition = new EditeurProjet(&p);
//    editionScroll->children().at(0)->deleteLater();
    editionScroll->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    editionScroll->setWidget(edition);
    editionScroll->setContentsMargins(11, 11, 11, 20);
    connect(edition, SIGNAL(projetUpdated(Projet*)), &treeView, SLOT(updateName(Projet*)));
}

//void MainWindow::slotSaveProjet(QLineEdit *titre, QCalendarWidget *dispo){
//    cout<<titre->text().toStdString();
//    dispo->selectedDate();
//}
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


void MainWindow::clearTFL(){
    QLayoutItem *child;

    if (tachesFormLayout != 0){
        int count = tachesFormLayout->count();
        while ( count-- != 0) {
            child = tachesFormLayout->takeAt(0);
            child->widget()->setParent(NULL);
            tachesFormLayout->removeItem(child);
        }
        tachesFormLayout->deleteLater();
    }
}



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
        Date aujourdhui;
        aujourdhui.setDateAujourdhui();
        Unitaire& tache = projetParent->ajouterUnitaire("Nouvelle tache unitaire", aujourdhui, aujourdhui,Duree(1,30), false);

        // Déplacement de la tâche au bon endroit
        if (item->parent() != 0) { // si la tache est composante
            Tache* tacheParente = treeView.getTacheFromItem(item);
            if (typeid(*tacheParente) == typeid(Composite)){
//                cout<<"Tache déplacée"<<endl;
                projetParent->moveTacheTo(tacheParente, &tache);

                treeView.addTache(tacheParente, &tache);
            } else { // La mère n'est pas une tache composite, on la crée "à coté" de la tache unitaire sélectionnée
                cout<<"Tache parent unitaire"<<endl;
                if(item->parent()->parent() == 0){ // ie. on crée une tache dans un projet
//                    cout<<"Tache créée à la racine à coté de la TU sélectionnée"<<endl;
                    treeView.addTache(projetParent, &tache);
                } else {
                    Tache* tacheParente = treeView.getTacheFromItem(item->parent());
                    projetParent->moveTacheTo(tacheParente, &tache);

                    treeView.addTache(tacheParente, &tache);
                }
            }
        } else {
//            cout<<"Tache créée à la racine"<<endl;
            treeView.addTache(projetParent, &tache);
        }
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
        Date aujourdhui;
        aujourdhui.setDateAujourdhui();
        Composite& tache = projetParent->ajouterComposite("Nouvelle tache composite", aujourdhui, aujourdhui); // XXX /!\ id

        // Déplacement de la tâche au bon endroit
        if (item->parent() != 0) { // si la tache est composante
            Tache* tacheParente = treeView.getTacheFromItem(item);
            if (typeid(*tacheParente) == typeid(Composite)){
//                cout<<"Tache déplacée"<<endl;
                projetParent->moveTacheTo(tacheParente, &tache);

                treeView.addTache(tacheParente, &tache);
            } else { // La mère n'est pas une tache composite, on la crée "à coté" de la tache unitaire sélectionnée
                cout<<"Tache parent unitaire"<<endl;
                if(item->parent()->parent() == 0){ // ie. on crée une tache dans un projet
//                    cout<<"Tache créée à la racine à coté de la TU sélectionnée"<<endl;
                    treeView.addTache(projetParent, &tache);
                } else {
                    Tache* tacheParente = treeView.getTacheFromItem(item->parent());
                    projetParent->moveTacheTo(tacheParente, &tache);

                    treeView.addTache(tacheParente, &tache);
                }
            }
        } else {
//            cout<<"Tache créée à la racine"<<endl;
            treeView.addTache(projetParent, &tache);
        }
    }
}




