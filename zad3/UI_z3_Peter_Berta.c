#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GEN_NUM   10000
#define DUDE_NUM   200
#define PRINT_NUM   10

struct dude {
	char *cells;
	double fit;
};

char **init_field(){

	FILE *file = fopen("input.txt","r");

	char **tempPole;
	int fieldSize,treasureNum;
	int i,j;
	int x,y;
	
	fscanf(file,"%d",&fieldSize);

	tempPole = (char**)malloc(sizeof(char*)*fieldSize);

	for (i=0;i<fieldSize;i++){
	
		tempPole[i] = (char*)malloc(sizeof(char)*fieldSize);
	}

	for (i=0;i<fieldSize;i++) for (j=0;j<fieldSize;j++) tempPole[i][j] = '*';

	fscanf(file,"%d %d", &x, &y);	// zaciatok

	tempPole[x-1][y-1] = 'S';

	fscanf(file,"%d",&treasureNum);

	for (i=0;i<treasureNum;i++){
	
		fscanf(file,"%d %d", &x, &y);
		tempPole[x-1][y-1] = 'T';
	}

	fclose(file);

	return tempPole;
}

void print_field(char **pole, int size){

	int i,j;

	for (i=0;i<size;i++) {
		for (j=0;j<size;j++) 
			printf(" %c",pole[i][j]);
		printf("\n");
	}

}

void tobyte(char znak) {

	char temp = znak;
	int i;
	for (i = 0;i < 8;i++) {
		if (znak & 128) { putchar('1'); }
		else { putchar('0'); }
		znak = znak << 1;
	}
}

int get_instruction(char znak) {

	char instr = znak >> 6;

	return instr & 3;
}

double get_fit(char **field, struct dude *dude) {

	char cells[64];
	int i, act = 0, move;
	int x = 6, y = 3;
	double result = 0;

	// kopirovanie buniek, aby sa povodne zachovali
	for (i = 0;i < 64;i++) {
		cells[i] = dude->cells[i];
	}

	int pole[100]; // ci sme uz navstivili poklad alebo nie

	for (i = 0;i < 100;i++) {
		pole[i] = 0;
	}

	for (i = 0;i < 500;i++) {

		switch (get_instruction(cells[act])) {

		case 0:
			cells[act] = (cells[act] + 1) & 255;
			break;
		case 1:
			cells[act] = (cells[act] - 1) & 255;
			break;
		case 2:
			act = cells[act] & 63;
			break;
		case 3:
			move = cells[act] & 3;

			if (move == 0) {
				x -= 1;
				if (x < 0) {
					result = result - i * 0.001;
					if (result < 0) return (-1)*result;
					return result;
				}
				//printf(" H");
			}
			if (move == 1) {
				x += 1;
				if (x > 6) {
					result = result - i * 0.001;
					if (result < 0) return (-1)*result;
					return result;
				}
				//printf(" D"); 
			}
			if (move == 2) {
				y += 1;
				if (y > 6) {
					result = result - i * 0.001;
					if (result < 0) return (-1)*result;
					return result;
				}
				//printf(" P");
			}
			if (move == 3) {
				y -= 1;
				if (y < 0) {
					result = result - i * 0.001;
					if (result < 0) return (-1)*result;
					return result;
				}
				//printf(" L"); 
			}

			// vykonal som uspesny pohyb....
			// som na nejakom poklade teraz?

			if (field[x][y] == 'T') {
				if (pole[x * 7 + y] == 0) {
					pole[x * 7 + y]++;
					result++;
				}
			}

			if (result == 5) {
				result = result - i * 0.001;
				if (result < 0) return (-1)*result;
				return result - i * 0.001;
			}

			break;
		}

		act = (act + 1) & 63;
	}

	result = result - i * 0.001;
	if (result < 0) return (-1)*result;
	return result;
}

struct dude *new_dude(char **field) {

	struct dude *tempDude = (struct dude*)malloc(sizeof(struct dude));
	int i;
	//srand((unsigned)&tempDude);

	tempDude->cells = (char*)malloc(sizeof(char) * 64);
	tempDude->fit = 0;

	for (i = 0;i < 64;i++){
		if (i < 32) {
			tempDude->cells[i] = rand() % 256;
		}
		else {
			tempDude->cells[i] = 0;
		}
	}

	tempDude->fit = get_fit(field, tempDude);

	return tempDude;
}

struct dude *comb_dude(char **field, struct dude *dude1, struct dude *dude2) {

	struct dude *tempDude = (struct dude*)malloc(sizeof(struct dude));

	int i;

	tempDude->cells = (char*)malloc(sizeof(char) * 64);
	tempDude->fit = 0;

	for (i = 0;i < 64;i++) {
		if (i & 1) {
			tempDude->cells[i] = dude1->cells[i];
		}
		else {
			tempDude->cells[i] = dude2->cells[i];
		}
	}

