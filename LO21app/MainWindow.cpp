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

//    showProject();
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

void MainWindow::showUnitaire(const Unitaire& t){
    QLineEdit *titre = new QLineEdit;

    QCalendarWidget *dispo = new QCalendarWidget;
    dispo->setMinimumDate(QDate::currentDate());

    QCalendarWidget *echeance = new QCalendarWidget;
    echeance->setMinimumDate(QDate::currentDate());

    QSpinBox *duree = new QSpinBox;
    QCheckBox *preemptible = new QCheckBox;

    QPushButton *predecesseurs = new QPushButton("Gérer les contraintes de précédence");
    QPushButton *annuler = new QPushButton("Annuler les modifications");
    QPushButton *sauver = new QPushButton("Sauver les changements");

    // Remplissage des champs
    titre->setText(QString::fromStdString(t.getTitre()));
    dispo->setSelectedDate(t.getDateDisponibilite().getQDate());
    echeance->setSelectedDate(t.getDateEcheance().getQDate());
    duree->setValue(t.getDuree().getDureeEnMinutes());
    preemptible->setChecked(t.isPreemp());

    tachesFormLayout = new QFormLayout;
    tachesFormLayout->addRow("Titre de la tache", titre);
    tachesFormLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tôt que le début du projet !
    tachesFormLayout->addRow("Échéance", echeance); // vérifier qu'elle est après la disponibilité
    tachesFormLayout->addRow("Durée (en minutes)", duree);
    tachesFormLayout->addRow("Préemptible", preemptible);


    // afficher durée faite
    //          + durée restante

    // mettre des taches prédécesseurs
    tachesFormLayout->addRow("", predecesseurs);
    tachesFormLayout->addRow("", annuler);
    tachesFormLayout->addRow("", sauver);


    tachesLayout->addLayout(tachesFormLayout);
}

void saveUnitaire(){
    QDate date = QDate::fromString("1MM12car2003", "d'MM'MMcaryyyy");
}

void MainWindow::showComposite(const Composite& t){
    QLineEdit *titre = new QLineEdit;

    QCalendarWidget *dispo = new QCalendarWidget;
    dispo->setMinimumDate(QDate::currentDate());

    QCalendarWidget *echeance = new QCalendarWidget;
    echeance->setMinimumDate(QDate::currentDate());

    QPushButton *predecesseurs = new QPushButton("Gérer les contraintes de précédence");
    QPushButton *annuler = new QPushButton("Annuler les modifications");
    QPushButton *sauver = new QPushButton("Sauver les changements");

    // Remplissage des champs
    titre->setText(QString::fromStdString(t.getTitre()));
    dispo->setSelectedDate(t.getDateDisponibilite().getQDate());
    echeance->setSelectedDate(t.getDateEcheance().getQDate());

    tachesFormLayout = new QFormLayout;
    tachesFormLayout->addRow("Titre de la tache", titre);
    tachesFormLayout->addRow("Disponibilité", dispo); // vérifier que ça n'est pas plus tôt que le début du projet !
    tachesFormLayout->addRow("Échéance", echeance); // vérifier qu'elle est après la disponibilité


    // afficher durée faite
    //          + durée restante

    // mettre des taches prédécesseurs
    tachesFormLayout->addRow("", predecesseurs);
    tachesFormLayout->addRow("", annuler);
    tachesFormLayout->addRow("", sauver);


    tachesLayout->addLayout(tachesFormLayout);
}


