// Genilotti Fabrizio
#include "replay.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

Replay::Replay(std::ifstream &read) {   //Memorizzo mosse ed imposto la scacchiera iniziale
    std::string row;                  
    while (!read.eof()) {              
        std::getline(read, row);
        rec.push_back(row);
    }
    char whitepieces[9] = {'t', 'c', 'a', 'd', 'r', 'a', 'c', 't', '\0'};
    char blackpieces[9] = {'T', 'C', 'A', 'D', 'R', 'A', 'C', 'T', '\0'};
    nmove = 0;
    //Momentum
    for (int a = 0; a < 8; a++) {
        momentum[0][a] = blackpieces[a];
    }
    for (int a = 0; a < 8; a++) {
        momentum[1][a] = 'P';
    }
    for (int b = 0; b < 8; b++) {
        momentum[2][b] = ' ';
        momentum[3][b] = ' ';
        momentum[4][b] = ' ';
        momentum[5][b] = ' ';
    }
    for (int c = 0; c < 8; c++) {
        momentum[6][c] = 'p';
    }
    for (int a = 0; a < 8; a++) {
        momentum[7][a] = whitepieces[a];
    }
}

void Replay::write_stream(std::ofstream &write) {  //Stream in output
    std::vector<std::string>::iterator iter = rec.begin();
    write_config(write, "Inizio partita!");
    for (int i = 0; i < rec.size(); i++) {   //Ciclo fino a quando non rimangono mosse
        //Eseguo mossa
        int status = move(*iter);           
        turn(write);
        iter++;
        write_config(write, real_status(status));  //Mostra scacchiera con mossa appena eseguita
    }
}

void Replay::stream_file() {  //Stream su terminale
    std::vector<std::string>::iterator iter = rec.begin();
    stream_screen("Inizio partita!");
    for (int i = 0; i < rec.size(); i++) {  //Ciclo fino a quando non rimangono mosse
        sleep(1); 
        //Eseguo mossa
        int status = move(*iter);  
        turn();
        iter++;
        stream_screen(real_status(status));        //Mostra scacchiera con mossa appena eseguita
    }
}

int Replay::move(std::string command) {  //Eseguo mossa controllando mosse speciali
    //Primo termine                 
    char oCol = command.at(0);  
    int originCol = oCol - 65; 
    char oRow = command.at(1); 
    int originRow = oRow - 48;
    originRow = row_calibration(originRow);
    //Secondo termine
    char dCol = command.at(3);
    int destCol = dCol - 65;
    char dRow = command.at(4);
    int destRow = dRow - 48;
    destRow = row_calibration(destRow);
    //Acquisizione status
    char status = command.at(6);
    int stat = status - 48;
    // Arrocco
    if (!casteling_wb(originRow, originCol, destRow, destCol)) {  
        char movLetter = momentum[originRow][originCol];
        momentum[destRow][destCol] = movLetter;
    }
    //Enpassaint
    enpassaint_wb(destRow, destCol);
    //Promozione
    if (command.size() == 9) {
        promotion_wb(originRow, originCol, destRow, destCol, command.at(8));
    }
    nmove++; 
    //Controllo turno
    momentum[originRow][originCol] = ' ';  
    return stat;                           
}

//PRIVATE MEMBER
//***STREAMS***
void Replay::write_config(std::ofstream &write, const std::string status) { //Scrive su file
    std::string nturn = std::to_string(nmove);
    if (nturn != "0") {            //Mostro informazioni sulla mossa in corso e la situazione generale
        write << "Mossa: " + nturn << "    Turno: " + std::to_string(nmove / 2 + nmove % 2) << "\n"
              << "Status: " + status << "\n";
    }else{
        write << "Status: " + status <<  "\n";
    }
    for (int i = 0; i < 8; i++) {  //Ciclo per mostrare scacchiera in goni istante
        write << row_calibration(i) << " ";
        for (int j = 0; j < 8; j++) {
            write << momentum[i][j];
        }
        write << std::endl;
    }
    write << "\n  ABCDEFGH"
          << "\n\n\n";
}

