#pragma once
#include <string>
#include <vector>
#include "game.h"
#include "cardpiles.h"




struct RulesetSchereSteinPapier : public Ruleset{
    public: 
        int Gewinnpunkte;
        std::string dateiname;
        
        RulesetSchereSteinPapier() : dateiname("") {} //Funktionen initialisiert
        int gewinnerStich(Trick& trick, Gamestate& game);
        std::string gewinnerSpiel(Gamestate& game);
        void Regeln();
        bool karteErlaubt(); //damit ich einfach chooseCard aufrufen kann 
};