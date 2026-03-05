# Binary Blitz

**Game Jam Dates:** March 6–7  
**Genre:** Lane Strategy / Auto-Battler  
**Camera:** Isometric  
**Players:** 1 (Player vs AI)

---

# Game Design Document (GDD)

## Overview

**Binary Blitz** is a fast-paced lane strategy game where players deploy units that automatically march toward the enemy base across three lanes. Units fight enemies and defensive structures along the way.

Players earn **coins from defeated enemies**, which can be used to deploy more units or **build defensive towers**.

The battlefield is split into two halves. Players can **only place units and towers on their side of the map**, forcing strategic decisions on lane pressure and defense.

The objective is to **destroy the enemy base before yours is destroyed**.

---

# Camera & Perspective

The game uses an **isometric camera view**.

Benefits:
- Clear overview of the three lanes
- Easy visualization of unit movement
- Simple placement of towers and units

Camera characteristics:
- Fixed angle
- Slight zoom out to view most of the battlefield
- Minimal camera movement (to keep scope small for the game jam)

---

# Core Gameplay Loop

1. Player starts with a small amount of **coins**
2. Player **places units** in one of the three lanes on their side of the map
3. Units **automatically move toward the enemy base**
4. Units **fight enemy units and towers**
5. Defeated units **drop coins**
6. Player collects coins and **spawns more units or builds towers**
7. Strategic pressure across lanes leads to **base destruction**

---

# Map Layout

The battlefield contains **three lanes**:

- Top Lane  
- Mid Lane  
- Bottom Lane  

Each player controls **one half of the battlefield**.

```
Player Side Enemy Side

Player Base → Lane → Lane Center → Lane → Enemy Base
```

Each side contains:

- Base
- Tower build zones
- Unit spawn zones

---

# Player Placement Rules

## Unit Placement

- Units can **only be spawned on the player's side of the map**
- Units are spawned into a **selected lane**
- Once spawned, units **automatically march forward**

## Tower Placement

- Towers can be **placed anywhere on the player's side**
- Towers **cannot be moved once placed**
- Towers automatically attack enemies in range

---

# Structures

## Base

Main objective structure.

Properties:

- Very high health
- Stationary
- Game ends when destroyed

---

## Towers

Defensive structures built by players.

Properties:

- Stationary
- Automatically attack enemies in range

Example stats:

| Stat | Value |
|-----|------|
| HP | Medium |
| Attack | Medium |
| Range | Medium |
| Cost | 6 Coins |

---

# Unit Types

Units automatically move down lanes and attack enemies.

---

## Light Unit

Fast swarm unit.

| Stat | Value |
|-----|------|
| HP | Low |
| Damage | Low |
| Speed | Fast |
| Cost | 2 Coins |

Purpose:
- Lane pressure
- Farming coins

---

## Medium Unit

Balanced fighter.

| Stat | Value |
|-----|------|
| HP | Medium |
| Damage | Medium |
| Speed | Medium |
| Cost | 4 Coins |

---

## Heavy Unit

Tank unit.

| Stat | Value |
|-----|------|
| HP | High |
| Damage | High |
| Speed | Slow |
| Cost | 6 Coins |

Purpose:
- Break towers
- Absorb damage

---

## Flying Unit

Air unit that bypasses ground congestion.

| Stat | Value |
|-----|------|
| HP | Low |
| Damage | Medium |
| Speed | Medium |
| Cost | 5 Coins |

Special Traits:

- Ignores ground obstacles
- Moves directly toward enemy base

---

# Combat System

Units and towers automatically attack enemies.

**Target priority:**

1. Enemy units  
2. Enemy towers  
3. Enemy base  

Combat mechanics:

- Units attack on cooldown
- When HP reaches zero → unit dies
- Dead units **drop coins**

---

# Resource System

## Coins

Coins are the main resource.

**Sources:**

- Killing enemy units

**Used for:**

- Spawning units
- Building towers

---

# Player Controls

Mouse-driven controls.

Actions:

- Select unit type
- Select lane
- Spawn unit
- Select tower placement location
- Build tower

Example UI layout:
```
Units
[Light] [Medium] [Heavy] [Flying]

Structures
[Tower]

Lanes
[Top] [Mid] [Bottom]
```

---

# Win & Lose Conditions

## Win

Enemy **Base HP = 0**

## Lose

Player **Base HP = 0**

---

# Visual Style

Game jam friendly style:

- Minimalist models
- Clear silhouettes
- Distinct unit colors
- Simple tower designs
- Visible lane markings

Example colors:

- Player: **Blue**
- Enemy: **Red**

---

# Scope for Game Jam

## Must Have

- Isometric view
- 3 lanes
- 4 unit types
- Player-side unit placement
- Player-built towers
- Coins from defeated units
- Unit auto movement and combat
- Base destruction win condition

---

## Nice To Have

- Simple AI opponent
- Unit spawn cooldown
- Sound effects
- Basic animations
- Tower placement grid

---