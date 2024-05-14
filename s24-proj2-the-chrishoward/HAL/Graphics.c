/*
 * Graphics.c
 *
 *  Created on: Dec 30, 2019
 *      Author: Matthew Zhong
 */

#include <HAL/Graphics.h>
#include <proj2_app.h>

GFX GFX_construct(uint32_t defaultForeground, uint32_t defaultBackground)
{
    GFX gfx;

    gfx.defaultForeground = defaultForeground;
    gfx.defaultBackground = defaultBackground;

    // initializing the display
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // setting up the graphics
    Graphics_initContext(&gfx.context, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setFont(&gfx.context, &g_sFontFixed6x8);

    GFX_resetColors(&gfx);
    GFX_clear(&gfx);

    return gfx;
}

void GFX_resetColors(GFX* gfx_p)
{
    gfx_p->foreground = gfx_p->defaultForeground;
    gfx_p->background = gfx_p->defaultBackground;

    Graphics_setForegroundColor(&gfx_p->context, gfx_p->defaultForeground);
    Graphics_setBackgroundColor(&gfx_p->context, gfx_p->defaultBackground);
}

void GFX_clear(GFX* gfx_p)
{
    Graphics_clearDisplay(&gfx_p->context);
}

void GFX_print(GFX* gfx_p, char* string, int row, int col)
{
    int yPosition = row * Graphics_getFontHeight(gfx_p->context.font);
    int xPosition = col * Graphics_getFontMaxWidth(gfx_p->context.font);

    Graphics_drawString(&gfx_p->context, (int8_t*) string, -1, xPosition, yPosition, OPAQUE_TEXT);
}

void GFX_setForeground(GFX* gfx_p, uint32_t foreground)
{
    gfx_p->foreground = foreground;
    Graphics_setForegroundColor(&gfx_p->context, foreground);
}

void GFX_setBackground(GFX* gfx_p, uint32_t background)
{
    gfx_p->background = background;
    Graphics_setBackgroundColor(&gfx_p->context, background);
}

void GFX_drawSolidCircle(GFX* gfx_p, int x, int y, int radius)
{
    Graphics_fillCircle(&gfx_p->context, x, y, radius);
}

// Function to draw a solid rectangle representing a lane
void GFX_drawLane(GFX* gfx_p, int x, int y) {
    Graphics_Rectangle laneRect;
    laneRect.xMin = x;
    laneRect.yMin = y;
    laneRect.xMax = x + LANE_WIDTH - 1;
    laneRect.yMax = y + LANE_HEIGHT - 1;
    Graphics_fillRectangle(&gfx_p->context, &laneRect);
}

void GFX_drawHollowCircle(GFX* gfx_p, int x, int y, int radius)
{
    Graphics_drawCircle(&gfx_p->context, x, y, radius);
}

void GFX_removeSolidCircle(GFX* gfx_p, int x, int y, int radius)
{
    uint32_t oldForegroundColor = gfx_p->foreground;
    GFX_setForeground(gfx_p, gfx_p->background);
    GFX_drawSolidCircle(gfx_p, x, y, radius);
    GFX_setForeground(gfx_p, oldForegroundColor);
}

void GFX_removeHollowCircle(GFX* gfx_p, int x, int y, int radius)
{
    uint32_t oldForegroundColor = gfx_p->foreground;
    GFX_setForeground(gfx_p, gfx_p->background);
    GFX_drawHollowCircle(gfx_p, x, y, radius);
    GFX_setForeground(gfx_p, oldForegroundColor);
}

void GFX_removeObstacle(GFX* gfx_p, int x, int y, int width, int height) {
    // Save the old foreground color
    uint32_t oldForegroundColor = gfx_p->foreground;

    // Set the foreground color to match the background color
    GFX_setForeground(gfx_p, gfx_p->background);

    // Create a rectangle covering the area of the obstacle
    Graphics_Rectangle obstacleRect;
    obstacleRect.xMin = x;
    obstacleRect.yMin = y;
    obstacleRect.xMax = x + width - 1;
    obstacleRect.yMax = y + height - 1;

    // Fill the rectangle with the background color to erase the obstacle
    Graphics_fillRectangle(&gfx_p->context, &obstacleRect);

    // Restore the old foreground color
    GFX_setForeground(gfx_p, oldForegroundColor);
}

