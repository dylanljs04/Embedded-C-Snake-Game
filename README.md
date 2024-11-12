# Embedded-C-Snake-Game

## Final result
### Below is a video of the live demonstration of the snake game

https://github.com/user-attachments/assets/97747642-c8e7-4d5e-b063-bff88730635d

# Snake Game on STM32 Nucleo F401RE

## Project Overview
This project is a classic Snake game implemented in Embedded C for the STM32 Nucleo F401RE microcontroller. The game runs on a 7x11 LED matrix screen and is controlled using an analog stick. The game includes several interactive features, providing an engaging experience for the player.

## Components and Functionality
### 1. **STM32 Nucleo F401RE Microcontroller**
- **Role**: The core of the project, where the game logic and control code is executed.

### 2. **7x11 LED Matrix Screen**
- **Purpose**: Displays the snake, food, and game events in real-time.
- **Functionality**: Visualizes the movement of the snake, allowing the player to control and interact with the game environment.

### 3. **Analog Stick Controller**
- **Purpose**: Used to control the movement of the snake.
- **Features**:
  - **Single Click**: Speeds up the snake.
  - **Double Click**: Pauses the game.
  - **Directional Control**: Moves the snake up, down, left, or right on the LED matrix.

### 4. **External LED Indicator**
- **Purpose**: Provides additional feedback to the player.
- **Functionality**: Lights up when the game is over, signalling that the player has lost.

## Game Features
- **Speed Control**: The snake’s speed can be increased by pressing the analog button once, adding an extra level of challenge.
- **Pause Function**: Double-clicking the analog button pauses the game, allowing the player to take a break.
- **Game Over Indicator**: An external LED lights up when the snake hits itself or the boundary, signalling that the game is over.

## How to Set Up and Run the Game
1. **Assemble the Hardware**:
   - Connect the STM32 Nucleo F401RE microcontroller to the 7x11 LED matrix and the analog stick controller.
   - Attach the external LED to the microcontroller as per the wiring diagram provided.

2. **Upload the Code**:
   - Use an IDE such as STM32CubeIDE to compile and upload the Embedded C code to the STM32 Nucleo board.

3. **Play the Game**:
   - Use the analog stick to control the direction of the snake.
   - Click the analog button to speed up the snake, or double-click to pause the game.
   - When the game ends, the external LED will light up to indicate a game over.

## Repository Contents
- **Source Code**: Contains the complete code for the game logic and control functions.
- **Schematics**: Wiring diagrams for connecting the STM32 microcontroller with the LED matrix, analog stick, and external LED.
- **README.md**: This file, providing an overview and setup instructions for the project.
