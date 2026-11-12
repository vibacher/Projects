#include "wizard.h"
#include "game.h"
#include "player.h"
#include "cardpiles.h"
#include <algorithm>
#include <iostream>


RulesetWizard::RulesetWizard(const std::string& d){//Konstruktor speichert den übergebenen Dateinamen
    this->dateiname = d;
}

std::string RulesetWizard::TrumpfWizard(Gamestate& game, Player* aktuellerAusteiler){//Trumpf festlegen

    if (game.Nachziehstapel.size() == 0 ){//in der letzten Runde gibt es keinen Trumpf, erstausgespielte Karte ist Trumpf
        game.Trumpffarbe =  "kein Trumpf";
    } else {game.Trumpffarbe = game.Nachziehstapel.zieheKarte().farbe;}
    
    if (game.Trumpffarbe == "Narr"){
        game.Trumpffarbe = "kein Trumpf";
    }

    if (game.Trumpffarbe  == "Zauberer"){//Trumpf wird vom Austeiler ausgesucht nachdem er seine Karten gesehen hat
        std::cout << "Bei der aufgedeckten Karte 'Zauberer' darf sich " << aktuellerAusteiler->name << " einen Trumpf aussuchen: " << std::endl;

        int antwort = aktuellerAusteiler->TrumpfWizardWahl();        

        if (antwort == 0){
            game.Trumpffarbe  = "Menschen(blau)";
            std::cout << "Die Trumpffarbe für diese Runde ist " << game.Trumpffarbe   << std::endl;
        } else if (antwort == 1) {
            game.Trumpffarbe  = "Elfen(grün)";
            std::cout << "Die Trumpffarbe für diese Runde ist " << game.Trumpffarbe  << std::endl;
        } else if (antwort == 2) {
            game.Trumpffarbe  = "Zwerge(rot)";
            std::cout << "Die Trumpffarbe für diese Runde ist " << game.Trumpffarbe  << std::endl;
        } else if (antwort == 3) {
            game.Trumpffarbe  = "Riesen(gelb)";
            std::cout << "Die Trumpffarbe für diese Runde ist " << game.Trumpffarbe  << std::endl;
        } else {
            std::cout << "Ungültige Eingabe!" << std::endl;
        }
    }

    return game.Trumpffarbe;
}


int RulesetWizard::gewinnerStich(Trick& trick, Gamestate& game) {
    if (trick.gespielteKarten.empty()){
        return -1;
    }

    std::vector<std::string> rank = {"1","2","3","4","5","6","7","8","9","10","11","12","13"}; //damit ich nichts umwandeln muss

    for (int i = 0; i < trick.gespielteKarten.size(); ++i) {//Zauberer schlagen alles, der erstausgespielte gewinnt
        if (trick.gespielteKarten[i].farbe == "Zauberer") {
            trick.Gewinner(i);
            Player* gewinnerSpieler = trick.spieler[i];
            auto it = std::find(game.spieler.begin(), game.spieler.end(), gewinnerSpieler);
            int spielerIndex = std::distance(game.spieler.begin(), it);
            SticheRunde[spielerIndex]++;
            return i;
        }
    }

    
    std::string stichfarbe = "";
    for (const auto& c : trick.gespielteKarten) {//Stichfarbe ist erstausgespielte die nicht Narr oder Zauberer ist
        if (c.farbe != "Zauberer" && c.farbe != "Narr") {
            stichfarbe = c.farbe;
            break;
        }
    }

    int gewinner = -1;
    for (int i = 0; i < trick.gespielteKarten.size(); ++i) {//Initialisierung eines Gewinners als erste nicht Narr karte
        if (trick.gespielteKarten[i].farbe != "Narr") {
            gewinner = i;
            break;
        }
    }

    for (int i = 0; i < trick.gespielteKarten.size(); ++i) {
        
        if (trick.gespielteKarten[i].farbe == "Narr"){//Narren werden übersprungen
            continue;
        }


        if (trick.gespielteKarten[gewinner].farbe != "Zauberer" && !game.Trumpffarbe.empty() && game.Trumpffarbe != "kein Trumpf") {//Trumpf sticht normale Karten
            if (trick.gespielteKarten[i].farbe == game.Trumpffarbe && trick.gespielteKarten[gewinner].farbe != game.Trumpffarbe) {
                gewinner = i;
                continue;
            } else if (trick.gespielteKarten[i].farbe != game.Trumpffarbe && trick.gespielteKarten[gewinner].farbe == game.Trumpffarbe) {
                continue;
            }
        }

       
        if (trick.gespielteKarten[i].farbe == trick.gespielteKarten[gewinner].farbe) {//gleiche Farbe, höhere Zahl gewinnt
            int indexKarte = std::distance(rank.begin(), std::find(rank.begin(), rank.end(), trick.gespielteKarten[i].rang));
            int indexGewinner = std::distance(rank.begin(), std::find(rank.begin(), rank.end(), trick.gespielteKarten[gewinner].rang));

            if (indexKarte > indexGewinner){
                gewinner = i;
            }
        }
    }

    
    if (gewinner == -1){//nur Narren, erst ausgespielter gewinnt
        gewinner = 0;
    } 

    trick.Gewinner(gewinner);
    Player* gewinnerSpieler = trick.spieler[gewinner];
    auto it = std::find(game.spieler.begin(), game.spieler.end(), gewinnerSpieler);
    int spielerIndex = std::distance(game.spieler.begin(), it);
    SticheRunde[spielerIndex]++;

    return gewinner;
}




