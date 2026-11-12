#include "game.h"
#include "player.h"
#include "cardpiles.h"
#include "schnapsn.h"
#include <algorithm>
#include <iostream>


RulesetSchnapsn::RulesetSchnapsn(const std::string& d){//Konstruktor speichert den übergebenen Dateinamen
    this->dateiname = d;
}

int RulesetSchnapsn::gewinnerStich(Trick& trick){//wer gewinnt den stich?
    if (trick.gespielteKarten.empty()){
        return -1;
    }

    int gewinner = 0;
    const std::string& stichfarbe = trick.gespielteKarten[0].farbe; //erste ausgespielte Karte ist Trumpffarbe
    std::vector<std::string> rank = {"10", "Bub", "Dame", "König", "Ass"}; //richtige Reihenfolge

    for (int i = 1; i < (int)trick.gespielteKarten.size(); ++i) {
        const Card& karte = trick.gespielteKarten[i];
        const Card& aktuelleGewinnerKarte = trick.gespielteKarten[gewinner];

        if ((karte.farbe == adut.farbe) && (aktuelleGewinnerKarte.farbe != adut.farbe)){ //adut oder Atout (wie auch immer man das schreibt) schlägt nicht adut
            gewinner = i;
        } else if ((karte.farbe == adut.farbe) && (aktuelleGewinnerKarte.farbe == adut.farbe)){//beide haben Adut -> höhere karte gewinnt
            int indexKarte = std::distance(rank.begin(), std::find(rank.begin(), rank.end(), karte.rang));
            int indexGewinner = std::distance(rank.begin(), std::find(rank.begin(), rank.end(), aktuelleGewinnerKarte.rang));
            if (indexKarte > indexGewinner){
                gewinner = i;
            }
        }
        else if ((karte.farbe != adut.farbe) && (aktuelleGewinnerKarte.farbe != adut.farbe) && (karte.farbe == stichfarbe) && (aktuelleGewinnerKarte.farbe == stichfarbe)) {//beide haben Trumpffarbe -> höhere Karte gewinnt
            int indexKarte = std::distance(rank.begin(), std::find(rank.begin(), rank.end(), karte.rang));
            int indexGewinner = std::distance(rank.begin(), std::find(rank.begin(), rank.end(), aktuelleGewinnerKarte.rang));
            if (indexKarte > indexGewinner){
                gewinner = i;
            }
        }
    }

    trick.Gewinner(gewinner); //gibt trick den index
    return gewinner;
}




std::string RulesetSchnapsn::gewinnerSpiel(Gamestate& game){
    int maxPunkte = -1;
    std::vector<int> gewinnerIndexe;


    for (int i = 0; i < (int)game.spieler.size(); ++i){//findet heraus welcher Spieler die meisten Punkte hat
        if (game.Punktestand[i] > maxPunkte) {
            maxPunkte = game.Punktestand[i];
            gewinnerIndexe = {i};
        } else if (game.Punktestand[i] == maxPunkte){ //Gleichstand
            gewinnerIndexe.push_back(i);
        }
    }

    if (gewinnerIndexe.size() == 1){
        return game.spieler[gewinnerIndexe[0]]->name; //zeigt auf den Spieler der mit diesem index assoziiert wird
    }

    return "Unentschieden";
}

void RulesetSchnapsn::Regeln(){ //gibt Regeln des Spiels aus
    std::cout << "Willkommen bei den Regeln zu Schnapsn! Sie können die erstaufgedeckte Karte, welches den Trumpf festlegt austauschen," <<
    "wenn Sie den Bub der jeweiligen Farbe haben. Weiters gibt es Sonderpunkte, wenn man Dame und König derselben Farbe besitzt." <<
    "Es zählt wer die meisten Punkte hat! Der letzte Stich bringt noch zusätzliche 10 Punkte! Have fun!" << std::endl;
}


int RulesetSchnapsn::Punkteverteilung(const Card& karte){//wie viele Punkte sind die jeweiligen Karten wert?
    if (karte.rang == "Ass") {return 11;}
    if (karte.rang == "10") {return 10;}
    if (karte.rang == "König") {return 4;}
    if (karte.rang == "Dame") {return 3;}
    if (karte.rang == "Bub") {return 2;}
    return 0;
}

int RulesetSchnapsn::punktefürstich(const Trick& trick){//wie viele Punkte bringt der Stich?
    int punkte = 0;
    for (const Card& karte: trick.gespielteKarten){
        punkte += Punkteverteilung(karte);
    }
    return punkte;
}


bool RulesetSchnapsn::bubtausch (const Player& player, const Gamestate& game){//kann ich den Bub mit dem Adutkarte tauschen?
    if (game.Nachziehstapel.size() == 0){
        return false;
    }

    for (const Card& karte: player.hand.karten) {
        if (karte.rang == "Bub" && karte.farbe == adut.farbe) {
            return true;
        } 
    }

    return false;
}

void RulesetSchnapsn::tauscheBub(Player& player, const Gamestate& game){//Bub mit Adutkarte tauschen
    for (int i = 0; i < (int)player.hand.karten.size(); ++i){
        Card& karte = player.hand.karten[i];

        if (bubtausch(player, game)){
            Card temp = karte;
            karte = adut;
            adut = temp;
            break;
        }
    }
}


int RulesetSchnapsn::BonusDameKönig(const Trick& trick, const Player& player){//Bonuspunkte wenn man Dame und König derselben Farbe hat und mit dem einen Stich gewinnt den man eröffnet hat
    int bonus = 0;

    for (int i = 0; i < (int)player.hand.karten.size(); ++i){
        for (int j = 0; j < (int)player.hand.karten.size(); ++j){
            if (player.hand.karten[i].farbe == player.hand.karten[j].farbe && player.hand.karten[i].rang == "König" && player.hand.karten[j].rang == "Dame"){
                if (player.hand.karten[i].farbe == adut.farbe){
                    bonus += 40;
                } else {bonus += 20;}
            }
        }
    }

    if (!trick.gespielteKarten.empty() && (trick.gespielteKarten[0].rang == "König" || trick.gespielteKarten[0].rang == "Dame")){//nur die erst ausgegebene Karte kann die Punkte bringen
        return bonus;
    } else {return 0;}
}

