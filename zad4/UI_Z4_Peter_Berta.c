#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define LINE_LIM 100
#define NAME_LIM  10

struct Lines {
	char *text;
	struct Lines *next;
};

struct variable {
	char *name, *value;
	struct variable *next;
};

struct rule {
	char *name;
	char *condition;
	char *result;
	struct rule *next;
};

// global variables
struct Lines *vysledky;

// FUNCTIONS
struct Lines *init_file(char *name) {

	FILE *fakty = fopen(name, "r");
	struct Lines *fact = NULL, *other = NULL;
	char *line = malloc(LINE_LIM);
	char znak;
	int i, position = 0;

	while (1) {
		znak = fgetc(fakty);
		if (znak == EOF) {
			line[position] = '\0';
			position = 0;

			other->next = (struct Lines*)malloc(sizeof(struct Lines));

			other->next->text = (char*)malloc(LINE_LIM);
			for (i = 0;i < strlen(line);i++) {
				other->next->text[i] = line[i];
			}
			other->next->text[strlen(line)] = '\0';
			other->next->next = NULL;
			other = other->next;

			break;
		}

		if (znak == '\n') {
			line[position] = '\0';
			position = 0;

			if (fact == NULL) {

				fact = (struct Lines*)malloc(sizeof(struct Lines));

				fact->text = (char*)malloc(LINE_LIM);
				for (i = 0;i < strlen(line);i++) {
					fact->text[i] = line[i];
				}
				fact->text[strlen(line)] = '\0';
				fact->next = NULL;

				other = fact;
			}
			else {
				other->next = (struct Lines*)malloc(sizeof(struct Lines));

				other->next->text = (char*)malloc(LINE_LIM);
				for (i = 0;i < strlen(line);i++) {
					other->next->text[i] = line[i];
				}
				other->next->text[strlen(line)] = '\0';
				other->next->next = NULL;
				other = other->next;
			}
		}
		else {
			line[position++] = znak;
		}
	}

	fclose(fakty);

	return fact;
}

struct rule *init_rules(char *name) {

	FILE *pravidla = fopen(name, "r");
	struct rule *rule = NULL, *other = NULL;
	char * line = malloc(LINE_LIM);
	char znak;
	int i, position = 0;

	while (1) {
		znak = fgetc(pravidla);
		if (znak == EOF) break;
		if (znak == '\n') znak = fgetc(pravidla);
			

		if (other == NULL) {
		
			rule = (struct rule*)malloc(sizeof(struct rule));
			rule->name = (char*)malloc(LINE_LIM);
			rule->condition = (char*)malloc(LINE_LIM);
			rule->result = (char*)malloc(LINE_LIM);
			rule->next = NULL;

			// CITANIE MENA
			i = 0;
			while (znak != '\n') {
				rule->name[i++] = znak;
				znak = fgetc(pravidla);
			}
			rule->name[i] = '\0';
			znak = fgetc(pravidla);

			// CITANIE PODMIENOK
			i = 0;
			while (znak != '\n') {
				rule->condition[i++] = znak;
				znak = fgetc(pravidla);
			}
			rule->condition[i] = '\0';
			znak = fgetc(pravidla);

			// CITANIE DOSLEDKU
			i = 0;
			while (znak != '\n' && znak != EOF) {
				rule->result[i++] = znak;
				znak = fgetc(pravidla);
			}
			rule->result[i] = '\0';

			other = rule;
		}
		else {
			other->next = (struct rule*)malloc(sizeof(struct rule));
			other->next->name = (char*)malloc(LINE_LIM);
			other->next->condition = (char*)malloc(LINE_LIM);
			other->next->result = (char*)malloc(LINE_LIM);
			other->next->next = NULL;

			// CITANIE MENA
			i = 0;
			while (znak != '\n') {
				other->next->name[i++] = znak;
				znak = fgetc(pravidla);
			}
			other->next->name[i] = '\0';
			znak = fgetc(pravidla);

			// CITANIE PODMIENOK
			i = 0;
			while (znak != '\n') {
				other->next->condition[i++] = znak;
				znak = fgetc(pravidla);
			}
			other->next->condition[i] = '\0';
			znak = fgetc(pravidla);

			// CITANIE DOSLEDKU
			i = 0;
			while (znak != '\n' && znak != EOF) {
				other->next->result[i++] = znak;
				znak = fgetc(pravidla);
			}
			other->next->result[i] = '\0';

			other = other->next;
		}
	}

	fclose(pravidla);

	return rule;
}

