#include "cardpiles.h"
#include "game.h"
#include "player.h"
#include "schnapsn.h"
#include <iostream>
#include <random>
#include <algorithm>
#include "game_engine.h"
#include "wizard.h"
#include <memory>
#include "scheresteinpapier.h"


std::random_device rd;  
std::mt19937 rng(rd()); //damit random Karten gewählt werden können

//Schnapsn implementieren
int play_schnapsn(){
    Gamestate game; //ladet Gamestate
    RulesetSchnapsn rules("schnapskarten.txt");
    rules.ladeKartendeck("schnapskarten.txt"); //Karten werden geladen
    std::shuffle(rules.kartendeck.karten.begin(), rules.kartendeck.karten.end(), rng);
    rules.Regeln(); //Regeln werden angezeigt

    game.Nachziehstapel = rules.kartendeck; //der Nachziehstapel hat am Anfang alle geladenen Karten

    int modus = 0;

    std::cout << "Bei Schnapsn können 2 Spieler spielen. Wähle einen Spielmodus (Je nach Loneliness-grad):\n";
    std::cout << "1 = Mensch vs Computer\n";
    std::cout << "2 = Mensch vs Mensch\n";
    std::cout << "3 = Computer vs Computer\n";
    std::cin >> modus;


    Human h1("Spieler1"); //Spieler laden
    Human h2("Spieler2");
    AIPlayer_Best ai1("Computer1");
    AIPlayer_Random ai2("Computer2");


    if (modus == 1){//Mensch spielt gegen "klugen" Computer
        std::string name;
        std::cout << "Name Spieler: "; //namen des Spielers festlegen
        std::cin >> name;
        h1.name = name;

        game.addPlayer(h1); //Spieler hinzufügen
        game.addPlayer(ai1);
    } else if (modus == 2){//Menschen spielen gegeneinander
        std::string name1, name2;
        std::cout << "Name Spieler 1: "; //namen der Spieler festlegen
        std::cin >> name1;
        std::cout << "Name Spieler 2: ";
        std::cin >> name2;
        h1.name = name1;
        h2.name = name2;

        game.addPlayer(h1);
        game.addPlayer(h2);
    } else if (modus == 3){//Zufall vs Best Computer
        game.addPlayer(ai1);
        game.addPlayer(ai2);
    } else { 
        std::cout << "Ähm...das ist keiner der möglichen Modi..." << std::endl;
        return 0;
    }


    game.Punktestand.resize(game.spieler.size(), 0); //Vektor an Anzahl der Spieler anpassen und auf 0 setzen
    rules.adut = game.Nachziehstapel.zieheKarte(); //adut wird festgelegt (erste Karte die gezogen/aufgedeckt wird)
    game.Trumpffarbe = rules.adut.farbe;
    std::cout << "Trumpf ist: " << rules.adut.giveCard() << std::endl; //Spielern wird gezeigt was Adut ist

    //Karten austeilen
    rules.kartenProSpieler = 5;
    for (int i = 0; i < rules.kartenProSpieler; ++i){
        for (auto p : game.spieler){
            Card gezogen = game.Nachziehstapel.zieheKarte();
            p->hand.addCard(gezogen);
        }
    }

    bool adutAusgegeben = false; //adut ist die letzte Karte die aufgehoben wird
    bool play = true; //wird weitergespielt?
    bool Bubausgetauscht = false;
    
    while (play) { 
        bool nochKarten = false; // Prüfen, ob Spieler noch Karten haben
        for (auto* p : game.spieler){
            if (p->hand.size() > 0){nochKarten = true;}
        if (!nochKarten){break;}
        }

        
        for (int i = 0; i < (int)game.spieler.size(); ++i){ //jeder Spieler spielt Karte aus
            Player* aktSpieler = game.spieler[(game.aktuellerSpieler + i) % (int)game.spieler.size()];

            if (!Bubausgetauscht && i == 0 && rules.bubtausch(*aktSpieler, game)) {//Bub austauschen?
                if (aktSpieler->isHuman()) { 
                    char antwort;
                    std::cout << aktSpieler->name << ", willst du den Bub tauschen? (j/n): ";
                    std::cin >> antwort;
                    if (antwort == 'j'){
                        rules.tauscheBub(*aktSpieler, game);
                        Bubausgetauscht = true;
                        std::cout << aktSpieler->name << " tauscht Trumpf mit Bub!" << std::endl;
                    }
                } else {//AI tauscht immer, wenn möglich
                    rules.tauscheBub(*aktSpieler, game);
                    Bubausgetauscht = true; 
                    std::cout << aktSpieler->name << " tauscht Trumpf mit Bub!" << std::endl;
                }
            }
            


            Card c = aktSpieler->chooseCard(game.aktuellerStich, game, rules); //wählt Karte
            game.aktuellerStich.spieleKarte(aktSpieler, c); //spielt Karte aus
            std::cout << aktSpieler->name << " spielt: " << c.giveCard() << std::endl; //Ausgabe was im Spiel passiert
        }
        std::cout << std::endl; //damit schöner formatiert in Terminal
       
        int gewinnerKarteIndex = rules.gewinnerStich(game.aktuellerStich); //wer den Stich gewonnen hat
        

        if (gewinnerKarteIndex >= 0){
            Player* gewinnerSpieler = game.aktuellerStich.spieler[gewinnerKarteIndex];

            auto it = std::find(game.spieler.begin(), game.spieler.end(), gewinnerSpieler); //damit richtiger Spieler betrachtet wird
            int spielerIndex = std::distance(game.spieler.begin(), it);

            int stichPunkte = rules.punktefürstich(game.aktuellerStich);
            stichPunkte += rules.BonusDameKönig(game.aktuellerStich, *gewinnerSpieler);
            game.Punktestand[spielerIndex] += stichPunkte;

            if (game.Nachziehstapel.size() == 0 && gewinnerSpieler->hand.size() == 0) {//letzter Stich bringt 10 zusätzliche Punkte
                game.Punktestand[spielerIndex] += 10;
                std::cout << gewinnerSpieler->name << " bekommt für den letzten Stich +10 Punkte." << std::endl;
            }


            game.aktuellerStich.Gewinner(gewinnerKarteIndex);

            game.aktuellerSpieler = spielerIndex; //Nächster Stich beginnt mit Gewinner der Runde

            std::cout << game.aktuellerStich.getWinner() << " bekommt " << stichPunkte << " Punkte für diesen Stich." << std::endl;
        }

        
        play = false; //spielt geht nur mit paar bedingungen weiter
        for (int j = 0; j < (int)game.spieler.size(); ++j){
            Player* p = game.spieler[(game.aktuellerSpieler + j) % game.spieler.size()];

            if (p->hand.size() >= rules.kartenProSpieler) {continue;}
            
            if (!adutAusgegeben && game.Nachziehstapel.size() == 0 && j == 1) {//bestimmt wie Nachziehen der Karten erfolgt
                p->hand.addCard(rules.adut);
                adutAusgegeben = true; //letzte Karte die aufgehoben wird ist adut
                play = true;
            }
            else if (game.Nachziehstapel.size() > 0) { //ansonsten einfach Karte aufnehmen
                Card c = game.Nachziehstapel.zieheKarte();
                p->hand.addCard(c);
                play = true;
            } else if (p->hand.size() > 0){ //es wird gespielt bis kein Spieler mehr Karten in der Hand hat
                play = true;
            }
        }
        game.aktuellerStich.gespielteKarten.clear(); //die Vektoren werden geleert bevor die nächste Runde beginnt!!!
        game.aktuellerStich.spieler.clear();
    }

    // Spielende
    std::cout << game.spieler[0]->name << ": "<< game.Punktestand[0] << " Punkte \n" << game.spieler[1]->name << ": "<< game.Punktestand[1] << " Punkte\n";
    std::cout << "Gesamtsieger: " << rules.gewinnerSpiel(game) << std::endl;

    return 0;
}




