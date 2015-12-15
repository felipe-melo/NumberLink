#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct Pair Pair;
typedef struct ColorPair ColorPair;
typedef struct Stack Stack;
typedef struct ColorPairAux ColorPairAux;

class Solution;

int quantSolutions;

void generateSolution(ColorPairAux *colorPairAux, Solution *solution);
int **startMatrix(int rows, int cols);
bool canPLayMore(int x, int y, Solution *solution);
bool isEndNear(int x, int y, int xEnd, int yEnd);
void rankSolutions(Solution **solutions);
void makeSelection(Solution **roulette, Solution **solutions, int fitnessOfAllSolutions, double crossTax);

void crossOver(Solution **roulette, Solution **solutions, double crossTax);

Solution **inicializeSolutions(int quant);
ColorPair *inicializePairColors(int quant);
ColorPairAux *inicializePairColorsAux(int quant);
void copyCromossomo(Solution *parent, Solution *children, bool beginX, bool beginY);
void copyColorPair(ColorPairAux *colorPairAux, ColorPair *colorPair, int quantColors);

Stack *startStack();
Stack *insertStack(Stack *stack, Pair *value);
Pair *pop(Stack **stack);

struct Pair {
	int x;
	int y;
};

struct ColorPairAux {
	Pair origem;
	Pair destino;
	bool visited;
};

struct ColorPair {
	Pair origem;
	Pair destino;
};

struct Stack {
	Pair *value;
	Stack *next;
};

class Solution {

	public:
		int **_grid;
		double _fitness;
		double _probability;
		int _quantColors;
		int _rows, _cols;
		ColorPair *_colorPair;

		//this->solution(); // = 0.0f;
		Solution(int **grid, ColorPair *colorPair, int quantColors, int rows, int cols){
			_grid = startMatrix(rows, cols);
			_fitness = 0; _colorPair = colorPair;
			_quantColors = quantColors;
			_probability = 0.0; _rows = rows; _cols = cols;
			for (int i = 0; i < _rows; i++){
				for (int j = 0; j < _cols; j++){
					_grid[i][j] = grid[i][j];
				}
			}
		}

		Solution(ColorPair *colorPair, int quantColors, int rows, int cols){
			_rows = rows; _cols = cols;
			_grid = startMatrix(rows, cols);
			_fitness = 0.0;
			_probability = 0.0;
			_colorPair = colorPair;
			_quantColors = quantColors;
		}

		void mutation(int **gridOriginal, double tax){
			int randNum, color;
			for (int i = 0; i < _rows; i++){
				for (int j = 0; j < _cols; j++){
					randNum = rand();
					if (gridOriginal[i][j] == 0 && randNum <= tax){
						do {
							color = (rand() % _quantColors) + 1;
						}while (color == _grid[i][j]);
						_grid[i][j] = color;
					}
				}
			}
		}

		void rank(int colorIndex, int color){

			if (color > _quantColors){
				return;
			}

			Pair *origem = (Pair*) malloc(sizeof(Pair));
			Pair *destino = (Pair*) malloc(sizeof(Pair));

			origem->x = _colorPair[colorIndex].origem.x;
			origem->y = _colorPair[colorIndex].origem.y;
			destino->x = _colorPair[colorIndex].destino.x;
			destino->y = _colorPair[colorIndex].destino.y;

			Stack *stack = startStack();
			stack = insertStack(stack, origem);

			Pair *pair;

			int **gridAux = startMatrix(_rows, _cols);
			for (int i = 0; i < _rows; i++){
				for (int j = 0; j < _cols; j++){
					gridAux[i][j] = _grid[i][j];
				}
			}

			do {
				pair = pop(&stack);
				//printf("(%d, %d) = %d\n", pair->x, pair->y, color);
				if (pair->x == destino->x && pair->y == destino->y){
					_fitness++;
					break;
				}
				if (pair->x-1 >= 0 && gridAux[pair->x-1][pair->y] == color){
					Pair *aux = (Pair*) malloc(sizeof(Pair));
					aux->x = pair->x-1;
					aux->y = pair->y;
					stack = insertStack(stack, aux);
					gridAux[pair->x-1][pair->y] = 0;
				}
				if (pair->x+1 < _rows && gridAux[pair->x+1][pair->y] == color){
					Pair *aux = (Pair*) malloc(sizeof(Pair));
					aux->x = pair->x+1;
					aux->y = pair->y;
					stack = insertStack(stack, aux);
					gridAux[pair->x+1][pair->y] = 0;
				}
				if (pair->y-1 >= 0 && gridAux[pair->x][pair->y-1] == color){
					Pair *aux = (Pair*) malloc(sizeof(Pair));
					aux->x = pair->x;
					aux->y = pair->y-1;
					stack = insertStack(stack, aux);
					gridAux[pair->x][pair->y-1] = 0;
				}
				if (pair->y+1 < _cols && gridAux[pair->x][pair->y+1] == color){
					Pair *aux = (Pair*) malloc(sizeof(Pair));
					aux->x = pair->x;
					aux->y = pair->y+1;
					stack = insertStack(stack, aux);
					gridAux[pair->x][pair->y+1] = 0;
				}
				gridAux[pair->x][pair->y] = 0;
			} while(stack != NULL);
			return rank(colorIndex+1, color+1);
		}

