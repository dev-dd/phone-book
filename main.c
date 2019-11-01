/* Implementare una rubrica telefonica seguendo le seguenti specifiche:

0) Definire una enumerazione 'TipologiaContatto' che permetta di utilizzare i seguenti valori: lavoro, famiglia, amici,
 altri.
1) Definire una struttura 'Contatto' contenente:
- nome, cognome, numero di telefono, indirizzo email, tipologia contatto
2) Implementare una funzione o procedura che acquisisca i campi di un Contatto e restituisca tale contatto. Effettuare
 i controlli sull'input:
- nome (non vuoto, non deve contenere cifre)
- cognome (non vuoto, non deve contenere cifre)
- numero di telefono (non vuoto, non deve contenere simboli o lettere)
- indirizzo email (deve contenere almeno un carattere prima della chiocciola, almeno un carattere chiocciola, almeno un
 carattere dopo la chiocchiola, un punto e un dominio come com o it)
- tipologia (deve essere richiesto in modo semplice per l'utente, ad esempio usando un menu)
3) Implementare una procedura che stampi in modo chiaro e ordinato tutti i dati del contatto
4) Implementare una funzione o procedura che permetta la modifica di un contatto. Riceverà come parametro un contatto,
 lo stamperà e chiederà quale campo deve essere modificato. Una volta inserito il nuovo valore, a seconda dei casi il
 contatto dovrà essere restituito al chiamante o meno (dipende se si utilizza una procedura o una funzione).
5) Implementare una funzione che inizializzi un vettore dinamico. Essa dovrà allocare lo spazio per un contatto e
 restituire l'indirizzo di memoria relativo se l'allocazione è andata a buon fine. Altrimenti il programma terminerà
 comunicando all'utente tale malfunzionamento.
6) Implementare la funzione di inserimento seguendo la logica seguente: passo il contatto, aumento lo spazio del
 vettore dinamico per contenere un nuovo elemento e a questo punto inserisco il nuovo contatto seguendo l'ordine
 alfabetico. Si possono presentare due casi: inserimento in coda, inserimento non in coda. Se in coda non dovremo
 far altro che inserire il contatto nell'ultima (e nuova) posizione, altrimenti una volta trovato il contatto
 successivo in ordine alfabetico dovremo far scorrere tutti i contatti di una posizione e inserire il nuovo contatto
 nella corretta posizione. Al termine delle operazioni ricordarsi di tenere traccia dell'eventuale cambio di valore del
 puntatore al vettore dinamico.
7) Implementare la funzione di eliminazione offrendo le seguenti possibilità:
- eliminare tutta la rubrica
- eliminare un contatto con un certo indice
- eliminare tutti i contatti con un certo nome o cognome
- elininare una categoria di contatti
 8) Aggiungere alle funzionalità del programma la ricerca dei contatti offrendo le seguenti possibilità:
- ricerca mediante nome (o parte del nome)
- ricerca mediante cognome (o parte del cognome)
- ricerca mediante mail (o parte del mail)
- ricerca mediante numero
- ricerca mediante categoria

In tutti i casi dovremo sempre stare attenti a modificare lo spazio occupato dal vettore, se ad esempio abbiamo un
 vettore di 3 contatti A, B e C e dobbiamo eliminare B sposteremo C in B e riallocheremo lo spazio eliminando un
 elemento (l'ultimo). Ricordarsi sempre di restituire il puntatore al vettore dinamico. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LENGTH 30 // Lunghezza massima dei campi nome e cognome.
#define MESSAGE 100 // Lunghezza massima dei messaggi di errore.
#define MODIF_MIN 1 // Valore minimo per la scelta durante la modifica del contatto.
#define MODIF_MAX 5 // Valore massimo per la scelta durante la modifica del contatto.

#define NAME_LABEL "Nome"
#define SRNAME_LABEL "Cognome"
#define TEL_LABEL "Telefono"
#define EMAIL_LABEL "E-mail"
#define GROUP_LABEL "\nGruppo:\n1) Lavoro\n2) Famiglia\n3) Amici\n4) Altro\n"

typedef enum {LAVORO, FAMIGLIA, AMICI, ALTRO} TipologiaContatto;
typedef enum {NOME, TELEFONO, EMAIL} Controllo; // Enum utilizzata dalla funzione validator

typedef struct {
    char nome[LENGTH + 1];
    char cognome[LENGTH + 1];
    char telefono[LENGTH + 1];
    char email[LENGTH + 1];
    TipologiaContatto gruppo;
} Contatto;

/* Struttura per ritornare un messaggio di errore e un codice ad esso associato. Il codice non si è poi rivelato
 * utile ai fini dell'esecuzione del programma. */
