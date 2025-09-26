# Snake Game

A classic Snake game implemented in C++ using the SDL2 library for graphics and input.

## Features

- Classic snake gameplay
- Simple graphics powered by SDL2
- Score tracking
- Game over and restart mechanics

## Requirements

- **SDL2 library**  
  You need to have the SDL2 library installed on your system to build and run this game.

### Installing SDL2

#### On Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install libsdl2-dev
```

#### On macOS (with Homebrew)

```bash
brew install sdl2
```

#### On Windows

- Download and install SDL2 from the [SDL2 Downloads page](https://www.libsdl.org/download-2.0.php)
- Follow the instructions for setting up SDL2 with your compiler/IDE.

## Building the Game

1. Clone the repository:

   ```bash
   git clone https://github.com/1maciek90/Snake-game.git
   cd Snake-game
   ```

2. Compile the code (example for g++):

   ```bash
   g++ -o snake_game main.cpp -lSDL2
   ```

   > Replace `main.cpp` with your actual source file(s) if different.

3. Run the game:

   ```bash
   ./snake_game
   ```

## Gameplay

- Use the **arrow keys** to control the direction of the snake.
- Eat food to grow and increase your score.
- Avoid hitting the walls or yourself.

## Screenshots

_(Add screenshots here if available)_

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for more information.

## Author

- [1maciek90](https://github.com/1maciek90)
