#include "eventwidget.h"
#include "ui_eventwidget.h"

EventWidget::EventWidget( Programmation* prog, QWidget *parent ) :
    QWidget(parent),
    programmation(prog),
    ui(new Ui::EventWidget)
{
    ui->setupUi(this);
    ui->titre->setText( prog->getEvenement().getNom() );
}

EventWidget::~EventWidget()
{
    delete ui;
}
