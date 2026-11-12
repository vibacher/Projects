#pragma once
#include <string>
#include <vector>
#include "game.h"
#include "cardpiles.h"


struct Player;

struct RulesetWizard : public Ruleset{
    public: 
        std::vector<int> Stichvorhersage; //vektor der die Stichvorhersage speichert
        std::vector<int> SticheRunde;

        RulesetWizard(const std::string& d); //Funktionen initialisiert
        std::string TrumpfWizard(Gamestate& game, Player* aktuellerAusteiler);
        int gewinnerStich(Trick& trick, Gamestate& game);
        void PunkteRunde(Gamestate& game);
        std::string gewinnerSpiel(Gamestate& game);
        void Regeln();
        bool karteErlaubt(const CardPile& hand, const Card& karte, const std::string& trumpf, const Trick& trick);

};