/*
 * Chris Howard
 * 3.27.2024
 * ECE 2564 Project 2
 */

/* Include Driver Lib */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* HAL and Application includes */
#include <HAL/HAL.h>
#include <HAL/Timer.h>
#include <proj2_app.h>
#include <HAL/Joystick.h>

/* New includes - used throughout testing*/
#include <ctype.h>
#include <stdlib.h> // for malloc, free
#include <string.h> // for strlen

int main(void)
{
    // Stop Watchdog Timer - THIS SHOULD ALWAYS BE THE FIRST LINE OF YOUR MAIN
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and the HAL.
    HAL hal = HAL_construct();
    App_proj2 app = App_proj2_construct(&hal);
//    App_proj2_showTitleScreen(&hal.gfx);

    // Main super-loop! In a polling architecture, this function should call
    // your main FSM function over and over.
    while (true)
    {
        App_proj2_loop(&app, &hal);  //update my program, application state, output
        HAL_refresh(&hal); // check the inputs
    }
}

App_proj2 App_proj2_construct(HAL* hal_p)
{
    // The App_proj2 object to initialize
    App_proj2 app;

//    // Initialization of FSM variables
    app.state = TITLE_SCREEN;
    bool splashDisplayed = false;
    if (!splashDisplayed) { // Was getting weird issues with splashScreen being displayed again on occasion when moving to another screen, and then back to title. This seems to have fixed it.
        app.timer = SWTimer_construct(TITLE_SCREEN_WAIT);
        splashDisplayed = true;
    }
    app.obstacleSpawning = SWTimer_construct(NEW_OBSTACLE_WAIT);
    app.obstacleTimer = SWTimer_construct(OBSTACLE_TIME);
    app.jumpTimer = SWTimer_construct(JUMP_TIME);
    app.collisionTimer = SWTimer_construct(NEW_COLLISION_TIME);
    SWTimer_start(&app.timer);

    //initialize cursor
    app.cursorPosition.x = 7;       // start at play game
    app.cursorPosition.y = 16;      // good point on axis
    app.highScore1 = 0;             // set high scores to zero to start on launch
    app.highScore2 = 0;
    app.highScore3 = 0;
    app.currentScore = 0000000000;  //initialize current score
    app.lives = 3;                  //initialize amount of lives for player
    app.playerPosition.x = 5;       //initialize playerpostion on the x axis
    app.playerPosition.y = 43;      //initialize playerpostion on the y axis

    // Initialize previous cursor position
    app.prevCursorPosition = app.cursorPosition;
    app.menuScreenDisplayed = false;
    app.firstCallResults = false;

    // Return the completed Application struct to the user
    return app;
}

//for testing purposes from startup - used to verify functionality of joystick when debugging.
unsigned colormix(unsigned r, unsigned g, unsigned b) {
    return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}

/**
 * The main super-loop function of the application. We place this inside of a
 * single infinite loop in main. In this way, we can model a polling system of
 * FSMs. Every cycle of this loop function, we poll each of the FSMs one time.
 */
