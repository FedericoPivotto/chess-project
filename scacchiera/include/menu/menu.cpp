// Molinaroli Davide
#include "menu.h"

// costruttore di default
// apre il file ed inizializza il seed per rng
Menu::Menu() {
    std::string file_name = "../log/" + LOG_FILE_NAME;
    log.open(file_name);
    if (!log.good()) {
        std::cout << "Errore: file non aperto correttamente";
        throw FileErrorException();
    }
    log.clear();
    srand(time(NULL));
}

// scrive la mossa (vettore di 2 pair) passata come parametro, status della partita e carattere di promozione nel file di log
void Menu::write_log_line(const std::vector<Pair> move, int status, char prom_id) {
    std::string line = move_to_string(move) + " " + std::to_string(status);
    std::string nl = (status != 1 && status != 2 && status != 5 && status != 6) ? "\n" : "";

    if (prom_id == '0') {
        line += nl;
    } else {
        line += " " + std::string(1, prom_id) + nl;
    }

    log << line;
}

// chiusura dell'ofstream
void Menu::close_log() { log.close(); }

// in base agli argomenti passati all'eseguibile imposta il tipo dei giocatori,
// se vi è un umano ne assegna casualmente il colore
std::vector<Player> Menu::get_players(int argc, char** argv) const {
    if (argc != 2) throw InvalidArgumentException();

    std::string cmd(argv[1]);
    for (char& c : cmd) c = tolower(c);

    if (cmd == "pc") {
        int human_first = get_random_int(0, 1);
        return std::vector<Player>{Player('w', human_first == 1 ? 'h' : 'c'), Player('b', human_first == 1 ? 'c' : 'h')};
    } else if (cmd == "cc") {
        return std::vector<Player>{Player('w', 'c'), Player('b', 'c')};
    }

    throw InvalidArgumentException();
}

// funzione chiamata dal main, restituisce il carattere corrispondente alla pedina a cui si vuole promuovere un pawn,
// utilizza due sottofunzioni private
char Menu::get_promotion(const Player& p) const {
    char prom_id;
    bool uppercase = p.get_color() == 'b';

    if (p.get_type() == 'c') {
        prom_id = get_random_promotion();
    } else {
        prom_id = get_human_promotion();
    }

    return uppercase ? toupper(prom_id) : tolower(prom_id);
}

// chiede al giocatore umano il carattere corrispondente alla pedina a cui vuole promuovere un suo pawn e lo ritorna
char Menu::get_human_promotion() const {
    std::string line;
    char input;
    do {
        std::cout << "Promozione (D,T,A,C): ";
        input = std::getchar();
        std::getchar();
    } while (toupper(input) != 'D' && toupper(input) != 'T' && toupper(input) != 'A' && toupper(input) != 'C');

    return input;
}

// utilizzata per il giocatore computer, ritorna casualmente un carattere (tra quelli previsti dal gioco) per promuovere un pedone
char Menu::get_random_promotion() const {
    const int prom_size = 4;
    char prom[prom_size] = {'D', 'T', 'A', 'C'};
    int i = get_random_int(0, prom_size - 1);
    return prom[i];
}

// in base al tipo di giocatore, ritorna una mossa sottoforma di vector di pair.
// se il giocatore è umano, il vector ha size == 1 se inserisce 'PP PP', size == 0 se inserisce 'XX XX'
std::vector<Pair> Menu::get_move(Player& player) const {
    if (player.get_type() == 'c') {
        return get_random_move(player);
    }

    return get_human_move();
}

