// Molinaroli Davide
#ifndef PAIR_H
#define PAIR_H

class Pair {

public:

        int row;
        int col;

        Pair operator=(const Pair &p1);
};

Pair operator+(const Pair &p1, const Pair &p2); 
Pair operator-(const Pair &p1, const Pair &p2); 
Pair operator*(const Pair &p1, int k);
Pair operator/(const Pair &p1, int k);
bool operator==(const Pair &p1, const Pair &p2);
bool operator!=(const Pair &p1, const Pair &p2);

#endif // PAIR_H