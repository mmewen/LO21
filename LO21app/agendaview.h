#ifndef AGENDAVIEW_H
#define AGENDAVIEW_H

#include "Calendar.h"
#include "eventwidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <stdlib.h>
#include <string>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class AgendaView;
}

/*! \class AgendaView
    \brief Classe d'UI gérant le contenu de l'onglet Agenda

Contient la barre de titre avec le choix de semaine et l'exportation
ainsi que l'ensemble de layouts des jours de la semaine.
*/
class AgendaView : public QWidget
{
    Q_OBJECT
    //! Retourne la date du lundi du début de la semaine du jour sélectionné dans le calendrier
    Date getLundiSelectionne(); 
    //! Un tableau de pointeurs sur les layouts verticaux des jours de la semaine
    QVBoxLayout* layoutSemaine[7]; 
    //! Affiche la semaine correspondant à la date sélectionnée
    void showSemaine(); 
    //! Efface la semaine
    void clearSemaine(); 
    //! Exporte la semaine en XML
    void save(const string &f); 
public:
    explicit AgendaView(QWidget *parent = 0);
    ~AgendaView();
public slots:
    //! Appelle l'affichage de semaine
    void slotEventsChanged(); 
    //! Appelle l'affichage de semaine
    void slotEventsChanged(const QDate& date); 
    //! Appelle l'exportation
    void slotExporter(); 

private:
    Ui::AgendaView *ui;
};


#endif // AGENDAVIEW_H
