#pragma once
#include <string>
#include "cardpiles.h"


struct Trick; //damit Compiler weiß dass sie existieren
struct Gamestate;
struct Ruleset;

struct Player {
    CardPile hand; //Player besitzt eine Hand und einen Namen
    std::string name;
    virtual bool isHuman() = 0; //virtual damit an Unterklassen weitervererbt

    virtual Card chooseCard(Trick&, Gamestate&, Ruleset&) = 0; //Funktionen die ein Player hat
    virtual int Stichvorhersage(int runde, const Gamestate& game, const Ruleset& rules) = 0;
    virtual int TrumpfWizardWahl() = 0;
    virtual ~Player() = default; //Destruktor auch für Unterklassen
};

struct Human : public Player {//Unterklassen erben die Eigenschaften von Player
    Human(const std::string& n);
    Card chooseCard(Trick&, Gamestate&, Ruleset&);
    int Stichvorhersage(int runde, const Gamestate& game, const Ruleset& rules);
    int TrumpfWizardWahl();
    bool isHuman();

};

struct AIPlayer_Random : public Player {//Unterklassen erben die Eigenschaften von Player
    AIPlayer_Random(const std::string& n);
    Card chooseCard(Trick&, Gamestate&, Ruleset&);
    int Stichvorhersage(int runde, const Gamestate& game, const Ruleset& rules);
    int TrumpfWizardWahl();
    bool isHuman();
};


struct AIPlayer_Best : public Player{//Unterklassen erben die Eigenschaften von Player
    AIPlayer_Best(const std::string& n);
    Card chooseCard(Trick&, Gamestate&, Ruleset&);
    int Stichvorhersage(int runde, const Gamestate& game, const Ruleset& rules);
    int TrumpfWizardWahl();
    bool isHuman();
};