		void toString(){
			printf("\n");
			printf("%f\n", _fitness);
			for (int i = 0; i < _rows; i++){
				for (int j = 0; j < _cols; j++){
					printf("%d ", _grid[i][j]);
				}
				printf("\n");
			}
		}
};

int main(int argc, char* argv[]){
	FILE *gameFile;
	FILE *gameSolutions;

	Solution **roulette;

	int cell, games, **grid, fitnessOfAllSolutions = 0, iterations;
	char aux;
	double mutationTax, crossTax;

	ColorPair *colorPair;

	ColorPairAux *colorPairAux;

	int rows, cols, quantColors;
	
	Solution **solutions;

	srand(time(NULL));

	gameFile = fopen ("games5x5.dat", "r");
	if (gameFile == NULL)
		printf("Erro ao ler o arquivo\n");
	else {
		fscanf (gameFile, "%d\n", &games);
		quantSolutions = atoi(argv[1]);
		mutationTax = atof(argv[2]);
		crossTax = atof(argv[3]);
		iterations = atoi(argv[4]);
		while (games > 0){
			fscanf (gameFile, "%dx%dx%d\n", &rows, &cols, &quantColors);

			solutions = inicializeSolutions(quantSolutions);
			roulette  = inicializeSolutions(100);
			colorPair = inicializePairColors(quantColors);
			colorPairAux = inicializePairColorsAux(quantColors);

			grid = startMatrix(rows, cols);

			//Carrega um vetor com todos as coordenadas de origem e destino das cores
			//Carrega a matriz do jogo
			for (int i = 0; i < rows; i++){
				for (int j = 0; j < cols; j++){
					fscanf (gameFile, "%d", &cell);
					grid[i][j] = cell;

					if (cell != 0){
						if (colorPair[cell-1].origem.x == -1){
							colorPair[cell-1].origem.x = i;
							colorPair[cell-1].origem.y = j;
						}else{
							colorPair[cell-1].destino.x = i;
							colorPair[cell-1].destino.y = j;
						}
					}
				}
				fscanf (gameFile, "%c", &aux);
			}

			games--;

			for (int count = 0; count < quantSolutions; count++){

				if (solutions[count] != NULL)
					delete solutions[count];
				
				solutions[count] = new Solution(grid, colorPair, quantColors, rows, cols);
				copyColorPair(colorPairAux, colorPair, quantColors); //Copia as cores para um vetor auxiliar com um parâmetros bool
				generateSolution(colorPairAux, solutions[count]); //gera a solução para a matriz
				fitnessOfAllSolutions += solutions[count]->_fitness; //acumula o fitness da solução gerada
				if (solutions[count]->_fitness == quantColors){ //verifica se a solução foi encontrada
					printf("Solução encontrada\n");
					solutions[count]->toString();
					return 0;
				}
			}
			int count = 0, bestFitness = -1;
			
			//Roda o algoritmo genético em si
			do{
				rankSolutions(solutions);
				makeSelection(roulette, solutions, fitnessOfAllSolutions, crossTax);
				crossOver(roulette, solutions, crossTax);
				for (int i = quantSolutions*crossTax; i < quantSolutions; i++){
					solutions[i]->mutation(grid, mutationTax);
					solutions[i]->rank(0, 1);
					if (solutions[i]->_fitness > bestFitness)
						bestFitness = solutions[i]->_fitness;
				}
				count++;
			} while (count < iterations && bestFitness < quantColors);
			
			rankSolutions(solutions);
			
			for (count = 0; count < 5; count++){
				printf("\n%d\n", count);
				if (solutions[count]->_fitness == quantColors){
					printf("achou\n");
					solutions[count]->toString();
					break;
				}
				solutions[count]->toString();
			}
		}
		fclose (gameFile);
	}
	return 0;
}

