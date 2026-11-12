#pragma once
#include <string>
#include <vector>
#include "game.h"

struct RulesetSchnapsn : public Ruleset {
    Card adut{"",""}; //damit zb Card c; geschrieben werden kann weil kein default Konstruktor festgelegt ist (leere Karte wenig Sinn)
   

    public:        
        RulesetSchnapsn(const std::string& d); //Funktionen initialisiert
        int gewinnerStich(Trick& trick);
        std::string gewinnerSpiel(Gamestate& game);
        void Regeln();
        int Punkteverteilung(const Card& karte);
        int punktefürstich(const Trick& trick);
        bool bubtausch (const Player& player, const Gamestate& game);
        void tauscheBub(Player& player, const Gamestate& game); //hand (Objekt von Player) wird verändert deswegen nicht const
        int BonusDameKönig(const Trick& trick, const Player& player);
};


