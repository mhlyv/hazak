#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Config Config;
typedef struct Haz Haz;
typedef struct Varos Varos;
typedef struct Megye Megye;

static char ** split(char *);
static Config * config_valtoztat(Config *);
static Haz * haz(char **);
static int validhaz(Haz *, Config *);
static Megye * hazak_olvas(Megye *, Config *);
static Megye * varosok_olvas();
static Varos * uj_haz(Varos *, Haz *);
static Megye * uj_varos(Megye *, char *);
static Megye * uj_megye(Megye *, char *);
static void print_config(Config *);
static void print_hazak(Haz *);
static void print_varosok(Varos *);
static void print_megyek(Megye *);

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
            while (*++tmp == ' ') {}
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

Config *
config_valtoztat(Config *config)
{
	int done = 0;
	char input[64];

	if (!config) {
		config = malloc(sizeof(Config));
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
		scanf("%s", input);

		if (!strcmp(input, "megye")) {
			printf(uzenetek[megye]);
			scanf("%s", config->megye);
			print_config(config);
		} else if (!strcmp(input, "varos")) {
			printf(uzenetek[varos]);
			scanf("%s", config->varos);
			print_config(config);
		} else if (!strcmp(input, "meret")) {
			printf(uzenetek[minmax]);
			scanf("%d %d", &config->meret[0], &config->meret[1]);
			print_config(config);
		} else if (!strcmp(input, "ar")) {
			printf(uzenetek[minmax]);
			scanf("%d %d", &config->ar[0], &config->ar[1]);
			print_config(config);
		} else if (!strcmp(input, "szobak")) {
			printf(uzenetek[minmax]);
			scanf("%d %d", &config->szobak[0], &config->szobak[1]);
			print_config(config);
		} else if (!strcmp(input, "extraszoba")) {
			printf(uzenetek[opcionalis]);
			scanf("%d", &config->extraszoba);
			print_config(config);
		} else if (!strcmp(input, "garazs")) {
			printf(uzenetek[opcionalis]);
			scanf("%d", &config->garazs);
			print_config(config);
		} else if (!strcmp(input, "sufni")) {
			printf(uzenetek[opcionalis]);
			scanf("%d", &config->sufni);
			print_config(config);
		} else if (!strcmp(input, "vissza")) {
		} else if (!strcmp(input, "kesz")) {
			done = 1;
			print_config(config);
		} else if (!strcmp(input, "ujra")) {
			free(config);
			config = malloc(sizeof(Config));
			config->megye = NULL;
			config->varos = NULL;
			print_config(config);
		} else {
			printf(uzenetek[conf]);
		}
	}

	return config;
}

Haz *
haz(char **words)
{
	Haz *haz = malloc(sizeof(Haz));

	if (!haz) {
		printf("Haz memórialefoglalása nem sikerült");
		exit(1);
	}

	haz->next       = NULL;
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
validhaz(Haz *haz, Config *config)
{
	if ((config->meret[0] && haz->meret < config->meret[0]) || 
		(config->meret[1] && haz->meret > config->meret[1])) {
		return 0;
	}
	if ((config->ar[0] && haz->ar < config->ar[0]) || 
		(config->ar[1] && haz->ar > config->ar[1])) {
		return 0;
	}
	if ((config->szobak[0] && haz->szobak < config->szobak[0]) || 
		(config->szobak[1] && haz->szobak > config->szobak[1])) {
		return 0;
	}
	if (!config->extraszoba) {
		if (haz->extraszoba != config->extraszoba) return 0;
	}
	if (!config->garazs) {
		if (haz->garazs != config->garazs) return 0;
	}
	if (!config->sufni) {
		if (haz->sufni != config->sufni) return 0;
	}
	return 1;
}

Megye *
hazak_olvas(Megye *megye, Config *config)
{
	FILE *fajl;
    Megye *temp;
	Varos *varos;
	Haz *uj;
    char *line;
    char **words;
	int found;

    if (!(fajl = fopen("hazak.txt", "r"))) {
        printf("hazak.txt nem létezik\n");
        exit(1);
    }

    line = malloc(128);
    while (fgets(line, 128, fajl)) {

        if (!(words = split(line))) {
            printf("string feldarabolása sikertelen\n");
            exit(1);
        }

		uj = haz(words);
		if (!validhaz(uj, config)) {
			continue;
		}

		temp = megye;
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

	while (*words++) {
		free(*words);
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

	while (*words++) {
		free(*words);
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
	strcpy(uj->nev, nev);

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
        printf("Ház ár: %d\n", haz->ar);
		haz = haz->next;
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

int
main()
{
	printf(uzenetek[udvozles]);
    Megye * elso_megye = varosok_olvas();
	printf(elso_megye->nev);
	print_varosok(elso_megye->varos);
	Config * config = config_valtoztat(NULL);
	elso_megye = hazak_olvas(elso_megye, config);
    return 0;
}
