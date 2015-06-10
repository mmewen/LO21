#ifndef AGENDAVIEW_H
#define AGENDAVIEW_H

#include "Calendar.h"

#include <QWidget>
#include <QVBoxLayout>

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

private:
    Ui::AgendaView *ui;
};


#endif // AGENDAVIEW_H
