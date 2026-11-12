## Object Oriented Card Games (C++)

Software development project focusing on object-oriented design, clean header/implementation separation, game state management, and easy expansion in C++.

# Architecture
Clean separation of class classification (.h) and implentation (.cpp) among the game entity.
The structure is shown in `UML-Klassendiagramm C++ Card Game.pdf`.

```mermaid
classDiagram
    class Card {
        +string farbe
        +string rang
        +giveCard() string
    }
    class CardPile {
        +vector~Card~ karten
        +zieheKarte() Card
        +entferneKarte(index) Card
        +addCard(Card) void
        +size() int
    }
    class Trick {
        +vector~Card~ gespielteKarten
        +vector~Player*~ spieler
        +spieleKarte(Player, Card) void
        +Gewinner(index) void
        +getWinner() string
    }
    class Gamestate {
        +vector~Player*~ spieler
        +vector~int~ Punktestand
        +string Trumpffarbe
        +CardPile Nachziehstapel
        +addPlayer(Player) void
    }
    class Player {
        +string name
        +CardPile hand
        +chooseCard(Trick, Gamestate, Ruleset) Card
        +isHuman() bool
        +Stichvorhersage(Gamestate, Ruleset, runde) int
        +TrumpfWizardWahl() int
    }
    class Ruleset {
        +CardPile kartendeck
        +string dateiname
        +vector~Card~ karten
        +string Stichfarbe
        +ladeKartendeck(dateiname) void
        +karteErlaubt(Player, Card, Trick) bool
        +gewinneStich(Trick) string
        +gewinneSpiel(Gamestate) string
        +Regeln() void
    }

    CardPile *-- Card
    Trick o-- Card
    Gamestate *-- CardPile
    Gamestate o-- Player
    Player *-- CardPile
    Ruleset *-- CardPile
    Ruleset o-- Card

    Player <|-- Human
    Player <|-- AI_Random
    Player <|-- AI_Best

    Ruleset <|-- RulesetSchnapsen
    Ruleset <|-- RulesetWizard
    Ruleset <|-- RulesetSchereSteinPapier
```

# How to Compile and Run
```bash
g++-std=c++17 cardgame.cpp cardpiles.cpp game.cpp game engine.cpp player.cpp schnapsn.cpp wizard.cpp scheresteinpapier.cpp -o spiel
./spiel
```
