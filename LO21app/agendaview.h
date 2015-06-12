#ifndef AGENDAVIEW_H
#define AGENDAVIEW_H

#include "Calendar.h"
#include "eventwidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <stdlib.h>
#include <string>

namespace Ui {
class AgendaView;
}

class AgendaViewClass : public QWidget
{
    Q_OBJECT
    Date getLundiSelectionne();
    QVBoxLayout* layoutSemaine[7];
    void showSemaine();
    void clearSemaine();
public:
    explicit AgendaViewClass(QWidget *parent = 0);
    ~AgendaViewClass();
public slots:
    void slotEventsChanged();
    void slotEventsChanged(const QDateTime & date);

private:
    Ui::AgendaView *ui;
};


#endif // AGENDAVIEW_H
