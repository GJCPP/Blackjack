#include <memory.h>
#include <time.h>

#include <random>

#include "Blackjack.h"

int rand(int left, int right) {
    static std::random_device                  rand_dev;
    static std::mt19937                        generator(rand_dev());
    static std::uniform_int_distribution<int>  distr(left, right);
    return distr(generator);
}

void _swap(int *a, int *b) {
    if (a != b)
        *a ^= *b ^= *a ^= *b;
}

int _rand_draw(struct deck *state) {
    int ind = rand(0, state->n_undrawn - 1);
    state->n_undrawn--;
    _swap(&state->undrawn[ind], &state->undrawn[state->n_undrawn]);
    return state->undrawn[state->n_undrawn];
}

void _player_rand_draw(struct deck *state) {
    int card = _rand_draw(state);
    state->player_card[state->n_player_card++] = card;
}

void _dealer_rand_draw(struct deck *state) {
    int card = _rand_draw(state);
    state->dealer_card[state->n_dealer_card++] = card;
}

struct deck * deal_cards_1(void * _) {
    static struct deck ret;
    memset(&ret, 0, sizeof(ret));

    // Init random seed.
    srand(time(0));

    // Init deck to be full.
    ret.n_undrawn = 0;
    for (int i = 1; i <= NUM_CARDTYPE; ++i) {
        for (int j = 0; j != NUM_EACHCARD; ++j) {
            ret.undrawn[ret.n_undrawn++] = i;
        }
    }

    // Draw card for dealer and player.
    _dealer_rand_draw(&ret);
    _dealer_rand_draw(&ret);
    _player_rand_draw(&ret);
    _player_rand_draw(&ret);
    return &ret;
}

// One more card for player
struct deck * hit_1(struct deck *state) {
    _player_rand_draw(state);
    return state;
}

// Calculate the best score.
int _calculate(int card[MAXCARD], int n) {
    int available[MAX_VAL] = { 0 };
    available[0] = 1;
    for (int i = 0; i != n; ++i) {
        int val = card[i];
        for (int j = MAX_VAL - 1; j >= 0; --j) {
            if (!available[j]) continue;
            available[j] = false;
            switch (val)
            {
            case 1: // ACE
                available[j + 1] = true;
                available[j + 11] = true;
                break;

            case 11:
            case 12:
            case 13:
                available[j + 10] = true;
                break;

            default:
                available[j + val] = true;
                break;
            }
        }
    }
    int ret = WIN_SCORE;
    while (ret >= 0 && !available[ret]) --ret;
    if (ret < 0) {
        ret = WIN_SCORE;
        while (ret < MAX_VAL && !available[ret]) ++ret;
    }
    return ret;
}

int _dealer_win(int player_score, int dealer_score) {
    if (player_score > WIN_SCORE) return 1;
    if (dealer_score > WIN_SCORE) return -1;
    return dealer_score - player_score;
}

// Calculate the pr. of dealer not losing after one more drawing.
double _calculate_dealer_prob(int player_val, const struct deck *state) {
    double ret = 0;
    int card[MAXCARD], n_card = state->n_dealer_card + 1;
    memcpy(card, state->dealer_card, sizeof(card));
    for (int i = 0; i != state->n_undrawn; ++i) {
        card[n_card - 1] = state->undrawn[i];
        int dealer_val = _calculate(card, n_card);
        if (_dealer_win(player_val, dealer_val) >= 0) {
            ret += 1.0 / state->n_undrawn;
        }
    }
    return ret;
}

// Draw cards for dealer
struct deck *stand_1(struct deck *state) {
    int player_val = _calculate(state->player_card, state->n_player_card);

    // The dealer must draw until it can reach >= MIN_SCORE
    int dealer_val = -1;
    while ((dealer_val = _calculate(state->dealer_card, state->n_dealer_card)) < MIN_SCORE) {
        _dealer_rand_draw(state);
    }

    // The dealer draws until it will not lose, or it exceeds the bound.
    while (_dealer_win(player_val, dealer_val) < 0 && dealer_val < WIN_SCORE) {
        _dealer_rand_draw(state);
        dealer_val = _calculate(state->dealer_card, state->n_dealer_card);
    }

    if (_dealer_win(player_val, dealer_val) > 0 || dealer_val > WIN_SCORE) {
        // GG
        return state;
    }

    // if it's tie, calculate the probability of winning by drawing one more card.
    if (_dealer_win(player_val, dealer_val) == 0) {
        if (_calculate_dealer_prob(player_val, state) >= 0.8) {
            _dealer_rand_draw(state);
        }
    }
    return state;
}

// Compute optimal score for player
int *calculate_1(struct deck *state) {
    static int res;
    res = _calculate(state->player_card, state->n_player_card);
    return &res;
}

// Compute optimal score for dealer
int *calculate_dealer_1(struct deck *state) {
    static int res;
    res = _calculate(state->dealer_card, state->n_dealer_card);
    return &res;
}

// Return >0 if player win.
int *judge_1(struct deck *state) {
    static int ret;
    ret = - _dealer_win(
        _calculate(state->player_card, state->n_player_card),
        _calculate(state->dealer_card, state->n_dealer_card)
    );
    return &ret;
}
