# AMPS
CENG/ELEC 299 Design Project

## Objective

Create a line following robot to solve a maze autonomously, this was accomplished by:

- Designing and manufacturing our own H-Bridge circuit (N-Channel Bootstrapped circuit was our design)
- Developing infrared (IR) circuit to detect the electrical black tape vs white table
- Develop an algorithm to solve the maze and implement using an Arduino Leonardo

## Outcomes

All software, circuits and creative ideas were developed 100% by:
- Ross Jespersen (Electrical Engineering) 
- Maitreya Panse (Electrical Engineering) 
- Marc Laventure (Computer Engineering) 

Our robot solved the maze, as seen below, at the three labelled starting points, in the fastest time in our class.
We recieved 100% on the final demo coupled with the average of 16.33 seconds to solve the maze (next lowest average was 47 seconds)

*Please note drawing not to scale and starting points are labelled 1, 2 and 3 respectively*
``` 
                            --------
                            |FINISH|
                            --------
                                |
          2                     |
          |                     |
1         |                     |
|         |-----------          |
|         |          |          |
|         |          |          |
|----------          |          |
|                    |-----------
|                    |
                     |
                     |
                     3
```
## What We Would Do Differently Next Time

- 6 sensor config, same 5 line we used however have a leading middle sensor
- Place the sensors in the middle of robot vs front, allows for smooth pivoting
- Different gear ratio but we haz trouble meching. [Sums up us](http://1.bp.blogspot.com/-elgq8JTN02A/UOjOnOPqATI/AAAAAAAAAWc/DAbHj8jiQ_Q/s1600/197125_429131937141818_620085953_n.jpg)
- A hardware solution for the IR circuit so we dont have to poll analog signals, this can be done using schmitt triggers
- More coffee

Thanks for a great term and see you on the course Leonard
