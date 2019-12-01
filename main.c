#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Haz Haz;
typedef struct Varos Varos;
typedef struct Megye Megye;

static char ** split(char *);
static void print_hazak(Haz *);
static void print_varosok(Varos *);
static void print_megyek(Megye *);
static Megye * haz_olvas();
static Megye * varosok_olvas();
static Megye * uj_haz(Megye *);
static Megye * uj_varos(Megye *, char *);
static Megye * uj_megye(Megye *, char *);

struct Haz {
    Haz *next;
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

char **
split(char *str)
{
    char **szavak;
    char *tmp = str;
    int count = 1;
    int szo = 0;
    int karakter = 0;

    //szavak megszamolasa
    while (*tmp != '\0') {
        if (*tmp == ' ') {
            count++;
            while (*tmp++ == ' ') {}
        }
        tmp++;
    }

    //memóriafoglalás a szavakhoz (max 64 char/szo)
    //az utolsó 'szó' NULL hogy tudjuk hogy itt a szavak vége
    szavak = malloc((count + 1) * sizeof(char *));
    for (int i = 0; i < count; i++) {
        szavak[i] = malloc(64 * sizeof(char));
    }
    szavak[count] = NULL;

    //kezdo szoközök átugrása
    if (*str == ' ') {
        while (*str++ == ' ') {}
    }

    //karakterek szavakba rakása
    while (szo < count &&  str && *str != '\0') {
        while (*str != '\0' && *str != ' ') {
            if (*str != '\n') {
                szavak[szo][karakter] = *str;
                karakter++;
            }
            str++;
        }
        
        //záró karakter
        szavak[szo][karakter] = '\0';

        //szóközök átugrása
        if (*str == ' ') {
            szo++;
            karakter = 0;
            while (*str == ' ') {
                str++;
            }
        }
    }

    return szavak;
}

void
print_hazak(Haz *head)
{
    Haz *haz = head;
    while ((haz = haz->next)) {
        // TODO: what to print
        printf("haz\n");
    }
}

void
print_varosok(Varos *head)
{
    Varos *varos = head;
    while (varos) {
        printf("%s\n", varos->nev);
		varos = varos->next;
    }
}

void
print_megyek(Megye *head)
{
    Megye *megye = head;
    while (megye) {
        printf("%s\n", megye->nev);
        megye = megye->next;
    }
}

Megye *
varosok_olvas()
{
    FILE *fajl;
    Megye *megye = NULL;
	Megye *temp;
    char *line;
    char **words;

    if (!(fajl = fopen("varosok.txt", "r"))) {
        printf("varosok.txt nem létezik\n");
        exit(1);
    }

    line = malloc(64);
    while (fgets(line, 64, fajl)) {

        if (!(words = split(line))) {
            printf("string feldarabolása sikertelen\n");
            exit(1);
        }

		//létezik e már a megye
		temp = megye;
		int megye_letezik = 0;
		while (temp) {
			if (!strcmp(temp->nev, words[1])) {
				megye_letezik = 1;
				temp = uj_varos(temp, words[0]);
			}
			temp = temp->next;
		}

		//ha nem, hozzuk létre
		if (!megye_letezik) {
			megye = uj_megye(megye, words[1]);
			megye = uj_varos(megye, words[0]);
		}
    }

    //print_megyek(megye);
	//printf("%s varosai:\n", (megye->next)->nev);
	//print_varosok((megye->next)->varos);

    free(line);
    fclose(fajl);
    return megye;
}

Megye *
uj_varos(Megye *megye, char *nev)
{
    Varos *uj = malloc(sizeof(Varos));

    if (!uj) {
        printf("Varos memóriafoglalása nem sikerült\n");
        exit(1);
    } else {
        uj->next       = megye->varos;
		megye->varos   = uj;
        uj->haz        = NULL;
        uj->nev        = malloc(strlen(nev)+1);
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

int
main()
{
    Megye * elso_megye = varosok_olvas();
    return 0;
}
