// Pivotto Federico
#include "chessboard.h"

Chessboard::Chessboard(Player &p1, Player &p2)
    : kings{std::vector<std::shared_ptr<Piece>>(2)}, status{ongoing}, turn_count{1}, both_cp{p1.get_type() == p2.get_type()} {
    // creazione scacchiera vuota
    board = std::vector<std::vector<std::shared_ptr<Piece>>>(SIDE, std::vector<std::shared_ptr<Piece>>(SIDE));

    std::vector<char> ids_b = {'P', 'R', 'D', 'C', 'A', 'T'};
    std::vector<char> ids_w = {'p', 'r', 'd', 'c', 'a', 't'};

    // preparazione pedine nella scacchiera
    if (p1.get_color() == 'b' && p2.get_color() == 'w') {
        initialize(p1, ids_b);
        initialize(p2, ids_w);
    } else {
        initialize(p1, ids_w);
        initialize(p2, ids_b);
    }

    update_history();
}

int Chessboard::get_status() const {
    return status;
}

int Chessboard::get_turn() const {
    return turn_count;
}

const std::vector<std::vector<char>> Chessboard::get_board() const {
    std::vector<std::vector<char>> pcs(SIDE, std::vector<char>(SIDE));

    for (int i = 0; i < SIDE; i++) {
        for (int j = 0; j < SIDE; j++) {
            pcs[i][j] = (board[i][j]) ? board[i][j]->get_id() : ' ';
        }
    }

    return pcs;
}

void Chessboard::initialize(Player &p, std::vector<char> &ids) {
    std::vector<std::shared_ptr<Piece>> &vp = p.get_pieces();

    // riconosco di quale giocatore sono le pedine ricevute e lavoro di conseguenza
    int row = (std::isupper(ids[0])) ? 1 : 6;

    // segue il posizionamento delle pedine nella scacchiera

    for (int i = 0; i < SIDE; i++)
        vp.push_back(board[row][i] = get_piece(ids[0], {row, i}));  // pawns

    (row == 1) ? row-- : row++;  // cambio riga per le pedine speciali

    vp.push_back(kings[row % 2] = board[row][4] = get_piece(ids[1], {row, 4}));  // king

    vp.push_back(board[row][3] = get_piece(ids[2], {row, 3}));  // queen

    vp.push_back(board[row][1] = get_piece(ids[3], {row, 1}));
    vp.push_back(board[row][6] = get_piece(ids[3], {row, 6}));  // knight

    vp.push_back(board[row][2] = get_piece(ids[4], {row, 2}));
    vp.push_back(board[row][5] = get_piece(ids[4], {row, 5}));  // bishops

    vp.push_back(board[row][0] = get_piece(ids[5], {row, 0}));
    vp.push_back(board[row][7] = get_piece(ids[5], {row, 7}));  // towers
}

std::shared_ptr<Piece> Chessboard::get_piece(char id, Pair pos) const {
    return std::make_shared<Piece>(Piece(id, pos));
}

void Chessboard::set_manual_stall() {
    set_status(stall);
}

void Chessboard::set_status(chessboard_status st) {
    status = st;
}

// da non modificare l'ordine dei controlli
void Chessboard::update_status(const Player &p, const Player &enemy_p) {
    if (is_nil()) {
        set_status(nil);
        return;
    }

    if (!kings[0]->is_alive()) {
        set_status(checkmate_black);
        return;
    }

    if (!kings[1]->is_alive()) {
        set_status(checkmate_white);
        return;
    }

    if (is_checkmate(p, enemy_p)) {
        set_status(p.get_color() == 'w' ? checkmate_black : checkmate_white);
        return;
    }

    if (is_checkmate(enemy_p, p)) {
        set_status(enemy_p.get_color() == 'w' ? checkmate_black : checkmate_white);
        return;
    }

    if (is_stall(p, enemy_p)) {
        set_status(stall);
        return;
    }

    if (is_check(p)) {
        set_status(p.get_color() == 'w' ? check_black : check_white);
        return;
    }

    if (is_check(enemy_p)) {
        set_status(enemy_p.get_color() == 'w' ? check_black : check_white);
        return;
    }

    set_status(ongoing);
}

void Chessboard::update_history() {
    std::string board_string = get_board_string();

    try {
        history.at(board_string);
        history[board_string]++;
    } catch (std::out_of_range e) {
        history.insert(std::make_pair(board_string, 1));
    }
}

bool Chessboard::is_free(Pair pos) const {
    if (!board[pos.row][pos.col])  // se il puntatore e' valorizzato da nullptr
        return true;

    return false;
}

