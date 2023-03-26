// Genilotti Fabrizio
#include <iostream>

#include "chessboard/chessboard.h"
#include "menu/menu.h"

int main(int argc, char **argv) {
    Menu m;

    // impostazione dei players
    std::vector<Player> players;
    try {
        players = m.get_players(argc, argv);
    } catch (Menu::InvalidArgumentException e) {
        std::cout << "Errore: argomenti non validi";
        return -1;
    }

    // p1 è sempre bianco, p2 è sempre nero
    Player p1(players[0]);
    Player p2(players[1]);

    Chessboard b(p1, p2);

    // white_moves consente di capire se tocca a p1 o p2
    bool white_moves = true;
    while (b.get_status() != 1 && b.get_status() != 2 && b.get_status() != 5 && b.get_status() != 6) {
        // stampa stato
        m.print_status(b.get_turn(), b.get_status(), white_moves);

        Player &cur_player = white_moves ? p1 : p2;
        Player &cur_enemy = white_moves ? p2 : p1;

        std::vector<Pair> move;
        Pair result;
        // ottenimento comando ed esecuzione mossa
        do {
            if (white_moves) {
                move = m.get_move(p1);
            } else {
                move = m.get_move(p2);
            }

            if (move.empty() || move.size() == 1)
                break;

            result = b.move(cur_player, cur_enemy, move[0], move[1]);
        } while (result == Pair{-1, -1});

        // stampa scacchiera
        if (move.empty()) {
            std::cout << b;
            continue;
        }
        if (move.size() == 1) {
            b.set_manual_stall();
            break;
        }

        // stampa la mossa eseguita
        m.print_move(move);

        // promozione
        char prom_id = '0';
        if (result != Pair{8, 8}) {
            prom_id = m.get_promotion(cur_player);
            b.promote(cur_player, cur_enemy, prom_id, result);
        }

        // scrittura nel file di log
        white_moves = !white_moves;
        m.write_log_line(move, b.get_status(), prom_id);
    }

    m.print_status(b.get_turn(), b.get_status(), white_moves);
    m.close_log();

    return 0;
}