## ``` TetrisGameArduino```
> Introduction to Robotics project #1: LED matrix 8x8 game

## Backstory
| ![image](https://user-images.githubusercontent.com/19687103/147365068-16a3ab76-9fa3-41f4-9915-e6e26e64be42.png) |I am Mădălin Frîncu, a CS student at Unibuc and this repository showcases my project in Arduino: a fully operating Tetris game. I have picked this game not because it's just retro but because I have a current passion for this game and I love playing Tetris with my 3D glasses for the great experience it can offer for keen minds. |
|---|---|

# Showcase
>### Main Menu
![image](https://user-images.githubusercontent.com/19687103/147365418-abdf7610-53a2-464b-a1f0-e27e41743424.png)
>### Contrast settings
![image](https://user-images.githubusercontent.com/19687103/147365597-b3bc7a17-b396-4e8d-9c13-ec2028ea90e3.png)
>### Tetris gameplay
![image](https://user-images.githubusercontent.com/19687103/147365687-657af42f-13e0-4d6f-a0ae-c4749ddfc567.png)

---------


## Video on YouTube

[2]: https://youtu.be/E9lpa9JYtPs
[1]: https://user-images.githubusercontent.com/19687103/147366010-57772f8a-3fd1-4d4e-90d3-051aca318e99.png

[![youtube link][1]][2]




# Code architecture


The following source code files are used in the project.

| File | Description |
|---|---|
| game.h | Defines game variables and game logic in a few functions |
| myLedControl.h | A copy of LedControl.h for improving speed in digitalRead() and digitalWrite() operations  |
| myLiquidCrystal.h | A copy of LiquidCrystal.h for improving speed in digitalRead() and digitalWrite() operations |
| registerIO.h | File containing functions for digital I/O on Arduino pins using registers |
| main_menu.h | File defining main menu functionalities |
| project.ino | Has the loop() and setup() functions for main program  |
| song.h | Defines a short Tetris theme song and it's used on startUp and on changeName |
| utils.h | Defines essential variables and utility functions used by other files |

|Much more code comments to be found in source code|![ezgif com-gif-maker](https://user-images.githubusercontent.com/19687103/147366983-b3af8bde-6af2-480a-9dd3-35e2e83f5e74.gif)|
|---|---|



Call graph on handleMenu() from main_menu.h called in main program. It's made by Doxygen using dot tool from Graphviz package  

![image](https://user-images.githubusercontent.com/19687103/147364069-3723d8e5-35bc-4261-a119-cad28c77aff5.png)

