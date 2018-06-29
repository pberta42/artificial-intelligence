#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// globalne premenne - jednoduchsia statistika
int generated = 1, used = 0;

struct list {	// spajany zoznam, v ktorom su ukladane generovane stavy
	struct node *node;	
	struct list *next;
};

struct node {	// obsahuje informacie o stave
	char *state;
	int move; // 1 hore, 2 dole, 3 vlavo, 4 vpravo
	int depth;	// informacia o hlbke vrcholu
	int heur;	// hodnota heuristickej funkcie
	struct node *last;	// odkaz na predchadzajuci stav
};

struct tableNode {	// vrchol tabulky - spajany zoznam
	char *state;
	struct tableNode *next;
};

struct node *create_node(char *state, int depth, int heur, struct node *dad, int move) {
	struct node *newNode = NULL;

	if (state == NULL) {
		return NULL;
	}

	newNode = (struct node*)malloc(sizeof(struct node));
	newNode->heur = heur;
	newNode->state = state;
	newNode->last = dad;
	newNode->depth = depth;
	newNode->move = move;

	return newNode;
}

void print_state(char *state){	// vypis stavu v peknom formate
	int i, pos = 0;
	if (state == NULL) {
		printf("WHOA! Nic tu nie je...\n");
		return;
	}

	for (i = 0;i<3;i++) {
		printf(" %c | %c | %c\n", state[pos], state[pos + 1], state[pos + 2]);
		pos += 3;
	}
	printf("\n");
}

char *read_state(){	// precitanie stavu zo vstupu
	int pos=0;
	char znak, *stav = (char*)malloc(sizeof(char) * 9);

	while ((znak = getchar()) != '\n' && znak != EOF) {
		if (znak >= '0' && znak < '9') {
			stav[pos++] = znak;
		}
	}
	stav[pos] = '\0';
	return stav;
}

int find(char *state, char blob) {	// najdenie znaku 'blob' v stave
	int i;
	for (i = 0;i<9;i++)
		if (state[i] == blob) return i;
	return -1;	// return index znaku v stave
}

int heur(char *begState, char*endState, int which) {	// funkcia pre vypocitanie heuristickych funkcii
	int i, result = 0;
	int secInd;

	if (which == 1) {
		if (begState == NULL) return 0;

		for (i = 0;i < 8;i++)
			if (begState[i] != endState[i]) result++;

		return result;
	}
	else {
		if (begState == NULL) return 0;

		for (i = 0;i < 9;i++)
		{
			if (begState[i] != '0') {
				secInd = find(endState, begState[i]);
				result += abs(secInd - i) / 3 + abs(secInd - i) % 3;
			}
		}
		return result;
	}
}

char *exchange(char *state, int ind1, int ind2) {	// funkcia pre vymenu dvoch policok v stave
	char *result = (char*)malloc(sizeof(char) * 9); // generacia noveho stavu
	char help;
	int i;

	for (i = 0;i < 9;i++) {
		result[i] = state[i];
	}

	help = result[ind1];
	result[ind1] = result[ind2];
	result[ind2] = help;
	return result;
}

char *hore(char *state) {
	char *result = NULL;
	int emptInd = find(state, '0'); // najdenie prazdneho miesta v stave

	if (emptInd > 5) result = NULL;	// kontrola uskutocnitelnosti pohybu
	else result = exchange(state, emptInd, emptInd + 3);

	return result;
}

char *dole(char *state) {
	char *result = NULL;
	int emptInd = find(state, '0'); // najdenie prazdneho miesta v stave

	if (emptInd < 3) result = NULL;	// kontrola uskutocnitelnosti pohybu
	else result = exchange(state, emptInd, emptInd -3);

	return result;
}

char *vlavo(char *state) {
	char *result = NULL;
	int emptInd = find(state, '0'); // najdenie prazdneho miesta v stave

	if (emptInd % 3 < 1) result = NULL;	// kontrola uskutocnitelnosti pohybu
	else result = exchange(state, emptInd, emptInd - 1);

	return result;
}

char *vpravo(char *state) {
	char *result = NULL;
	int emptInd = find(state, '0'); // najdenie prazdneho miesta v stave

	if (emptInd % 3 > 1) result = NULL;	// kontrola uskutocnitelnosti pohybu
	else result = exchange(state, emptInd, emptInd + 1);

	return result;
}

int hash(char *state) {	// hashovacia funkcia
	int result = 0, i, ten = 1;

	for (i = 8;i >= 0;i--) {
		result += (state[i] - 48)*ten;
		ten *= 10;
	}
	result = ((result / 100000) + (result % 100000)) % 100000;
	return abs(result);
}

