#include "cardpiles.h"
#include <fstream>
#include <iostream>

Card::Card(const std::string& f, const std::string& r) : farbe(f), rang(r) {} //Konstruktor

std::string Card::getFarbe() const {return farbe; }
std::string Card::getRang() const {return rang; }
std::string Card::giveCard() const {return farbe + " " + rang; }

Card CardPile::zieheKarte() {
    if (karten.empty()) { //damit leere Vektoren kein Problem machen
        std::cerr << "Achtung das Kartendeck ist leer!\n";
    }

    Card c = karten.back();
    karten.pop_back(); //löscht die Karte aus dem Stapel
    return c;
}

void CardPile::addCard(const Card& c){ //gibt die Karte hinzu
    karten.push_back(c);
}

int CardPile::size() const { //wie viele Karten in Stapel
    return karten.size();
}

CardPile leseKarteausDatei(const std::string& dateiname){
    CardPile kartendeck; 
    std::ifstream datei(dateiname);

    if (!datei) { //falls einlesen nicht funktionieren sollte
        std::cerr << "Die Karten können nicht gelesen werden:/";
        return kartendeck;
    }

    std::string farbe, rang;
    while (datei >> farbe >> rang) { //liest Karten ein, Format: Farbe Rang
        kartendeck.addCard(Card(farbe, rang)); //eingelesene Karte wird dem Kartendeck hinzugefügt
    }

    return kartendeck;
}

Card CardPile::entferneKarte(int index) { //um eine BESTIMMTE Karte zu nehmen keine zufällige
    if (index < 0 || index >= (int)karten.size()) {
        std::cerr << "Ungültiger Kartenindex!\n";
    }

    Card c = karten[index];

    karten.erase(karten.begin() + index); //karte entfernen
    return c;
}





