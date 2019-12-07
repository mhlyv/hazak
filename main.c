#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Config Config;
typedef struct Haz Haz;
typedef struct Varos Varos;
typedef struct Megye Megye;

//TODO: fix bullshit errors in visualC++

static int szoszam(char *);
static Config * config_valtoztat(Config *);
static Haz * haz(char *[6]);
//TODO: clean up
static int validhaz(Haz *, Megye *, Config *);
static int varos_letezik(Megye *, char *);
static int megyeben_haz(Megye *);
static Megye * hazak_olvas(Megye *, Config *);
static Megye * varosok_olvas();
static Varos * uj_haz(Varos *, Haz *);
static Megye * uj_varos(Megye *, char *);
static Megye * uj_megye(Megye *, char *);
//TODO: implement
static int atlag(Megye *);
//TODO: implement
static void rendez(Megye *);
static void print_config(Config *);
static void print_hazak(Haz *);
static void print_varosok(Varos *);
static void print_megyek(Megye *);
//TODO: clean up
static void print_talalatok(Megye *, Config *);

struct Config {
    char * megye;
    char * varos;
    //config [min, max]
    int meret[2];
    int ar[2];
    int szobak[2];
    //opcionalis
    //   0 -> neutrális
    //   1 -> kell
    //  -1 -> nem kell
    int extraszoba;
    int garazs;
    int sufni;
};

struct Haz {
    Haz *next;
    char *varos;
    int meret;
    int ar;
    int szobak;
    int extraszoba;
    int garazs;
    int sufni;
};

struct Varos {
    Varos *next;
    char *nev;
    Haz *haz;
};

struct Megye {
    Megye *next;
    char *nev;
    Varos *varos;
};

enum {
    udvozles,
    config,
    conf,
    megye,
    varos,
    minmax,
    opcionalis,
};

static const char *uzenetek[] = {
    [udvozles] = "Ez egy ingatlankereső program\n",
    [conf] = "Szűrő opciók:\n\
\tmegye\n\
\tvaros\n\
\tmeret\n\
\tar\n\
\tszobak\n\
\textraszoba\n\
\tgarazs\n\
\tsufni\n\n\
\tkesz -> konfiguráció befejezése\n\
\tujra -> konfiguráció újrakezdése\n",
    [megye] = "Adjon meg egy megyét\n",
    [varos] = "Adjon meg egy várost\n",
    [minmax] = "Szóközzel elválasztva adjon meg egy minimum és egy maximum értéket\n\
A 0 érték nem lesz értelmezve\n",
    [opcionalis] = "Opciók:\n\
\t 1 -> kell\n\
\t 0 -> nem számít (alapértelmezett)\n\
\t-1 -> nem kell\n",
};

int
szoszam(char *line)
{
    char buff[strlen(line)];
    int count = 0;
    char *tok = "\n ";
    char *words;

    strcpy(buff, line);

    if ((words = strtok(buff, tok))) {
        count++;
    } else {
        return 0;
    }

    while (strtok(NULL, tok)) {
        count++;
    }

    return count;
}

