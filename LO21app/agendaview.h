#ifndef AGENDAVIEW_H
#define AGENDAVIEW_H

#include <QWidget>

namespace Ui {
class AgendaView;
}

class AgendaView : public QWidget
{
    Q_OBJECT

public:
    explicit AgendaView(QWidget *parent = 0);
    ~AgendaView();

private:
    Ui::AgendaView *ui;
};

#endif // AGENDAVIEW_H
