// Pivotto Federico
#ifndef CHESSBOARD_H
#define CHESSBOARD_H

// librerie utilizzate
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

// header necessari per il funzionamento della scacchiera
#include "piece/piece.h"
#include "player/player.h"

// enumeratore per identificare gli stati di gioco in cui puo trovarsi la partita a scacchi
enum chessboard_status { ongoing,
                         nil,
                         stall,
                         check_white,
                         check_black,
                         checkmate_white,
                         checkmate_black };
// 0: ongoing (partita in corso)
// 1: nil (partita nulla)
// 2: stall (partita di stallo)
// 3: check_white (bianco sotto scacco)
// 4: check_black (nero sotto scacco)
// 5: checkmate_white (partita conclusa in scaccomatto, perde bianco)
// 6: checkmate_black (partita conclusa in scaccomatto, perde nero)

// classe descrivente la scacchiera di gioco
class Chessboard {
   public:
    // costruttore della scacchiera per i due giocatori
    Chessboard(Player &p1, Player &p2);

    // funzioni membro
    Pair move(Player &p, const Player &enemy_p, Pair start, Pair dest);            // esegue la mossa specificata dal giocatore p
    void promote(const Player &p, const Player &enemy_p, char prom_id, Pair pos);  // promuove un pedone

    // getter di chessboard
    int get_status() const;                                  // ritorna lo stato di gioco attuale della scacchiera
    int get_turn() const;                                    // ritorna il numero del turno della partita
    const std::vector<std::vector<char>> get_board() const;  // ritorna lo stato della scacchiera sottoforma di matrice di char

    // impostazione dello stallo se inserito PP PP dall'utente
    void set_manual_stall();

   private:
    // funzioni membro ausiliarie per il costruttore
    void initialize(Player &p, std::vector<char> &ids);         // posiziona nella scacchiera le pedine passate del giocatore p
    std::shared_ptr<Piece> get_piece(char id, Pair pos) const;  // facilita la creazione degli shared_pointer a pedine
    std::string get_board_string() const;                       // ritorna la configurazione della scacchiera sottoforma di stringa da inserire in history

    // setter dello stato della scacchiera
    void set_status(chessboard_status st);
    void update_status(const Player &p, const Player &enemy_p);
    void update_history();  // aggiornamento cronologia

    // funzione membro per ottenere le posizioni raggiungibili da una specifica pedina
    std::vector<Pair> get_reachable_pos(const Piece &pc) const;

    // funzioni membro di controllo sulle caselle della scacchiera
    bool is_free(Pair pos) const;                    // verifica se nella casella della scacchiera specificata non sia presente una pedina
    bool is_enemy(const Player &p, Pair pos) const;  // verifica se che nella casella della scacchiera specificata e' presente una pedina nemica per il giocatore p

    // funzioni membro di controllo per le mosse speciali
    bool is_castling(const Player &enemy_p, Pair start, Pair dest) const;  // arrocco
    bool is_promotable(Pair dest) const;                                   // promozione
    bool is_enpassant(Pair start, Pair dest) const;                        // en passant
    bool is_path_valid(Pair start, Pair dest) const;                       // verifica l'esistenza di un percorso valido fra le due posizioni per la pedina in posizione start

    // funzioni membro per la gestione degli stati di gioco in cui puo trovarsi la scacchiera
    bool is_check(const Player &p) const;                       // verifica se il giocatore nemico di p e' in scacco dopo la mossa di quest'ultimo
    bool is_checkmate(const Player &p, const Player &enemy_p);  // verifica se il giocatore nemico di p e' in scaccomatto dopo la mossa di quest'ultimo
    bool is_stall(const Player &p, const Player &enemy_p);      // verifica se la partita a scacchi ha raggiunto una situazione di stallo per entrambi i giocatori
    bool is_nil() const;                                        // in caso di partite computer vs computer, verifica se il numero massimo di mosse imposto dalle regole del gioco e' stato raggiunto

    // funzioni membro ausiliarie per la verifica di situazioni di scacco per il giocatore che esegue la mossa
    bool is_check_for_us(const Player &enemy_p, Pair pos) const;  // verifica la presenza di situazione di scacco in caselle specifiche della scacchiera
    bool is_defendable(const Player &p, const Player &enemy_p);   // controlla se il giocatore ha modo di difendere il re in qualche modo

    // funzioni membro che eseguono le mosse spostando i puntatori ed aggiornando le pedine
    void move_enpassant(Pair start, Pair dest);  // en passant
    void move_castling(Pair start, Pair dest);   // arrocco
    void move_normal(Pair start, Pair dest);     // mossa non speciale

    // dati membro costanti
    const int SIDE = 8;       // numero di caselle presenti nel lato di una scacchiera classica
    const int MAX_MOVES = 8;  // numero massimo di mosse consentite in partite computer vs computer

    // dati membro
    int turn_count;                                          // memorizza il turno al quale e' arrivata la partita a scacchi
    const bool both_cp;                                      // segnala se la partita e' del tipo computer vs computer o meno
    chessboard_status status;                                // memorizza la stato di gioco della scacchiera
    std::vector<std::shared_ptr<Piece>> kings;               // memorizza i puntatori ai re dei due giocatori
    std::vector<std::vector<std::shared_ptr<Piece>>> board;  // descrive lo stato della scacchiera e le pedine in essa presenti
    std::map<std::string, int> history;                      // storico dello stato della scacchiera e delle pedine in essa presenti
};

// overload dell'operatore <<
std::ostream &operator<<(std::ostream &out, const Chessboard &cb);  // permette di stampare la scacchiera di gioco

#endif  // CHESSBOARD_H