void App_proj2_loop(App_proj2* app_p, HAL* hal_p)
{
    /////////////////////////////////////////////
    //non blocking / blocking test
    if (Button_isPressed(&hal_p->launchpadS1)) {
        LED_turnOn(&hal_p->launchpadRed);
    }
    else {
        LED_turnOff(&hal_p->launchpadRed);
    }
    /////////////////////////////////////////////

    switch (app_p->state)
    {
    case TITLE_SCREEN:
                // Get cursor position before handling the title screen
                App_proj2_getCursorPoint(app_p, hal_p);
                // Handle title screen
                App_proj2_handleTitleScreen(app_p, hal_p);

                // Check if joystick button is pressed
                if (Button_isTapped(&hal_p->boosterpackJS)) {
                    // Determine which option is currently selected by the cursor
                    switch (app_p->cursorPosition.x) {
                        case 7: //location of PLAY GAME
                            // if game is selected, navigate to the play game screen.
                            GFX_clear(&hal_p->gfx);
                            app_p->state = GAME_SCREEN;
                            // PLAY GAME option selected, do nothing for now
                            break;
                        case 10: //location of HIGH SCORES
                            // if selected, we navigate to high scores screen state
                            GFX_clear(&hal_p->gfx);
                            app_p->state = HIGH_SCORES_SCREEN;
                            break;
                        case 13: //location of HOW TO PLAY
                            // HOW TO PLAY option selected, navigate to instructions screen
                            GFX_clear(&hal_p->gfx);
                            app_p->state = INSTRUCTIONS_SCREEN;
                            // Call function to display instructions screen
                            break;
                    }
                }
                break;

        case INSTRUCTIONS_SCREEN:
            App_proj2_handleInstructionsScreen(app_p, hal_p, &hal_p->gfx);
            break;
        case HIGH_SCORES_SCREEN:
            App_proj2_handleScoresScreen(app_p, hal_p, &hal_p->gfx);
            break;

        case GAME_SCREEN:
            App_proj2_handleGameScreen(app_p, hal_p, &hal_p->gfx);
            break;

        case RESULT_SCREEN:
            App_proj2_handleResultScreen(app_p, hal_p, &hal_p->gfx);
            break;
    }
}

// The function that handles the title screen checks timer to see if menu should be wiped
void App_proj2_handleTitleScreen(App_proj2* app_p, HAL* hal_p) {
    if (SWTimer_expired(&app_p->timer)) {
        App_proj2_showMenuScreen(app_p, hal_p, &hal_p->gfx);
    }
    else {
        App_proj2_showSplashScreen(app_p, hal_p, &hal_p->gfx);
    }
}

// function to help get the point of the cursor to display it later.
void App_proj2_getCursorPoint(App_proj2* app_p, HAL* hal_p)
{
    // Move cursor right if joystick is tapped down
    if (Joystick_isTappedtoBottom(&hal_p->joystick) && app_p->cursorPosition.x < 13)
    {
        app_p->cursorPosition.x += 3; // Move down by 3
    }
    // Move cursor left if joystick is tapped left
    else if (Joystick_isTappedtoTop(&hal_p->joystick) && app_p->cursorPosition.x > 7)
    {
        app_p->cursorPosition.x -= 3; // Move up by 3
    }
}

void App_proj2_showMenuScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p) {

    // Display the text
    GFX_print(gfx_p, "  MENU: JS to Select", 1, 0);
    GFX_print(gfx_p, "---------------------", 2, 0);
    GFX_print(gfx_p, "Please select:", 4, 0);
    GFX_print(gfx_p, "PLAY GAME:", 7, 0);
    GFX_print(gfx_p, "HIGH SCORES:", 10, 0);
    GFX_print(gfx_p, "HOW TO PLAY:", 13, 0);
    GFX_print(gfx_p, "---------------------", 15, 0);

    // Erase previous cursor
    GFX_print(&hal_p->gfx, " ", app_p->prevCursorPosition.x, app_p->cursorPosition.y);

    // Print cursor as asterisk at the current position
    GFX_print(&hal_p->gfx, "<", app_p->cursorPosition.x, app_p->cursorPosition.y);

    // Save current cursor position as previous for next loop iteration
    app_p->prevCursorPosition.x = app_p->cursorPosition.x;
}

// Function called on initial startup.
void App_proj2_showSplashScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p) {
    GFX_print(gfx_p, "    S24 Project 2", 2, 0);
    GFX_print(gfx_p, "ECE Surfers", 4, 4);
    GFX_print(gfx_p, "LOADING...", 6, 4);
    GFX_print(gfx_p, "By: Chris Howard", 10, 2);
    if (SWTimer_expired(&app_p->timer)) {
        GFX_clear(&hal_p->gfx);
    }
}