bool Chessboard::is_enemy(const Player &p, Pair pos) const {
    if (!is_free(pos)) {  // se c'e' una pedina
        char cl = p.get_color();
        bool is_up = std::isupper(board[pos.row][pos.col]->get_id());
        if ((cl == 'w' && is_up) || (cl == 'b' && !is_up))  // se in pos c'è una pedina nemica per il giocatore p
            return true;
    }

    return false;
}

bool Chessboard::is_castling(const Player &enemy_p, Pair start, Pair dest) const {  // arrocco
    if (is_free(dest)) return false;

    char ps = std::toupper(board[start.row][start.col]->get_id());
    char pd = std::toupper(board[dest.row][dest.col]->get_id());

    // se non sono re e torre non c'e' arrocco
    if ((ps != 'R' || pd != 'T') && (ps != 'T' || pd != 'R'))
        return false;

    // potrebbe esserci arrocco: seguono le opportune verifiche

    Piece &king = (ps == 'R' && pd == 'T') ? *board[start.row][start.col] : *board[dest.row][dest.col];
    Piece &tower = (ps == 'R' && pd == 'T') ? *board[dest.row][dest.col] : *board[start.row][start.col];

    // re e torre non devono aver gia fatto mosse
    if (king.get_counter() != 0 || tower.get_counter() != 0)
        return false;

    // controllo se re e torre sono nella stessa riga
    if (start.row != dest.row)
        return false;

    // controllo se tra torre e re ci sono caselle libere
    int st = std::min(start.col, dest.col);
    int en = std::max(start.col, dest.col);

    for (++st; st < en; st++) {
        if (!is_free({start.row, st}))
            return false;
    }

    // non deve esserci scacco per il re nelle posizioni che lo conducono a quella finale
    int k_col = king.get_pos().col, t_col = tower.get_pos().col;
    if (k_col < t_col) {  // arrocco corto
        for (int i = k_col + 1; i < k_col + 3; i++) {
            if (is_check_for_us(enemy_p, {start.row, i}))
                return false;
        }
    } else {  // arrocco lungo
        for (int i = k_col - 1; i > k_col - 3; i--) {
            if (is_check_for_us(enemy_p, {start.row, i}))
                return false;
        }
    }

    // controlli superati: arrocco
    return true;
}

bool Chessboard::is_promotable(Pair dest) const {
    if (is_free(dest))
        return false;

    char id_st = board[dest.row][dest.col]->get_id();

    // controllo se la riga raggiunta dal pedone e' la prima o l'ultima a seconda del suo colore
    if ((id_st == 'p' && dest.row == 0) || (id_st == 'P' && dest.row == 7))
        return true;  // controlli superati: promozione

    return false;
}

bool Chessboard::is_enpassant(Pair start, Pair dest) const {
    // la destinazione della possibile mossa di en passant dev'essere free
    if (!is_free(dest))
        return false;

    // controllo se e' un movimento in diagonale di un solo passo
    if (std::abs(start.row - dest.row) != 1 || std::abs(start.col - dest.col) != 1)
        return false;

    // controllo se nella cella adiacente non c'e' una pedina
    if (!board[start.row][dest.col])
        return false;

    // controllo se la pedina adiacente in riga e' una pedina nemico
    Piece &st = *board[start.row][start.col];
    Piece &adj = *board[start.row][dest.col];
    bool is_up = std::isupper(st.get_id());
    bool adj_up = std::isupper(adj.get_id());
    if ((adj_up && is_up) || (!adj_up && !is_up))
        return false;

    // controllo se la pedina adiacente e' un pedone
    if (std::toupper(adj.get_id()) != 'P')
        return false;

    // controllo che la pedina adiacente sia alla sua prima mossa
    Pair p_adj = adj.get_pos();
    if (adj.get_counter() != 1 || (p_adj.row != 3 && p_adj.row != 4))
        return false;

    // controllo che la pedina adiacente abbia eseguito una mossa di passo 2
    if ((adj_up && p_adj.row != 3) || (!adj_up && p_adj.row != 4))
        return false;

    // controllo se la direzione del pedone in start e' quella giusta per eseguire l'en passant
    // dest dev'essere nella riga sopra o sotto (pedone in start bianco / nero) a quella del pedone nemico (nero / bianco) affiancato
    if ((is_up && start.row > dest.row) || (!is_up && start.row < dest.row))
        return false;

    // controlli superati: en passant
    return true;
}

// p: giocatore per cui controlliamo lo scacco del NEMICO
bool Chessboard::is_check(const Player &p) const {
    const std::vector<std::shared_ptr<Piece>> &pcs = p.get_pieces();
    Piece &enemy_king = (is_enemy(p, kings[0]->get_pos())) ? *kings[0] : *kings[1];

    // controllo se almeno una pedina del giocatore p minaccia il re avversario
    for (int i = 0; i < p.get_pieces().size(); i++) {
        if (pcs[i]->is_alive() && is_path_valid(pcs[i]->get_pos(), enemy_king.get_pos()))
            return true;  // controlli superati: nemico in scacco
    }

    return false;
}

