#include "scheresteinpapier.h"
#include "game.h"
#include "player.h"
#include "cardpiles.h"
#include <iostream>
#include <algorithm>



int RulesetSchereSteinPapier::gewinnerStich(Trick& trick, Gamestate& game){
    int gewinner = -1;
    bool schere = false, stein = false, papier = false;
    std::string sieg = "";

    for (auto& gespielte: trick.gespielteKarten){ //was im Spiel ist
        if (gespielte.farbe == "Schere"){schere = true;}
        if (gespielte.farbe == "Stein"){stein = true;}
        if (gespielte.farbe == "Papier"){papier = true;}
    }

    if (schere && papier && !stein){sieg = "Schere";} //gewinnerkarte
    else if (papier && stein && !schere){sieg = "Papier";}
    else if (stein && schere && !papier){sieg = "Stein";}

    int count = 0;
    for (int i = 0; i < trick.gespielteKarten.size(); ++i){
        if (trick.gespielteKarten[i].farbe == sieg){
            count++;
            gewinner = i;
        }
    }

    if (count != 1){
        std::cout <<"Unentschieden!" << std::endl;
        return -1;
    } else {
        trick.Gewinner(gewinner);
        Player* gewinnerSpieler = trick.spieler[gewinner];
        auto it = std::find(game.spieler.begin(), game.spieler.end(), gewinnerSpieler);
        int spielerIndex = std::distance(game.spieler.begin(), it);
        game.Punktestand[spielerIndex]++;

        return gewinner;
    }
}



std::string RulesetSchereSteinPapier::gewinnerSpiel(Gamestate& game){ //gewinner des Spiels bestimmen
    for (int i = 0; i < game.Punktestand.size(); ++i){
        if (game.Punktestand[i] >= Gewinnpunkte){
            return game.spieler[i]->name;
        }
    }
    return "";
}


void RulesetSchereSteinPapier::Regeln(){
    std::cout << "Willkommen bei Schere, Stein, Papier! Die Regeln sind simpel. Am Anfang wird beschlossen, ab wie vielen Punkten jemand gewinnt. " <<
    "Schere schlägt Papier, Papier schlägt Stein, Stein schlägt Schere. Wenn ein Unentschieden herrscht, bekommt niemand Punkte" << std::endl;
}


bool RulesetSchereSteinPapier::karteErlaubt(){return true;} //alle Karten sind immer erlaubt aber ich möchte die Funktion choose Card verwenden können