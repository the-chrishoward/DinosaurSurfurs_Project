
// Chris Howard
// 3.26.2024
// ECE 2564 - Embedded Systems
// Proj2

#ifndef PROJ2_APP_H_
#define PROJ2_APP_H_

#include <HAL/HAL.h>
#include <HAL/Graphics.h>
#include <HAL/Timer.h>

#define TITLE_SCREEN_WAIT   3000  //3 seconds
#define RESULT_SCREEN_WAIT  3000  // 3 seconds
#define NEW_OBSTACLE_WAIT 1000    // 1 second
#define JUMP_TIME 400             // 0.5 seconds
#define OBSTACLE_SPEED 5
#define NEW_COLLISION_TIME 300    // 0.5 seconds

#define OBSTACLE_HEIGHT_FULL (LANE_SPACING - 1)
#define OBSTACLE_HEIGHT_HALF 13
#define MAX_OBSTACLES 25
#define OBSTACLE_WIDTH 5

#define OBSTACLE_TIME 3000          //3 seconds

#define LANE_WIDTH 127
#define LANE_HEIGHT 2
#define LANE_SPACING 27
#define PLAYER_RADIUS 2
#define PLAYER_SPEED 5
#define PLAYER_WIDTH 9

#define NUM_TEST_OPTIONS    4
#define NUM_RANDOM_NUMBERS  5

enum _GameState
{
    TITLE_SCREEN, MENU_SCREEN, INSTRUCTIONS_SCREEN, HIGH_SCORES_SCREEN, GAME_SCREEN, RESULT_SCREEN
};
typedef enum _GameState GameState;

enum _Cursor
{
    CURSOR_0 = 0, CURSOR_1 = 1, CURSOR_2 = 2, CURSOR_3 = 3
};
typedef enum _Cursor Cursor;

typedef struct {
    int x;
    int y;
} CursorPosition;

typedef struct {
    int x;
    double y;
    bool jumping;
} PlayerPosition;

typedef struct {
    int x; // x-coordinate of the obstacle
    int y; // y-coordinate of the obstacle
    int width; // width of the obstacle
    int height; // height of the obstacle
    bool exists; // Indicates whether the obstacle exists on the screen
    bool collided;    // flag indicating whether the obstacle has collided with the player
} Obstacle;

/**
 * The top-level Application object, initialized in the main() entry point and
 * passed around to basically every other function. Holds state variables
 * which YOU define as part of your application's state.
 */
struct _App_proj2
{
    GameState state;  // Determines which screen is currently shown
    SWTimer timer;    // General-purpose timer for when screens must disappear
    SWTimer obstacleSpawning;
    SWTimer jumpTimer;
    SWTimer obstacleTimer;
    SWTimer collisionTimer;

    // Booleans to determine which screen the user has selected
    bool instructionsSelect;
    bool highScoreSelect;
    bool newGameSelect;
    bool menuScreenDisplayed;
    bool firstCallInstructions;
    bool firstCallScores;
    bool firstCallGame;
    bool firstCallResults;

    // Cursor tracker variable
    Cursor cursor;
    Obstacle obstacles[MAX_OBSTACLES]; // Array to store obstacles
    int numObstacles; // Number of obstacles currently on the screen

    int highScore1;
    int highScore2;
    int highScore3;

    // Cursor position
    CursorPosition cursorPosition;
    CursorPosition prevCursorPosition;

    // Game info
    int lives;
    int currentScore;
    PlayerPosition playerPosition;
    PlayerPosition prevPlayerPosition;
};
typedef struct _App_proj2 App_proj2;

// Boilerplate constructor function for the Application
App_proj2 App_proj2_construct(HAL* hal_p);
void App_proj2_loop(App_proj2* app_p, HAL* hal_p);

// Handle callback functions used for each state of the App_proj2_loop()
// function. State transitions are embedded inside of each of these functions.
void App_proj2_handleTitleScreen(App_proj2* app, HAL* hal);
void App_proj2_handleInstructionsScreen(App_proj2* app, HAL* hal_p, GFX* gfx_p);
void App_proj2_handleGameScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p);
void App_proj2_handleResultScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p);

// Helper functions which clear the screen and draw the text for each statej
void App_proj2_showSplashScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p);
void App_proj2_showInstructionsScreen(App_proj2* app_p, GFX* gfx_p);
void App_proj2_showGameScreen(App_proj2* app_p, GFX* gfx_p);
void App_proj2_showMenuScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p);
void App_proj2_handleScoresScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p);

// Helper function which reduces the number of redraws required when moving the
// cursor.
void App_proj2_updateGameScreen(App_proj2* app_p, GFX* gfx_p);

// Used to reset the internal game variables
void App_proj2_initGameVariables(App_proj2* app_p, HAL* hal_p);

/* NEW */
void App_proj2_getCursorPoint(App_proj2* app_p, HAL* hal_p);
void App_proj2_handleGameScreen(App_proj2* app_p, HAL* hal_p, GFX* gfx_p);
void spawnObstacle(GFX* gfx_p, App_proj2* app_p);
void moveObstacles(GFX* gfx_p, App_proj2* app_p);
void drawObstacles(GFX* gfx_p, App_proj2* app_p);
bool isCollision(PlayerPosition* player, Obstacle* obstacle);
void updateHighScores(App_proj2* app_p);
void handlePlayerMovement(App_proj2* app_p, HAL* hal_p);

#endif /* PROJ2_APP_H_ */