//Wizard implementieren
int play_wizard(){
    Gamestate game;
    RulesetWizard rules("wizardkarten.txt");
    rules.Regeln();

    int gesamtspieler;
    std::cout << "Wie viele Spieler spielen? (3 - 6 Spieler erlaubt) ";
    std::cin >> gesamtspieler;

    if (gesamtspieler < 3 || gesamtspieler > 6) {
        std::cout << "Das ist nicht die erlaubte Spieleranzahl..." << std::endl;
        return -1;
    }

    int computerspieler;
    std::cout << "Wie viele davon sollen Computer sein? ";
    std::cin >> computerspieler;
    
    int menschenspieler = gesamtspieler - computerspieler;
    if (menschenspieler < 0){menschenspieler = 0;} //wird auf nur Computer gesetzt

    
    std::vector<std::unique_ptr<Player>> allPlayers;

    
    for (int i = 0; i < menschenspieler; ++i) {//Menschen werden geladen
        std::string name;
        std::cout << "Name des Spielers " << (i + 1) << ": ";
        std::cin >> name;
        allPlayers.push_back(std::make_unique<Human>(name));
        game.spieler.push_back(allPlayers.back().get());
    }

    int computerNummer = 1;
    for (int i = 0; i < computerspieler; ++i) {//Computerspieler werden geladen
        std::uniform_int_distribution<int> dist(0,1);
        int art = dist(rng);
        std::string name = "Computer " + std::to_string(computerNummer++);
        if (art == 0) {
            allPlayers.push_back(std::make_unique<AIPlayer_Random>(name)); //Random
        } else {
            allPlayers.push_back(std::make_unique<AIPlayer_Best>(name)); //Best
        }
        game.spieler.push_back(allPlayers.back().get());
    }

    
    game.Punktestand.resize(game.spieler.size(), 0); //Vektor wird auf die Anzahl der Spieler gesetzt und die Einträge sind 0
    rules.SticheRunde.resize(game.spieler.size(), 0); //Wie viele Stiche gemacht werden

    rules.ladeKartendeck("wizardkarten.txt"); //Kartendeck wird geladen
    std::shuffle(rules.kartendeck.karten.begin(), rules.kartendeck.karten.end(), rng);
    game.Nachziehstapel = rules.kartendeck;
    
    int aktuellerAusteilerIndex= 0; //Wer teilt aus
    
    int maxrunden = game.Nachziehstapel.size() / game.spieler.size(); //Maximale Spielrunden
    

    for (int runde = 1; runde <= maxrunden; ++runde){
        for (auto* p : game.spieler) {//Hände werden am Anfang der Runde geleert
            p->hand.karten.clear();
        }

        rules.ladeKartendeck("wizardkarten.txt"); //es wird immer mit ALLEN karten gemischt. Hier gelöst mit Kartendeck neu laden
        std::shuffle(rules.kartendeck.karten.begin(), rules.kartendeck.karten.end(), rng);
        game.Nachziehstapel = rules.kartendeck;

        Player* aktuellerAusteiler = game.spieler[aktuellerAusteilerIndex];

        rules.Stichvorhersage.resize(game.spieler.size(), 0); //Stichvorhersage wird am Anfang jeder Runde auf 0 gesetzt

        for (int j = 0; j < rules.SticheRunde.size(); ++j) { //SticheRunde wird ebenfalls wieder auf 0 gesetzt
            rules.SticheRunde[j] = 0;
        }

        
        std::cout << "Runde " << runde << " beginnt: " << std::endl;

        for (int i = 0; i < runde; ++i){
            for (int j = 0; j < game.spieler.size(); ++j) {
                Player* p = game.spieler[(aktuellerAusteilerIndex + 1 + j) % game.spieler.size()]; //links vom Austeiler beginnt
                while (p->hand.size() < runde && game.Nachziehstapel.size() > 0) {//Karten werden verteilt
                    Card c = game.Nachziehstapel.zieheKarte();
                    p->hand.addCard(c);
                }
            }
        }

        game.Trumpffarbe = rules.TrumpfWizard(game, aktuellerAusteiler); //Trumpf wird festgelegt
        std::cout << "Trumpf der Runde: " << game.Trumpffarbe << std::endl;
        std::cout << std::endl; //nur damit im Terminal übersichtlicher ausschaut
        
        std::cout << "Stichvorhersagen: " << std::endl;
        for (auto* p: game.spieler){
            int spielerIndex = std::distance(game.spieler.begin(), std::find(game.spieler.begin(), game.spieler.end(), p)); //damit auf richtigen Spieler zeigt
            int vorhersage = p->Stichvorhersage(runde, game, rules);
            rules.Stichvorhersage[spielerIndex] = vorhersage;
            std::cout << p->name << " sagt " << vorhersage << " Stiche vorher!" << std::endl;
        }
        std::cout << std::endl; //wieder nur damit schöner formatiert

        for (int stichnummer = 0; stichnummer < runde; ++stichnummer){//stiche in einer Runde werden ausgeführt
            Trick aktuellerStich; 
            for (int j = 0; j < game.spieler.size(); ++j){//karten werden ausgespielt
                Player* p = game.spieler[(aktuellerAusteilerIndex + 1 + j) % game.spieler.size()];
                Card c = p->chooseCard(aktuellerStich, game, rules);
                aktuellerStich.spieleKarte(p, c);
                std::cout << p->name << " spielt: " << c.giveCard() << std::endl;
            } 

            int gewinnerIndex = rules.gewinnerStich(aktuellerStich, game); //gewinner wird festgelegt

            Player* gewinner = aktuellerStich.spieler[gewinnerIndex];
            auto it = std::find(game.spieler.begin(), game.spieler.end(), gewinner); //findet den Spieler-Zeiger
            aktuellerAusteilerIndex = std::distance(game.spieler.begin(), it); //gibt gefundene Position zurück
            
            std::cout << "Gewinner: " << gewinner->name << std::endl;

            aktuellerAusteiler = game.spieler[aktuellerAusteilerIndex]; //neuer Austeiler ist Gewinner

            aktuellerStich.gespielteKarten.clear(); //Vektoren werden geleert am Ende jeder Runde
            aktuellerStich.spieler.clear();
        }

        rules.PunkteRunde(game); //Punkte der Runde werden berechnet
        std::cout << "Ergebnis Runde " << runde << ": \n";
        for (int i = 0; i < game.spieler.size(); ++i) {
            std::cout << game.spieler[i]->name << ": " << game.Punktestand[i] << " Punkte\n";
        }
        std::cout << std::endl;
    
        
        
    }


    std::cout << "Gesamtsieger: " << rules.gewinnerSpiel(game) << std::endl; //gesamtsieger wird ausgegeben

    return 0;

}




