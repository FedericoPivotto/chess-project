// Pivotto Federico
#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <vector>

#include "piece/piece.h"

// classe descrivente il giocatore
class Player {
   public:
    // costruttori del giocatore
    Player();                    // costruttore di default
    Player(char clr, char typ);  // costruttore di un giocatore arbitrario
    Player(const Player& p);     // costruttore di copia del giocatore

    // getter dello stato del giocatore
    char get_color() const;          // restituisce il colore delle pedine del giocatore
    char get_type() const;           // restituisce il tipo del giocatore
    int get_passive_streak() const;  // restituisce la streak di movimenti 'passivi'

    // funzioni membro per l'elaborazione della streak di movimenti 'passivi'
    void incr_passive_streak();   // incrementa la streak di movimenti 'passivi'
    void reset_passive_streak();  // azzera la streak di movimenti 'passivi'

    // funzioni membro per la gestione delle pedine del giocatore
    const std::vector<std::shared_ptr<Piece>>& get_pieces() const;
    std::vector<std::shared_ptr<Piece>>& get_pieces();

   private:
    // dati membro del giocatore
    const char color;    // b (black: pedine nere), w (white: pedine bianche)
    const char type;     // c (computer), h (human: umano)
    int passive_streak;  // streak di movimenti 'passivi' ovvero che portano alla condizione 5 dello stallo
    std::vector<std::shared_ptr<Piece>> pieces;
};

#endif  // PLAYER_H