void App_proj2_handleInstructionsScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p) {
    if (!app_p->firstCallInstructions) {
        GFX_clear(&hal_p->gfx);
        app_p->firstCallInstructions = true;
    }


    // display relevant information
    GFX_print(gfx_p, "    HOW TO PLAY: ", 1, 0);
    GFX_print(gfx_p, "---------------------", 2, 0);
    GFX_print(gfx_p, "1. Avoid obstacles", 3, 0);
    GFX_print(gfx_p, "2. Survive longer", 4, 0);
    GFX_print(gfx_p, "   to score more", 5, 0);
    GFX_print(gfx_p, "   points (+500)", 6, 0);
    GFX_print(gfx_p, "3. Collisions lose a", 7, 0);
    GFX_print(gfx_p, "   single life, (1/3)", 8, 0);
    GFX_print(gfx_p, "4. Check high score", 9, 0);
    GFX_print(gfx_p, "   in the menu", 10, 0);
    GFX_print(gfx_p, "JS: MOVE", 12, 0);
    GFX_print(gfx_p, "BB1: JUMP", 13, 0);
    GFX_print(gfx_p, "JSB: BACK TO MENU   <", 14, 0);
    GFX_print(gfx_p, "---------------------", 15, 0);

    // check for button press to go back
    if (Button_isTapped(&hal_p->boosterpackJS)) {
        GFX_clear(&hal_p->gfx);
        app_p->state = TITLE_SCREEN;
        app_p->firstCallInstructions = false; // reset for next call
    }
}

void App_proj2_handleScoresScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p) {
    if (!app_p->firstCallScores) {
        GFX_clear(gfx_p); // Clear the screen only if it's the first call
        app_p->firstCallScores = true;
    }

    // Display the top three scores
    GFX_print(gfx_p, "     HIGH SCORES: ", 0, 0);
    GFX_print(gfx_p, "---------------------", 1, 0);

    // Update the high scores if needed
    updateHighScores(app_p);

    // Print the updated top three scores
    char scoreStr[20];
    sprintf(scoreStr, "1. %d", app_p->highScore1);
    GFX_print(gfx_p, scoreStr, 3, 0);
    sprintf(scoreStr, "2. %d", app_p->highScore2);
    GFX_print(gfx_p, scoreStr, 7, 0);
    sprintf(scoreStr, "3. %d", app_p->highScore3);
    GFX_print(gfx_p, scoreStr, 11, 0);
    GFX_print(gfx_p, "JSB: GO HOME    <", 14, 0);
    GFX_print(gfx_p, "---------------------", 15, 0);

    // Check for joystick button press to return to the main menu
    if (Button_isTapped(&hal_p->boosterpackJS)) {
        GFX_clear(gfx_p); // Clear the screen before transitioning
        app_p->state = TITLE_SCREEN;
        app_p->firstCallScores = false;
    }
}

// Function to draw lanes on the screen
void drawLanes(GFX* gfx_p, int screenHeight) {
    int numLanes = 4;
    int y = 20; // Start drawing lanes from the top with spacing
    int i = 0;
    for (i = 0; i < numLanes; i++) {
        GFX_drawLane(gfx_p, 0, y);
        y += LANE_HEIGHT + LANE_SPACING; // Move to the next lane position
    }
}

