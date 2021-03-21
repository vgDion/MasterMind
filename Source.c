#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct node {
	int data;
	struct node* next;
}Node;

typedef struct list {
	Node* head;
}List;


Node* createnode(int data) {
	Node* newNode = malloc(sizeof(Node));
	if (!newNode) {
		return NULL;
	}
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}

List* makelist() {
	List* list = malloc(sizeof(List));
	if (!list) {
		return NULL;
	}
	list->head = NULL;
	return list;
}

void display(List* list) {
	Node* current = list->head;
	if (list->head == NULL)
		return;

	for (; current != NULL; current = current->next) {
		printf("%d\n", current->data);
	}
}

void add(int data, List* list) {
	Node* current = NULL;
	if (list->head == NULL) {
		list->head = createnode(data);
	}
	else {
		current = list->head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = createnode(data);
	}
}


void delete(int data, List* list) {
	Node* current = list->head;
	Node* previous = current;
	while (current != NULL) {
		if (current->data == data) {
			previous->next = current->next;
			if (current == list->head)
				list->head = current->next;
			free(current);
			return;
		}
		previous = current;
		current = current->next;
	}
}


/*structure for the possibility of answering the guessing test, which positions coincided how*/

typedef struct Response
{
	int white;
	int black;
}response;


/*the function checks the coincidence of the guesser's sample with the hidden code and gives information if there is a match*/

response checkÑode(int code, int guess)
{
	int ox = 0, cow = 0;
	int code_arr[4] = { 0 };
	int guess_arr[4] = { 0 };
	for (int i = 3; i >= 0; i--)
	{
		code_arr[i] = code % 10;
		code /= 10;
		guess_arr[i] = guess % 10;
		guess /= 10;
	}
	response currentResponse;
	//search for black
	for (int i = 0; i < 4; i++)
	{
		if (code_arr[i] == guess_arr[i])
		{
			ox++;
			code_arr[i] = -1;
			guess_arr[i] = -1;
		}
	}

	//search for white
	for (int i = 0; i < 4; i++)
	{
		if (code_arr[i] > 0)
		{
			for (int k = 0; k < 4; k++)
			{

				if (code_arr[i] == guess_arr[k])
				{
					cow++;
					guess_arr[k] = -1;
				}
			}

		}
	}
	currentResponse.black = ox;
	currentResponse.white = cow;
	return currentResponse;
};



/*the function removes from the entire solution space numbers that have different values of black and white
	from the number of black and white in the guessing test*/

void prune_codes(List* list, int currentGuess, response responsePegs)
{

	Node* current = list->head;

	while (current != NULL)
	{

		response currentResponse = checkÑode(current->data, currentGuess);
		if (currentResponse.black != responsePegs.black || currentResponse.white != responsePegs.white)
		{


			delete(current->data, list);
			current = list->head;
		}
		else current = current->next;

	}

}


typedef struct  ScoreCompare {
	int guess;
	int score;
} scoreCompare;

typedef struct ScoreCount {
	response r;
	int score;
} scoreCount;

//list length search function
int get_length(List* list)
{
	int count = 0;
	Node* current = list->head;
	while (current != NULL)
	{
		count++;
		current = current->next;
	}
	return count;
}

/*function in which the minmax algorithm is implemented for the optimal solution*/

List* min_max(List* candidates, List* combinations)
{
	int min, max;
	scoreCompare sCompare[1296];

	scoreCount sCount[15];
	int j = 5;
	int k = 0;
	for (int i = 0; i < 15; i++) {
		sCount[i].score = 0;
		if (k >= j) {
			j--;
			k = 0;
		}
		sCount[i].r.black = 5 - j;
		sCount[i].r.white = k;
		k++;
	}

	List* nextGuesses = makelist();

	int candidatesLength = get_length(candidates);
	int combinationsLength = get_length(combinations);

	Node* candidatesNode = candidates->head;
	Node* combinationsNode = combinations->head;

	for (int i = 0; i < combinationsLength; i++)
	{
		candidatesNode = candidates->head;
		for (int j = 0; j < candidatesLength; j++)
		{
			response pegScore = checkÑode(combinationsNode->data, candidatesNode->data);
			for (int k = 0; k < 15; k++) {
				if (pegScore.black == sCount[k].r.black && pegScore.white == sCount[k].r.white) {
					sCount[k].score++;
					break;
				}
			}
			candidatesNode = candidatesNode->next;
		}

		max = get_max_score(sCount);

		sCompare[i].guess = combinationsNode->data;
		sCompare[i].score = max;

		for (int k = 0; k < 15; k++) {
			sCount[k].score = 0;
		}

		combinationsNode = combinationsNode->next;
	}

	min = get_min_score(sCompare, combinationsLength);

	for (int i = 0; i < combinationsLength; i++) {

		if (sCompare[i].score == min) {
			add(sCompare[i].guess, nextGuesses);
		}
	}
	return nextGuesses;
}


