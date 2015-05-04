#include "MainWindow.h"

MainWindow::MainWindow(){
    this->setWindowTitle(QString::fromUtf8("LO21"));
    this->setFixedSize(W_WIDTH, W_HEIGHT);

    QWidget *zoneCentrale = new QWidget;

    // Onglets
    QTabWidget *tabs = new QTabWidget(zoneCentrale);
    tabs->setFixedSize(W_WIDTH, W_HEIGHT);
    QWidget *planningTab = new QWidget();
    QWidget *tachesTab = new QWidget();
    QWidget *trucTab = new QWidget();
    tabs->addTab(planningTab,"Planning");
    tabs->addTab(tachesTab,"Tache");
    tabs->addTab(trucTab,"Truc");

    // Tree onglet Taches (modèle/vue
    QStandardItemModel *modele = new QStandardItemModel;
    QStandardItem *item = new QStandardItem("Truc");
    modele->appendRow(item);
    item->appendRow(new QStandardItem("Sous truc"));
    QStandardItem *item2 = new QStandardItem("Plep");
    modele->appendRow(item2);
    item2->appendRow(new QStandardItem("Sous plep"));
    QTreeView *vue = new QTreeView(tachesTab);
    vue->setHeaderHidden(true);
    vue->setFixedSize(200,W_HEIGHT);
    vue->setModel(modele);

//    QMenu *menuFichier = menuBar()->addMenu("&Fichier");
//    QMenu *menuEdition = menuBar()->addMenu("&Edition");
//    QAction *actionQuitter = new QAction("&Quitter", this);
//    menuFichier->addAction(actionQuitter);


    this->setCentralWidget(zoneCentrale);
}

/* Penser au QFormLayout pour l'édition des tâches
 * Ça parmet de faire :
 * layout->addRow("Votre nom", nom);
 * :)
*/