void handlePlayerMovement(App_proj2* app_p, HAL* hal_p) {
    if (Joystick_isPressedtoRight(&hal_p->joystick)) {
                // Move player right
                if (app_p->playerPosition.x + PLAYER_SPEED <= 127 - PLAYER_RADIUS) {
                    app_p->playerPosition.x += PLAYER_SPEED;
                }
            } else if (Joystick_isPressedtoLeft(&hal_p->joystick)) {
                // Move player left
                if (app_p->playerPosition.x - PLAYER_SPEED >= 0 + PLAYER_RADIUS) {
                    app_p->playerPosition.x -= PLAYER_SPEED;
                }
            } else if (Joystick_isTappedtoBottom(&hal_p->joystick)) {
                // Move player down (change lanes down)
                app_p->playerPosition.y += LANE_SPACING  + LANE_HEIGHT;
                // Constrain player's y-position to be within the bounds
                if (app_p->playerPosition.y > 101) {
                    app_p->playerPosition.y = 101;
                }
            } else if (Joystick_isTappedtoTop(&hal_p->joystick)) {
                // Move player up (change lanes up)
                app_p->playerPosition.y -= LANE_SPACING + LANE_HEIGHT;
                // Constrain player's y-position to be within the bounds
                if (app_p->playerPosition.y < 43) {
                    app_p->playerPosition.y = 43;
                }
            }
            // Check if the jump button is tapped and start the jump timer
            if (Button_isTapped(&hal_p->boosterpackS1) && (app_p->playerPosition.y == 43 || app_p->playerPosition.y == 72 || app_p->playerPosition.y == 101)) {
                SWTimer_start(&app_p->jumpTimer);
                app_p->playerPosition.jumping = true; // Set jumping flag
            }
            // Update player position based on jump timer
            if (app_p->playerPosition.jumping) {
                if (!SWTimer_expired(&app_p->jumpTimer)) {
                    // If jump timer hasn't expired and player is not at the top boundary, move player up
                    if (app_p->playerPosition.y > 28) {
                        app_p->playerPosition.y -= 1; // Adjust jump distance as needed for precision
                    }
                } else {
                    // Jump timer expired, reset jumping flag
                    app_p->playerPosition.jumping = false;
                    app_p->jumpTimer = SWTimer_construct(JUMP_TIME); // Reset jump timer
                }
            } else {
                // Player is not jumping, check conditions for decrementing player position
                if (app_p->playerPosition.y != 101 && app_p->playerPosition.y != 43 && app_p->playerPosition.y != 72) {
                    app_p->playerPosition.y += 1;
                }
            }
            // Constrain player's y-position to be within the bounds
            if (app_p->playerPosition.y < 28) {
                app_p->playerPosition.y = 28;
            } else if (app_p->playerPosition.y > 101) {
                app_p->playerPosition.y = 101;
      }
}

// Helper function to handle the game screen
void App_proj2_handleGameScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p) {
    if (!app_p->firstCallGame) {
        GFX_clear(&hal_p->gfx);
        app_p->firstCallGame = true;
        app_p->currentScore = 0; // reset score if its a new game.
        app_p->lives = 3;
        // Reset obstacles
        app_p->numObstacles = 0;
        memset(app_p->obstacles, 0, sizeof(app_p->obstacles));
    }

    // Display the label "SCORE:"
    GFX_print(&hal_p->gfx, "SCORE:", 1, 1);

    // Display the current score value
    char scoreStr[20];
    sprintf(scoreStr, "%d", app_p->currentScore);
    GFX_print(&hal_p->gfx, scoreStr, 1, 8);
    drawLanes(&hal_p->gfx, 127);

    // Display lives counter at the bottom of the screen
        char livesStr[20];
        sprintf(livesStr, "Lives: ");
        GFX_print(gfx_p, livesStr, 14, 1);

        int i, j;
        // Erase previous lives by drawing filled rectangles with the background color
        for (i = 0; i < 3; ++i) {
            GFX_removeSolidCircle(gfx_p, 50 + i * 7, 116, 2);
        }

        // Draw lives as filled circles
        int numLivesToDraw = (app_p->lives < 3) ? app_p->lives : 3;
        for (j = 0; j < numLivesToDraw; ++j) {
            GFX_drawSolidCircle(gfx_p, 50 + j * 7, 116, 2);
        }

    // Erase previous player position by drawing a solid circle with the background color
    GFX_removeSolidCircle(gfx_p, app_p->prevPlayerPosition.x, app_p->prevPlayerPosition.y, PLAYER_RADIUS);

    // Handle player movement
    handlePlayerMovement(app_p, hal_p);

        // Handle transitions to game over screen when lives run out
            if (app_p->lives <= 0) {
                GFX_clear(&hal_p->gfx);
                // Update high scores if necessary
                updateHighScores(app_p);
                app_p->firstCallResults =false;
                app_p->state = RESULT_SCREEN;
                return; // Exit the function to prevent further gameplay processing
            }


    // Draw player at the updated position
    GFX_drawSolidCircle(&hal_p->gfx, app_p->playerPosition.x, app_p->playerPosition.y, PLAYER_RADIUS);
    // Update previous player position for next iteration
    app_p->prevPlayerPosition = app_p->playerPosition;
    // Spawn Obstacles
    spawnObstacle(gfx_p, app_p);
    // Move obstacles
    moveObstacles(gfx_p, app_p);
    // Draw obstacles
    drawObstacles(gfx_p, app_p);
}

