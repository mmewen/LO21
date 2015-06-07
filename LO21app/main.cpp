#include "MainWindow.h"
#include <QApplication>
#include "Calendar.h"
#include "UIClasses.h"

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

//    Projet proj("1", "Projet machin", "truc.xml", Date(9,6,2015));
//    Unitaire t1 = proj.ajouterUnitaire("t1", "Tache unitaire de test", Date(10,6,2015), Date(20,6,2015), Duree(1,20), true);

//    Unitaire t1 = Unitaire (Date(10,6,2015), Date(20,6,2015), "t1", "Tache unitaire de test", Duree(1,20), true);
//    Composite t2 = Composite("t2", "Tache composite de test", Date(15,6,2015), Date(25,6,2015));

    ProjetManager& pjm = ProjetManager::getInstance();
    Projet& p1 = pjm.ajouterProjet("Projet 1", "pj1.xml", Date(4,6,2015));
    Unitaire& t11 = p1.ajouterUnitaire("Tache unitaire de test", Date(10,6,2015), Date(20,6,2015), Duree(1,20), true);
    Composite& t12 = p1.ajouterComposite("Tache composite de test 3", Date(15,6,2015), Date(25,6,2015));

    Projet& p2 = pjm.ajouterProjet("Projet 2", "pj1.xml", Date(4,6,2015));
    Unitaire& t21 = p2.ajouterUnitaire("Tache unitaire de test 2", Date(10,6,2015), Date(20,6,2015), Duree(1,20), true);
    Composite& t22 = p2.ajouterComposite("Tache composite de test 4", Date(15,6,2015), Date(25,6,2015));
    Unitaire& t24 = p2.ajouterUnitaire("Tache unitaire de test 5", Date(16,6,2015), Date(20,6,2015), Duree(45,20), true);
    Composite& t23 = p2.ajouterComposite("Tache bref", Date(15,6,2015), Date(25,6,2015));


//    t22.addCompo(&t23);
    p2.moveTacheTo(&t22, &t23);
    p2.moveTacheTo(&t23, &t24);

    MainWindow wind;
    wind.show();



//    wind.showUnitaire(t11);
//    wind.showComposite(t2);



//    QPushButton quit("Quitter", window);
//    quit.show();
//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));


    // Une fois qu'on aura une classe d'affichage
//    QTreeView::connect(vue,  SIGNAL(clicked()), &app, SLOT(quit()));

//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));

    return app.exec();
}
