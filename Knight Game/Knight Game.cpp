#include <iostream>
#include <vector>
#include <raylib.h>
#include "Button.h"

const int tileSize = 100;
const int gridSize = 8;
const int screenWidth = tileSize * gridSize;
const int screenHeight = tileSize * gridSize;

Color darkWood = Color{ 101, 67, 33, 255 };
Color lightWood = Color{ 222, 184, 135, 255 };
Color MenuBackground = Color{ 80, 50, 20, 255 };

int fadeAlpha = 255;

Button start;
Button restart;
Button end;

bool showMoves = true;

typedef enum GameScreen { LOADING = 0, INITIAL_MENU, GAMEPLAY,PAUSE_MENU,STUCK, WIN } GameScreen;
GameScreen currentScreen = LOADING;

std::vector <std::pair<int, int>> pos = { {-1,2},{1,2 },{1,-2},{-1,-2 },{2,-1},{2,1 },{-2,1},{-2,-1 } };

static bool checkCollision(Vector2 mousePos, Vector2 knightPos)
{
    return CheckCollisionPointCircle(mousePos, knightPos, tileSize / 3);
}

class Knight {
public:
    int row, col;
    

    Knight(int startRow, int startCol) : row(startRow), col(startCol) {
        row = startRow;  
        col = startCol;
    }

    void move(int newRow, int newCol) {
        
        row = newRow;    
        col = newCol;
    }

    Vector2 getPosition() {
        return { (float)col * tileSize + tileSize / 2, (float)row * tileSize + tileSize / 2 };
    }
};
class Board {
public:
    int visited[gridSize][gridSize];

    Board() {
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                visited[i][j] = 0; 
            }
        }
    }

    bool isValidMove(int row, int col) {
        return (row >= 0 && row < gridSize && col >= 0 && col < gridSize && visited[row][col] == 0);
    }

    void markVisited(int row, int col) {
        visited[row][col] = 1;
    }

    void draw() {
        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                Color tileColor = ((row + col) % 2 == 0) ? lightWood : darkWood;
                DrawRectangle(col * tileSize, row * tileSize, tileSize, tileSize, tileColor);
            }
        }
    }

    // Draw visited squares (in red color)
    void drawVisited() {
        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                if (visited[row][col] == 1) {
                    DrawRectangle(col * tileSize, row * tileSize, tileSize, tileSize, RED);
                }
            }
        }
    }
    bool isStuck(int knightRow, int knightCol) {
        for (const auto& move : pos) {
            int newRow = knightRow + move.second;
            int newCol = knightCol + move.first;

            if (isValidMove(newRow, newCol)) {
                return false;
            }
        }
            return true;
        }
    bool hasWon(Knight& knight) {
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                if (visited[i][j] == 0 && !(i == knight.row && j == knight.col)) {
                    return false;
                }
            }
        }
        return true;
    }
};

static void reset(Knight& knight, Board& board) {
    knight.move(0, 0);
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            board.visited[i][j] = 0; 
        }
    }
    showMoves = true; 
}