void spawnObstacle(GFX* gfx_p, App_proj2* app_p) {
    // Check if the timer for obstacle spawning has expired
    if (SWTimer_expired(&app_p->obstacleSpawning)) {
        // Reset the obstacle spawning timer for the next spawn
        SWTimer_start(&app_p->obstacleSpawning);

        // Define the width of the obstacle
        int obstacleWidth = 5; // Adjust as needed

        // Choose a random lane (lane 1, 2, or 3)
        int lane = rand() % 3 + 1; // Random number between 1 and 3

        // Determine the type of obstacle to spawn (full height, half height from bottom, or half height from top)
        int obstacleType = rand() % 3; // Random number between 0 and 2
        int obstacleHeight;
        int y;
        switch (obstacleType) {
            case 0: // Full height obstacle
                obstacleHeight = LANE_SPACING - 1; // Adjust as needed, leaving some gap between lanes
                y = (lane - 1) * (LANE_HEIGHT + LANE_SPACING) + 23; // Center of the chosen lane with offset
                break;
            case 1: // Half height obstacle from bottom
                obstacleHeight = 10; // Adjust as needed
                y = (lane - 1) * (LANE_HEIGHT + LANE_SPACING) + LANE_HEIGHT + 20; // Bottom of the chosen lane with offset
                break;
            case 2: // Half height obstacle from top
                obstacleHeight = 10; // Adjust as needed
                y = (lane - 1) * (LANE_HEIGHT + LANE_SPACING) + 39; // Top of the chosen lane with offset
                break;
            default:
                obstacleHeight = LANE_SPACING - 1;
                y = (lane - 1) * (LANE_HEIGHT + LANE_SPACING);
                break;
        }

        // Calculate the initial position of the obstacle
        int x = 127 - obstacleWidth; // Right side of the screen

        // Create a new obstacle instance
        Obstacle newObstacle;
        newObstacle.x = x;
        newObstacle.y = y;
        newObstacle.width = obstacleWidth;
        newObstacle.height = obstacleHeight;
        newObstacle.exists = true;

        // Add the obstacle to the obstacles array
        if (app_p->numObstacles < MAX_OBSTACLES) {
            app_p->obstacles[app_p->numObstacles++] = newObstacle;
        }
    }
}

// Function to move obstacles across the screen based on a timer
void moveObstacles(GFX* gfx_p, App_proj2* app_p) {
    // Calculate the distance to move obstacles based on time elapsed since last update
    double distanceToMove = 2;

    // Loop through all obstacles
    int i = 0;
    while (i < app_p->numObstacles) {
        // Check if the obstacle exists
        if (app_p->obstacles[i].exists) {
            // Remove the obstacle from its previous position
            GFX_removeObstacle(gfx_p, app_p->obstacles[i].x, app_p->obstacles[i].y,
                               app_p->obstacles[i].width, app_p->obstacles[i].height);

            // Move the obstacle by the calculated distance
            app_p->obstacles[i].x -= distanceToMove;

            // Check if the obstacle has moved off the screen
            if (app_p->obstacles[i].x + app_p->obstacles[i].width < 0) {
                // Increment the score by 500 when the obstacle disappears
                app_p->currentScore += 500;

                // If so, remove the obstacle
                app_p->numObstacles--; // Decrement the count of obstacles
                app_p->obstacles[i] = app_p->obstacles[app_p->numObstacles]; // Replace current obstacle with the last obstacle
                continue; // Skip incrementing 'i' since we replaced the current obstacle
            }

            // Check if the obstacle has collided and if enough time has elapsed to reset the collision flag
            if (app_p->obstacles[i].collided && SWTimer_expired(&app_p->collisionTimer)) {
                app_p->obstacles[i].collided = false; // Reset the collided flag
            }

            // Check for collision with the player
            if (isCollision(&app_p->playerPosition, &app_p->obstacles[i])) {
                // Handle collision: Decrement player's life by one
                if (!app_p->obstacles[i].collided) {
                    app_p->lives--;
                    app_p->obstacles[i].collided = true; // Mark the obstacle as collided
                    SWTimer_start(&app_p->collisionTimer); // Start the collision timer
                }
            }
        }
        i++;
    }
}

