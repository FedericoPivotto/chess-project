// Molinaroli Davide
#include "chessboard.h"

// funzione che esegue tutti i controlli sui movimenti. 
// ritorna {-1 -1} se la mossa non è valida, {8, 8} se è valida e non prevede promozioni, dest se bisogna promuovere il pedone spostato 
Pair Chessboard::move(Player &p, const Player &enemy_p, Pair start, Pair dest) {
    Pair invalid{-1, -1}, valid{8, 8}, promotion{dest};

    // in start non c'e' alcuna pedina
    if (!board[start.row][start.col])
        return invalid;

    // le posizioni non possono coincidere
    if (start == dest)
        return invalid;

    // si possono muovere solo pedine alleate
    if (is_enemy(p, start))
        return invalid;

    char id = board[start.row][start.col]->get_id();

    // pedone che si muove verticalmente deve avere dest libera
    if (toupper(id) == 'P' && start.col == dest.col && !is_free(dest))
        return invalid;

    // ogni mossa deve avere che dest sia enemy o free a meno che non si tratti di un arrocco
    if (!is_enemy(p, dest) && !is_free(dest) && !is_castling(enemy_p, start, dest))
        return invalid;

    // pedone può muoversi in diagonale solo se dest è enemy o se fa un enpassant
    if (toupper(id) == 'P' && start.col != dest.col && !is_enemy(p, dest) && !is_enpassant(start, dest))
        return invalid;

    // la destinazione deve essere raggiungibile attraverso una direzione specifica della pedina
    if (!is_path_valid(start, dest))
        return invalid;

    // arrivati qui abbiamo una mossa valida
    // ora bisogna riconoscere il tipo di mossa (normale oppure arrocco oppure enpassant) ed eseguirla

    // tecnica del cortocircuito
    if (!is_enemy(p, dest) && !is_free(dest) && is_castling(enemy_p, start, dest))
        move_castling(start, dest);

    else if (is_enpassant(start, dest))
        move_enpassant(start, dest);

    // mossa normale: dest è enemy oppure free (else rispetto a castling e enpassant)
    else {
        if (is_enemy(p, dest)) board[dest.row][dest.col]->set_alive(false);
        move_normal(start, dest);
    }

    // aggiornamento numero turno se ha mosso il giocatore nero
    if (p.get_color() == 'b') turn_count++;

    // se un giocatore non muove un pedone e non mangia un nemico incrementa il contatore per lo stallo
    if (toupper(id) != 'P' && !is_enemy(p, dest))
        p.incr_passive_streak();
    else
        p.reset_passive_streak();

    // se la mossa risulta in una promozione, ritorna la dest al main
    if (!is_free(dest) && is_promotable(dest))
        return promotion;

    // inserisce la nuova configurazione della scacchiera in history
    update_history();
    // aggiornamento dello stato
    update_status(p, enemy_p);

    return valid;  // mossa andata a buon fine
}

// funzione che controlla se la pedina che si trova nella posizione start ha una direzione il cui percorso nella scacchiera
// è libero per raggiungere dest. Vengono anche fatti controlli su movimenti speciali di  pedone e re
// dato che questa funzione viene chiamata anche da quelle che controllano lo stato della partita 
// (is_check, is_defendable)
bool Chessboard::is_path_valid(Pair start, Pair dest) const {
    Pair dir;
    const char id = board[start.row][start.col]->get_id();
    const bool has_long_moves = toupper(id) == 'D' || toupper(id) == 'T' || toupper(id) == 'A';

    // dest_id serve per controllare che il re si muova di più di 1 solo per l'arrocco
    char dest_id;
    try {
        dest_id = board.at(dest.row).at(dest.col) ? board.at(dest.row).at(dest.col)->get_id() : '_';
    } catch (std::out_of_range) {
        dest_id = '_';
    }

    dir = board[start.row][start.col]->dir_to_dest(dest);
    if (dir == Pair{0, 0}) return false;

    // se per un re viene generata una direzione orizzontale più lunga di 1, è valida solo per un arrocco
    if (toupper(id) == 'R' && abs(dir.row) == 0 && abs(dir.col) != 1 && toupper(dest_id) != 'T') {
        return false;
    }

    // un pedone deve avere la destinazione libera se si muove verticalmente (necessario per is_check 
    // e gli altri controlli di stato)
    if (toupper(id) == 'P' && start.col == dest.col && !is_free(dest)) {
        return false;
    }

    // se un pedone vuole muoversi di 2 devo controllare che la cella di fronte sia libera
    if (!has_long_moves && !(toupper(id) == 'P' && abs(dir.row) == 2 && dir.col == 0)) {
        return true;
    }

    // temp_dir è la direzione composta solo da +-1
    // non accade mai che dir.row = dir.col = 0
    const Pair temp_dir = dir / std::max(abs(dir.row), abs(dir.col));
    int k = 1;

    // minore e non minore uguale perchè la cella in dest può essere occupata da nemico o alleato (arrocco),
    while (k < std::max(abs(dir.row), abs(dir.col))) {
        if (!is_free(start + (temp_dir * k))) {
            return false;
        }
        k++;
    }

    return true;
}

