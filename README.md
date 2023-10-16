# minesweeper
A simple minesweeper game made with WASM and C.

All the game logic is written in C and compiled to WASM using emscripten.

To build the transactional version you can play in your terminal simply run:
```bash
  make
  output/minesweeper
```

To build the WASM version simply run:
```bash
  make wasm
```
The files you need to host are located under `output/www`.

A version of the WASM game is hosted on https://mine.toastyfiles.com
