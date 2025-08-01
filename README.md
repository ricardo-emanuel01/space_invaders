# Space Invaders Clone using Raylib in C

## Win condition
The player wins when all *slow aliens* and *fast aliens* are destroyed.

## Lose condition
The player loses when aliens reach the ship's level.

## Player
Controls a ship that can move left or right and fire a single bullet upward. While under the **double shot (not implemented)** buff, the ship can fire two bullets at once (increasing it hit box horizontally).
The delay between shots is initially **0.5 seconds**, but with the **fast shot** buff it is reduced to **0.1 seconds**.

## Enemies
### Slow Alien
Has an initial **1-second** delay between shots. Drops a **fast shot** buff with **30%** drop rate.
### Fast Alien
Has an initial **0.5-second** delay between shots. Drops a **fast shot** buff with a **30%** drop rate.
### Alien Ship (Not implmented)
Appears occasionally and fires a rapid sequence of bullets until the player destroys it or it retreats. Drops both **double shot** and **fast shot** buffs when destroyed.

## AI
### Slow and Fast Aliens
If an alien is able to shoot and the player passes through its **x position**, it will fire.

### Alien Ship
Fires multiple bullets while moving left (or right) and retreating, continuing until it either retreats completely or is destroyed.

### Credits
- [Master484](https://opengameart.org/content/shiny-and-cute-invaders): ship, bullets and aliens.
- [Gundatsch](https://opengameart.org/content/raining-bits): background music.
- [spuispuin](https://opengameart.org/content/won-orchestral-winning-jingle): victory jingle.
- [Little Robot Sound Factory](https://opengameart.org/content/8-bit-sound-effects-library): defeat jingle.
- [setaiso](https://opengameart.org/content/space-ships-sounds-and-background-music): ship and enemy fire and explosion.
- [rubberduck](https://opengameart.org/content/50-cc0-retro-synth-sfx): defeat jingle.
