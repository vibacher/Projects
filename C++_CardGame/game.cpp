#include "game.h"
#include "player.h"
#include "cardpiles.h"
#include <iostream>

void Trick::spieleKarte(Player* player, const Card& karte) { //speichert karte und zugehörigen Spieler
    gespielteKarten.push_back(karte);
    spieler.push_back(player);
}


std::string Trick::getWinner() const { //Gewinner eines Stiches
        std::string name = "Gewinner unbekannt";

        if (gewinner_index >= 0 && gewinner_index < (int)spieler.size()) {
            name = spieler[gewinner_index]->name;
        }

        return name;
    }

void Gamestate::addPlayer(Player& player){//fügt Spieler hinzu
    spieler.push_back(&player);
}



void Ruleset::ladeKartendeck(const std::string& dateiname){
    kartendeck = leseKarteausDatei(dateiname); 
}


bool Ruleset::karteErlaubt(const CardPile& hand, const Card& karte, const std::string&, const Trick& trick){
    if (trick.gespielteKarten.empty()) {
        return true; //die erste Karte die gespielt wird kann alles sein
    }

    std::string stichfarbe = trick.gespielteKarten[0].farbe; //erste ausgespielte Karte ist trumpffarbe, die auch Gamestate hat

    bool hatStichfarbe = false;
    for (const auto& c : hand.karten) { //schauen ob Trumpffarbe besitzt
        if (c.farbe == stichfarbe) {
            hatStichfarbe = true;
            break;
        }
    }
    
    if (hatStichfarbe) {
        return karte.farbe == stichfarbe;
    }

    return true; //falls nicht Trumpffarbe alle Karten erlaubt
}

