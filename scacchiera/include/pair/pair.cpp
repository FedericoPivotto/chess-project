// Molinaroli Davide
#include "pair.h"

// assegnamento
Pair Pair::operator=(const Pair &p1) {
    row = p1.row;
    col = p1.col;

    return *this;
}

// somma
Pair operator+(const Pair &p1, const Pair &p2) {
    return { p1.row + p2.row, p1.col + p2.col };
}

// sottrazione
Pair operator-(const Pair &p1, const Pair &p2) {
    return { p1.row - p2.row, p1.col - p2.col };
}

// moltiplica entrambe le componenti di un pair per l'intero k
Pair operator*(const Pair &p1, int k) {
    return { p1.row * k, p1.col * k };
}

// divide entrambe le componenti di un pair per l'intero k
Pair operator/(const Pair &p1, int k) {
    return { p1.row / k, p1.col / k };
}

// controlla se le componenti di due pair sono a due a due uguali
bool operator==(const Pair &p1, const Pair &p2) {
    return p1.row == p2.row && p1.col == p2.col;
}

bool operator!=(const Pair &p1, const Pair &p2) {
    return !(p1 == p2);
}
