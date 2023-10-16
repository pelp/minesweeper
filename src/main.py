#!/bin/env python3
import pygame
from enum import Enum
import subprocess
import os

class State(Enum):
    BLANK = 0
    ONE = 1
    TWO = 2
    THREE = 3
    FOUR = 4
    FIVE = 5
    SIX = 6
    SEVEN = 7
    EIGHT = 8
    MINE = 9
    COVERED = -1
    FLAG = -2
    def __str__(self):
        if self == State.BLANK:
            return ""
        elif self == State.ONE:
            return "1"
        elif self == State.TWO:
            return "2"
        elif self == State.THREE:
            return "3"
        elif self == State.FOUR:
            return "4"
        elif self == State.FIVE:
            return "5"
        elif self == State.SIX:
            return "6"
        elif self == State.SEVEN:
            return "7"
        elif self == State.EIGHT:
            return "8"
        elif self == State.MINE:
            return "M"
        elif self == State.FLAG:
            return "F"
        elif self == State.COVERED:
            return "#"

pygame.init()
SCREEN_WIDTH = 1280
SCREEN_HEIGHT = 720
FPS = 60
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
clock = pygame.time.Clock()
screen.fill((0, 0, 0))
running = True
width, height = 30, 30
mines = 99

SIDE_LEN = min(SCREEN_WIDTH / width, SCREEN_HEIGHT / height)
LEFT = (SCREEN_WIDTH - SIDE_LEN * width) / 2
TOP = (SCREEN_HEIGHT - SIDE_LEN * height) / 2
MARGIN = 4

state = []
for i in range(height):
    state.append([])
    for j in range(width):
        state[i].append(State.COVERED)

# Connect to server and setup game
server = subprocess.Popen(["./minesweeper"], stdin=subprocess.PIPE,
                          stdout=subprocess.PIPE, universal_newlines=True,
                          bufsize=1)

server.stdin.write(f"{mines} {width} {height}\n")
while True:
    line = server.stdout.readline().strip()
    print(line)
    if "OK" in line:
        break

def draw(state):
    """Handle drawing the game."""
    font = pygame.font.SysFont("Arial", int(SIDE_LEN))

    for i, row in enumerate(state):
        for j, col in enumerate(row):
            x = LEFT + j * SIDE_LEN
            y = TOP + i * SIDE_LEN
            pygame.draw.rect(screen, (255, 255, 255),
                             (x, y, SIDE_LEN - MARGIN, SIDE_LEN - MARGIN))
            text = font.render(str(state[i][j]), False, (0, 0, 0))
            screen.blit(text, (x, y))

def handle(state, coord, mouse_btn):
    """Handle game logic."""
    sx, sy = coord
    x = (sx - LEFT) // SIDE_LEN
    y = (sy - TOP) // SIDE_LEN
    mouse1, mouse2, mouse3 = mouse_btn
    # Guard against clicks outside game area
    if x < 0 or x >= width or y < 0 or y >= height:
        return

    # Send info to server
    if mouse1:
        server.stdin.write(f"P {x} {y}\n")
    elif mouse2:
        server.stdin.write(f"PRINT\n")
    elif mouse3:
        server.stdin.write(f"F {x} {y}\n")
    else:
        return

    # Read back info from server
    for line in server.stdout:
        line = line.strip()
        try:
            val, x, y = line.split()
            x = int(x)
            y = int(y)

    # Update state
            if val == "F":
                state[y][x] = State.FLAG
            elif val == "C":
                state[y][x] = State.COVERED
            else:
                state[y][x] = State(int(val))
        except ValueError:
            if line == "L":
                return 1
            if line == "W":
                return 2

        if line == "END":
            break
    return 0

# Declare static variables

# Run setup
draw(state)
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            x, y = pygame.mouse.get_pos()
            mouse1, mouse2, mouse3 = pygame.mouse.get_pressed()
            status = handle(state, (x, y), (mouse1, mouse2, mouse3))
            if status == 1:
                print("LOST")
            elif status == 2:
                print("WON")
            draw(state)
    pygame.display.flip()
    clock.tick(FPS)
