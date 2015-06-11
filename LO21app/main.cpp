#include "MainWindow.h"
#include <QApplication>
#include "Calendar.h"
#include "UIClasses.h"

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    ProjetManager& pjm = ProjetManager::getInstance();
    Projet& p1 = pjm.ajouterProjet("Projet 1", "pj1.xml", Date(4,6,2015));
    Unitaire& t11 = p1.ajouterUnitaire("Tache unitaire de test", Date(10,6,2015), Date(20,6,2015), Duree(1,20), true);
    Composite& t12 = p1.ajouterComposite("Tache composite de test 3", Date(15,6,2015), Date(25,6,2015));

    Projet& p2 = pjm.ajouterProjet("Projet 2", "pj1.xml", Date(4,6,2015));
    Unitaire& t21 = p2.ajouterUnitaire("Tache unitaire de test 2", Date(10,6,2015), Date(20,6,2015), Duree(1,20), true);
    Composite& t22 = p2.ajouterComposite("Tache composite de test 4", Date(15,6,2015), Date(25,6,2015));
    Unitaire& t24 = p2.ajouterUnitaire("Tache unitaire de test 5", Date(16,6,2015), Date(20,6,2015), Duree(45,20), true);
    Composite& t23 = p2.ajouterComposite("Tache bref", Date(15,6,2015), Date(25,6,2015));

    t22.addItem(&t21);
    p2.moveTacheTo(&t22, &t23);
    p2.moveTacheTo(&t23, &t24);

    MainWindow wind;
    wind.show();

    return app.exec();
}
