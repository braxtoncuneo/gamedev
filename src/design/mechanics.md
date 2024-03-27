# Mechanics
<!-- slider row-split -->
<!-- slider web -->

Broadly, a **mechanic** is a rule governing what actions players can take and what results from said actions.
These are also called **ludemes**, since they are elements of gameplay, much as a gene is an element of genetics and a meme is an element of thought.

Since there are an infinite set of possible rules that one could define for a game, there are an infinite number of mechanics.
To classify them all would be to grapple with the infinite complexity of formal logic.

Nonetheless, there are quite a few common types of mechanics that are used in games, and being aware of these common building blocks is useful when considering the design of a game.

<!-- slider slide -->

- A **mechanic** is a rule governing what actions players can take and what results from said actions.

- There are an infinite number of possible rules, so an infinite number of mechanics.

- Still, there are categories of commonly used rules that are good to know about

<!-- slider both -->

- We will focus upon mechanics involving:
  - time
  - resources
  - information
  - winning

<!-- slider slide -->
<!-- slider cell-split -->

![](https://upload.wikimedia.org/wikipedia/commons/thumb/d/d4/The_bird_is_in_the_nest_%28Root_board_game%29.jpg/1280px-The_bird_is_in_the_nest_%28Root_board_game%29.jpg)

*[By Daniel Montoya, public domain, hosted by Wikimedia Commons](https://commons.wikimedia.org/wiki/File:The_bird_is_in_the_nest_(Root_board_game).jpg)*

<!-- slider both -->

<!-- slider split -->
## Time

<!-- slider row-split 2 -->

Time is a fundamental component of games.

<!-- slider web -->

In games, things happen. {{footnote: citation needed}}
In order for things to *happen*, some state must be effected.
If what happens next depends upon that state, it is important to determine what counts as "previous" and what counts as "next".

Hence, to have a game where things happen, there must be at least one rule (**mechanic**) that establishes this ordering.
The system of rules determining the sequencing of events in a game comprise the game's **timekeeping**.

<!-- slider slide -->

- In games, things happen {{footnote: citation needed}}.
- In order for things to *happen*, some state must be effected.
- If what happens next depends upon that state, it is important to determine what counts as "previous" and what counts as "next".

Hence, to have a game where things happen, there must be at least one rule (**mechanic**) that establishes this ordering.

The system of rules determining the sequencing of events in a game comprise the game's **timekeeping**.


<!--slider cell-split -->

<div style="width: 90%; margin: auto;">

![](https://upload.wikimedia.org/wikipedia/commons/6/62/Carriage_Clock_MET_233550.jpg)

*[Photo by the MET, public domain, hosted by Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Carriage_Clock_MET_233550.jpg)*
</div>
<!-- slider both -->

<!-- slider split -->
### Real-Time vs Turn-Based Time Keeping

<!-- slider row-split 2 -->

<!-- slider web -->
The rate at which events progress in a game does not necessarily match with the rate that events progress from the player's perspective.
In order to make this game playable, there must be a mapping between the player's time and the game's time.

One solution is to not constrain the actions of players in-game, having the ordering of their real-life actions match the ordering of actions in-game.
Such games are **real-time**.

Another solution is to divide a game into phases called **turns**, which allow a specific set of players to designate their actions in-game (potentially denying in-game actions to other players at that time).
Such games are **turn-based**.


<!-- slider slide -->
- The rate at which events progress in a game does not necessarily match with the rate that events progress from the player's perspective.
- In order to make this game playable, there must be a mapping between the player's time and the game's time.
- **real-time**: in-game actions are not constrained, with ordering of player actions in real life matching the ordering of actions in-game
- **turn-based**: divide a game into phases during which a specific set of players takes actions in-game


```admonish question tile="Question"
Can you provide examples of games that don't fit cleanly into this binary?
```
<!-- slider cell-split -->

<div style="width: 90%; margin: auto;">

![](https://upload.wikimedia.org/wikipedia/commons/6/62/Carriage_Clock_MET_233550.jpg)

*[Photo by the MET, public domain, hosted by Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Carriage_Clock_MET_233550.jpg)*
</div>

<!-- slider both -->
<!-- slider split -->

### Simultaneous vs Sequential Time Keeping

<!-- slider row-split 2 -->

<!-- slider web -->
Actions must have an ordering if the side effects of one action can change the effects/correctness of the other action.
However, if two actions will produce different results depending upon order, they can be performed simultaneously.

Games that overlap actions have **simultaneous** time keeping.
In cases where an ordering is enforced upon actions, such games have **sequential** time keeping

<!-- slider slide -->
- Actions must have an ordering if the side effects of one action can change the effects/correctness of the other action.
- However, if two actions will produce different results depending upon order, they can be performed simultaneously.
- Games that overlap actions have **simultaneous** time keeping.
- In cases where an ordering is enforced upon actions, such games have **sequential** time keeping


```admonish question tile="Question"
Can you provide examples of games that don't fit cleanly into this binary?
```
<!-- slider cell-split -->

<div style="width: 90%; margin: auto;">

![](https://upload.wikimedia.org/wikipedia/commons/6/62/Carriage_Clock_MET_233550.jpg)

*[Photo by the MET, public domain, hosted by Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Carriage_Clock_MET_233550.jpg)*
</div>

<!-- slider both -->
<!-- slider split -->

### Additional Considerations
<!-- slider row-split 2 -->

Some games that enforce ordering through turns may not necessarily have a static cycle of turns with pre-determined sets of players.
In some cases, players can perform actions that:
- modify the ordering of turns
- add additional turns
- remove pre-existing turns
- add or remove players from specific turns

<!-- slider slide -->
```admonish question
Can you name another way conventional turn structure may be defied?
```
<!-- slider cell-split -->

<div style="width: 90%; margin: auto;">

![](https://upload.wikimedia.org/wikipedia/commons/6/62/Carriage_Clock_MET_233550.jpg)

*[Photo by the MET, public domain, hosted by Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Carriage_Clock_MET_233550.jpg)*
</div>

<!-- slider both -->
<!-- slider split -->
## Resources
<!-- slider row-split -->

Broadly, resources are things with a limited supply associated with some entity (owner/occupant/consumer) which allows that entity to perform some action.

Under this broad definition, most of the mechanics that make up a game are resources.
In fact, *turns themselves* could be considered a resource, since they enable actions for some players.

By tying capability to entities that may or may not be held, mechanics can act upon those entities, forming "higher-order" mechanics that regulate the application of other mechanics.


<!-- slider cell-split -->

<!-- slider split -->

### Scarcity

<!-- slider row-split -->

In order for resources to be meaningful, there must be some mechanics that limit what resources can be held when.
If any player could acquire or drop any resources at any time, then the associated capabilities are effectively always available.

Scarcity could be a limitation upon the total number of units of a resource in a game.
Alternatively, it could be a limitation upon the rate at which resources may be produced, or some requirement on when they may be produced.

<!-- slider cell-split -->

<!-- slider split -->

### Barter/Trade

<!-- slider row-split -->

There may be circumstances where a pair of players each hold resources that they value less than thes other player does.

A game may allow both players to exchange these items, allowing both player to experience a gain in perceived value without any explicit production of work.


<!-- slider cell-split -->

<!-- slider split -->
### Bidding

<!-- slider row-split -->

Bidding is essentially a more competitive form of trade, allowing players to fight over a resource at the cost of another resource.

<!-- slider cell-split -->

<!-- slider split -->
### Units of Action vs Units of Action Throughput

<!-- slider row-split -->

Resources may be expended once used to perform an action, or they may remain after use.
In the latter case, such a resource does not represent a single unit of action, but actually a certain measure of action throughput.

The genre of **worker placement** games focuses upon this type of mechanic, with players managing the allocation of action throughput across a set of tasks.
This allows for interesting higher-order play, where the acquisition of these resources doesn't correspond with the zero-order derivative of possible actions, but a first-order or higher derivative.

<!-- slider cell-split -->

<!-- slider split -->

### Space/Topology

<!-- slider row-split -->

If resources have a sense of adjacency, where holding one resource allows a player to hold an "adjacent" resource (with or without relinquishing the original), those resources form a sort of space or topology.
In this sense, the squares on a chessboard could be considered a resource, with movement being the trading of one resource by a piece in exchange for another resource.
Without such an "exchange", both resources remain held, creating a pattern of expansion.

<!-- slider cell-split -->

<!-- slider split -->


## Information

<!-- slider row-split -->

Information, in and of itself, usually does not enable specific actions within a game.
For example, if an action required a player to accurately answer what number a person was thinking, a player could still fulfill this requirement by being lucky and guessing the correct number.
However, if a player *did* somehow know what this number was ahead of time, or if they knew of some way to narrow the space of possibilities, it would be much more likely for the player to succeed.
In both cases, the action of selecting the correct number is always possible, but the certainty of success is much different.

In this respect, information doesn't exactly enable actions, but rather helps players select which actions to take.
Hence, the mechanics that govern what information is available to players is integral to gameplay.

<!-- slider cell-split -->

<!-- slider split -->
### Perfect Information and Complete Information

<!-- slider row-split -->
**Perfect information** is a condition where all aspects of a game's state are known to all parties.
A classic example of this is chess, where the positions of all pieces are known to both players at all times.

Perfect information allows players to act with more certainty, but the outcome of any given play may still be in doubt.
If another player has an unknown objective, it would be difficult to determine how the game's state may progress.

In contrast, **complete information** is a condition where knowledge about all relevant objectives, strategies, etc. are known to all parties.
Hence, if a game has imperfect information but complete information, players may not know the state of the board, but they would be able to perfectly predict future moves if they did.
In cases where a video game has a very simple AI opponent, one may find themselves in a game of complete information.
If one understands the mechanics behind this AI, one can subvert their strategy, deriving advantage.

<!-- slider cell-split -->

<!-- slider split -->
### Chance

<!-- slider row-split -->
In cases where there is information that no parties know about, there is **chance**.
Classically, when actions are randomly determined in a game, these actions are considered to be "rules by nature", where "nature" is some entity with an unpredictable objective.

<!-- slider cell-split -->

<!-- slider split -->
### Screening

<!-- slider row-split -->

When a game has different objectives between players involves at least one player that benefits from dishonesty, it is considered a **screening game**.
A classic example of this is Among Us, where a few players seek to kill off all other players in the game, but the other players can vote to eliminate potential killers.

<!-- slider cell-split -->

<!-- slider split -->
### Signaling

<!-- slider row-split -->

In contrast to screening games, there are **signaling games**, where there is imperfect/incomplete information and at least one player can reveal hidden information to other players for some benefit.
An example of this is charades, where the silent player's objective is to get the other players to guess the correct term without speaking.

<!-- slider cell-split -->

<!-- slider split -->
## Winning

<!-- slider row-split -->

Generally, games require some objective for players to pursue.
Some of these objectives may be intrinsic, derived by the inherent interests of the player (i.e. why do players build things in Minecraft?).
However, some objectives are usually provided explicitly by a game (e.g. getting the highest score, or winning).

Needless to say, the objectives given to players by mechanics shape gameplay by effecting their behavior.

<!-- slider cell-split -->

<!-- slider split -->
### Zero/Positive-Sum Games

<!-- slider row-split -->

Games where the net benefit of each action across all players is always zero is called a zero-sum game.
Chess is an example of a zero-sum game, since winning is mutually exclusive across both players.
Any move that makes it easier for one player to win would necessarily make it more difficult for the other to win.

In contrast, games that allow for both players to gain a net-positive benefit from an action are called positive-sum games.

<!-- slider cell-split -->

<!-- slider split -->

<!--
### Victory Conditions
-->
<!-- slider row-split -->

<!-- slider cell-split -->

<!-- slider split -->







