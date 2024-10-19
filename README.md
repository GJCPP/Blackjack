## Blackjack

A terminal game for Blackjack, written in C++.

## Game Rule

There is a deck without Joker. Cards are separated into three types:

1. Number Cards (2-10). The value of the card equals to its face value.
2. Face Cards (Jack, Queen, King), each is of value $10$.
3. Ace, which can be either $1$ or $11$ points, depending on which is more beneficial for the player's hand.

The game flow is as follows.

1. At the beginning, the player and dealer are dealt two cards. Player's cards are face up, while the dealer has one card face up and one card face down.
2. Player then chooses either to hit, double bet or stand. If it chooses to hit, it draws another card. If it chooses to double bet, it doubles the bet and hits. The player can continue to hit until it chooses to stand or his hand exceeds $21$ (called "busting").
3. Dealer reveals its face-down card, and must hit until his hand total $17$ or higher.

When the game finishes,

- If the player's hand exceeds $21$, it busts and loses.
- If the dealer busts, the player wins.
- If neither busts, the hand closer to $21$ wins.
- It is a tie otherwise.

## Have Fun!