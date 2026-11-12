#include "player.h"
#include "game.h"
#include <iostream>
#include <random>

extern std::mt19937 rng;

Human::Human(const std::string& n){//Menschen haben einen Namen
    name = n;
}

Card Human::chooseCard(Trick& trick, Gamestate& game, Ruleset& rules){//Menschen wählen die zu spielende Karte selbst
    std::vector<int> erlaubteIndexe;

    for (int i = 0; i < hand.size(); ++i) {
        if (rules.karteErlaubt(hand, hand.karten[i], game.Trumpffarbe, trick)){//erlaubte karten werden angezeigt
            erlaubteIndexe.push_back(i);
            std::cout << i << ": " << hand.karten[i].giveCard() << "\n";
        }
    }

    while(true){ //Abfangen ob eingabe richtig war
        std::string eingabe;
        std::cout << "Wähle eine Karte oder drücke ’q’ zum Beenden: ";
        std::cin >> eingabe;

        if (eingabe == "q" || eingabe == "Q") {//Spiel wird beendet wenn man q oder Q drückt
            std::cout << "Spiel wird beendet!\n";
            exit(0);
        }

        bool nurZiffern = true; //schauen ob nur Ziffern eingetippt wurden
        for (int i = 0; i < eingabe.size(); ++i){
            if (eingabe[i]< '0' || eingabe[i] > '9'){
                nurZiffern = false;
                break;
            }
        }

        if (!nurZiffern){//wenn nicht wird man gebeten nochmal einzugeben
            std::cout << "Ungültige Eingabe! Bitte nur die angegebenen Ziffern eingeben!" << std::endl;
            continue;
        }

        int index = 0; //umwandeln str in int
        for (int i = 0; i < eingabe.size(); ++i) {
            index = index * 10 + (eingabe[i] - '0');
        }

        bool erlaubt = false; //schauen ob ein erlaubter index eingegeben wurde
        for (int i : erlaubteIndexe) {
            if (i == index) { 
                erlaubt = true; 
                break; 
            }
        }

        if (erlaubt) {//wenn erlaubter index dann ausgespielt wenn nicht nochmal gefragt
            Card c = hand.entferneKarte(index);
            game.Ablagestapel.addCard(c);
            return c;
        } else {
            std::cout << "Dieser Index ist nicht erlaubt. Bitte erneut wählen.\n";
        }
    }
}

int Human::Stichvorhersage(int runde, const Gamestate& game, const Ruleset& rules){
    std::cout << "Karten von " << name << ": ";
    for (int j = 0; j < hand.karten.size(); ++j){
        std::cout << hand.karten[j].giveCard() << " ";
    }
    std::cout << std::endl;
    std::cout << name << ", wie viele Stiche machen Sie die Runde? ";
    

    while(true){ //Abfangen ob eingabe richtig war
        std::string vorhersage;
        std::cin >> vorhersage;

        if (vorhersage == "q" || vorhersage == "Q") {//Spiel wird beendet wenn man q oder Q drückt
            std::cout << "Spiel wird beendet!\n";
            exit(0);
        }

        bool nurZiffern = true; //schauen ob nur Ziffern eingetippt wurden
        for (int i = 0; i < vorhersage.size(); ++i){
            if (vorhersage[i]< '0' || vorhersage[i]> '9'){
                nurZiffern = false;
                break;
            }
        }

        if (!nurZiffern){//wenn nicht wird man gebeten nochmal einzugeben
            std::cout << "Ungültige Eingabe! Bitte nur die angegebenen Ziffern eingeben!" << std::endl;
            continue;
        }

        int vorhersage_int = 0; //umwandeln str in int
        for (int i = 0; i < vorhersage.size(); ++i) {
            vorhersage_int = vorhersage_int * 10 + (vorhersage[i] - '0');
        }

        if (vorhersage_int > runde){
            std::cout << "Das ist nicht möglich! Wähle nochmal." << std::endl;
            continue;
        } else {return vorhersage_int;}
    }
    
}

int Human::TrumpfWizardWahl(){
    
    

    while(true){ //Abfangen ob eingabe richtig war
        std::string eingabe;

        std::cout << "Wähle: \n 0: Menschen(blau) \n 1: Elfen(grün) \n 2: Zwerge(rot) \n 3: Riesen(gelb)" << std::endl;
        std::cout << "Hier Ihre Handkarten für die Entscheidung: " << std::endl;
        for (int j = 0; j < hand.karten.size(); ++j){
            std::cout << hand.karten[j].giveCard() << " ";

        }
        std::cin >> eingabe;

        if (eingabe == "q" || eingabe == "Q") {//Spiel wird beendet wenn man q oder Q drückt
            std::cout << "Spiel wird beendet!\n";
            exit(0);
        }

        bool nurZiffern = true; //schauen ob nur Ziffern eingetippt wurden
        for (int i = 0; i < eingabe.size(); ++i){
            if (eingabe[i]< '0' || eingabe[i] > '9'){
                nurZiffern = false;
                break;
            }
        }

        if (!nurZiffern){//wenn nicht wird man gebeten nochmal einzugeben
            std::cout << "Ungültige Eingabe! Bitte nur die angegebenen Ziffern eingeben!" << std::endl;
            continue;
        }

        int eingabe_int = 0; //umwandeln str in int
        for (int i = 0; i < eingabe.size(); ++i) {
            eingabe_int = eingabe_int * 10 + (eingabe[i] - '0');
        }

        if (eingabe_int < 0 || eingabe_int > 4 ){
            std::cout << "Das ist keine gültige Eingabe!" << std::endl;
            continue;
        } else {return eingabe_int;}
    }
    

    
    
}

