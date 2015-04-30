#include "Calendar.h"
#include <fstream>

ostream& operator<<(ostream& fout, const Unitaire& t){
	fout<<t.getId()<<"\n";
	fout<<t.getTitre()<<"\n";
	fout<<t.getDispo()<<"\n";
	fout<<t.getEcheance()<<"\n";
	fout<<t.getDuree()<<"\n";
	return fout;
}

ostream& operator<<(ostream& f, const Programmation& p);

ProgrammationManager::ProgrammationManager():programmations(0),nb(0),nbMax(0){}

void ProgrammationManager::addItem(Programmation* t){
	if (nb==nbMax){
		Programmation** newtab=new Programmation*[nbMax+10];
		for(unsigned int i=0; i<nb; i++) newtab[i]=programmations[i];
		nbMax+=10;
		Programmation** old=programmations;
		programmations=newtab;
		delete[] old;
	}
	programmations[nb++]=t;
}

Programmation* ProgrammationManager::trouverProgrammation(const Unitaire& t)const{
	for(unsigned int i=0; i<nb; i++)
		if (&t==&programmations[i]->getTache()) return programmations[i];
	return 0;
}

void ProgrammationManager::ajouterProgrammation(const Unitaire& t, const Date& d, const Horaire& h){
	if (trouverProgrammation(t)) throw CalendarException("erreur, ProgrammationManager, Programmation deja existante");	
	Programmation* newt=new Programmation(t,d,h);
	addItem(newt);
}

ProgrammationManager::~ProgrammationManager(){
	for(unsigned int i=0; i<nb; i++) delete programmations[i];
	delete[] programmations;
}

ProgrammationManager::ProgrammationManager(const ProgrammationManager& um):nb(um.nb),nbMax(um.nbMax), programmations(new Programmation*[um.nb]){
	for(unsigned int i=0; i<nb; i++) programmations[i]=new Programmation(*um.programmations[i]);
}

ProgrammationManager& ProgrammationManager::operator=(const ProgrammationManager& um){
	if (this==&um) return *this;
	this->~ProgrammationManager();
	for(unsigned int i=0; i<um.nb; i++) addItem(new Programmation(*um.programmations[i]));
	return *this;
}