// dosadim do 'source' namiesto 'first' hodnotu 'second' ...OK
// return: 'source'
char *replace(char *source, char *first, char *second) {

	int i = 0, j;
	int a = strlen(source);

	while (1) {

		while ((source[i] != first[0] || source[i + 1] != first[1]) && i < a) i++;

		if (i == a) return source;

		j = strlen(source) + strlen(second);

		while (j > i + 1) {
			source[j - 2] = source[j - strlen(second)];
			j--;
		}

		for (j = 0;j < strlen(second);j++) 
			source[i + j] = second[j];
	}
	return source;
}

// prida premennu do zoznamu var, ak tam este nie je
// returns: 1 ak bolo pridanie uspesne
// returns: 0 ak taka premenna uz existuje ale s inou hodnotou
int pridaj_var(struct variable **var, char *name, char *value) {

	struct variable *last = *var, *tempVar;

	if (last == NULL) {
		(*var) = (struct variable*)malloc(sizeof(struct variable));
		(*var)->name = (char*)malloc(3);
		(*var)->name = strcpy((*var)->name, name);
		(*var)->value = (char*)malloc(NAME_LIM);
		(*var)->value = strcpy((*var)->value, value);
		(*var)->next = NULL;
		last = *var;

	}
	else {
		// uz mam nejake premenne
		// potrebujem zistit, ci uz nahodou nemam taku, aku chcem pridat
		tempVar = *var;

		while (tempVar != NULL) {
			
			if (strcmp(tempVar->name, name) == 0) {
				
				if (strcmp(tempVar->value, value) == 0) {
					// name sa rovnaju a value sa rovnaju
					return 1;
				}
				else {
					// name sa rovnaju, ale value sa nerovnaju
					return 0;
				}
			}

			tempVar = tempVar->next;
		}

		while (last->next != NULL) {
			last = last->next;
		}

		last->next = (struct variable*)malloc(sizeof(struct variable));
		last->next->name = (char*)malloc(3);
		last->next->name = strcpy(last->next->name, name);
		last->next->value = (char*)malloc(NAME_LIM);
		last->next->value = strcpy(last->next->value, value);
		last->next->next = NULL;

		last = last->next;
	}

	return 1;
}

struct variable *copy_var(struct variable *premenne) {

	struct variable *var = NULL, *temp2, *temp;

	var = premenne;

	if (premenne != NULL) {
		var = (struct variable *)malloc(sizeof(struct variable));
		var->name = (char*)malloc(NAME_LIM);
		var->name = strcpy(var->name, premenne->name);
		var->value = (char*)malloc(NAME_LIM);
		var->value = strcpy(var->value, premenne->value);
		var->next = NULL;
		temp2 = premenne->next;
		temp = var;

		while (temp2 != NULL) {
			temp->next = (struct variable *)malloc(sizeof(struct variable));
			temp->next->name = (char*)malloc(NAME_LIM);
			temp->next->name = strcpy(temp->next->name, temp2->name);
			temp->next->value = (char*)malloc(NAME_LIM);
			temp->next->value = strcpy(temp->next->value, temp2->value);
			temp->next->next = NULL;

			temp2 = temp2->next;
			temp = temp->next;
		}
	}

	return var;
}

char *get_value(struct variable *premenne, char *name) {

	struct variable *temp = premenne;
	
	while (temp != NULL && strcmp(temp->name, name) != 0) {
		temp = temp->next;
	}

	return temp->value;
}

void vypis_premenne(struct variable *premenne) {
	struct variable *temp = premenne;

	if (premenne != NULL) {
		temp = premenne;
		while (temp != NULL) {
			printf("%s %s ", temp->name, temp->value);
			temp = temp->next;
		}
	}
	else {
		printf("Ziadne premenne");
	}
	printf("\n");
}

// dosadi vsetky 'premenne' do 'potom'
// returns: doplneny string
char *dosad_var(char *potom, struct variable *premenna) {

	struct variable *temp = premenna;
	char *result = (char*)malloc(LINE_LIM);
	int i, j = 0;

	for (i = 6;i < strlen(potom);i++) {
		result[j++] = potom[i];
	}
	result[j] = '\0';

	while (temp != NULL) {
		result = replace(result, temp->name, temp->value);
		temp = temp->next;
	}

	return result;
}

void pridaj_vysledok(char *result) {

	struct Lines *temp = vysledky;

	if (vysledky == NULL) {
		vysledky = (struct Lines*)malloc(sizeof(struct Lines));
		vysledky->text = (char*)malloc(LINE_LIM);
		vysledky->text = strcpy(vysledky->text, result);
		vysledky->next = NULL;

		return;
	}

	while (temp->next != NULL) {

		if (strcmp(temp->text, result) == 0) return;

		temp = temp->next;
	}
	if (strcmp(temp->text, result) == 0) return;

	temp->next = (struct Lines*)malloc(sizeof(struct Lines));
	temp->next->text = (char*)malloc(LINE_LIM);
	temp->next->text = strcpy(temp->next->text, result);
	temp->next->next = NULL;
}