typedef struct {
    int code;
    char message[MESSAGE];
} Response;

void newContact(Contatto *new);
void readLine(char str[], int length);
void ignoreInputUntil(char endCh);
Response validator(char str[], Controllo ctrl);
void toLowercase(char str[]);
void getInput(char *inStr, int length, Controllo ctrl, char label[]);
TipologiaContatto getGroup();
void printContact(Contatto *cont);
_Bool checkRange(int toCheck, int min, int max, char msg[MESSAGE]);
void modifyContact(Contatto *cont);

int main() {

    Contatto nuovoContatto;
    Contatto nuovoContatto2 = {"alberto", "barabba", "3221134", "reverendjack@hotmail.com", AMICI};

    newContact(&nuovoContatto);
    printContact(&nuovoContatto);
    modifyContact(&nuovoContatto);
    printContact(&nuovoContatto);

    return 0;
}

/**
 * Prende in ingresso un nuovo contatto e acquisisce in input tutti i campi.
 *
 * @param nuovo Puntatore al nuovo contatto.
 */
void newContact(Contatto *new) {

    getInput(new->nome, LENGTH + 1, NOME, NAME_LABEL);
    getInput(new->cognome, LENGTH + 1, NOME, SRNAME_LABEL);
    getInput(new->telefono, LENGTH + 1, TELEFONO, TEL_LABEL);
    getInput(new->email, LENGTH + 1, EMAIL, EMAIL_LABEL);
    new->gruppo = getGroup();

}

/**
 * Popola una stringa di caratteri inserendo il terminatore alla fine di essa quando viene premuto il carattere '\n'
 * o quando si raggiunge la sua fine. Vengono effettuati controlli sulla lunghezza della stringa per evitare di
 * riempire il buffer con input indesiderati.
 *
 * @param str Stringa da popolare.
 * @param length Lunghezza della stringa.
 */
void readLine(char str[], int length) {
    int i = 0; // Indice.
    char ch; // Variabile di buffer.

    /* Il ciclo viene ripetuto finché non viene premuto il carattere '\n' */
    do {
        /* Se */
        if (i != length) {
            ch = getchar();
            str[i] = ch;
            i++;
            /* Se si arriva alla fine della stringa, ogni nuovo carattere digitato viene ignorato dalla funzione
             * ignoreInputUntil e viene inserito il carattere '\n' nella variabile di buffer ch in maniera tale da
             * causare l'uscita dal ciclo do-while. */
        } else {
            ignoreInputUntil('\n');
            ch = '\n';
            str[i] = ch;
            i++;
        }
    } while(ch != '\n');

    str[i - 1] = '\0'; // Inserimento del terminatore dopo l'ultimo carattere digitato in input.
}

/**
 * Ignora tutti i caratteri digitati finché non viene digitato il carattere endCh.
 *
 * @param endCh Quando viene digitato il carattere endCh la funzione termina.
 */
void ignoreInputUntil(char endCh) {
    char ch;

    do {
        ch=getchar();
    } while(ch != endCh);
}

/**
 * Riceve in ingresso la stringa da validare ed un intero che indica il tipo di validazione.
 *
 * @param str Stringa da validare.
 * @param ctrl Tipo di validazione.
 * @return 0 e il messaggio "OK" se la stringa è valida, altrimenti un codice di errore e il relativo messaggio.
 */
