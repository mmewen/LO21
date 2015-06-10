#include "agendaview.h"
#include "ui_agendaview.h"

AgendaView::AgendaView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgendaView)
{
    ui->setupUi(this);
}

AgendaView::~AgendaView()
{
    delete ui;
}