void Replay::stream_screen(const std::string status) {  
    std::string nturn = std::to_string(nmove);
    if (nturn != "0") {       // Mostro informazioni sulla mossa in corso e la situazione generale
        std::cout << "Mossa: " + nturn << "    Turno: " + std::to_string(nmove/2 +nmove % 2) << "\n"
                  << "Status: " + status << "\n";
    } else {
        std::cout << "Status: " + status << "\n";
    }
    for (int i = 0; i < 8; i++) {  // Ciclo per mostrare scacchiera in ogni istante
        std::cout << row_calibration(i) << " ";
        for (int j = 0; j < 8; j++) {
            std::cout << momentum[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "\n  ABCDEFGH"
              << "\n\n"
              << "\n";
}

//***CALIBRATION***
int Replay::row_calibration(int row) {  //Funzione f(x)=|x-8| mi da le rige adatte alla matrice
    return abs(row - 8);
}
std::string Replay::real_status(int status) { //Stampo lo status 
    if (status == 0) {
        return "PARTITA IN CORSO";
    }
    if (status == 1) {
        return "PARTITA ANNULLATA";
    }
    if (status == 2) {
        return "STALLO";
    }
    if (status == 3) {
        return "BIANCO SOTTO SCACCO";
    }
    if (status == 4) {
        return "NERO SOTTO SCACCO";
    }
    if (status == 5) {
        return "BIANCO PERDE PER SCACCO MATTO";
    }
    if (status == 6) {
        return "NERO PERDE PER SCACCO MATTO";
    }
    return "NERO PERDE PER SCACCO MATTO";
}

//***SPECIAL MOVES***
void Replay::enpassaint_wb(int destRow, int destCol) {
    if (momentum[destRow][destCol] == 'p' && momentum[destRow + 1][destCol] == 'P') {  
        momentum[destRow + 1][destCol] = ' ';                                          
    }
    if (momentum[destRow][destCol] == 'P' && momentum[destRow - 1][destCol] == 'p') {
        momentum[destRow - 1][destCol] = ' ';  
    }
}

void Replay::promotion_wb(int originRow, int originCol, int destRow, int destCol, char cm8) {
    if (momentum[destRow][destCol] == 'p' && destRow == 0) {
        char promotion = cm8;
        momentum[destRow][destCol] = promotion;
    }
    if (momentum[destRow][destCol] == 'P' && destRow == 7) {
        char promotion = cm8;
        momentum[destRow][destCol] = promotion;
    }
}

bool Replay::casteling_wb(int originRow, int originCol, int destRow, int destCol) { //Controllo tutti i casi
    int distance = abs(originCol - destCol);
    if (momentum[originRow][originCol] == 'R' && momentum[destRow][destCol] == 'T' && momentum[destRow][destCol] == momentum[originRow][originCol + 3] && originRow == 0 && destRow == 0 && distance == 3) {  // Arrocco nero e corto
        momentum[originRow][originCol + 2] = 'R'; 
        momentum[originRow][originCol + 1] = 'T';  
        momentum[destRow][destCol] = ' ';   
        return true;
    }
    if (momentum[originRow][originCol] == 'r' && momentum[destRow][destCol] == 't' && momentum[destRow][destCol] == momentum[originRow][originCol + 3] && originRow == 7 && destRow == 7 && distance == 3) {  // Arrocco bianco e corto
        momentum[originRow][originCol + 2] = 'r';
        momentum[originRow][originCol + 1] = 't'; 
        momentum[destRow][destCol] = ' ';       
        return true;
    }
    if (momentum[originRow][originCol] == 'R' && momentum[destRow][destCol] == 'T' && momentum[destRow][destCol] == momentum[originRow][originCol - 4] && originRow == 0 && destRow == 0) {  // Arrocco nero e lungo
        momentum[originRow][originCol - 2] = 'R';  
        momentum[originRow][originCol - 1] = 'T';  
        momentum[destRow][destCol] = ' ';     
        return true;
    }
    if (momentum[originRow][originCol] == 'r' && momentum[destRow][destCol] == 't' && momentum[destRow][destCol] == momentum[originRow][originCol - 4] && originRow == 7 && destRow == 7) {  // Arrocco bianco e lungo
        momentum[originRow][originCol - 2] = 'r';  
        momentum[originRow][originCol - 1] = 't';  
        momentum[destRow][destCol] = ' ';      
        return true;
    }
    //VICEVERSA
    if (momentum[originRow][originCol] == 'T' && momentum[destRow][destCol] == 'R' && momentum[destRow][destCol] == momentum[originRow][originCol - 3] && originRow == 0 && destRow == 0 && distance == 3) {  // Arrocco nero e corto
        momentum[originRow][originCol - 1] = 'R';
        momentum[originRow][originCol - 2] = 'T';
        momentum[destRow][destCol] = ' ';
        return true;
    }
    if (momentum[originRow][originCol] == 't' && momentum[destRow][destCol] == 'r' && momentum[destRow][destCol] == momentum[originRow][originCol - 3] && originRow == 7 && destRow == 7 && distance == 3) {  // Arrocco bianco e corto
        momentum[originRow][originCol - 1] = 'r';
        momentum[originRow][originCol - 2] = 't';
        momentum[destRow][destCol] = ' ';
        return true;
    }
    if (momentum[originRow][originCol] == 'T' && momentum[destRow][destCol] == 'R' && momentum[destRow][destCol] == momentum[originRow][originCol + 4] && originRow == 0 && destRow == 0) {  // Arrocco nero e lungo
        momentum[originRow][originCol + 2] = 'R';
        momentum[originRow][originCol + 3] = 'T';
        momentum[destRow][destCol] = ' ';
        return true;
    }
    if (momentum[originRow][originCol] == 't' && momentum[destRow][destCol] == 'r' && momentum[destRow][destCol] == momentum[originRow][originCol + 4] && originRow == 7 && destRow == 7) {  // Arrocco bianco e lungo
        momentum[originRow][originCol + 2] = 'r';
        momentum[originRow][originCol + 3] = 't';
        momentum[destRow][destCol] = ' ';
        return true;
    }
    return false;
}

void Replay::turn() {
    if (nmove % 2 != 0) {
        std::cout << "Giocatore bianco esegue: "
                  << (rec[nmove - 1]).substr(0, 5) << "\n";
    } else if (nmove % 2 == 0) {
        std::cout << "Giocatore nero esegue: "
                  << (rec[nmove - 1]).substr(0, 5) << "\n";
    }
}

void Replay::turn(std::ofstream &write) {
    if (nmove % 2 != 0) {
        write << "Giocatore bianco esegue: "
              << (rec[nmove-1]).substr(0,5) << "\n";
    } else if (nmove % 2 == 0) {
        write << "Giocatore nero esegue: "
              << (rec[nmove-1]).substr(0,5) << "\n";
    }
}