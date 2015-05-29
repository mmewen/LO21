#include "MainWindow.h"
#include <QApplication>
#include "Calendar.h"

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    MainWindow wind;
    wind.show();


//    QPushButton quit("Quitter", window);
//    quit.show();
//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));


    // Une fois qu'on aura une classe d'affichage
//    QTreeView::connect(vue,  SIGNAL(clicked()), &app, SLOT(quit()));

//    QPushButton::connect(&quit, SIGNAL(clicked()), &app, SLOT(quit()));

    return app.exec();
}
