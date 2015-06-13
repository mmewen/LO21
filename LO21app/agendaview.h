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
    Date getLundiSelectionne(); //! Retourne la date du lundi du début de la semaine du jour sélectionné dans le calendrier
    QVBoxLayout* layoutSemaine[7]; //! Un tableau de pointeurs sur les layouts verticaux des jours de la semaine
    void showSemaine(); //! Affiche la semaine correspondant à la date sélectionnée
    void clearSemaine(); //! Efface la semaine
    void save(const string &f); //! Exporte la semaine en XML
public:
    explicit AgendaView(QWidget *parent = 0);
    ~AgendaView();
public slots:
    void slotEventsChanged(); //! Appelle l'affichage de semaine
    void slotEventsChanged(const QDate& date); //! Appelle l'affichage de semaine
    void slotExporter(); //! Appelle l'exportation

private:
    Ui::AgendaView *ui;
};


#endif // AGENDAVIEW_H
