#include <stdio.h>
#include <ctime>

#include <chrono>
#include <thread>

#include "Blackjack.h"
#include "Blackjack_proc.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

const char cardname[NUM_CARDTYPE + 1][10] = {
    "",
    "Ace",
    "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "Jack",
    "Queen",
    "King"
};

void print_cards(const int cards[MAXCARD], int n) {
	if (n == 0) return;
        printf("%s", cardname[cards[0]]);
    for (int i = 1; i != n; ++i) {
        printf(", %s", cardname[cards[i]]);
    }
}

void sleep(int t) {
    std::this_thread::sleep_for(std::chrono::seconds(t));
}

#define BAR_LENGTH 100
void print_money(int player_money, int dealer_money) {
    int tot_money = player_money + dealer_money;
    printf("Your money v.s. dealer's money: %d / %d\n", player_money, dealer_money);
    static char bar[BAR_LENGTH + 1];
    int left = MIN(BAR_LENGTH - 1, BAR_LENGTH * (1.0 * player_money) / tot_money);
    for (int i = 0; i <= left; ++i) bar[i] = '-';
    for (int i = left; i < BAR_LENGTH; ++i) bar[i] = '=';
    printf("Dealer HP: %s\n", bar);
}

void print_info(const struct deck *state, int score, int bet) {
    printf("Your cards: ");
    print_cards(state->player_card, state->n_player_card);
    printf("\n");
    printf("Your present optimal score is %d\n", score);
    printf("Current bet: %d\n", bet);
}

int main(int argc, char** argv) {
    struct deck state;
    int score;
    int player_money = 20, dealer_money = 80;
    int lose = 0, tot = 0;
    while (player_money > 0 && dealer_money > 0) {
        state = *deal_cards_1(NULL);
        score = *calculate_1(&state);
        int bet = 4, giveup = false;
        print_money(player_money, dealer_money);
        printf("Dealer's cards: %s, ?\n", cardname[state.dealer_card[0]]);
        print_info(&state, score, bet);
        while (score <= WIN_SCORE) {
            char cmd[10];
            
            printf("Would you like to hit/double-bet/stand/give up? (h/d/s/g)\n");
			int stop = 0;
			while (true) {
				scanf("%s", cmd);
				if (cmd[0] == 'h' || cmd[0] == 'H') {
					state = *hit_1(&state);
					score = *calculate_1(&state);
				} else if (cmd[0] == 'd' || cmd[0] == 'D') {
					bet = bet * 2;
					state = *hit_1(&state);
					score = *calculate_1(&state);
				} else if (cmd[0] == 's' || cmd[0] == 'S') {
					stop = true;
				} else if (cmd[0] == 'g' || cmd[0] == 'G') {
					giveup = true;
					stop = true;
				} else {
					printf("Please take one of four actions!\n");
					continue;
				}
				break;
			}
			if (stop) break;
            print_info(&state, score, bet);
        }
        if (giveup) {
            printf("You gived up, lost %d coins!\n", (bet + 1) / 2);
            sleep(3);
            player_money -= (bet + 1) / 2;
            dealer_money += (bet + 1) / 2;
            printf("\n\n\n");
            continue;
        }
        printf("Dealer's turn!\n");
        sleep(2);
        printf("Dealer reveals its card, and holds ");
		print_cards(state.dealer_card, state.n_dealer_card);
		printf("!\n");
        state = *stand_1(&state);
        sleep(2);
        for (int i = 2; i < state.n_dealer_card; ++i) {
            printf("The dealer draws another card, and holds ");
			print_cards(state.dealer_card, i + 1);
			printf("!\n");
            sleep(2);
        }
        printf("The dealer stands. It is now holding ");
        print_cards(state.dealer_card, state.n_dealer_card);
        printf("\n");
        printf("The score for dealer is %d\n", *calculate_dealer_1(&state));
        int judgement = *judge_1(&state);
        if (judgement > 0) {
            printf("Congratulations! You win and gain %d coins!\n", 2 * bet);
            player_money += 2 * bet;
            dealer_money -= 2 * bet;
        } else if (judgement == 0) {
            printf("Wow, it's a tie! No gain no lost!\n");
        }
        else { 
            printf("What a pity! You lose %d coins!\n", bet);
            player_money -= bet;
            dealer_money += bet;
            ++lose;
        }
        ++tot;
        printf("Your losing rate is current %d/%d = %f\n", lose, tot, lose/1.0/tot);
        sleep(3);
        printf("\n\n\n");
    }
	print_money(player_money, dealer_money);
	if (player_money < 0) {
		printf("Sadly, you go home bare handed...\n");
		printf("Good luck next time!\n");
	}
	else if (player_money == 0) {
		printf("Well, at least you go home with your pants still your own...\n");
		printf("Good luck next time!\n");
	}
	else if (dealer_money <= 0) {
		printf("Impressive! You have the dealer beaten!\n");
		printf("Good luck next time for the dealer, LOL!\n");
	}
    sleep(10);
    return 0;
}