Config *
config_valtoztat(Config *config)
{
    int done = 0;
    char input[64];
if (!config) {
        Config * uj = malloc(sizeof(Config));
        config = uj;
        config->megye = NULL;
        config->varos = NULL;
        config->meret[0] = 0;
        config->meret[1] = 0;
        config->ar[0] = 0;
        config->ar[1] = 0;
        config->szobak[0] = 0;
        config->szobak[1] = 0;
        config->extraszoba = 0;
        config->garazs = 0;
        config->sufni = 0;
    }

    printf(uzenetek[conf]);
    while (!done) {
        printf("> ");
        if (!scanf("%s", input)) {
            printf("Nem sikerült olvasni!\n");
        }

        if (!strcmp(input, "megye")) {
            printf(uzenetek[megye]);
            config->megye = malloc(64);
            if (!scanf("%62s", config->megye)) {
                printf("Nem sikerült olvasni!\n");
                free(config->megye);
            }
            print_config(config);
        } else if (!strcmp(input, "varos")) {
            printf(uzenetek[varos]);
            config->varos = malloc(64);
            if (!scanf("%62s", config->varos)) {
                printf("Nem sikerült olvasni!\n");
                free(config->varos);
            }
            print_config(config);
        } else if (!strcmp(input, "meret")) {
            printf(uzenetek[minmax]);
            if (!scanf("%d %d", &config->meret[0], &config->meret[1])) {
                printf("Nem sikerült olvasni!\n");
            }
            print_config(config);
        } else if (!strcmp(input, "ar")) {
            printf(uzenetek[minmax]);
            if (!scanf("%d %d", &config->ar[0], &config->ar[1])) {
                printf("Nem sikerült olvasni!\n");
            }
            print_config(config);
        } else if (!strcmp(input, "szobak")) {
            printf(uzenetek[minmax]);
            if (!scanf("%d %d", &config->szobak[0], &config->szobak[1])) {
                printf("Nem sikerült olvasni!\n");
            }
            print_config(config);
        } else if (!strcmp(input, "extraszoba")) {
            printf(uzenetek[opcionalis]);
            if (!scanf("%d", &config->extraszoba)) {
                printf("Nem sikerült olvasni!\n");
            }
            print_config(config);
        } else if (!strcmp(input, "garazs")) {
            printf(uzenetek[opcionalis]);
            if (!scanf("%d", &config->garazs)) {
                printf("Nem sikerült olvasni!\n");
            }
            print_config(config);
        } else if (!strcmp(input, "sufni")) {
            printf(uzenetek[opcionalis]);
            if (!scanf("%d", &config->sufni)) {
                printf("Nem sikerült olvasni!\n");
            }
            print_config(config);
        } else if (!strcmp(input, "vissza")) {
        } else if (!strcmp(input, "kesz")) {
            done = 1;
            print_config(config);
        } else if (!strcmp(input, "ujra")) {
            free(config);
            Config * uj = malloc(sizeof(Config));
            config = uj;
            config->megye = NULL;
            config->varos = NULL;
            config->meret[0] = 0;
            config->meret[1] = 0;
            config->ar[0] = 0;
            config->ar[1] = 0;
            config->szobak[0] = 0;
            config->szobak[1] = 0;
            config->extraszoba = 0;
            config->garazs = 0;
            config->sufni = 0;
            print_config(config);
        } else {
            printf(uzenetek[conf]);
        }
    }

    return config;
}

Haz *
haz(char *words[7])
{
    Haz *haz = malloc(sizeof(Haz));

    if (!haz) {
        printf("Haz memórialefoglalása nem sikerült");
        exit(1);
    }

    haz->next       = NULL;
    haz->varos      = words[0];
    haz->meret      = atoi(words[1]);
    haz->ar         = atoi(words[2]);
    haz->szobak     = atoi(words[3]);

    haz->extraszoba = atoi(words[4]);
    if (haz->extraszoba < 0) haz->extraszoba = -1;
    else if (haz->extraszoba > 0) haz->extraszoba = 1;
    else haz->extraszoba = 0;

    haz->garazs     = atoi(words[5]);
    if (haz->garazs < 0) haz->garazs = -1;
    else if (haz->garazs > 0) haz->garazs = 1;
    else haz->garazs = 0;

    haz->sufni      = atoi(words[6]);
    if (haz->sufni < 0) haz->sufni = -1;
    else if (haz->sufni > 0) haz->sufni = 1;
    else haz->sufni = 0;

    return haz;
}

int
validhaz(Haz *haz, Megye *head, Config *config)
{
    Megye *megye;
    int valid = 1;

    if ((config->meret[0] && haz->meret < config->meret[0]) || 
        (config->meret[1] && haz->meret > config->meret[1])) {
        valid = 0;
    }
    if ((config->ar[0] && haz->ar < config->ar[0]) || 
        (config->ar[1] && haz->ar > config->ar[1])) {
        valid = 0;
    }
    if ((config->szobak[0] && haz->szobak < config->szobak[0]) || 
        (config->szobak[1] && haz->szobak > config->szobak[1])) {
        valid = 0;
    }
    if (config->extraszoba) {
        if (haz->extraszoba != config->extraszoba)  {
            valid = 0;
        }
    }
    if (config->garazs) {
        if (haz->garazs != config->garazs)  {
            valid = 0;
        }
    }
    if (config->sufni) {
        if (haz->sufni != config->sufni)  {
            valid = 0;
        }
    }
    if (config->varos && strcmp(haz->varos, config->varos)) {
        valid = 0;
    }
    if (valid && config->megye) {
        megye = head;
        while (megye && strcmp(megye->nev, config->megye)) {
            megye = megye->next;
        }

        valid = varos_letezik(megye, haz->varos);
    }

    return valid;
}

