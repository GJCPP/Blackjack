#pragma once
#define MAXCARD 52
#define NUM_CARDTYPE 13
#define NUM_EACHCARD 4
#define MAX_VAL 100
#define WIN_SCORE 21
#define MIN_SCORE 17

struct deck {
	int player_card[MAXCARD];
	int n_player_card;
	int dealer_card[MAXCARD];
	int n_dealer_card;
	int undrawn[MAXCARD];
	int n_undrawn;
};
typedef struct deck deck;
