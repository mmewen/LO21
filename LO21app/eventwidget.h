#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QWidget>
#include <iostream>
#include "Calendar.h"

namespace Ui {
class EventWidget;
}

/*! \class EventWidget
        \brief Classe UI affichant un évènement dans l'agenda

*/
class EventWidget : public QWidget
{
    Q_OBJECT
    //! Pointeur vers la programmation liée au widget
    Programmation* programmation; 
public:
    explicit EventWidget(  Programmation*, QWidget *parent = 0 );
    ~EventWidget();

private:
    Ui::EventWidget *ui;
};

#endif // EVENTWIDGET_H
