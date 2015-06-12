#include "eventwidget.h"
#include "ui_eventwidget.h"

EventWidget::EventWidget( Programmation* prog, QWidget *parent ) :
    QWidget(parent),
    programmation(prog),
    ui(new Ui::EventWidget)
{
    ui->setupUi(this);
    ui->titre->setText( prog->getEvenement().getNom() );
    ui->date_debut->setText( QString::fromStdString(prog->getHoraire().toString()) );
    cout<<prog->getHoraire().toString();
    ui->date_fin->setText( QString::fromStdString(prog->getHoraire().toString()) );
}

EventWidget::~EventWidget()
{
    delete ui;
}
