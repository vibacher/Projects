#pragma once
#include <vector>
#include <string>

struct Card { //da struct alles public
    std::string farbe; //Karte besitzt farbe und rang
    std::string rang;

    Card(const std::string& f, const std::string& r); //Konstruktor

    std::string getFarbe() const; //welche Funktionen Card besitzt
    std::string getRang() const;
    std::string giveCard() const;
};

struct CardPile {
    std::vector<Card> karten;

    Card zieheKarte(); //Funktionen die CardPile besitzt
    Card entferneKarte(int index); 
    void addCard(const Card& c);
    int size() const;
};

CardPile leseKarteausDatei(const std::string& dateiname); //Funktion, um die verwendeten Karten einzulesen