void RulesetWizard::PunkteRunde(Gamestate& game){//Punkte der gesamten Runde nach Wizard Regeln
    int daneben;
    for (int i = 0; i < game.spieler.size(); ++i){
        if (Stichvorhersage[i] == SticheRunde[i]){
            game.Punktestand[i] += 20 + Stichvorhersage[i] * 10;
        } else {
            daneben = abs(Stichvorhersage[i] - SticheRunde[i]);
            game.Punktestand[i] -= daneben * 10;
        }
    }
}


std::string RulesetWizard::gewinnerSpiel(Gamestate& game){//gewinner des Spieles
    int maxPunkte = game.Punktestand[0];
    std::vector<int> gewinnerIndexe = {0};


    for (int i = 1; i < (int)game.spieler.size(); ++i){//findet heraus welcher Spieler die meisten Punkte hat
        if (game.Punktestand[i] > maxPunkte) {
            maxPunkte = game.Punktestand[i];
            gewinnerIndexe = {i};
        } else if (game.Punktestand[i] == maxPunkte){ //Gleichstand
            gewinnerIndexe.push_back(i);
        }
    }

    std::string result = "";
    for (int idx : gewinnerIndexe) {//alle Gewinner werden ausgegeben falls Gleichstand
        result += game.spieler[idx]->name + " ";
    }
    return result;
}


void RulesetWizard::Regeln(){
    std::cout << "Willkommen beim Spiel Wizard! Es werden immer gleich viele Karten ausgeteilt wie Anzahl der Runde (1. Runde: 1 Karte, 2. Runde: 2 Karten, etc.)." <<
    "Am Anfang jeder Runde wird eine Trumpffarbe aufgedeckt. Sollte der Nachziehstapel leer sein (letzte Runde) oder die Karte ein Narr sein, " <<
    "so gibt es in dieser Runde keinen Trumpf und die erst ausgespielte Karte entscheidet. Sollte die aufgedeckte Karte ein Zauberer sein," <<
    "so darf sich die Person, die die Karten ausgeteilt hat eine Trumpffarbe wünschen. Bei den Stichen gilt: Narren verlieren immer, Zauberer gewinnen immer," <<
    "werden mehrere Zauberer hingelegt in einer Runde gewinnt der erste. Ansonsten gilt Farbzwang! Zauberer und Narren können trotz Farbzwang immer ausgespielt werden." <<
    "Es ist egal wenn die Anzahl der geschätzten Stiche nicht der Rundenanzahl entspricht. Es werden die Karten im Uhrzeigersinn ausgeteilt. Viel Spaß!" << std::endl;
}


bool RulesetWizard::karteErlaubt(const CardPile& hand, const Card& karte, const std::string& trumpf, const Trick& trick){
    if (trick.gespielteKarten.empty()) {
        return true; //die erste Karte die gespielt wird kann alles sein
    }

   stichfarbe = trick.gespielteKarten[0].farbe; //erste ausgespielte Karte ist trumpffarbe, die auch Gamestate hat

    bool hatStichfarbe = false;
    for (const auto& c : hand.karten) { //schauen ob Trumpffarbe besitzt
        if (c.farbe == stichfarbe) {
            hatStichfarbe = true;
            break;
        }
    }
    
    if (hatStichfarbe) {
        return karte.farbe == stichfarbe || karte.farbe == "Zauberer" || karte.farbe == "Narr";
    }
    return true;//wenn man Stichfarbe nicht hat dürfen alle Karten gespielt werden
    
}