int insert_table(struct tableNode *(*table)[100000], char *state) {	// vkladanie do hash tabulky
	struct tableNode *temp = NULL;
	int key = hash(state);

	if ((*table)[key] == NULL) {	// kontrola, ci je miesto v tabulke prazdne
		(*table)[key] = (struct tableNode *)malloc(sizeof(struct tableNode));
		((*table)[key])->state = state;
		((*table)[key])->next = NULL;
	}
	else {
		temp = (*table)[key];

		// prechod po spajanom zozname
		while (temp->next != NULL && strcmp(state, temp->state) != 0) {
			temp = temp->next;
		}

		if (strcmp(state, temp->state) == 0) return 0;

		temp->next = (struct tableNode *)malloc(sizeof(struct tableNode));
		temp->next->state = state;
		temp->next->next = NULL;
	}
	return 1;
}

struct list *insert_que(struct list *list, struct node *node, struct tableNode *(*table)[100000]) {
	// funkcia pre vkladanie vygenerovanych stavov do radu

	struct list *pos = list, *tempRad = NULL;
	struct node *tempNode = NULL;

	if (node == NULL) return list;

	if (list == NULL) {	// kontrola, ci je zoznam prazdny
		list = (struct list *)malloc(sizeof(struct list));
		list->next = NULL;
		list->node = node;
		return list;
	}

	// nachadza sa tento stav uz v hash tabulke?
	if (!insert_table(table, node->state)) return list;

	// ma sa tento stav nachadzat na prvom mieste?
	if (list->node->heur > node->heur) {
		if (list->next == NULL || list->next->node->heur > node->heur) {
			tempNode = list->node;
			list->node = node;
			node = tempNode;
		}
	}

	while (pos->next != NULL && pos->next->node->heur <= node->heur) {
		pos = pos->next;
	}

	tempRad = pos->next;

	pos->next = (struct list *)malloc(sizeof(struct list));
	pos->next->next = tempRad;
	pos->next->node = node;
	
	return list;
}

struct node *pull_list(struct list **list) {	// vybratie prveho stavu zo zoznamu
	struct node *temp = NULL;
	
	if (*list == NULL) return NULL;
	
	used++;	// statistika

	temp = (*list)->node;
	(*list) = (*list)->next;
	return temp;
}

struct list *generate(struct list *list, char *endState, struct tableNode *(*table)[100000], int which) {
	//	vygenerovanie novych stavov

	struct node *temp = pull_list(&list);	// vybratie prveho prvku zo zoznamu
	char *horeState, *doleState, *vlavoState, *vpravoState;
	

	horeState = hore(temp->state);
	if (horeState != NULL) {	// je mozne vykonat tento pohyb?
		generated++;	// statistika
		list = insert_que(list, create_node(horeState, temp->depth + 1, heur(horeState, endState, which), temp, 1), table);
	}
	doleState = dole(temp->state);
	if (doleState != NULL) {	// je mozne vykonat tento pohyb?
		generated++;	// statistika
		list = insert_que(list, create_node(doleState, temp->depth + 1, heur(doleState, endState, which), temp, 2),table);
	}
	vlavoState = vlavo(temp->state);
	if (vlavoState != NULL) {	// je mozne vykonat tento pohyb?
		generated++;	// statistika
		list = insert_que(list, create_node(vlavoState, temp->depth + 1, heur(vlavoState, endState, which), temp, 3),table);
	}
	vpravoState = vpravo(temp->state);
	if (vpravoState != NULL) {	// je mozne vykonat tento pohyb?
		generated++;	// statistika
		list = insert_que(list, create_node(vpravoState, temp->depth + 1, heur(vpravoState, endState, which), temp, 4),table);
	}

	return list;
}

void print_list(struct list *list) {	// vypisanie celeho radu
	struct list *temp = list;
	printf("VYPISOVANIE RADU\n");
	while (temp != NULL) {
		print_state(temp->node->state);
		printf("%d %d %p %p\n", temp->node->heur, temp->node->depth, temp->node->last, temp->node);
		printf("\n");
		temp = temp->next;
	}
	printf("KONIEC VYPISOVANIA RADU\n");
}