// funzione che genera randomicamente una mossa per il giocatore computer.
// dal vector di pieces del giocatore sceglie randomicamente un pezzo, poi scorre tutte le sue direzioni possibili finchè
// non ne viene trovata una che porta in una destinazione che sta nei limiti della scacchiera.
// se il pezzo è un re, gli consente di fare i movimenti previsti dall'arrocco solo se rispetta alcune condizioni basilari;
// se il pezzo è un pedone, viene generato un movimento di 2 solo se rispetta i casi previsti dal gioco, mentre le direzioni 
// diagonali del pedone qui possono essere pescate sempre, sarà poi la scacchiera a verificare le altre condizioni.
// se il pezzo è una regina, un alfiere o una torre, la direzione pescata viene moltiplicata per un numero casuale tra 1 e 7.
// in sostanza: questa funzione ha il compito di generare un movimento casuale che rispetti quanto più possibile i casi particolari, ma sarà poi la scacchiera ad eseguire i controlli approfonditi
std::vector<Pair> Menu::get_random_move(Player& player) const {
    if (player.get_type() != 'c') throw NotComputerException();

    std::vector<std::shared_ptr<Piece>> pieces = player.get_pieces();

    Piece p(' ', {-1, -1});
    Pair dir;
    do {
        do {
            p = *pieces[get_random_int(0, pieces.size() - 1)];
        } while (!p.is_alive());

        std::vector<Pair> directions = p.get_piece_directions();

        dir = directions[get_random_int(0, directions.size() - 1)];

        // se viene pescato un re quando il suo counter è 0 e la direzione orizzontale, diamo la possibilità al computer di eseguire l'arrocco.
        // poi la scacchiera controllerà tutte le altre condizioni (anche se controlla già che counter == 0, un controllino in più
        // non fa mai male). In caso di arrocco, la direzione più lunga può essere 4 (long castling)
        if (toupper(p.get_id()) == 'R' && p.get_counter() == 0 && dir.row == 0) {
            dir = dir * get_random_int(1, 4);
        }

        // Se viene pescata una donna, torre o alfiere moltiplica la direzione per una costante casuale da 0 a 7
        if (toupper(p.get_id()) == 'D' || toupper(p.get_id()) == 'T' || toupper(p.get_id()) == 'A') {
            dir = dir * get_random_int(1, Piece::DIRECTION_MAX_LENGTH);
        }

        // Se viene pescato il pedone del giocatore bianco, moltiplica per -1 la direzione della riga
        // (anche se viene pescata una direzione diagonale, l'unico verso da invertire è quello della riga)
        if (p.get_id() == 'p') dir.row *= -1;

        // Se viene pescato un pedone con direzione verticale e non diagonale e counter == 0, può muoversi di 2
        if (toupper(p.get_id()) == 'P' && p.get_counter() == 0 && dir.col == 0) dir.row *= get_random_int(1, 2);

    } while (!is_line_valid(move_to_string(p.get_pos(), p.get_pos() + dir)));

    return std::vector<Pair>{p.get_pos(), p.get_pos() + dir};
}

// chiede un comando ad un giocatore umano e lo ritorna sottoforma di vector di pair
std::vector<Pair> Menu::get_human_move() const {
    std::string line;
    do {
        std::cout << "Inserisci la mossa: ";
        std::getline(std::cin, line);
        // converte in uppercase
        for (char& c : line) c = toupper(c);

    } while (line != "XX XX" && line != "PP PP" && !is_line_valid(line));

    return string_to_move(line);
}

// stampa lo stato della partita, il turno e a quale giocatore tocca
void Menu::print_status(int turn_count, int status, bool white_moves) const {
    std::cout << "\n-------------TURNO " << turn_count << "-------------\n";
    std::cout << "\nTOCCA AL GIOCATORE " << (white_moves ? "BIANCO" : "NERO") << "\n";

    std::string arr[7] = {
        "PARTITA IN CORSO",
        "PARTITA ANNULLATA",
        "STALLO",
        "BIANCO SOTTO SCACCO",
        "NERO SOTTO SCACCO",
        "BIANCO PERDE PER SCACCO MATTO",
        "NERO PERDE PER SCACCO MATTO"};
    std::cout << arr[status] << "\n\n";
}

// stampa la mossa che è appena stata eseguita
void Menu::print_move(const std::vector<Pair>& move) const {
    std::cout << "\nE' STATA ESEGUITA LA MOSSA " << move_to_string(move) << "\n";
}

// helper function che restituisce un intero tra [min, max]. L'inizializzazione del seed è stata fatta in Menu::Menu()
int get_random_int(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// controlla la sintassi di una mossa inserita da un utente
bool is_line_valid(std::string line) {
    return line.size() == 5 &&
           line.at(0) >= 'A' && line.at(0) <= 'H' &&
           line.at(1) >= '1' && line.at(1) <= '8' &&
           line.at(3) >= 'A' && line.at(3) <= 'H' &&
           line.at(4) >= '1' && line.at(4) <= '8';
}

// converte una stringa che rappresenta una mossa in vettore di pair,
// i pair contengono indici che corrispondono a quelli di una matrice, quindi A1 viene convertito in Pair{7, 0}
std::vector<Pair> string_to_move(std::string line) {
    if (line == "XX XX") return std::vector<Pair>(0);
    if (line == "PP PP") return std::vector<Pair>(1);

    // Le lettere indicano le colonne
    Pair src = {-1 * (line.at(1) - '8'), line.at(0) - 'A'};
    Pair dest = {-1 * (line.at(4) - '8'), line.at(3) - 'A'};

    return std::vector<Pair>{src, dest};
}

// converte un vector di pair che rappresenta una mossa in una stringa con notazione che rispecchia la scacchiera
std::string move_to_string(const std::vector<Pair>& move) {
    std::string line;

    line.push_back(static_cast<char>(move[0].col + 'A'));
    line.push_back(static_cast<char>(-1 * (move[0].row - 8) + '0'));

    line.push_back(' ');

    line.push_back(static_cast<char>(move[1].col + 'A'));
    line.push_back(static_cast<char>(-1 * (move[1].row - 8) + '0'));

    return line;
}

// overload del metodo precedente, il alcuni casi è risultato comodo passare singolarmente i pair anzichè creare un nuovo vector
std::string move_to_string(Pair src, Pair dest) {
    return move_to_string(std::vector<Pair>{src, dest});
}
