# RL-Based Othello Opponent (C Implementation)

This project implements a **Reinforcement Learning-based Othello opponent** fully in **C**, with an  **SDL-based GUI**. It combines self-play, neural networks, and strategic reward shaping to train an AI capable of playing Othello (6x6) against human players.

---

## 🧠 Features

- **Q-learning with a neural network**
- **Depth:** 1-move lookahead for Q-value updates
- **Reward Function:** Based on a **mobility strategy**

---

### Mobility Strategy

The goal of this strategy is to maximize the number of possible moves (mobility) while minimizing the opponent’s moves.
It also values controlling the corners of the board more in the midgame because corners are stable and give a strong positional advantage.
In the endgame, the strategy focuses more on the difference in mobility.

---

### Position Strategy

This strategy evaluates the board based on the value of each piece's position. It favors occupying strong squares (like corners and edges) and avoids weak squares (like spots near corners that are easily flipped).
During the endgame, it simply counts the difference in the number of pieces between the player and the opponent, focusing on piece advantage.

---

## Neural Network to predict Q-values
- **Input:** 36 values (`+1` for own piece, `-1` for opponent, `0` for empty)
- **Hidden Layer:** 44 `tanh` units
- **Output Layer:** 36 `tanh` units → Q-values for each board position


#### ♟️ Phase 1: Self-Play and Train
- AI learns by playing against itself
- Q-values predicted using 1-move lookahead
- Reward based on **mobility strategy**

#### ♟️ Phase 2: Play against Position Strategy and Train
- Opponent uses **position strategy**:
- AI is trained against this standard strategy

---

## 🖥️ GUI

A basic GUI is implemented using **SDL** to render the board and let humans play against the trained agent.

> ✅ SDL must be installed for GUI support.

---

### How to Play

[Othello Rules](https://www.worldothello.org/about/about-othello/othello-rules/official-rules/english)

---

## Demo

<video src='demo/trial_othello_game.mp4' width=180/> 

---

## 🚀 How to Run

### 1. Clone the repository

```bash
git clone https://github.com/SohamW101/RL-based-opponent-in-Othello.git
cd RL-based-opponent-in-Othello/c_implementation
```

### 2. Run commands

For training:
```bash
gcc -g q_learning.c -o q_learning $(sdl2-config --cflags --libs) -lm
./q_learning
```
To play:
```bash
gcc -g GUI_impl.c -o GUI_impl $(sdl2-config --cflags --libs) -lm
./GUI_impl
```

---