void print_solution(struct node *node,int type) {	// vypisanie spravneho riesenia
	int i;

	if (node != NULL) {		// rekurzivne vypisovanie stavov
		print_solution(node->last, type);

		switch (node->move) {
		case 0:
			printf("START ");
			break;
		case 1:
			printf("HORE  ");
			break;
		case 2:
			printf("DOLE  ");
			break;
		case 3:
			printf("VLAVO ");
			break;
		case 4:
			printf("VPRAVO");
			break;
		}

		if (type == 1) {
			printf("\n");
			print_state(node->state);
		}
		else {
			for (i = 0;i < 9;i++) {
				printf(" %c", node->state[i]);
			}
			printf("\n");
		}
	}
}

void empty_list(struct list **list) {	// vyprazdnenie zoznamu
	struct list *temp=*list,*temp2;
	while (temp != NULL) {
		temp2 = temp->next;
		free(temp);
		temp = temp2;
	}
	*list = NULL;
}

void empty_table(struct tableNode *(*table)[100000]) {
	int i;
	struct tableNode *temp1, *temp2;
	for (i = 0;i < 100000;i++) {
		temp1 = (*table)[i];
		if ( temp1 != NULL ) {
			temp2 = temp1->next;
			free(temp1);
			temp1 = temp2;
		}
		(*table)[i] = NULL;
	}
}

void compare(clock_t diff1, clock_t diff2) {	// zhodnotenie rychlosti algoritmov
	int num = 1;
	if (diff1 > diff2) {
		num = 2;
	}
	printf("\nRychlejsi bol algoritmus, ktory pouzival heuristicku funkciu c. %d...\n", num);
}

int main(void) {

	char *begState = NULL, *endState = NULL;	// zaciatocny a konecny stav (zo vstupu)
	struct list *list = NULL;	// zoznam, v ktorom budu ulozene generovane stavy
	struct tableNode *table[100000];	// hash tabulka
	int i, msec, typZobrazenia = 2; // 1 - pokrocile, 2 - jednoduche
	clock_t start, diff1, diff2;


	begState = read_state();	// precitanie stavov zo vstupu
	endState = read_state();
	printf("Zaciatocny stav:\n");
	print_state(begState);
	printf("Konecny stav:\n");
	print_state(endState);

	empty_list(&list);	// vyprazdnenie zoznamu
	for (i = 0;i < 100000;i++) table[i] = NULL;	// vyprazdnenie hash tabulky

	printf("Zaciname hladanie s pouzitim PRVEJ heuristickej funkcie...\n");
	start = clock();

	// pridanie prveho (zaciatocneho) stavu do zoznamu
	list = insert_que(list, create_node(begState, 0, heur(begState, endState, 1), NULL, 0), &table);
	while (list->node->heur > 0 && generated < 362880) {	
		// generovanie novych stavov, kym nevycerpame vsetky moznosti
		list = generate(list, endState,&table, 1);
	}

	if (generated > 362880) {
		printf("Ziadne riesenie nebolo najdene...\n");
		getchar();
		return;
	}

	diff1 = clock() - start;	// casovy rozdiel
	msec = msec = diff1 * 1000 / CLOCKS_PER_SEC;
	print_solution(list->node, typZobrazenia);	// vypis riesenia
	printf("Dlzka hladania riesenia bola %d sec a %d msec\n", msec/1000, msec % 1000);
	printf("Konecny stav najdeny v hlbke %d...\n", list->node->depth);
	printf("Bolo generovanych %d stavov a z toho pouzitych %d stavov...\n", generated, used);

	generated = 1;
	used = 0;

	empty_list(&list);	// vyprazdnenie zoznamu
	empty_table(&table);
	for (i = 0;i < 100000;i++) table[i] = NULL;// vyprazdnenie hash tabulky

	printf("Zaciname hladanie s pouzitim DRUHEJ heuristickej funkcie...\n");
	start = clock();

	// pridanie prveho (zaciatocneho) stavu do zoznamu
	list = insert_que(list, create_node(begState, 0, heur(begState, endState, 2), NULL, 0), &table);
	while (list->node->heur > 0 && generated < 362880) {
		// generovanie novych stavov, kym nevycerpame vsetky moznosti
		list = generate(list, endState, &table, 2);
	}

	diff2 = clock() - start;	// casovy rozdiel
	msec = msec = diff2 * 1000 / CLOCKS_PER_SEC;
	print_solution(list->node, typZobrazenia);	// vypis riesenia
	printf("Dlzka hladania riesenia bola %d sec a %d msec\n", msec / 1000, msec % 1000);
	printf("Konecny stav najdeny v hlbke %d...\n", list->node->depth);
	printf("Bolo generovanych %d stavov a z toho pouzitych %d stavov...\n", generated, used);

	compare(diff1,diff2);	// vyhodnotenie

	getchar();
	return 0;
}	