// esegue lo spostamento di pedine a livello di puntatori, 
// inoltre aggiorna la variabile membro position e counter del pezzo spostato,
// così possiamo controllare lo stato dei pedoni di un giocatore dalla sua variabile membro (privata) Player::pieces
void Chessboard::move_normal(Pair start, Pair dest) {
    // aggiorna la posizione nella scacchiera
    board[dest.row][dest.col] = board[start.row][start.col];

    // aggiorna lo stato della pedina
    board[start.row][start.col]->set_pos(dest);
    board[start.row][start.col]->incr_counter();

    // reset del puntatore a start
    board[start.row][start.col].reset();
}

// aggiorna lo stato della pedina che verrebbe eliminata con un en passant
void Chessboard::move_enpassant(Pair start, Pair dest) {
    move_normal(start, dest);

    // resetta il puntatore al pedone eliminato
    board[start.row][dest.col]->set_alive(false);
    board[start.row][dest.col].reset();
}

// esegue l'arrocco a livello di puntatori
void Chessboard::move_castling(Pair start, Pair dest) {
    // arrocco dx dir == 3: re va in avanti di 2, torre va in re.col +1
    // arrocco sx dir == 4: re va indietro di 2, torre va in re.col -1
    const int sign = abs(start.col - dest.col) == 3 ? +1 : -1;

    Piece &start_piece = *board[start.row][start.col];
    Piece &dest_piece = *board[dest.row][dest.col];

    // re start
    if (toupper(start_piece.get_id()) == 'R') {
        board[start.row][start.col + (sign * 2)].reset(&start_piece);
        board[start.row][start.col + sign].reset(&dest_piece);

        start_piece.set_pos({start.row, start.col + (sign * 2)});
        dest_piece.set_pos({start.row, start.col + sign});
    }
    // torre start
    else {
        board[dest.row][dest.col + (sign * 2)].reset(&dest_piece);
        board[dest.row][dest.col + sign].reset(&start_piece);

        dest_piece.set_pos({dest.row, dest.col + (sign * 2)});
        start_piece.set_pos({dest.row, dest.col + sign});
    }

    // reset finale
    board[start.row][start.col].reset();
    board[dest.row][dest.col].reset();
}

// promuove una pedina aggiornandone l'id. Dopodichè aggiorna, se necessario, lo stato della partita
void Chessboard::promote(const Player &p, const Player &enemy_p, char prom_id, Pair pos) {
    if (is_free(pos)) return;

    board[pos.row][pos.col]->set_id(prom_id);

    update_history();
    update_status(p, enemy_p);
}

// ritorna un vettore contenente tutte le posizioni della scacchiera raggiungibili da un pezzo.
// necessaria per is_defendable
std::vector<Pair> Chessboard::get_reachable_pos(const Piece &pc) const {
    std::vector<Pair> recheable_pos;

    if (!pc.is_alive())
        return recheable_pos;  // nessuna direzione valida (vettore vuoto)

    for (int i = 0; i < SIDE; i++) {
        for (int j = 0; j < SIDE; j++) {
            Pair tmp{i, j};
            if (is_path_valid(pc.get_pos(), tmp))
                recheable_pos.push_back(tmp);
        }
    }

    // posizioni raggiungibili dalla pedina pc
    return recheable_pos;
}