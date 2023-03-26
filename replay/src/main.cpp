// Genilotti Fabrizio
#include <iostream>
#include "replay/replay.h"

/*****HELPER*****/
void exe_cmd(const std::string input, Replay &registra, int argc) {
    if (argc == 4) {
        int find = input.find(" ");
        std::ofstream streamOut;
        streamOut.open(input.substr(find + 1));
        if (!streamOut.good()) {  
            std::cout << "Errore: file output non esistente";
            return;
        }
        registra.write_stream(streamOut);
        streamOut.close();
    } else {
        registra.stream_file();
    }
}
bool get_cmd(std::string &input, std::vector<std::string> &cmd, int argc) {
    if (argc == 3 && cmd[0] == "v") {
        input = "../log/" + cmd[1];
        std::cout << input;
        return true;
    } else if (argc == 4 && cmd[0] == "f") {
        input = "../log/" + cmd[1] + " " + "../log/" + cmd[2];
        return true;
    }
    return false;
}
void open_file(std::string input, int argc, std::ifstream &streamIn) {
    if (argc == 3) {
        streamIn.open(input);
    } else {
        int find = input.find(" ");
        streamIn.open(input.substr(0, find));
    }
}

/*****MAIN*****/
int main(int argc, char **argv) {
    std::vector<std::string> cmd(argv + 1, argv + argc); 
    std::string input;
    //Get and validating command
    if (!get_cmd(input, cmd, argc)) {  //Verifico che il comando sia corretto
        std::cout << "Errore: comando non supportato";
        return -1;
    }
    //Opening file
    std::ifstream streamIn;
    open_file(input, argc, streamIn); //Distinguo l'apertura di file in base al comando
    if (!streamIn.good()) {
        std::cout << "Errore: file input non esistente";
        return -1;
    }
    Replay registra(streamIn);
    streamIn.close();
    //Command execution
    exe_cmd(input, registra, argc); //Eseguo in base al comando ottenuto
    return 0;
}