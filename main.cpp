#include<bits/stdc++.h>
#include "include/raylib.h"
#include "include/raymath.h"
using namespace std;

Color green = {173,204,96,255};
Color darkGreen = {43,51,24,255};

int cellSize = 28;
int cellCount = 25;
int offSet = 75;
// to keep track of time when snake was updated last
double lastUpdateTime=0;

bool eventTrigger(double interval){
    double currTime = GetTime();
    if((currTime - lastUpdateTime) >= interval){
        lastUpdateTime = currTime;
        return true;
    }
    return false;
}

bool ElementOnSnake(Vector2 ele,deque<Vector2>deq){
    for(unsigned int i=0;i<deq.size();i++){
        if(Vector2Equals(ele,deq[i]))return true;
    }
    return false;
}

class Food{
    public:
        // position holds the starting point of the food
        Vector2 position;
        Texture2D texture;

        Food(deque<Vector2>snakeBody){
            Image image = LoadImage("images/apple.png");
            //to adjust image size widht * height
            ImageResize(&image, 35, 32);    
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = generateRandomPos(snakeBody);
        }

        ~Food(){
            UnloadTexture(texture);
        }

        void Draw(){
            DrawTexture(texture,offSet+position.x*cellSize,offSet+position.y*cellSize,WHITE);
        }

        Vector2 GenerateRandomCell(){
            float x = GetRandomValue(0,cellCount-1);
            float y = GetRandomValue(0,cellCount-1);
            return {x,y};
        }
        Vector2 generateRandomPos(deque<Vector2>snakeBody){
            Vector2 pos =  GenerateRandomCell();
            while(ElementOnSnake(pos,snakeBody)){
                pos = GenerateRandomCell();
            }
            return pos;
        }
};

class Snake{
    public:
        deque<Vector2>body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        Vector2 direction = {1,0};
        bool addSeg = false;

        void Draw(){
            for(unsigned int i=0;i<body.size();i++){
                float x = body[i].x;
                float y = body[i].y;
                Rectangle seg = Rectangle{offSet+x*cellSize,offSet+y*cellSize,(float)cellSize,(float)cellSize};
                DrawRectangleRounded(seg,0.5,6,darkGreen);
            }
        }

        void Update(){
            body.push_front(Vector2Add(body[0],direction));
            if(addSeg == true){
                addSeg = false;
            }

            else{
                body.pop_back();
            }
        }

        void Reset(){
            body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
            direction = {1,0};
        }
};

class Game{
    public:
        bool running = true;
        Snake snake = Snake();
        Food food = Food(snake.body);
        int score = 0;
        Sound eatSound,wallSound;

        Game(){
            InitAudioDevice();
            eatSound = LoadSound("sounds/eat.mp3");
            wallSound = LoadSound("sounds/wall.mp3");
        }

        ~Game(){
            UnloadSound(eatSound);
            UnloadSound(wallSound);
            CloseAudioDevice();
        }

        void Draw(){
            food.Draw();
            snake.Draw();
        }

        void Update(){
            if(running){
                snake.Update();
                checkCollisionWithFood();
                checkCollisionWithEdges();
                checkCollisionWithSnake();
            }
        }

        void checkCollisionWithFood(){
            if(Vector2Equals(snake.body[0],food.position)){
                food.position = food.generateRandomPos(snake.body);
                snake.addSeg = true;
                score++;
                PlaySound(eatSound);
            }
        }

        void checkCollisionWithEdges(){
            if(snake.body[0].x == cellCount || snake.body[0].x == -1){
                GameOver();
            }
            if(snake.body[0].y == cellCount || snake.body[0].y == -1){
                GameOver();
            }
        }

        void checkCollisionWithSnake(){
            deque<Vector2>headlessBody = snake.body;
            headlessBody.pop_front();
            if(ElementOnSnake(snake.body[0],headlessBody)){
                GameOver();
            } 
        }

        void GameOver(){
            snake.Reset();
            food.position = food.generateRandomPos(snake.body);
            running = false;
            score = 0;
            PlaySound(wallSound);
        }
};

int main(){
    cout<<"Starting game...."<<endl;
    InitWindow(2*offSet + cellSize*cellCount,2*offSet +cellSize*cellCount,"Eat Snake");
    SetTargetFPS(60);

    Game game = Game();

    while(WindowShouldClose() == false){
        BeginDrawing();

        if(eventTrigger(0.2))game.Update();

        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0,-1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0,1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1,0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1,0};
            game.running = true;
        }

        // Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offSet-5,(float)offSet-5,(float)cellSize*cellCount+10
                                        ,(float)cellSize*cellCount+10},5,darkGreen);
        DrawText("Snake Game",offSet-5,20,40,darkGreen);
        DrawText(TextFormat("%i",game.score),640,20,40,darkGreen);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
// posY: offSet+cellSize*cellCount+10