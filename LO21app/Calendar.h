#ifndef CALENDAR_h
#define CALENDAR_h
#include <string>
#include <iostream>
#include "timing.h"
using namespace std;
using namespace TIME;

/// \class CalendarException
/// \brief Classe d'affichage des erreurs
///
class CalendarException{
public:
	CalendarException(const string& message):info(message){}
	string getInfo() const { return info; }
private:
	string info;
};

//IL MANQUE TOUS LES CONSTRUCTEURS/DESTRUCTEURS...
class Truc/*à changer bien sûr !*/ {
public:
	Date dispo;
	Date getDispo() const { return dispo; }
};

/// \class Tache
/// \brief Classe abstraite mère de toutes les taches.
///     Comporte un titre, une échéance et une liste de précédences (taches à réaliser avant de pouvoir à commencer celle-ci)
///
class Tache : public Truc/*à changer*/ {
private:
	Tache** precedentes;
	unsigned int nbPred;
	unsigned int nbMaxPred;
	string identificateur;
	Tache(const Tache& t);
	Tache& operator=(const Tache& t);
public:
    string titre;
    Date echeance;
	//bool programmee;
    //! accesseur
	string getId() const { return identificateur; }
    //! accesseur
	string getTitre() const { return titre; }
    //! accesseur
	Date getEcheance() const { return echeance; }
	friend class Iterator;
	class Iterator{
    private:
		Tache** tab;
		int nb;
		int indice_tache;
		friend class Tache;
		Iterator(Tache** t, int n):tab(t), nb(n), indice_tache(0){}
	public:
		Tache& current() const {
			if(indice_tache>=nb)
				throw "incrementation d'un iterateur en fin de sequence";
			return *tab[indice_tache];
		}
		bool isDone() const { return indice_tache==nb; }
		void next(){ ++indice_tache; }
		void first(){ indice_tache=0; }
	};
	Iterator getIterator(){ return Iterator(precedentes,nbPred); }
};

/// \class Unitaire
/// \brief Tache programmable
///
class Unitaire : public Tache {
public:
	Duree duree; //si non-preemtable -> duree<=12h
	Duree dureeRestante;
	Duree dureeFaite;
	bool preemptable;
    //! accesseur
	Duree getDuree() const { return duree; }
    //! accesseur
	bool isPreemp() const { return preemptable; }
    //! accesseur
	Duree getFait() const { return dureeFaite; }
    //! accesseur, retour calculé
	Duree getRestant() const; //se calcule facilement ?
};

/// \class Composite
/// \brief Tache qui peut contenir des tâches mais ne peut pas être programmée
///
class Composite : public Tache {
private:
	Tache** composition;
	unsigned int nbCompo;
	unsigned int nbMaxCompo;
public:
    friend class iterator;
	class iterator{
    private:
		Tache** tab;
		int nb;
		int indice_tache;
		friend class Composite;
		iterator(Tache** t, int n):tab(t), nb(n), indice_tache(0){}
	public:
		Tache& current() const {
			if(indice_tache>=nb)
				throw "incrementation d'un iterateur en fin de sequence";
			return *tab[indice_tache];
		}
		bool isDone() const { return indice_tache==nb; }
		void next(){ ++indice_tache; }
		void first(){ indice_tache=0; }
	};
	iterator getiterator(){ return iterator(composition,nbCompo); }
};

class Activite/*ou Evenement*/ {
private:
	string titre;
	string lieu;
	Duree duree;
public:
    //! accesseur
	string getTitre() const { return titre; }
    //! accesseur
	string getLieu() const { return lieu; }
    //! accesseur
	Duree getDuree() const { return duree; }
};

/// \class Projet
/// \brief Un ensemble de tache à réaliser
///
class Projet {
private:
	Tache** taches;
	unsigned int nb;
	unsigned int nbMax;
	//string identificateur;
	string nom;
public:
	//string getId() const { return identificateur; }
    //! accesseur
	string getNom() const { return nom; }
	friend class Iterator;
	class Iterator{
    private:
		Tache** tab;
		int nb;
		int indice_tache;
		friend class Projet;
		Iterator(Tache** t, int n):tab(t), nb(n), indice_tache(0){}
	public:
		Tache& current() const {
			if(indice_tache>=nb)
				throw "incrementation d'un iterateur en fin de sequence";
			return *tab[indice_tache];
		}
		bool isDone() const { return indice_tache==nb; }
		void next(){ ++indice_tache; }
		void first(){ indice_tache=0; }
	};
	Iterator getIterator(){ return Iterator(taches,nb); }
};

class Programmation { // ne pas faire 2 programmations qui se chevauchent
	const Unitaire* tache; /*et Activite* - eventuellement besoin d'un superclasse qui englobe Unitaire et Activite ?*/
	Date date;
	Horaire horaire;
public:
	Programmation(const Unitaire& t, const Date& d, const Horaire& h):tache(&t), date(d), horaire(h){}
    //! accesseur
	const Tache& getTache() const { return *tache; }
    //! accesseur
	Date getDate() const { return date; }
    //! accesseur
	Horaire getHoraire() const { return horaire; }
};

/// \class ProgrammationManager
/// \brief Gestion des évènements ou taches programmés
///
class ProgrammationManager {
private:
	Programmation** programmations;
	unsigned int nb;
	unsigned int nbMax;
	void addItem(Programmation* t);
	Programmation* trouverProgrammation(const Unitaire& t) const;
public:
	ProgrammationManager();
	~ProgrammationManager();
	ProgrammationManager(const ProgrammationManager& um);
	ProgrammationManager& operator=(const ProgrammationManager& um);
	void ajouterProgrammation(const Unitaire& t, const Date& d, const Horaire& h);
};

ostream& operator<<(ostream& f, const Unitaire& t);
ostream& operator<<(ostream& f, const Programmation& p);

#endif