int
varos_letezik(Megye *megye, char *nev)
{
    if (!megye) return 0;
    Varos *varos = megye->varos;
    while (varos) {
        if (!strcmp(varos->nev, nev)) {
            return 1;
        }
        varos = varos->next;
    }
    return 0;
}

int
megyeben_haz(Megye *megye)
{
    if (!megye) return 0;
    Varos *varos = megye->varos;

    while (varos) {
        if (varos->haz) {
            return 1;
        }
        varos = varos->next;
    }

    return 0;
}

Megye *
hazak_olvas(Megye *megye, Config *config)
{
    FILE *fajl;
    Megye *temp;
    Varos *varos;
    Haz *uj;
    char *line;
    char *words[7];
    int found;

    if (!(fajl = fopen("hazak.txt", "r"))) {
        printf("hazak.txt nem létezik\n");
        exit(1);
    }

    line = malloc(128);
    while (fgets(line, 128, fajl)) {

        if (szoszam(line) != 7) {
            printf("Hiba a hazak.txt fajlban, egy sor nem 7 szóból áll!\n");
            exit(1);
        }

        words[0] = strtok(line, "\n ");
        words[1] = strtok(NULL, "\n ");
        words[2] = strtok(NULL, "\n ");
        words[3] = strtok(NULL, "\n ");
        words[4] = strtok(NULL, "\n ");
        words[5] = strtok(NULL, "\n ");
        words[6] = strtok(NULL, "\n ");
        while (strtok(NULL, "\n ")) {}

        uj = haz(words);

        temp = megye;
        if (!validhaz(uj, temp, config)) {
            continue;
        }

        //ugorjuk at a megyét ahol nincs város
        while (temp && !temp->varos) {
            temp = temp->next;
        }

        //álj ha a megyek vegenel vagyunk
        if (!temp) {
            continue;
        }

        varos = temp->varos;
        found = 0;
        while (!found) {
            if (!strcmp(words[0], varos->nev)) {
                found = 1;
                varos = uj_haz(varos, uj);
            }

            if (!varos || !(varos = varos->next)) {
                if (!temp || !(temp = temp->next)) {
                    break;
                }

                varos = temp->varos;
            }
        }
    }

    free(line);
    fclose(fajl);
    return megye;
}

Megye *
varosok_olvas()
{
    FILE *fajl;
    Megye *megye = NULL;
    Megye *temp;
    char *line;
    char *words[2];

    if (!(fajl = fopen("varosok.txt", "r"))) {
        printf("varosok.txt nem létezik\n");
        exit(1);
    }

    line = malloc(64);
    while (fgets(line, 64, fajl)) {

        if (szoszam(line) != 2) {
            printf("Hiba a varosok.txt fajlban, egy sor nem 2 szóból áll!\n");
            exit(1);
        }
        
        words[0] = strtok(line, "\n ");
        words[1] = strtok(NULL, "\n ");
        //printf("%s\n%s\n", words[0], words[1]);
        while (strtok(NULL, " ")) {}

        //létezik e már a megye
        temp = megye;
        int megye_letezik = 0;
        while (temp) {
            if (!strcmp(temp->nev, words[1])) {
                megye_letezik = 1;
                //ha a város már létezik, ne hozzuk létre még1szer
                if (!varos_letezik(temp, words[0])) {
                    temp = uj_varos(temp, words[0]);
                }
            }
            temp = temp->next;
        }

        //ha nem, hozzuk létre
        if (!megye_letezik) {
            megye = uj_megye(megye, words[1]);
            megye = uj_varos(megye, words[0]);
        }
    }

    free(line);
    fclose(fajl);
    return megye;
}

Varos *
uj_haz(Varos *varos, Haz *uj)
{
    uj->next = varos->haz;
    varos->haz = uj;
    return varos;
}

