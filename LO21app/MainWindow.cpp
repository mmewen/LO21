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
    QLineEdit *nom = new QLineEdit;
    QLineEdit *prenom = new QLineEdit;
    QLineEdit *age = new QLineEdit;

    tachesFormLayout = new QFormLayout;
    tachesFormLayout->addRow("Votre nom", nom);
    tachesFormLayout->addRow("Votre prénom", prenom);
    tachesFormLayout->addRow("Votre âge", age);

    tachesLayout->addLayout(tachesFormLayout);

}

/* Penser au QFormLayout pour l'édition des tâches
 * Ça parmet de faire :
 * layout->addRow("Votre nom", nom);
 * :)
*/




