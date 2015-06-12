#include "eventwidget.h"
#include "ui_eventwidget.h"

EventWidget::EventWidget( Programmation* prog, QWidget *parent) :
    QWidget(parent),
    programmation(prog),
    ui(new Ui::EventWidget)
{
    ui->setupUi(this);
    ui->titre->setText( prog->getEvenement().getNom() );
    ui->date_debut->setText( QString::fromStdString(prog->getHoraire().toString()) );
    cout<<prog->getHoraire().toString();
    ui->date_fin->setText( QString::fromStdString( (prog->getHoraire() + prog->getDuree()).toString()) );

    // On met une taille proportionnelle à la durée
    this->setMinimumHeight( (4*(prog->getDuree().getDureeEnMinutes() * 500) / (24*60))<100?100:((prog->getDuree().getDureeEnMinutes() * 500) / (24*60)) );
//    cout<<endl<<"Hauteur idéale:"<<((prog->getDuree().getDureeEnMinutes() * 500) / (24*60))<<endl<<"Hauteur réelle:"<<this->height()<<endl;
}

EventWidget::~EventWidget()
{
    delete ui;
}