Megye *
uj_varos(Megye *megye, char *nev)
{
    Varos *uj = malloc(sizeof(Varos));

    if (!uj) {
        printf("Varos memóriafoglalása nem sikerült\n");
        exit(1);
    }

    uj->next       = megye->varos;
    megye->varos   = uj;
    uj->haz        = NULL;
    uj->nev        = malloc(strlen(nev)+1);
    if (nev) {
        strcpy(uj->nev, nev);
    }

    return megye;
}

Megye *
uj_megye(Megye *megye, char *nev)
{
    Megye *uj = malloc(sizeof(Megye));

    if (!uj) {
        printf("Megye memóriafoglalása nem sikerült\n");
        exit(1);
    }

    uj->next       = megye;
    uj->varos      = NULL;
    uj->nev        = malloc(strlen(nev)+1);
    strcpy(uj->nev, nev);

    return uj;
}

void
print_config(Config *config)
{
    printf("Konfiguráció:\n");
    printf("\tmegye: '%s'\n", config->megye);
    printf("\tvaros: '%s'\n", config->varos);
    printf("\t%d < meret < %d\n", config->meret[0], config->meret[1]);
    printf("\t%d < ár < %d\n", config->ar[0], config->ar[1]);
    printf("\t%d < szobak < %d\n", config->szobak[0], config->szobak[1]);
    printf("\textra szoba: %d\n", config->extraszoba);
    printf("\tgarazs: %d\n", config->garazs);
    printf("\tsufni: %d\n", config->sufni);
}

void
print_hazak(Haz *head)
{
    Haz *haz = head;
    while (haz) {
        printf("Ház ár: %d méret: %d, szobák: %d\n", haz->ar, haz->meret, haz->szobak);
        haz = haz->next;
    }
}

void
print_varosok(Varos *head)
{
    Varos *varos = head;
    while (varos) {
        //csak akkor írja ki ha van ott ház
        if (varos->haz) {
            printf("%s\n", varos->nev);
        }
        varos = varos->next;
    }
}

void
print_megyek(Megye *head)
{
    Megye *megye = head;
    while (megye) {
        //csak akkor írja ki ha van ott ház
        if (megyeben_haz(megye)) {
            printf("%s\n", megye->nev);
        }
        megye = megye->next;
    }
}

void
print_talalatok(Megye *head, Config *config)
{
    Megye *megye = head;
    Varos *varos;
    char input[64];

    if (config->megye) {

        while (megye && strcmp(megye->nev, config->megye)) {
            megye = megye->next;
        }
        
        if (!megyeben_haz(megye)) {
            printf("Ebben a megyében nincs ház!\n");
            return;
        }

        if (config->varos) {
            varos = megye->varos;
            while (varos && strcmp(varos->nev, config->varos)) {
                varos = varos->next;
            }

            if (!varos) {
                printf("Ebben a városban nincs ház!\n");
                return;
            }

            print_hazak(varos->haz);
            return;
        }

    } else if (!config->varos) {

        print_megyek(megye);
        printf("\nAdjon meg egy megyét\n");
        if (!scanf("%s", input)) {
            printf("Nem sikerült beolvasni\n");
        }

        megye = head;
        while (megye && strcmp(megye->nev, input)) {
            megye = megye->next;
        }

        if (!megyeben_haz(megye)) {
            printf("Ebben a megyében nincs ház!\n");
            return;
        }
    }

    if (config->varos) {
        megye = head;
        while (megye) {
            varos = megye->varos;
            while (varos) {
                if (!strcmp(varos->nev, config->varos)) {
                    print_hazak(varos->haz);
                }
                varos = varos->next;
            }
            megye = megye->next;
        }
    } else {
        varos = megye->varos;
        print_varosok(varos);

        printf("\nAdjon meg egy varost\n");
        if (!scanf("%s", input)) {
            printf("Nem sikerült beolvasni\n");
        }

        varos = megye->varos;
        while (varos && strcmp(varos->nev, input)) {
            varos = varos->next;
        }

        if (!varos) {
            printf("Ebben a városban nincs ház!\n");
            return;
        }

        print_hazak(varos->haz);
    }
}

int
main()
{
    printf(uzenetek[udvozles]);
    Megye * elso_megye = varosok_olvas();
    Config * config = config_valtoztat(NULL);
    elso_megye = hazak_olvas(elso_megye, config);
    print_talalatok(elso_megye, config);
    return 0;
}