Response validator(char str[], Controllo ctrl) {
    Response res;
    int i, j, k;
    char strLocal[strlen(str) + 1]; // Stringa locale dove verrà copiata la stringa ricevuta come parametro per essere elaborata dai controlli.

    strcpy(strLocal, str); // Copia della stringa ricevuta come parametro.
    toLowercase(strLocal); // strLocal viene convertita il lowercase per facilitare i controlli

    i = 0;
    j = strlen(strLocal);

    /* Se la stringa inizia con '\n' o ' ', viene restituito il codice di errore -1: il campo non può essere vuoto o
     * iniziare con uno spazio. */
    if (strLocal[i] == ' ' || strLocal[i] == '\0') {
        strcpy(res.message, "Errore, il campo non può essere vuoto.");
        res.code = -1;
        return res;
    }

    switch(ctrl) {

        case NOME:
            while(strLocal[i] != '\0') {
                /* Se all'interno della stringa del nome o del cognome sono presenti caratteri diversi dalle lettere
                 * dell'alfabeto latino, viene restituito il codice di errore -2. */
                if (strLocal[i] < 'a' || strLocal[i] > 'z') {
                    strcpy(res.message, "Errore, il nome e il cognome possono contenere solo lettere.");
                    res.code = -2;
                    return res;
                }
                i++;
            }
            break;

        case TELEFONO:
            while(strLocal[i] != '\0') {
                /* Se all'interno della stringa del numero di telefono sono presenti caratteri diversi dalle cifre,
                 * viene restituito il codice di errore -3. */
                if (strLocal[i] < '0' || strLocal[i] > '9'){
                    strcpy(res.message, "Errore, il numero di telefono può contenere solamente cifre.");
                    res.code = -3;
                    return res;
                }
                i++;
            }
            break;

        case EMAIL:
            while(strLocal[i] != '@' && i != j) // Cicla fino a trovare la posizione del carattere '@' o, nel caso non fosse presente, fino alla fine della stringa.
                i++;
            while(strLocal[j] != '.' && j != -1) // Cicla fino a trovare la posizione del carattere '.' o, nel caso non fosse presente, fino all'inizio della stringa.
                j--;

            /* Se l'indirizzo email termina con il carattere '.' viene restituito il codice di errore -4. */
            if (strLocal[j + 1] == '\0') {
                strcpy(res.message, "Errore, l'indirizzo email non può terminare con un punto.");
                res.code -4;
                return res;
            }

            /* Se il carattere '.' si trova prima del carattere '@' viene restituito il codice di errore -5. */
            if (i > j) {
                strcpy(res.message, "Errore, il formato dell'email non è valido.");
                res.code = -5;
                return res;
            }

            /* Controlla che il nome utente non inizi e non finisca con un carattere diverso da una lettera o una
             * cifra. Se lo fa restituisce il codice di errore -6. */
            if (((strLocal[0] < 'a' || strLocal[0] > 'z') && (strLocal[0] < '0' || strLocal[0] > '9')) || (strLocal[i - 1] < 'a' || strLocal[i - 1] > 'z') && (strLocal[i - 1] < '0' || strLocal[i - 1] > '9')) {
                strcpy(res.message, "Errore, il nome utente deve iniziare e terminare con una lettera o un numero.");
                res.code = -6;
                return res;
            }

            /* Controlla che il dominio di secondo livello non inizi e non finisca con un carattere diverso da una
             * lettera o una cifra. Se lo fa restituisce il codice di errore -7. */
            if (((strLocal[i + 1] < 'a' || strLocal[i + 1] > 'z') && (strLocal[i + 1] < '0' || strLocal[i + 1] > '9')) || (strLocal[j - 1] < 'a' || strLocal[j - 1] > 'z') && (strLocal[j - 1] < '0' || strLocal[j - 1] > '9')) {
                strcpy(res.message, "Errore, il dominio deve iniziare e terminare con una lettera o un numero.");
                res.code = -7;
                return res;
            }

            /* Controlla che non siano presenti caratteri invalidi nel nome utente. Se presenti restituisce il
             * codice di errore -8. */
            for(k = 0; k < i; k++)
                if ((strLocal[k] < 'a' || strLocal[k] > 'z') && (strLocal[k] < '0' || strLocal[k] > '9') && strLocal[k] != '-' && strLocal[k] != '.' && strLocal[k] != '_') {
                    strcpy(res.message, "Errore, sono presenti caratteri non validi nel nome utente.");
                    res.code = -8;
                    return res;
                }

            /* Controlla che non siano presenti caratteri invalidi nel dominio di secondo livello. Se presenti
             * restituisce il codice di errore -9. */
            for(k = i + 1; k < j; k++)
                if ((strLocal[k] < 'a' || strLocal[k] > 'z') && (strLocal[k] < '0' || strLocal[k] > '9') && strLocal[k] != '-' && strLocal[k] != '.' && strLocal[k] != '_') {
                    strcpy(res.message, "Errore, sono presenti caratteri non validi nel dominio.");
                    res.code = -9;
                    return res;
                }

            /* Controlla che non siano presenti caratteri invalidi nel dominio di primo livello. Se presenti
             * restituisce il codice di errore -10. */
            for(k = j + 1; k < strlen(strLocal); k++)
                if (strLocal[k] < 'a' || strLocal[k] > 'z') {
                    strcpy(res.message, "Errore, sono presenti caratteri non validi nel dominio.");
                    res.code = -10;
                    return res;
                }
            break;
    }

    /* Se all'interno della stringa non è presente alcun errore, viene restituito il valore 0 */
    strcpy(res.message, "OK");
    res.code = 0;
    return res;
}