bool Chessboard::is_checkmate(const Player &p, const Player &enemy_p) {
    if (is_check(p) && !is_defendable(enemy_p, p))
        return true;  // controlli superati: scaccomatto

    return false;
}

bool Chessboard::is_stall(const Player &p, const Player &enemy_p) {
    // il giocatore a cui tocca muovere non può eseguire alcuna mossa legale ed il suo re NON si trova sotto scacco
    if (!is_check(p) && !is_defendable(enemy_p, p)) return true;

    // un giocatore può dichiarare la patta nel momento in cui la medesima posizione si ripresenta per la terza volta nel corso della partita (non necessariamente per tre volte consecutive)
    if (history[get_board_string()] == 3) return true;

    // sono state giocate 50 mosse consecutive senza che uno dei due giocatori abbia spostato un pedone o catturato un pezzo
    if (p.get_passive_streak() == 50) return true;

    return false;
}

bool Chessboard::is_nil() const {
    return both_cp && turn_count == 50;
}

bool Chessboard::is_check_for_us(const Player &enemy_p, Pair pos) const {
    const std::vector<std::shared_ptr<Piece>> &pcs = enemy_p.get_pieces();

    // controllo se almeno una pedina del giocatore nemico minaccia un'eventuale pedina posizionata in pos
    for (int i = 0; i < enemy_p.get_pieces().size(); i++) {
        if (pcs[i]->is_alive() && is_path_valid(pcs[i]->get_pos(), pos)) {
            return true;  // controlli superati: potenziale scacco in pos
        }
    }

    return false;
}

// p: giocatore che si deve difendere
// enemy_p: giocatore che minaccia il re del giocatore p
bool Chessboard::is_defendable(const Player &p, const Player &enemy_p) {
    // il re e' difendibile quando riesce a scappare dallo scacco con le sue gambe
    // il re e' difendibile quando una qualunque mossa delle altre pedine amiche in celle vuote o dove c'e' un nemico, rende is_check == false

    // flag della difesa del re
    bool defendable = false;

    // controllo se e' possibile ostacolare o mangiare una minaccia
    const std::vector<std::shared_ptr<Piece>> &pcs = p.get_pieces();
    for (int i = 0; i < pcs.size(); i++) {
        if (!pcs[i]->is_alive())
            continue;

        std::vector<Pair> tmp = get_reachable_pos(*pcs[i]);
        for (int j = 0; j < tmp.size(); j++) {
            if (!is_free(tmp[j]) && !is_enemy(p, tmp[j]))  // c'e' un alleato
                continue;                                  // analizza un altra direzione per lo stesso pezzo

            // segue il posizionamento "astratto" di pcs[i] in obst

            Pair obst = tmp[j];  // posizione raggiungibile dalla pedina pcs[i]

            // backup del pezzo
            std::shared_ptr<Piece> p_pc = pcs[i];
            std::shared_ptr<Piece> p_tmp = board[tmp[j].row][tmp[j].col];

            // sposto il pezzo perche direzione raggiungibile
            Pair start = pcs[i]->get_pos();          // posizione di partenza
            board[tmp[j].row][tmp[j].col] = pcs[i];  // sposto la pedina
            board[start.row][start.col].reset();     // annullo la partenza

            // aggiorno la posizione della pedina spostata
            pcs[i]->set_pos(tmp[j]);

            // controllo se per il giocatore p c'e scacco
            defendable = !is_check(enemy_p);

            // ripristino della scacchiera
            board[start.row][start.col] = p_pc;
            board[tmp[j].row][tmp[j].col] = p_tmp;

            // ripristino la posizione della pedina spostata
            pcs[i]->set_pos(start);

            // se ce difendibilita per il re
            if (defendable)
                return true; // re difendibile
        }
    }

    return defendable;
}

std::string Chessboard::get_board_string() const {
    std::string board_string;
    const std::vector<std::vector<char>> b = get_board();

    // costruzione della configurazione della scacchiera in un'unica stringa
    for (int i = 0; i < b.size(); i++) {
        board_string += std::string(b[i].begin(), b[i].end());
    }

    return board_string;
}

std::ostream &operator<<(std::ostream &out, const Chessboard &cb) {
    const std::vector<std::vector<char>> b = cb.get_board();

    // costruzione della scacchiera per la stampa
    for (int i = 0, len = b.size(), cn = 8; i < len; i++, cn--, out << "\n") {
        out << cn << " ";
        for (int j = 0; j < len; j++) {
            out << b[i][j];
        }
    }

    // inserimento lettere nell'ascissa
    char ch = 'A';
    out << "\n  ";
    for (int i = 0; i < b.size(); i++, ch++)
        out << ch;

    return out << "\n";
}