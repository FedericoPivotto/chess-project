# Progetto Finale di Laboratorio di Programmazione
## Scacchiera Elettronica

GRUPPO: Gli Autostoppisti
AUTORI: Molinaroli Davide, Pivotto Federico, Genilotti Fabrizio



### Osservazioni sul funzionamento del programma
 
 - Il modulo "scacchiera" produce un file di log avente sempre il nome "log.txt": questo si può modificare in riga 39 di "menu.h".
 - La mossa speciale di "arrocco" richiede come sorgente una torre (o re) e come punto di arrivo un re (o torre), e non posizioni intermedie come destinazione: ad esempio "E1 H1" è una mossa di arrocco, mentre "E1 G1" non lo è.
 - Nel modulo "scacchiera" è previsto il comando "PP PP" indifferentemente maiuscolo o minuscolo (come gli altri comandi) per accordare la patta fra i due giocatori (risulta sempre accordata).
 - Per un corretto funzionamento del modulo "replay", avviarlo a riga di comando con parametro [nome_file_log] = "log.txt" e parametro [nome_file_output_replay] a piacimento.



### Osservazioni generali sulla struttura del programma
 
 - Nel modulo "scacchiera":
    * La classe "Menu" ha il compito di interfacciarsi con l'utente, generare o ricevere le mosse e scrivere nel file di log.
    * La classe "Chessboard" ha il compito di gestire il gioco degli scacchi, il movimento delle pedine e controllare lo stato della partita.
      Il codice di "Chessboard" è diviso in due file ".cpp" dove: il file "chessboard.cpp" contiene tutti i controlli di mosse speciali e aggiornamenti dello stato, inizializzazione della scacchiera; nel file "movement.cpp" viene gestito il movimento delle pedine all'interno della scacchiera.
    * La classe "Piece" rappresenta le pedine ed in essa vi sono funzioni membro utili alla classe "Chessboard".
    * La classe "Player" rappresenta i giocatori, contiene un vettore puntatori (shared_ptr) alle pedine che ci consentono di elaborare più agevolmente lo stato della partita.
    Abbiamo scelto di utilizzare gli shared_ptr per facilitare la gestione dei puntatori e per garantire una corretta sincronizzazione tra vector di pedine del giocatore e puntatori della scacchiera.
    * La classe "Pair" è una classe ausiliaria per gestire agevolmente le coordinate delle pedine.
      Abbiamo deciso di lasciare public le variabili membro row e col per non appesantire le notazioni nel codice
 
 - Nel modulo "replay":
    * La classe "Replay" ha il compito di interpretare il file di log prodotto dal modulo "scacchiera" e specificato da riga di comando, ed eventualmente produce un file di output con all'interno il replay della partita a scacchi.
    * Non è possibile usare nomi di file che contengono spazi quando si scrivono negli argomenti dell'eseguibile replay



### Esempio di comandi di esecuzione
.\scacchiera cc
.\scacchiera pc
.\replay f log.txt out.txt
.\replay v log.txt
