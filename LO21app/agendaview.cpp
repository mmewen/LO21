#include "agendaview.h"
#include "ui_agendaview.h"

AgendaViewClass::AgendaViewClass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgendaView)
{
    ui->setupUi(this);
    ui->semaineSelector->setDate(QDate::currentDate());
    // Si on change, on affiche les taches qu'il faut

    // Initialisation du beau tableau de QHBoxLayout
    layoutSemaine[0] = ui->vLayoutLundi;
    layoutSemaine[1] = ui->vLayoutMardi;
    layoutSemaine[2] = ui->vLayoutMercredi;
    layoutSemaine[3] = ui->vLayoutJeudi;
    layoutSemaine[4] = ui->vLayoutVendredi;
    layoutSemaine[5] = ui->vLayoutSamedi;
    layoutSemaine[6] = ui->vLayoutDimanche;

    connect(ui->semaineSelector, SIGNAL(editingFinished()), this, SLOT(slotEventsChanged()));

    showSemaine();
}

AgendaViewClass::~AgendaViewClass()
{
    delete ui;
}

Date AgendaViewClass::getLundiSelectionne(){
    QDate temp = ui->semaineSelector->date();
    temp = temp.addDays( - ( temp.dayOfWeek() - 1 ) ); // on prend le lundi en soustrayant le numéro de jour
    return Date::toTimingDate( temp );
}

void AgendaViewClass::showSemaine(){
    clearSemaine();

    // On met le titre bien
    ostringstream numsemaine, annee;
    numsemaine << ui->semaineSelector->date().weekNumber();
    annee << ui->semaineSelector->date().year();
    ui->titre->setText( QString::fromStdString("Semaine numéro "+numsemaine.str()+", année "+annee.str() ) );


    Date lundi = getLundiSelectionne();

    // On récupère les programmations de la semaine
    Programmation* progsDeLaSemaine[100];
    int nbProgs = 0;
    for( ProgrammationManager::Iterator it = ProgrammationManager::getInstance().getIterator(); !it.isDone() && (nbProgs<100) ; it.next() ){
        if ( (lundi <= it.current().getDate()) && (it.current().getDate() < lundi+7) ){
            progsDeLaSemaine[nbProgs++] = &it.current();
        }
    }

    cout<<nbProgs<<" programmations cette semaine"<<endl;

    QLabel* nomJour;
    for(int k=0; k<7 ; k++){
        nomJour = new QLabel(QDate::longDayName(k+1));
        layoutSemaine[k]->setAlignment(Qt::AlignTop | Qt::AlignCenter);
        layoutSemaine[k]->addWidget( nomJour );
    }


    for (int i = nbProgs-1 ; i>=0 ; i--){
        // On cherche la plus tôt pas encore affichée
        Programmation* progTemp = 0;
        int indice = -1;

        for (int j = 0 ; j < nbProgs ; j++){
            if ( (progsDeLaSemaine[j] != 0) && ( (progTemp == 0) || ((progTemp != 0) && ( progsDeLaSemaine[j]->getDate() < progTemp->getDate() )))){
                progTemp = progsDeLaSemaine[j];
                indice = j;
            }
        }
        if ((indice == -1) || (progTemp == 0)){
            throw CalendarException("Erreur lors de la recherche de la tâche la plus tôt");
        }

        // On l'affiche
        EventWidget* nouvelEvent = new EventWidget( progTemp );
        layoutSemaine[ progTemp->getDate().getQDate().dayOfWeek() - 1 ]->addWidget( nouvelEvent );


        // Si besoin, on l'ajoute sur le jour suivant

        // On vide la case
        progsDeLaSemaine[indice] = 0;
    }
}

void AgendaViewClass::clearSemaine(){
    for(int k=0; k<7 ; k++){
        QLayoutItem *child;
        while ((child = layoutSemaine[k]->takeAt(0)) != 0) {
            child->widget()->deleteLater();
//             delete child;
        }
    }
}


void AgendaViewClass::slotEventsChanged(){
    showSemaine();
}


void AgendaViewClass::slotEventsChanged(const QDateTime & date){
    showSemaine();
}
