#include "agendaview.h"
#include "ui_agendaview.h"

AgendaView::AgendaView(QWidget *parent) :
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

    QObject::connect(ui->semaineSelector, SIGNAL(userDateChanged(QDate)), this, SLOT(slotEventsChanged(QDate)));
    QObject::connect(ui->exporterLaSemaine, SIGNAL(clicked()), this, SLOT(slotExporter()));
    showSemaine();
}

AgendaView::~AgendaView()
{
    delete ui;
}

Date AgendaView::getLundiSelectionne(){
    QDate temp = ui->semaineSelector->date();
    temp = temp.addDays( - ( temp.dayOfWeek() - 1 ) ); // on prend le lundi en soustrayant le numéro de jour
    return Date::toTimingDate( temp );
}

void AgendaView::showSemaine(){
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

    if (nbProgs==0)
        ui->exporterLaSemaine->setEnabled(false);


    QLabel* nomJour;
    for(int k=0; k<7 ; k++){
        nomJour = new QLabel(QDate::longDayName(k+1) + ' ' + QString::fromStdString((lundi+k).getJourMoisString()));
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
//        cout<<endl<<"Hauteur : "<<nouvelEvent->height()<<endl;

        ui->exporterLaSemaine->setEnabled(true);

        // On vide la case
        progsDeLaSemaine[indice] = 0;
    }
}

void AgendaView::clearSemaine(){
    for(int k=0; k<7 ; k++){
        QLayoutItem *child;
        while ((child = layoutSemaine[k]->takeAt(0)) != 0) {
            child->widget()->deleteLater();
//             delete child;
        }
    }
}


void AgendaView::slotEventsChanged(){
    showSemaine();
}


void AgendaView::slotEventsChanged(const QDate& date){
    showSemaine();
}

void AgendaView::slotExporter(){
    try{
        QMessageBox msgBox;
        msgBox.setText("Dans un monde parfait, la semaine s'enregistre, MAINTENANT !");

        QString fileName = QFileDialog::getSaveFileName(this->parentWidget(),
                QString::fromStdString("Exporter la semaine"),
                "export_semaine.xml",
                "Fichier XML (*.xml)");

        this->save( fileName.toStdString() );
        msgBox.exec();
    }
    catch(CalendarException e){
        QMessageBox *erreur = new QMessageBox;
        erreur->setText(QString::fromStdString(e.getInfo()));
        erreur->exec();
    }
}


void AgendaView::save(const string &f){
    ostringstream numsemaine, annee;
    numsemaine << ui->semaineSelector->date().weekNumber();
    annee << ui->semaineSelector->date().year();

    QFile newfile( QString::fromStdString(f));
    if (!newfile.open(QIODevice::WriteOnly | QIODevice::Text))
        throw CalendarException("erreur sauvegarde tâches : ouverture fichier xml");
    QXmlStreamWriter stream(&newfile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("Programmation");
    stream.writeTextElement("annee",QString::fromStdString(annee.str()));
    stream.writeTextElement("semaine",QString::fromStdString(numsemaine.str()));
    stream.writeStartElement("programmations");
    Date lundi = AgendaView::getLundiSelectionne();

    // On récupère les programmations de la semaine
    Programmation* progsDeLaSemaine[100];
    int nbProgs = 0;
    for( ProgrammationManager::Iterator it = ProgrammationManager::getInstance().getIterator(); !it.isDone() && (nbProgs<100) ; it.next() ){
        if ( (lundi <= it.current().getDate()) && (it.current().getDate() < lundi+7) ){
            progsDeLaSemaine[nbProgs++] = &it.current();
        }
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

        stream.writeStartElement("programmation");
        stream.writeTextElement("date",progTemp->getDate().getQDate().toString(Qt::ISODate));
        stream.writeTextElement("horaire",QString::fromStdString(progTemp->getHoraire().toString()));
        if(typeid(progTemp->getEvenement())==typeid(Activite&)){
            stream.writeTextElement("titre",QString::fromStdString(dynamic_cast<const Activite&>(progTemp->getEvenement()).getTitre()));
            stream.writeTextElement("lieu",QString::fromStdString(dynamic_cast<const Activite&>(progTemp->getEvenement()).getLieu()));
        }
        else
            stream.writeTextElement("titre",QString::fromStdString(dynamic_cast<const Unitaire&>(progTemp->getEvenement()).getTitre()));
        QString str;
        str.setNum(progTemp->getDuree().getDureeEnMinutes());
        stream.writeTextElement("duree",str);
        stream.writeEndElement();

        // On vide la case
        progsDeLaSemaine[indice] = 0;
    }
    stream.writeEndElement();
    stream.writeEndElement();
    stream.writeEndDocument();
    newfile.close();
}