int play_scheresteinpapier(){
    Gamestate game;
    RulesetSchereSteinPapier rules;
    rules.Regeln();

    std::cout << "Wie viele Punkte braucht man zum Gewinnen? ";
    std::cin >> rules.Gewinnpunkte;

    int gesamtspieler;
    std::cout << "Wie viele Spieler spielen? ";
    std::cin >> gesamtspieler;

    int computerspieler;
    std::cout << "Wie viele davon sollen Computer sein? ";
    std::cin >> computerspieler;
    
    int menschenspieler = gesamtspieler - computerspieler;
    if (menschenspieler < 0){menschenspieler = 0;} //wird auf nur Computer gesetzt

    

    std::vector<std::unique_ptr<Player>> allPlayers;

    
    for (int i = 0; i < menschenspieler; ++i) {//Menschen werden geladen
        std::string name;
        std::cout << "Name des Spielers " << (i + 1) << ": ";
        std::cin >> name;
        allPlayers.push_back(std::make_unique<Human>(name));
        game.spieler.push_back(allPlayers.back().get());
    }

    int computerNummer = 1;
    for (int i = 0; i < computerspieler; ++i) {//Computerspieler werden geladen
        std::string name = "Computer " + std::to_string(computerNummer++);
        allPlayers.push_back(std::make_unique<AIPlayer_Random>(name)); //nur random weil spiel ist random
        game.spieler.push_back(allPlayers.back().get());
    }
    
    game.Punktestand.resize(game.spieler.size(), 0);

    std::cout << "Das Spiel beginnt!" << std::endl;
    bool play = true;
    while (play){


        for (int j = 0; j < game.spieler.size(); ++j) {//Karten werden ausgeteilt
                Player* p = game.spieler[j];
                p->hand.karten.clear();
                p->hand.addCard(Card("Schere", "-"));
                p->hand.addCard(Card("Stein", "-"));
                p->hand.addCard(Card("Papier", "-"));
            }


        std::cout << "Schere, Stein, Papier!" << std::endl;
        Trick aktuellerStich;
        for (auto* p: game.spieler){ //es wird gespielt
            if (p->isHuman()){
                Card c = p->chooseCard(aktuellerStich, game, rules);
                aktuellerStich.spieleKarte(p, c);
                std::cout << p->name << " spielt: " << c.giveCard() << std::endl;
            } else {
                std::uniform_int_distribution<int> dist(0, (int)p->hand.karten.size() - 1); //Computer wählt Karte zufällig
                int index = dist(rng);
                Card c = p->hand.karten[index];
                aktuellerStich.spieleKarte(p, c);
                std::cout << p->name << " spielt: " << c.giveCard() << std::endl;
            }
        }

        auto gewinnerIndex = rules.gewinnerStich(aktuellerStich, game);

        if (gewinnerIndex != -1) {
            Player* gewinner = aktuellerStich.spieler[gewinnerIndex];
            std::cout << "Gewinner der Runde: " << gewinner->name << std::endl;
            std::cout << std::endl; //nur für Formatierung
        }

        if (rules.gewinnerSpiel(game) != ""){
            std::cout << "Gewinner: " << rules.gewinnerSpiel(game) << std::endl;
            play = false;
        }
    }

    return 0;
}



