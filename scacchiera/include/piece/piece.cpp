// Molinaroli Davide
#include "piece.h"

// mappa contenente coppie <char, vector<Pair>>
// ovvero ad ogni tipo di pedina associa le direzioni 'unarie' che può intraprendere
// altre funzioni del programma moltiplicano queste direzioni, per le pedine per cui è consentito, per eseguire movimenti "lunghi"
const std::map<char, std::vector<Pair>> Piece::directions = {
    {'R',
     {{0, 1}, {1, 0}, {1, 1}, {0, -1}, {-1, 0}, {-1, -1}, {-1, 1}, {1, -1}}},
    {'D',
     {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}}},
    {'P',
     {{1, 0}, {1, 1}, {1, -1}}},
    {'C',
     {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}}},
    {'A',
     {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}}},
    {'T',
     {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}}
};

// unico costruttore di Piece
Piece::Piece(char identifier, Pair pos, bool a, int count)
    : id{identifier}, position{pos}, alive{a}, counter{count} {}

// getter e setter
char Piece::get_id() const { return id; }

void Piece::set_id(char identifier) { id = identifier; }

Pair Piece::get_pos() const { return position; }

void Piece::set_pos(Pair pos) { position = pos; }

const std::vector<Pair>& Piece::get_piece_directions() const { return directions.at(toupper(id)); }

std::vector<Pair> Piece::get_piece_directions() { return directions.at(toupper(id)); }

bool Piece::is_alive() const { return alive; }

void Piece::set_alive(bool a) { alive = a; }

int Piece::get_counter() const { return counter; }

void Piece::incr_counter() { counter++; }

// ritorna una direzione che porta a dest. return {0,0} se non esiste
Pair Piece::dir_to_dest(Pair dest) const {
    std::vector<Pair> piece_dirs = get_piece_directions();
    const bool has_long_moves = toupper(id) == 'D' || toupper(id) == 'T' || toupper(id) == 'A';
    int k;

    std::vector<Pair>::const_iterator iter;
    for (iter = piece_dirs.cbegin(); iter != piece_dirs.cend(); ++iter) {
        Pair d = *iter;
        k = 1;

        if (id == 'p') k *= -1;

        // primo movimento di un pedone se si vuole muovere in avanti di 2
        if (toupper(id) == 'P' && counter == 0 && d.col == 0 && dest == position + (d * 2 * k)) {
            k *= 2;
            return (*iter) * k;
        }

        // arrocco partendo dal re (quello che si fa partendo dalla torre è già previsto)
        if (toupper(id) == 'R' && counter == 0 && d.row == 0 && (dest == position + (d * (-4)) || dest == position + (d * 3))) {
            k = (dest == position + (d * -4) ? -4 : 3);
            return (*iter) * k;
        }

        if (dest == (position + (d * k))) return (*iter) * k;

        for (k = 1; k <= DIRECTION_MAX_LENGTH && has_long_moves; k++) {
            if (dest == (position + (d * k))) return (*iter) * k;
        }
    }

    return {0, 0};
}

bool operator==(const Piece& p1, const Piece& p2) {  // controllo sull'id
    return p1.get_id() == p2.get_id();
}