void generateSolution(ColorPairAux *colorAux, Solution *solution){
	int randLine, randCol, randColor, color;

	for (int i = 0; i <  solution->_quantColors; i++){
		if (olorAux[randColor].visited)
	}

	do {
		randColor = rand() % solution->_quantColors;
	}while(colorAux[randColor].visited);

	color = randColor+1;

	//Caso o ponto de origem já começe encuralado
	if (!(canPLayMore(colorAux[randColor].origem.x, colorAux[randColor].origem.y, solution))){
		colorAux[randColor].visited = true;
		generateSolution(colorAux, solution);
	}else while(true){
		randLine = (rand() % 3) + colorAux[randColor].origem.x - 1;
		randCol = (rand() % 3) + colorAux[randColor].origem.y - 1;

		//verifica se está gerando na cruz da posição anterior
		if (!((randLine != colorAux[randColor].origem.x && randCol == colorAux[randColor].origem.y) || 
			(randLine == colorAux[randColor].origem.x && randCol != colorAux[randColor].origem.y))) continue;
		//verifica se está no range da matriz
		if (randLine >= solution->_rows || randLine < 0 || randCol >= solution->_cols || randCol < 0) continue;

		//verifica se terminou o caminho ou se é uma posição inviável
		if ((randLine != colorAux[randColor].destino.x || randCol != colorAux[randColor].destino.y) && 
			solution->_grid[randLine][randCol] != 0)
			continue;

		//Analise se a posição gerada é vazia(0) e se o final está nas adjacencias
		if (solution->_grid[randLine][randCol] == 0 && isEndNear(randLine, randCol, colorAux[randColor].destino.x, colorAux[randColor].destino.y)){
			solution->_fitness += 1;
			solution->_grid[randLine][randCol] = color;
			colorAux[randColor].visited = true;
			generateSolution(colorAux, solution);
			break;
		}

		//Caso não tenha mais onde jogar e a posição gerada é vazia(0)
		//joga-se nela e passa para a próxima cor
		if (!canPLayMore(randLine, randCol, solution)) {
			if (solution->_grid[randLine][randCol] == 0)
				solution->_grid[randLine][randCol] = color;

			colorAux[randColor].visited = true;
			generateSolution(colorAux, solution);
			break;
		//Caso seja uma posição vazia(0), não tenha acabado e tenha-se onde jogar no futuro
		}else if (solution->_grid[randLine][randCol] == 0){
			colorAux[randColor].origem.x = randLine;
			colorAux[randColor].origem.y = randCol;
			solution->_grid[randLine][randCol] = color;
		}
	}
}

bool canPLayMore(int x, int y, Solution *solution){
	if ((x-1 >= 0   		   && solution->_grid[x-1][y] == 0) ||
		(x+1 < solution->_rows && solution->_grid[x+1][y] == 0) ||
		(y-1 >= 0   		   && solution->_grid[x][y-1] == 0) ||
		(y+1 < solution->_cols && solution->_grid[x][y+1] == 0)) return true;
	return false;
}

bool isEndNear(int x, int y, int xEnd, int yEnd){
	if ((x-1 == xEnd && y == yEnd) ||
		(x+1 == xEnd && y == yEnd) ||
		(y-1 == yEnd && x == xEnd) ||
		(y+1 == yEnd && x == xEnd)) return true;
	return false;
}

int **startMatrix(int rows, int cols){
	int **grid = new int*[rows];
	for (int i = 0; i < rows; i++)
		grid[i] = new int[cols];
	return grid;
}

void rankSolutions(Solution **solutions){
	Solution *aux;
	for (int i = 0; i < quantSolutions-1; i++){
		for (int j = i+1; j < quantSolutions; j++){
			if (solutions[j]->_fitness > solutions[i]->_fitness){
				aux = solutions[j];
				solutions[j] = solutions[i];
				solutions[i] = aux;
			}
		}
	}
}