/**
 * Converte una stringa in lettere minuscole.
 *
 * @param str Stringa da convertire.
 */
void toLowercase(char str[]) {
    int i = 0;

    while(str[i] != '\0') {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 32;
        i++;
    }
}

/**
 * Prende in ingresso un input e esegue un controllo su di esso.
 *
 * @param inStr Stringa di input.
 * @param length Lunghezza della stringa di input.
 * @param ctrl Tipo di controllo da eseguire.
 * @param label Label da visualizzare nella printf.
 */
void getInput(char *inStr, int length, Controllo ctrl, char label[]) {
    Response check;

    do {
        printf("\n%s: ", label);
        readLine(inStr, length);
        check = validator(inStr, ctrl);

        if (check.code != 0) {
            printf("%s", check.message);
            ignoreInputUntil('\n');
        }
    } while(check.code != 0);
}

/**
 * Acquisisce l'input relativo al gruppo di contatti.
 *
 * @return Gruppo di contatti.
 */
TipologiaContatto getGroup() {
    TipologiaContatto gruppo;
    _Bool isOk;

    do {
        printf(GROUP_LABEL);
        //choice = getchar();
        scanf("%d", &gruppo);
        ignoreInputUntil('\n');
        gruppo--;

        isOk = checkRange(gruppo, LAVORO, ALTRO, "Errore, scelta non valida.");
    } while(!isOk);
    return gruppo;
}

/**
 * Controlla se un valore è compreso all'interno di un certo intervallo. Se lo è restituisce true, altrimenti
 * restituisce false e stampa un messaggio di errore.
 *
 * @param toCheck Valore da controllare.
 * @param min Minimo valore dell'intervallo.
 * @param max Massimo valore dell'intervallo.
 * @param msg Messaggio da stampare nel caso in cui il valore non sia compreso nell'intervallo.
 * @return false se non è compreso, true se è compreso.
 */
_Bool checkRange(int toCheck, int min, int max, char msg[MESSAGE]) {
    if (toCheck < min || toCheck > max) {
        printf("%s", msg);
        ignoreInputUntil('\n');

        return false;
    }

    return true;
}

/**
 * Prende in ingresso un contatto e lo stampa a video.
 *
 * @param contact Puntatore al contatto da stampare a video.
 */
void printContact(Contatto *cont) {
    printf("Nome: %s\n"
           "Cognome: %s\n"
           "Telefono: %s\n"
           "E-mail: %s\n"
           "Gruppo: ",
           cont->nome, cont->cognome, cont->telefono,
           cont->email);

    switch (cont->gruppo) {
        case LAVORO:
            printf("Lavoro\n");
            break;
        case FAMIGLIA:
            printf("Famiglia\n");
            break;
        case AMICI:
            printf("Amici\n");
            break;
        case ALTRO:
            printf("Altro\n");
            break;
    }
}

/**
 * Prende in ingresso un contatto e ne modifica un campo dopo aver chiesto all'utente quale campo vuole modificare.
 *
 * @param cont Puntatore al contatto da modificare.
 */
void modifyContact(Contatto *cont) {
    int choice;
    _Bool isOk;

    do {
        printContact(cont);

        printf("Quale campo vuoi modificare?\n"
               "1) Nome\n"
               "2) Cognome\n"
               "3) Telefono\n"
               "4) E-mail\n"
               "5) Gruppo\n");
        scanf("%d", &choice);
        ignoreInputUntil('\n');

        isOk = checkRange(choice, MODIF_MIN, MODIF_MAX, "Errore: scelta non valida.");
    } while(!isOk);

    switch (choice) {
        case 1:
            getInput(cont->nome, LENGTH + 1, NOME, NAME_LABEL);
            break;
        case 2:
            getInput(cont->cognome, LENGTH + 1, NOME, SRNAME_LABEL);
            break;
        case 3:
            getInput(cont->telefono, LENGTH + 1, TELEFONO, TEL_LABEL);
            break;
        case 4:
            getInput(cont->email, LENGTH + 1, EMAIL, EMAIL_LABEL);
            break;
        case 5:
            cont->gruppo = getGroup();
            break;
    }
}