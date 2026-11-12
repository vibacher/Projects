#pragma once
#include <vector>
#include <string>
#include "cardpiles.h"
#include "player.h"


//vordefiniert, damit man in Trick, Gamestate und Ruleset darauf zugreifen kann
struct Ruleset;

class Trick {
    public:
        std::vector<Card> gespielteKarten;
        std::vector<Player*> spieler; //* zeigt auf die Spieler in Player initialisiert
        int gewinner_index = -1; //vor Festlegung -1, damit niemanden ausversehen punkte gegeben werden

        void spieleKarte(Player* player, const Card& karte); //Funktionen initialisieren

        void Gewinner(int index){gewinner_index = index;};

        std::string getWinner() const;
    };

struct Gamestate {
    int Stichnummer = 0;
    int aktuellerSpieler = 0;
    std::string Trumpffarbe; //welche Farbe in der jeweiligen Runde Trumpf ist, globales Trumpf ist bezeichnet mit adut
    std::vector<int> Punktestand; //verwaltet den Punktestand
    CardPile Nachziehstapel;
    CardPile Ablagestapel;
    Trick aktuellerStich;

    std::vector<Player*> spieler; //zeigt ebenfalls auf player

    void addPlayer(Player& player); //Funktionen initialisieren
};

struct Ruleset {
    CardPile kartendeck;
    std::string dateiname;
    std::vector<Card> karten;
    std::string stichfarbe; //stich in der jeweiligen runde
    int kartenProSpieler;

    virtual ~Ruleset() = default;
    void ladeKartendeck(const std::string& dateiname); //funktionen initialisieren
    bool karteErlaubt(const CardPile& hand, const Card& karte, const std::string& trumpf, const Trick& trick);
    std::string gewinnerStich(const Trick& trick); 
    std::string gewinnerSpiel(const Gamestate& game); 
    void Regeln();
};

