#include "MainWindow.h"
#include <QApplication>
#include "Calendar.h"

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    Unitaire t1 = Unitaire(Date(10,6,2015), Date(20,6,2015), "t1", "Tache unitaire de test", Duree(1,20), true);

    MainWindow wind;
    wind.show();

    wind.showUnitaire(t1);
//    QPushButton quit("Quitter", window);
//    quit.show();
//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));


    // Une fois qu'on aura une classe d'affichage
//    QTreeView::connect(vue,  SIGNAL(clicked()), &app, SLOT(quit()));

//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));

    return app.exec();
}