ColorPair* inicializePairColors(int quant){
	ColorPair *colorPair = (ColorPair*) malloc(sizeof(ColorPair) * quant);
	for (int c = 0; c < quant; c++){
		colorPair[c].origem.x = -1;
		colorPair[c].origem.y = -1;
		colorPair[c].destino.x = -1;
		colorPair[c].destino.y = -1;
	}
	return colorPair;
}

ColorPairAux* inicializePairColorsAux(int quant){
	ColorPairAux *colorPair = (ColorPairAux*) malloc(sizeof(ColorPairAux) * quant);
	for (int c = 0; c < quant; c++){
		colorPair[c].origem.x = -1;
		colorPair[c].origem.y = -1;
		colorPair[c].destino.x = -1;
		colorPair[c].destino.y = -1;
	}
	return colorPair;
}

Solution **inicializeSolutions(int quant){
	Solution **solutions = (Solution**) malloc(sizeof(Solution*) * quant);
	for (int c = 0; c < quant; c++){
		solutions[c] = NULL;
	}
	return solutions;
}

void makeSelection(Solution **roulette, Solution **solutions, int fitnessOfAllSolutions, double crossTax){
	int selectionCount = 0, k, randF, randM;
	for (int i = 0; i < quantSolutions && selectionCount < 100; i++){
		solutions[i]->_probability = solutions[i]->_fitness * 5 / fitnessOfAllSolutions;
		k = 0;
		while (k < (int)(solutions[i]->_probability * 100) && selectionCount < 100){
			roulette[selectionCount++] = solutions[i];
			k++;
		}
	}

	for (int count = quantSolutions*crossTax; count < quantSolutions; count++){
		if (solutions[count] != NULL)
			delete solutions[count];
		solutions[count] = NULL;
	}
}

void crossOver(Solution **roulette, Solution **solutions, double crossTax){
	int randM, randF;
	Solution *children;

	int k = quantSolutions*crossTax;
	while(k < quantSolutions){
		randF = (rand() % 100);
		randM = (rand() % 100);
		if (randF == randM) continue;

		children = new Solution(roulette[randF]->_colorPair, roulette[randF]->_quantColors, roulette[randF]->_rows, roulette[randF]->_cols);
		copyCromossomo(roulette[randF], children, true, true);
		copyCromossomo(roulette[randM], children, true, false);
		copyCromossomo(roulette[randM], children, false, true);
		copyCromossomo(roulette[randF], children, false, false);
		solutions[k++] = children;

		children = new Solution(roulette[randF]->_colorPair, roulette[randF]->_quantColors, roulette[randF]->_rows, roulette[randF]->_cols);
		copyCromossomo(roulette[randM], children, true, true);
		copyCromossomo(roulette[randF], children, true, false);
		copyCromossomo(roulette[randF], children, false, true);
		copyCromossomo(roulette[randM], children, false, false);
		solutions[k++] = children;
	}
}

void copyCromossomo(Solution *parent, Solution *children, bool beginX, bool beginY){
	for (int i = (beginX ? 0 : parent->_rows/2); i < (beginX ? parent->_rows/2 : parent->_rows); i++){
		for (int j = (beginY ? 0 : parent->_cols/2); j < (beginY ? parent->_cols/2 : parent->_cols); j++){
			children->_grid[i][j] = parent->_grid[i][j];
		}
	}
}

void copyColorPair(ColorPairAux *colorPairAux, ColorPair *colorPair, int quantColors){
	for (int i = 0; i < quantColors; i++){
		colorPairAux[i].origem.x = colorPair[i].origem.x;
		colorPairAux[i].origem.y = colorPair[i].origem.y;
		colorPairAux[i].destino.x = colorPair[i].destino.x;
		colorPairAux[i].destino.y = colorPair[i].destino.y;
		colorPairAux[i].visited = false;
	}
}

Stack *startStack(){
	return NULL;
}

Stack *insertStack(Stack *stack, Pair *value){
	Stack *newStack = (Stack*) malloc(sizeof(Stack));
	newStack->value = value;
	newStack->next = stack;
	return newStack;
}

Pair *pop(Stack **stack){
	if ((*stack) != NULL){
		Pair *pair = (*stack)->value;
		*stack = (*stack)->next;
		return pair;
	}
	return NULL;
}