void vypis_riadky(struct Lines *yo) {
	
	struct Lines *temp = yo;

	while (temp != NULL) {
		printf("%s\n",temp->text);
		temp = temp->next;
	}
}

char *get_word(char *sentence, int *index) {
	
	char *result = (char*)malloc(NAME_LIM);
	int i = *index, j=0;

	while (sentence[i] != ' ' && sentence[i] != ')') {
		result[j++] = sentence[i++];
	}
	result[j] = '\0';

	*index = i;

	return result;
}

struct Lines *uvolni_vysledky() {

	struct Lines *temp = vysledky;

	while (temp != NULL) {
		vysledky = temp;
		temp = temp->next;
		free(vysledky->text);
		free(vysledky);
	}

	return NULL;
}

struct variable *uvolni_premenne(struct variable *premenne) {

	struct variable *temp = premenne;

	while (temp != NULL) {
		premenne = temp;
		temp = temp->next;
		free(premenne->name);
		free(premenne->value);
		free(premenne);
	}

	return NULL;
}


// #rekurzivna funkcia, ktora pre kazde pravidlo prejde kazdu podmienku,
// prejde kazdy fakt a snazi sa ich matchovat... ked nahodou dojdem na koniec,
// tak vyplujem ten result co mam v pravidle
// returns: void
void pokus2(struct rule *pravidlo, int podmienka, struct Lines *fakt, struct variable *premenne, struct Lines *prvyFakt) {

	char *potom = NULL;
	char *name, *name2;
	char *value;
	int i = podmienka, j = 0, pos = 0, result;
	struct variable *premenneDalej = NULL;
	struct variable *povodneVar = copy_var(premenne);

    // skusam ist
	if (pravidlo == NULL) { 
		//printf("nemam pravidlo\n"); 
		return; 
	}
	if (fakt == NULL) { 
		//printf("nemam fakty\n"); 
		return; 
	}

	// prechadzam cez podmienku
	while (pravidlo->condition[i] != ')') {

		// nasiel som premennu
		if (pravidlo->condition[i] == '?') {

			// pokusim sa pridat premennu do zoznamu...
			name = get_word(pravidlo->condition,&i);
			value = get_word(fakt->text,&j);

			result = pridaj_var(&premenne, name, value);

			// ked sa mi premennu nepodarilo pridat, koncim
			if (!result) { 
				premenneDalej = copy_var(povodneVar);
				pokus2(pravidlo, podmienka, fakt->next, premenneDalej, prvyFakt);
				return; 
			}
		}
		else {
			// ked sa nerovnaju znaky
			if (pravidlo->condition[i++] != fakt->text[j++]) {
				// prejdem na dalsi fakt
				premenneDalej = copy_var(povodneVar);
				pokus2(pravidlo, podmienka, fakt->next, premenneDalej, prvyFakt);
				return;
			}
		}
	}

	// specialna podmienka
	if (pravidlo->condition[i + 2] == '<') {
		i = i + 5;
		name = get_word(pravidlo->condition, &i);
		i++;
		name2 = get_word(pravidlo->condition, &i);

		// rovnaju sa tieto premenne?
		if (strcmp(get_value(premenne, name), get_value(premenne, name2)) == 0) {
			// ak ano, tak koncim
			premenneDalej = copy_var(povodneVar);
			pokus2(pravidlo, podmienka, fakt->next, premenneDalej, prvyFakt);
			return;
		}
		else {
			// ak nie, tak pridam
			potom = (char*)malloc(LINE_LIM);
			potom = strcpy(potom, pravidlo->result);

			pridaj_vysledok(dosad_var(potom, premenne));
		}
	}

	// som uz na konci vsetkych podmienok?
	if (pravidlo->condition[i + 1] == ')') {

		potom = (char*)malloc(LINE_LIM);
		potom = strcpy(potom, pravidlo->result);

		pridaj_vysledok(dosad_var(potom, premenne));
		//return;
	}

	premenneDalej = copy_var(povodneVar);
	pokus2(pravidlo, podmienka, fakt->next, premenneDalej, prvyFakt);
	
	// ked som tu, tak este nie som na poslednej podmienke
	// potrebujem teraz len nahodit dalsiu podmienku
	premenneDalej = copy_var(premenne);
	pokus2(pravidlo, i+1, prvyFakt, premenneDalej, prvyFakt);

}