int main() {
    InitWindow(screenWidth, screenHeight, "Knight Game");
    SetTargetFPS(60);

    Board board;
    Knight knight(0, 0);  
    
    Texture2D knightTexture = LoadTexture("./knight.png");

    int textWidth = MeasureText("Knight Game", 100);
    int centerX = (screenWidth - textWidth) / 2;
    int centerY = screenHeight / 2;
    Vector2 origin = { 0, 0 };
    while (!WindowShouldClose())
    {
        BeginDrawing();
        Rectangle source = { 0, 0, (float)knightTexture.width, (float)knightTexture.height };  // 500x500
        Rectangle dest = {
            (float)knight.col * tileSize,
            (float)knight.row * tileSize,
            (float)tileSize,
            (float)tileSize
        };
        switch (currentScreen)
        {
        case LOADING:
            if (fadeAlpha > 0)
            {
                fadeAlpha -= 2;
                ClearBackground(MenuBackground);
                DrawText("KNIGHT GAME", centerX, centerY - 10, 80, Color{ 255, 255, 255, (unsigned char)fadeAlpha });
            }
            else 
                currentScreen = INITIAL_MENU;
             break;            
        case INITIAL_MENU:
            ClearBackground(MenuBackground);

            start.setPosition({ screenWidth / 2 - 150,screenHeight / 2 - 100 });
            end.setPosition({ screenWidth / 2 - 150, screenHeight / 2 + 50 });
            start.draw("START");
            end.draw("EXIT");
            if (start.checkCollision(GetMousePosition(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
            {
                currentScreen = GAMEPLAY;
                fadeAlpha = 0;
            }
            if (end.checkCollision(GetMousePosition(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
            {
                CloseWindow();
                return 0;
            }break;
        case GAMEPLAY:
            board.draw();
            board.drawVisited();
   

            // Draw the texture on the board
            DrawTexturePro(knightTexture, source, dest, origin, 0.0f, WHITE);

            Vector2 knightPos = knight.getPosition();
            DrawTexturePro(knightTexture, source, dest, origin, 0.0f, WHITE);

            // Check if knight was clicked
            if (checkCollision(GetMousePosition(), knightPos) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                showMoves = !showMoves;
            }

            if (showMoves)
            {
                // Highlight valid moves
                for (int i = 0; i < pos.size(); i++)
                {
                    int movRow = knight.row + pos[i].second;
                    int movCol = knight.col + pos[i].first;

                    if (board.isValidMove(movRow, movCol))
                    {
                        Rectangle rect = { movCol * tileSize, movRow * tileSize, tileSize, tileSize };
                        DrawRectangle(movCol * tileSize, movRow * tileSize, tileSize, tileSize, SKYBLUE);

                        // Handle move selection
                        if (CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        {
                            // Mark previous position red after moving
                            board.markVisited(knight.row, knight.col);

                            knight.move(movRow, movCol);

                            showMoves = false;
                        }
                    }
                }
            }
            if (board.isStuck(knight.row, knight.col)) {
                currentScreen = STUCK;
                
            }
           
            else if (IsKeyPressed(KEY_SPACE))
            {
                currentScreen = PAUSE_MENU;
            }
            if (board.hasWon(knight)) {
                currentScreen = WIN;
            }
            break;
        case STUCK:
            ClearBackground(MenuBackground);
            DrawText("You Are Stuck!", centerX, centerY - 10, 80, RAYWHITE);
            DrawText("Press SPACE to Restart", centerX, centerY + 100, 40, RAYWHITE);
            if (IsKeyPressed(KEY_SPACE)) {
                reset(knight, board);
                currentScreen = GAMEPLAY;
            }
            break;
        case PAUSE_MENU:
            ClearBackground(MenuBackground);
            start.setPosition({ screenWidth / 2 - 150,screenHeight / 2 - 250 });
            restart.setPosition({ screenWidth / 2 - 150,screenHeight / 2 - 100 });
            end.setPosition({ screenWidth / 2 - 150, screenHeight / 2 + 50 });

            if (start.checkCollision(GetMousePosition(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
            {
                currentScreen = GAMEPLAY;
                fadeAlpha = 0;
            }
            if (end.checkCollision(GetMousePosition(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
            {
                CloseWindow();
                return 0;
            }
            if (restart.checkCollision(GetMousePosition(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
            {
                reset(knight,board);
                currentScreen = GAMEPLAY;
            }
            start.draw("CONTINUE");
            restart.draw("RESTART");
            end.draw("EXIT");
            break;
        case WIN:
            ClearBackground(MenuBackground);
            DrawText("You Won!", centerX, centerY - 10, 80, RAYWHITE);
            DrawText("Press SPACE to Restart", centerX, centerY + 100, 40, RAYWHITE);

            if (IsKeyPressed(KEY_SPACE)) {
                reset(knight, board);
                currentScreen = GAMEPLAY;
            }
            break;
        }
            EndDrawing();
    }
    UnloadTexture(knightTexture);
    CloseWindow();
    return 0;
}
    