AIPlayer_Best::AIPlayer_Best(const std::string& n){
    name = n;
}

Card AIPlayer_Best::chooseCard(Trick& trick, Gamestate& game, Ruleset& rules){
    std::vector<int> erlaubteIndexe;

    for (int i = 0; i < hand.size(); ++i) {
        if (rules.karteErlaubt(hand, hand.karten[i], game.Trumpffarbe, trick)){
            erlaubteIndexe.push_back(i);
        }
    }

    int index; 
    bool gefunden = false;
    for (int i = 0; i < erlaubteIndexe.size(); ++i){
        int k = erlaubteIndexe[i];

        for (int j = 0; j < trick.gespielteKarten.size(); ++j){
            if (hand.karten[k].farbe == game.Trumpffarbe && trick.gespielteKarten[j].farbe != game.Trumpffarbe){
                index = erlaubteIndexe[i];
                gefunden = true;
                break;
            } else if (hand.karten[k].farbe == "Zauberer" && trick.gespielteKarten[j].farbe != "Zauberer"){
                index = erlaubteIndexe[i];
                gefunden = true;
                break;
            } else if (trick.gespielteKarten[j].farbe == "Zauberer" && hand.karten[k].farbe == "Narr"){
                index = erlaubteIndexe[i];
                gefunden = true;
                break;
            }
        }
    }

    if (!gefunden){
        std::uniform_int_distribution<int> dist(0, erlaubteIndexe.size() - 1);
        int wahl = dist(rng);
        index = erlaubteIndexe[wahl];

    }
    

    Card c = hand.entferneKarte(index);
    game.Ablagestapel.addCard(c);
    return c;
}


int AIPlayer_Best::Stichvorhersage(int runde, const Gamestate& game, const Ruleset& rules){
    int vorhersage = 0;

    for (int i = 0; i < hand.karten.size(); ++i){
        if (hand.karten[i].farbe == "Zauberer" || hand.karten[i].farbe == game.Trumpffarbe){
            vorhersage++;
        }
    }
    return vorhersage;
}

int AIPlayer_Best::TrumpfWizardWahl(){
    int antwort = 0;

    std::vector<int> KartenFarbe(4,0);
    int maxanzahl = 0;

    for (int i = 0; i < hand.karten.size(); ++i){
        if (hand.karten[i].farbe == "Menschen(blau)"){
            KartenFarbe[0]++;
        } else if (hand.karten[i].farbe == "Riesen(gelb)"){
            KartenFarbe[3]++;
        } else if (hand.karten[i].farbe == "Zwerge(rot)"){
            KartenFarbe[2]++;
        } else if (hand.karten[i].farbe == "Elfen(grün)"){
            KartenFarbe[1]++;
        }
    }

    for (int i = 0; i < KartenFarbe.size(); ++i){
        if (KartenFarbe[i] > maxanzahl) {
            maxanzahl= KartenFarbe[i];
            antwort = i;
        }
    }

    return antwort;

}




AIPlayer_Random::AIPlayer_Random(const std::string& n){
    name = n;
}


Card AIPlayer_Random::chooseCard(Trick& trick, Gamestate& game, Ruleset& rules){
    std::vector<int> erlaubteIndexe;

    for (int i = 0; i < hand.size(); ++i) {
        if (rules.karteErlaubt(hand, hand.karten[i], game.Trumpffarbe, trick)){
            erlaubteIndexe.push_back(i);
        }
    }

    int index;
    std::uniform_int_distribution<int> dist(0, erlaubteIndexe.size() - 1);
    int wahl = dist(rng);
    index = erlaubteIndexe[wahl];
    Card c = hand.entferneKarte(index);
    game.Ablagestapel.addCard(c);
    return c;
}


int AIPlayer_Random::Stichvorhersage(int runde, const Gamestate& game, const Ruleset& rules){
    std::uniform_int_distribution<int> dist(0, runde);
    int vorhersage = dist(rng);
    return vorhersage;
}

int AIPlayer_Random::TrumpfWizardWahl(){
    std::uniform_int_distribution<int> dist(0, 3);
    int antwort = dist(rng);
    return antwort;
}



bool Human::isHuman(){return true;}
bool AIPlayer_Random::isHuman(){return false;}
bool AIPlayer_Best::isHuman(){return false;}