void drawObstacles(GFX* gfx_p, App_proj2* app_p) {
    uint32_t oldForegroundColor = gfx_p->foreground;
    GFX_setForeground(gfx_p, ClrWhite);
    int i;
    for (i = 0; i < app_p->numObstacles; i++) {
        Obstacle obstacle = app_p->obstacles[i];
        if (obstacle.exists) {
            Graphics_Rectangle obstacleRect;
            obstacleRect.xMin = obstacle.x;
            obstacleRect.yMin = obstacle.y;
            obstacleRect.xMax = obstacle.x + obstacle.width - 1;
            obstacleRect.yMax = obstacle.y + obstacle.height - 1;
            Graphics_fillRectangle(&gfx_p->context, &obstacleRect);
        }
    }

    // Restore the old foreground color
    GFX_setForeground(gfx_p, oldForegroundColor);
}

// Function to check collision between player and obstacle
bool isCollision(PlayerPosition* player, Obstacle* obstacle) {
    // Check if the player's position intersects with the obstacle
    return (player->x + PLAYER_RADIUS >= obstacle->x &&
            player->x - PLAYER_RADIUS <= obstacle->x + obstacle->width &&
            player->y + PLAYER_RADIUS >= obstacle->y &&
            player->y - PLAYER_RADIUS <= obstacle->y + obstacle->height);
}

void App_proj2_handleResultScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p) {
    // Handle transitions to result screen
    if (app_p->lives <= 0) {
        // Check if it's the first call of the function
        if (!app_p->firstCallResults) {
            // Clear the screen only if it's the first call
            GFX_clear(gfx_p); // Using the provided GFX pointer
            app_p->firstCallResults = true;

            // Print "GAME OVER" at the top
            GFX_print(gfx_p, "  GAME OVER", 1, 4); // Predefined coordinates for "GAME OVER"
            GFX_print(gfx_p, "---------------------", 2, 0);
            GFX_print(gfx_p, "    JSB: GO HOME", 15, 0);

            // Print the player's score in the middle
            char scoreText[30];
            sprintf(scoreText, " Score: %d", app_p->currentScore);
            GFX_print(gfx_p, scoreText, 4, 4); // Predefined coordinates for the score text
        }

        // Check if joystick button is pressed to return to the main menu
        if (Button_isTapped(&hal_p->boosterpackJS)) {
            // Clear the screen before transitioning
            GFX_clear(gfx_p); // Using the provided GFX pointer

            // Reset the game state and variables
            app_p->state = TITLE_SCREEN;
            app_p->firstCallResults = false;
            app_p->firstCallGame = false;
            app_p->lives = 3;
            app_p->currentScore = 0;
            app_p->playerPosition.x = 5;
            app_p->playerPosition.y = 43;
        }
    }
}

void updateHighScores(App_proj2* app_p) {
    // Check if the current score is higher than the first high score
    if (app_p->currentScore > app_p->highScore1) {
        // If yes, update the first high score and shift the other scores down
        app_p->highScore3 = app_p->highScore2;
        app_p->highScore2 = app_p->highScore1;
        app_p->highScore1 = app_p->currentScore;
    }
    // Check if the current score is higher than the second high score but not the first
    else if (app_p->currentScore > app_p->highScore2) {
        // If yes, update the second high score and shift the third score down
        app_p->highScore3 = app_p->highScore2;
        app_p->highScore2 = app_p->currentScore;
    }
    // Check if the current score is higher than the third high score but not the first or second
    else if (app_p->currentScore > app_p->highScore3) {
        // If yes, update the third high score
        app_p->highScore3 = app_p->currentScore;
    }
}



/*
 * Chris Howard
 * Project 2: ECE 2564 - Embedded Systems
 * Dr. Ali
 * Last revised: 2 April 2024
 *
 * */