struct Lines *vymaz_fakt(struct Lines *fakty, char *fact) {

	struct Lines *temp = fakty, *uvolnit;


	if (strcmp(fakty->text, fact) == 0) {

		uvolnit = fakty;
		fakty = fakty->next;
		free(uvolnit->text);
		free(uvolnit);
		return fakty;
	}

	while (temp->next != NULL) {
		if (strcmp(temp->next->text, fact) == 0) {
			
			uvolnit = temp->next;
			temp->next = temp->next->next;
			free(uvolnit->text);
			free(uvolnit);
			return fakty;
		}
		temp = temp->next;
	}
	return fakty;
}

struct Lines *pridaj_fakt(struct Lines *fakty, char *fact) {

	struct Lines *temp = fakty;

	while (temp->next != NULL) {
		temp = temp->next;
	}

	temp->next = (struct Lines*)malloc(sizeof(struct Lines));
	temp->next->text = (char*)malloc(LINE_LIM);
	temp->next->text = strcpy(temp->next->text,fact);
	temp->next->next = NULL;

	return fakty;
}

struct Lines *sprava_fakt(struct Lines *fakty, char *fact) {

	MessageBox(0, fact, "SPRAVA", 0);
	printf("$*~ %s ~*$\n",fact);

	return fakty;
}

int filtruj(struct Lines *fakty,char *fact) {

	struct Lines *temp = fakty;

	while (temp != NULL) {
		if (strcmp(temp->text, fact) == 0) {
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

int vykonaj(struct Lines **fakty, char *result) {

	char *slovo;
	char *novyFakt = (char*)malloc(LINE_LIM);

	int ret = 1;
	int i = 2, j = 0;

	while (i < strlen(result)) {
 
		slovo = get_word(result, &i);
		

		if (strcmp("pridaj", slovo) == 0) {
			j = 0; i++;
			novyFakt[j++] = '(';
			while (result[i] != ')') { novyFakt[j++] = result[i++]; }
			novyFakt[j++] = ')';
			novyFakt[j] = '\0';

			if (filtruj(*fakty, novyFakt)) {
				return 0;
			}

			printf("pridaj %s\n", novyFakt);
			(*fakty) = pridaj_fakt(*fakty, novyFakt);
		}


		if (strcmp("vymaz", slovo) == 0) {

			j = 0; i++;
			novyFakt[j++] = '(';
			while (result[i] != ')') { novyFakt[j++] = result[i++]; }
			novyFakt[j++] = ')';
			novyFakt[j] = '\0';

			if (!filtruj(*fakty, novyFakt)) {
				return 0;
			}

			printf("vymaz %s\n",novyFakt);
			(*fakty) = vymaz_fakt(*fakty, novyFakt); 
		}

		if (strcmp("sprava", slovo) == 0) { 

			j = 0; i++;
			while (result[i] != ')') { novyFakt[j++] = result[i++]; }
			novyFakt[j] = '\0';

			printf("sprava %s\n",novyFakt);

			(*fakty) = sprava_fakt(*fakty, novyFakt); 
		}
		i = i + 2;

	}
	return 1;
}

int main() {

	struct Lines *fakty = init_file("fakty.txt");
	struct rule *pravidla = init_rules("pravidla.txt");
	struct rule *temp = pravidla;
	struct Lines *rule;

	vysledky = uvolni_vysledky();

	while (temp != NULL) {
		pokus2(temp, 4, fakty, NULL, fakty);

		temp = temp->next;
	}

	while (vysledky != NULL) {

		/* MOZNO ZAKOMENTOVAT */
		vypis_riadky(vysledky);

		rule = vysledky;

		// hladam pravidlo, ktore by sa dalo vykonat
		while (!vykonaj(&fakty, rule->text)) {
			rule = rule->next;
			if (rule == NULL) {
				printf("Konecne fakty:\n");
				vypis_riadky(fakty);
				printf("Koniec");
				getchar();
				return 0;
			}
		}
		printf("Nove fakty: \n");
		vypis_riadky(fakty);
		printf("\n");

		/* MOZNO ZAKOMENTOVAT */
		getchar();

		vysledky = uvolni_vysledky();
		temp = pravidla;

		while (temp != NULL) {
			pokus2(temp, 4, fakty, NULL, fakty);

			temp = temp->next;
		}
	}
	printf("Konecne fakty:\n");
	vypis_riadky(fakty);
	printf("Koniec");

	getchar();
	return 0;
}