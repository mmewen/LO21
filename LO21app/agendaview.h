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

class AgendaView : public QWidget
{
    Q_OBJECT
    Date getLundiSelectionne();
    QVBoxLayout* layoutSemaine[7];
    void showSemaine();
    void clearSemaine();
    void save(const string &f);
public:
    explicit AgendaView(QWidget *parent = 0);
    ~AgendaView();
public slots:
    void slotEventsChanged();
    void slotEventsChanged(const QDate& date);
    void slotExporter();

private:
    Ui::AgendaView *ui;
};


#endif // AGENDAVIEW_H
