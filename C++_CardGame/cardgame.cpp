#include "cardpiles.h"
#include "game.h"
#include "player.h"
#include "schnapsn.h"
#include <iostream>
#include "game_engine.h"

int main(){
    char spiel;

    std::cout << "Hallo! Welches Spiel möchten Sie heute spielen? Schnapsn (s), Wizard(w), Schere Stein Papier (p): " << std::endl;
    std::cin >> spiel;

    if (spiel == 's'){ //Schnapsn
        play_schnapsn();
    } else if (spiel == 'w') {//Wizard
        play_wizard();
    } else if (spiel == 'p') {//Scheresteinpapier
        play_scheresteinpapier();
    } else {std::cout << "Ungültige Eingabe!" << std::endl;}

}