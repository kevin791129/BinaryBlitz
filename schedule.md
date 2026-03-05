# Programmer Schedule (1 Developer)

Estimated total dev time: **16–20 hours**

Goal: **Playable by end of Day 1**

---

# Day 1 — Core Gameplay

## Hour 1 — Project Setup

Tasks:

- Create project
- Setup isometric camera
- Create map with 3 lanes
- Add placeholder assets

Result:

Playable scene with camera and map

---

## Hour 2 — Unit System

Tasks:

- Create base Unit class
- Variables:
  - HP
  - Damage
  - Speed
  - Attack range
  - Attack cooldown
- Simple forward movement

---

## Hour 3 — Lane System

Tasks:

- Define 3 lane paths
- Spawn units at lane start
- Units follow lane direction

---

## Hour 4 — Basic Combat

Tasks:

- Detect enemies in range
- Stop movement when attacking
- Attack on cooldown
- Apply damage

---

## Hour 5 — Coin System

Tasks:

- Unit death drops coins
- Player coin counter
- Automatic coin collection

---

## Hour 6 — Unit Spawning

Tasks:

- UI buttons:
  - Light
  - Medium
  - Heavy
  - Flying
- Spawn units in selected lane
- Deduct coins

Result:

Core gameplay loop complete

---

## Hour 7 — Enemy AI

Simple AI logic:

- Every few seconds:
  - Spawn random unit
  - Random lane

---

## Hour 8 — Towers

Tasks:

- Tower placement on player side
- Tower attack logic
- Tower cost

---

## Hour 9 — Base & Win Condition

Tasks:

- Base HP
- Units attack base
- Game over conditions

---

# End of Day 1 Goal

Playable prototype with:

- Unit spawning
- Unit combat
- Coins
- Towers
- AI enemy
- Base destruction

---

# Day 2 — Polish & Improvements

## Hour 10 — Flying Units

- Ignore ground collision
- Move over units

---

## Hour 11 — Improve AI

- Basic lane balancing
- Spawn based on available coins

---

## Hour 12 — Tower Placement UX

- Placement preview
- Prevent placement in enemy half
- Grid snapping

---

## Hour 13 — UI Improvements

Add:

- Coin counter
- Unit cost display
- Base HP
- Lane labels

---

## Hour 14 — Visual Feedback

Add:

- Attack animations
- Hit flashes
- Death effects

---

## Hour 15 — Balance Pass

Adjust:

- Unit HP
- Damage
- Tower stats
- Coin drops

Target match length: **2–4 minutes**

---

## Hour 16 — Bug Fix & Build

Tasks:

- Fix major bugs
- Export final build
- Add title screen

---

# Stretch Goals

Only attempt if time remains:

- Multiple tower types
- Unit abilities
- Sound effects
- Better enemy AI
- Improved visuals