	tempDude->fit = get_fit(field, tempDude);

	return tempDude;
}

struct dude *copy_dude(char **field, struct dude *dude) {

	struct dude *tempDude = (struct dude*)malloc(sizeof(struct dude));

	int i;

	tempDude->cells = (char*)malloc(sizeof(char) * 64);
	tempDude->fit = dude->fit;

	for (i = 0;i < 64;i++) {
		
		tempDude->cells[i] = dude->cells[i];
	}

	return tempDude;
}

struct dude **new_gen() {
	
	struct dude **tempGeneration = (struct dude**)malloc(sizeof(struct dude *)*DUDE_NUM);

	return tempGeneration;
}

void print_moves(char **field, struct dude *dude) {

	char cells[64];
	int i, act = 0, move;
	int x = 6, y = 3, result = 0;

	// kopirovanie buniek, aby sa povodne zachovali
	for (i = 0;i < 64;i++) {
		cells[i] = dude->cells[i];
	}

	int pole[100]; // ci sme uz navstivili poklad alebo nie

	for (i = 0;i < 100;i++) {
		pole[i] = 0;
	}

	for (i = 0;i < 500;i++) {

		switch (get_instruction(cells[act])) {

		case 0:
			cells[act] = (cells[act] + 1) & 255;
			break;
		case 1:
			cells[act] = (cells[act] - 1) & 255;
			break;
		case 2:
			act = cells[act] & 63;
			break;
		case 3:
			move = cells[act] & 3;

			if (move == 0) {
				x -= 1;
				if (x < 0) return;
				printf(" H");
			}
			if (move == 1) {
				x += 1;
				if (x > 6) return;
				printf(" D"); 
			}
			if (move == 2) {
				y += 1;
				if (y > 6) return;
				printf(" P");
			}
			if (move == 3) {
				y -= 1;
				if (y < 0) return;
				printf(" L"); 
			}

			// vykonal som uspesny pohyb....
			// som na nejakom poklade teraz?

			if (field[x][y] == 'T') {
				if (pole[x * 7 + y] == 0) {
					pole[x * 7 + y]++;
					result++;
				}
			}

			if (result == 5) return;

			break;
		}

		act = (act + 1) & 63;
	}
}

void sort_gen(struct dude **generation) {

	int i, j;
	struct dude *temp = NULL;

	for (i = 0; i < DUDE_NUM-1; i++){
		for (j = 0; j < DUDE_NUM - 1 - i; j++)
		{
			if (generation[j]->fit < generation[j+1]->fit) 
			{
				temp = generation[j];
				generation[j] = generation[j+1];
				generation[j+1] = temp;
			}
		}
	}
}

void print_gen(struct dude **generation) {
	
	int i;

	for (i = 0;i < PRINT_NUM;i++) {

		printf(" %.3f",generation[i]->fit);
	}
	printf("\n");
}

void free_gen(struct dude **generation) {

	int i;

	for (i = 0;i < DUDE_NUM;i++) {
		free(generation[i]);
	}

	free(generation);
}

char mutation(char znak) {
	return (znak ^ 255);
}

char mutation2(char znak) {
	return (znak ^ 170);
}

void selection1(char **pole) {

	struct dude **generation = NULL, **newGeneration = NULL;
	int i, j, count = 0, count2 = 0;
	int a,b,c,d; // medzniky

	while (1) {
		// aby po skonceni GEN_NUM stacilo stlacit enter a vyhladavanie bude pokracovat
		while (count++ < GEN_NUM) {
			printf("[%d]", ++count2);

			// ide o prvu generaciu???
			if (count2 == 1) {
				// ak ano, tak len vygenerujem DUDE_NUM novych ludi
				generation = new_gen();

				for (i = 0;i < DUDE_NUM;i++) generation[i] = new_dude(pole);

			}
			else {
				// ak je to druha, alebo dalsia generacia,
				// tak idem vyberat ludi z predchadzajucej generacie
				newGeneration = new_gen();

				// 10% best
				a = DUDE_NUM / 10;
				for (i = 0;i < a;i++) {
					newGeneration[i] = copy_dude(pole, generation[i]);
				}

				// 10 best mutation
				b = 2 * a;
				for (i = a;i < b;i++) {
					newGeneration[i] = copy_dude(pole, generation[i - 10]);

					for (j = 0;j < 64;j++) {
						newGeneration[i]->cells[j] = mutation(newGeneration[i]->cells[j]);
					}

					newGeneration[i]->fit = get_fit(pole, newGeneration[i]);
				}

				// 30 combi of (11 - 70)
				c = DUDE_NUM / 2;
				for (i = b;i < c;i++) {
					newGeneration[i] = comb_dude(pole, generation[0 + (i - 20) * 2], generation[1 + (i - 20) * 2]);
				}

				// 51-DUDE_NUM random
				for (i = c;i < DUDE_NUM;i++) {
					newGeneration[i] = new_dude(pole);
				}

				// uvolnim predchadzajucu generaciu
				free_gen(generation);

				generation = newGeneration;
			}

			// zoradim a vypisem aktualnu generaciu
			sort_gen(generation);
			print_gen(generation);

			// nenasiel som nahodou niekoho, kto nasiel vsetky poklady???
			if (generation[0]->fit > 4) {
				printf("\n DUDE :");

				print_moves(pole, generation[1]);

				print_field(pole, 7);

				break;
			}
		}

		printf("\n DUDE :");
		print_moves(pole, generation[1]);
		print_field(pole, 7);

		count = 0;
		getchar();
	}

}

