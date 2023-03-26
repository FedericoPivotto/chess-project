// Genilotti Fabrizio
#ifndef REPLAY_H
#define REPLAY_H

#include <fstream>
#include <string>
#include <vector>

class Replay {
   public:
    Replay(std::ifstream &stream);            
    void write_stream(std::ofstream &write); //Si occupa di mandare in output il replay  
    void stream_file();                      //Fa lo stream del replauy intero su terminale
   private:
    int move(const std::string command);    //Effettua mossa dei char delle pedine
    void stream_screen(const std::string status); //Stampa singola scacchiera su terminale
    void write_config(std::ofstream &write, const std::string status); //Scrive singola scacchiera su output
    int row_calibration(int row);  //Converte da riga da scacchiera a riga da matrice
    std::string real_status(int status);
    void enpassaint_wb(int destRow, int destCol);  //Controlla se avvengono mosse speciali
    void promotion_wb(int originRow, int originCol, int destRow, int destCol, char cm8);
    bool casteling_wb(int originRow, int originCol, int destRow, int destCol);
    void turn();      //Stampo la mossa eseguita 
    void turn(std::ofstream &write);

    char momentum[8][8];             //Istante in cui si trova la scacchiera
    std::vector<std::string> rec;    //Memorizza le mosse da svolgere
    int nmove;
};

#endif