int get_min_score(scoreCompare sCompare[], int length)
{
	int min = INT_MAX;

	for (int i = 0; i < length; i++) {
		if (sCompare[i].score < min) {
			min = sCompare[i].score;
		}
	}
	return min;
}

int get_max_score(scoreCount sCount[])
{
	int max = 0;

	for (int i = 0; i < 15; i++) {
		if (sCount[i].score > max) {
			max = sCount[i].score;
		}
	}

	return max;
}



int get_next_guess(List* nextGuesses, List* candidates, List* combinations)
{
	int nextGuess = 0;

	int ngLength = get_length(nextGuesses);
	int candidatesLength = get_length(candidates);
	int combinationsLength = get_length(combinations);

	Node* ngNode = nextGuesses->head;
	Node* candidatesNode = candidates->head;
	Node* combinationsNode = combinations->head;


	for (int i = 0; i < ngLength; i++) {
		candidatesNode = candidates->head;
		for (int j = 0; j < candidatesLength; j++) {
			if (ngNode->data == candidatesNode->data) {
				return ngNode->data;
			}
			candidatesNode = candidatesNode->next;
		}
		ngNode = ngNode->next;
	}

	ngNode = nextGuesses->head;
	for (int i = 0; i < ngLength; i++) {
		combinationsNode = combinations->head;
		for (int j = 0; j < combinationsLength; j++) {
			if (ngNode->data == combinationsNode->data) {
				return ngNode->data;
			}
			combinationsNode = combinationsNode->next;
		}
		ngNode = ngNode->next;
	}

	return nextGuess;
}



int main() {
	List* candidates = makelist();
	List* combinations = makelist();
	List* nextGuesses = makelist();
	int code = 0;//çàãàäûâàåìîå ÷èñëî
	bool won = false;
	srand(time(NULL));


	int starttm = clock();
	for (int i = 0; i < 4; i++)
	{
		code *= 10;
		code += 1 + rand() % 6;
	}


	/*code = 2222;*/
	/*code = 6212;*/
	/*code = 4613;*/
	/*code = 3335;*/
	/*code = 6556;*/
	printf("Code: %d \n", code);

	for (int i = 1111; i < 6667; i++)
	{
		int x = i % 10;
		if (x > 6 || x == 0)continue;
		x = i % 100;
		if (x / 10 > 6 || x / 10 == 0)continue;
		x = i % 1000;
		if (x / 100 > 6 || x / 100 == 0)continue;
		x = i;
		if (x / 1000 > 6 || x / 1000 == 0)continue;
		add(i, combinations);
		add(i, candidates);

	}


	int current_guess = 1122;
	response response_pegs;

	int turn = 0;
	while (!won)
	{


		delete(current_guess, candidates);
		delete(current_guess, combinations);


		response_pegs = checkÑode(code, current_guess);
		turn++;

		printf("Turn: %d \n", turn);
		printf("Guess: %d \n", current_guess);
		printf("Black: %d, White: %d \n", response_pegs.black, response_pegs.white);
		if (response_pegs.black == 4)
		{
			won = true;
			printf("Game Won!");
			int endtm = clock();
			printf(" %.3f", ((float)endtm - (float)starttm) / 1000);
			break;
		}

		prune_codes(candidates, current_guess, response_pegs);


		nextGuesses = min_max(candidates, combinations);

		current_guess = get_next_guess(nextGuesses, candidates, combinations);

	}
	return 0;
}