void selection2(char **pole) {

	struct dude **generation = NULL, **newGeneration = NULL;
	int i, j, count = 0, count2 = 0;
	int a, b, c, d; // medzniky
	int turnaj[10], turnajMax=0, turnajWhich=0;
	int turnajMom, turnajDad;

	while (1) {
		// aby po skonceni GEN_NUM stacilo stlacit enter a vyhladavanie bude pokracovat
		while (count++ < GEN_NUM) {
			printf("[%d]", ++count2);

			// ide o prvu generaciu???
			if (count2 == 1) {
				// ak ano, tak len vygenerujem DUDE_NUM novych ludi
				generation = new_gen();

				for (i = 0;i < DUDE_NUM;i++) generation[i] = new_dude(pole);

			}
			else {
				// ak je to druha, alebo dalsia generacia,
				// tak idem vyberat ludi z predchadzajucej generacie
				newGeneration = new_gen();

				// 10% best
				a = DUDE_NUM / 10;
				for (i = 0;i < a;i++) {
					newGeneration[i] = copy_dude(pole, generation[i]);
				}

				// 20% random mutation
				b = 2 * a;
				for (i = a;i < b;i++) {

					newGeneration[i] = copy_dude(pole, generation[rand() % DUDE_NUM]);

					for (j = 0;j < 64;j++) {
						newGeneration[i]->cells[j] = mutation2(newGeneration[i]->cells[j]);
					}

					newGeneration[i]->fit = get_fit(pole, newGeneration[i]);
				}

				// 20% combi -> turnaj
				c = DUDE_NUM / 2;
				for (i = b;i < c;i++) {

					turnajMax = 0;
					turnajWhich = 0;
					for (j = 0;j < 10;j++) {
						
						d = rand() % DUDE_NUM;

						while (generation[d]->fit == -1) {
							d = rand() % DUDE_NUM;
						}

						turnaj[j] = d;

						if (generation[turnaj[j]]->fit > turnajMax) {
							turnajMax = generation[turnaj[j]]->fit;
							turnajWhich = j;
						}
					}

					turnajMom = turnaj[turnajWhich];
					generation[turnajMom]->fit = -1;

					turnajMax = 0;
					turnajWhich = 0;
					for (j = 0;j < 10;j++) {

						d = rand() % DUDE_NUM;

						while (generation[d]->fit == -1) {
							d = rand() % DUDE_NUM;
						}

						turnaj[j] = d;

						if (generation[turnaj[j]]->fit > turnajMax) {
							turnajMax = generation[turnaj[j]]->fit;
							turnajWhich = j;
						}
					}

					turnajDad = turnaj[turnajWhich];

					
					generation[turnajDad]->fit = -1;

					//printf("Turnaj: %d %d\n", turnajMom, turnajDad);

					newGeneration[i] = comb_dude(pole, generation[turnajMom], generation[turnajDad]);
				}

				// 51-DUDE_NUM random
				for (i = c;i < DUDE_NUM;i++) {
					newGeneration[i] = new_dude(pole);
				}

				// uvolnim predchadzajucu generaciu
				free_gen(generation);

				generation = newGeneration;
			}

			// zoradim a vypisem aktualnu generaciu
			sort_gen(generation);
			print_gen(generation);

			// nenasiel som nahodou niekoho, kto nasiel vsetky poklady???
			if (generation[0]->fit > 4) {
				printf("\n DUDE :");

				print_moves(pole, generation[1]);


				return;
			}
		}

		printf("\n DUDE :");
		print_moves(pole, generation[1]);
		print_field(pole, 7);

		count = 0;
		getchar();
	}

}

int main(){

	char **pole = NULL;
	time_t t;

	// aby boli cisla naozaj nahodne
	srand((unsigned)time(&t));

	// precitanie vstupneho pola zo suboru
	pole = init_field();
	// vypisanie pola na vystup
	print_field(pole,7);	

	// VYBERTE SELECTION #


	selection1(pole);

	// KONIEC

	getchar();
	return 0;
}