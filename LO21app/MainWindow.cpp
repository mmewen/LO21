#include "MainWindow.h"

//! initialisation de la fenêtre principale
MainWindow::MainWindow():
    zoneCentrale(new QWidget),
    tabs(new QTabWidget(zoneCentrale)),
    planningTab(new QWidget()),
    tachesTab(new QWidget()),
    tachesLayout(new QHBoxLayout)
    {
    this->setWindowTitle(QString::fromUtf8("LO21"));
    this->setFixedSize(W_WIDTH, W_HEIGHT);

    // Onglets
    QTabWidget *tabs = new QTabWidget(zoneCentrale);
    tabs->setFixedSize(W_WIDTH, W_HEIGHT);

    tabs->addTab(planningTab,"Planning");
    tabs->addTab(tachesTab,"Tache");

    // Tree onglet Taches (modèle/vue
    QStandardItemModel *modele = new QStandardItemModel;
    QStandardItem *item = new QStandardItem("Truc");
    item->setEditable(false);
    modele->appendRow(item);
    item->appendRow(new QStandardItem("Sous truc"));
    QStandardItem *item2 = new QStandardItem("Plep");
    item2->setEditable(false);
    modele->appendRow(item2);
    item2->appendRow(new QStandardItem("Sous plep"));
    QTreeView *vue = new QTreeView();
    tachesLayout->addWidget(vue);
    vue->setHeaderHidden(true);
    vue->setFixedSize(200,W_HEIGHT);
    vue->setModel(modele);

    QMenu *menuFichier = menuBar()->addMenu("&Fichier");
//    QMenu *menuEdition = menuBar()->addMenu("&Edition");
    QAction *actionQuitter = new QAction("&Quitter", this);
    menuFichier->addAction(actionQuitter);
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    tachesTab->setLayout(tachesLayout);
    this->setCentralWidget(zoneCentrale);

    showProject();
}

//! affichage du contenu d'un projet
void MainWindow::showProject(){
    QLineEdit *titre = new QLineEdit;
    QLineEdit *dispo = new QLineEdit;

    tachesFormLayout = new QFormLayout;
    tachesFormLayout->addRow("Titre du projet", titre);
    tachesFormLayout->addRow("Disponibilité", dispo); // si on bouge la date, on bouge aussi les dates de début des taches
    // nb de taches actuelles
    // échéance calculée
    // afficher l'identifiant

    // bouton -> enregistrer

    tachesLayout->addLayout(tachesFormLayout);

}

void MainWindow::showUnitaire(){
    QLineEdit *titre = new QLineEdit;
    QLineEdit *dispo = new QLineEdit;
    QLineEdit *echeance = new QLineEdit;
    QLineEdit *duree = new QLineEdit;
    QCheckBox *preemptible = new QCheckBox;

    tachesFormLayout = new QFormLayout;
    tachesFormLayout->addRow("Titre de la tache", titre);
    tachesFormLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tôt que le début du projet !
    tachesFormLayout->addRow("Échéance", echeance); // vérifier qu'elle est après la disponibilité
    tachesFormLayout->addRow("Durée", duree);
    tachesFormLayout->addRow("Préemptible", preemptible);
    // afficher durée faite
    //          + durée restante


    tachesLayout->addLayout(tachesFormLayout);

}

void saveUnitaire(){
    QDate date = QDate::fromString("1MM12car2003", "d'MM'MMcaryyyy");
}



