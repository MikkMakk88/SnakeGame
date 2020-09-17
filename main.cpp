#include <iostream>
#include <ncurses.h>
using namespace std;

// declarations
bool gameOver, restart;
int x, y, fruitX, fruitY, score;
int gameSpeed;
int tailX[100], tailY[100];
enum eDirection {STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirection dir;

// parameters
#define width 12
#define height 12
#define gameSpeedInitial 200 - (width * height / 10)
#define gameSPeedModifier 2
#define gameSpeedCap 20

/* Features & bugs
    B: Currently the fruit can still spawn inside the tail
    F: add a 1 frame buffer to allow the player to steer away from a collision before triggering game over
    F: add a wrapping option
*/


void setup(bool startup)
{
    // initialize and clear the screen, disable line-buffering, hide cursor etc.
    if(startup)
    {
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    }

    gameOver = false;
    restart = true;
    gameSpeed = gameSpeedInitial;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
}

void draw()
{
    /* playing field should look something like this
      j(x)>
    i   #########
    (y) #       #
    v   #       #
        #       #
        #########

    */

    clear();
    // print the playing field
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            // print top and bottom walls
            if (i == 0 || i == height - 1)
            {
                mvprintw(i, j*2, "# ");
            } 
            // print side walls
            else if (j == 0 || j == width - 1)
            {
                mvprintw(i, j*2, "# ");
            } 
            // print fruit
            else if (j == fruitX && i == fruitY)
            {
                mvprintw(i, j*2, "* ");
            } 
            // print snake head
            else if (i == y && j == x)
            {
                mvprintw(i, j*2, "@ ");
            } 
        }
    }
    // draw tail
    for(int i=0; i<score; i++)
    {
        mvprintw(tailY[i], tailX[i] * 2, "o ");
    }
    // print score
    mvprintw(height, 0, "Score: %d", score);

    refresh();
}

void input()
{
    keypad(stdscr, true);
    // set timeout delay for input buffer
    timeout(gameSpeed);

    int c = getch();
    if(c == KEY_LEFT && dir != RIGHT)
    {
        dir = LEFT;
    } 
    else if(c == KEY_RIGHT && dir != LEFT)
    {
        dir = RIGHT;
    }
    else if(c == KEY_UP && dir != DOWN)
    {
        dir = UP;
    }
    else if(c == KEY_DOWN && dir != UP)
    {
        dir = DOWN;
    }
    // if q is pressed
    else if(c == 81 || c == 113)
    {
        gameOver = true;
    }
}

void logic()
{
    // check collision with fruit
    if(x == fruitX && y == fruitY)
    {
        score++;
        gameSpeed -= gameSPeedModifier;
        if(gameSpeed < gameSpeedCap)
        {
            gameSpeed = gameSpeedCap;
        }

        bool fruitFlag = true;
        int newFruitX, newFruitY;
        while(fruitFlag)
        {
            fruitFlag = false;
            newFruitX = rand() % (width - 2) + 1;
            newFruitY = rand() % (height - 2) + 1;
            for(int i=0; i<score; i++)
            {
                if(tailX[i] == x && tailY[i] == y)
                {
                    fruitFlag = true;
                    break;
                }
            }
        }
        fruitX = newFruitX;
        fruitY = newFruitY;
    }
    // update tail
    int prevX = tailX[0];
    int prevY = tailY[0];
    tailX[0] = x;
    tailY[0] = y;
    int prev2X, prev2Y;
    for(int i=1; i<score; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    // update head coordinate
    switch(dir)
    {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }
    // check collision with wall
    if(x == 0 || y == 0 || x == width - 1 || y == height - 1)
    {

        gameOver = true;
    }
    // check collision with tail
    for(int i=0; i<score; i++)
    {
        if(tailX[i] == x && tailY[i] == y)
        {
            gameOver = true;
        }
    }
}

int main()
{
    setup(true);
    while(restart)
    {
        setup(false);
        // main loop
        while (!gameOver)
        {
            draw();
            input();
            logic();
        }
        // print game over screen
        clear();
        mvprintw(height / 2 - 1, width - 4, "Game Over!");
        mvprintw(height / 2, width - 4, "Play again? y/n");
        mvprintw(height - 1, 0, " ");
        refresh();
        
        // set user input to blocking 
        timeout(-1);
        // loop until receive a valid input
        bool restartPrompt = true;
        while(restartPrompt)
        {
            int c = getch();

            // if input is n
            if((c == 78) || (c == 110))
            {
                restart = false;
                restartPrompt = false;
            }
            // if input is y
            else if((c == 89) || (c == 121))
            {
                restartPrompt = false;
            }
        }
    }
    clear();
    refresh();
    return 0;
}