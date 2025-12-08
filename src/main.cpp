#include <cgame.h> // download at https://github.com/mhassnaink/cgame/releases
// C++ STL
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

enum  ANIMSTATE
{
    GLIDING = 1, FLAPPING = 2
};
struct rect
{
    float x, y, w, h;
    bool      IsCollision (const rect &other) const
    {
        return !(x + w <= other.x || other.x + other.w <= x ||  
                 y + h <= other.y || other.y + other.h <= y); 
    }
};
struct Color { int r, g, b; }; // color struct
class Pillar
{
public :
    Pillar           () {} // empty constructor for memory protection
    ~Pillar          () {} // empty destructor  for memory protection
    void  Initialize ()
    {
        pillar = cgame.image.load ("resources/pillar.png");
        pillar = cgame.image.resize (&pillar, 100, 1620);
        for (int i = 0; i < 4; i++) 
        {
            pillarsXY[0][i] = 200.0f * i; // x position
            pillarsXY[1][i] = -350.0f;    // y position
        }
        for (auto& itr : pillarCollisionRect) { itr = {0.0f, 0.0f, pillarWidth, pillarHeight}; }
        time          =  0.0f;
        offset        = 20.0f; 
        score         =     0;
        preScore      =     0;
        preScoreCheck = false;
    }
    void  CleanUP     () { cgame.image.unload (&pillar); }
    void  Update     (float dt, bool running)
    {
        if (running)
        {
            time            += dt;
            float moveSpeed  = dt * 50.0f;
            for (int i = 0; i < 4; i++) 
            {
                pillarsXY    [0][i] -= moveSpeed;
                if (pillarsXY[0][i] <= -100.0f) 
                {
                    if (!preScoreCheck) { score = 4; preScoreCheck = true; preScore = 4; }
                    score    += 1;
                    preScore += 1;
                    pillarsXY[0][i] = pillarsXY[0][ (i + 3) % 4] + 200;
                    pillarsXY[1][i] = -400 + offset * sinf (time * 2.0f);
                    // std::cout << pillarsXY[1][i]  << "\n";
                }
            }
            if (preScore >= 5 && offset < 56.0f ) { preScore = 0; offset += 4.0f; }
            // now handling collision physics
            for (size_t i = 0; i < 4; i++)
            {
                pillarCollisionRect[(i * 2)    ] = {pillarsXY[0][i], pillarsXY[1][i]      , pillarWidth, pillarHeight};
                pillarCollisionRect[(i * 2) + 1] = {pillarsXY[0][i], pillarsXY[1][i] + 900, pillarWidth, pillarHeight};
            }
        }
    }
    void  Draw       ()
    {
        for (int i = 0; i < 4; i++)
        {
            cgame.image.draw (&pillar, pillarsXY[0][i], pillarsXY[1][i]);
        }
    }
    bool  CheckCollision (rect collisionBox)
    {
        for (const auto& itr : pillarCollisionRect) { if (itr.IsCollision (collisionBox)) { return true ; }}
        return false;
    }
private:
    CGameImage         pillar; 
    const float  pillarWidth  = 100.0f;
    const float  pillarHeight = 720.0f;
    const float  pillarSpaceX = 100.0f;
    const float  pollarSpaceY = 180.0f;
    float     pillarsXY[2][4]; // two rows five cols {for drawing}
    float                time;
    float              offset;
    int                 score; // also difficulty
    int              preScore;
    bool        preScoreCheck;
    std::vector<rect> pillarCollisionRect = std::vector<rect> (8); // 4 pillars but up and down acts as two seperate {for physics calculation}
};
// its the main game class
class Bird
{
public :
    Bird             () {} // empty constructor for memory protection
    ~Bird            () {} // empty destructor  for memory protection
    void  Initialize () 
    {
        bird               = cgame.image.load ("resources/bird.png"  );
        bird               = cgame.image.resize (&bird, (int)FrameWidth * 4, (int)FrameHeight);
        birdRect           = {0.0f  , 0.0f  , FrameWidth, FrameHeight}; 
        birdCollisionRect  = {100.0f, 404.0f, FrameWidth, FrameHeight - 8};
        scoreCollisionRect = {0.0f  , 0.0f  , FrameWidth, FrameHeight};
        time               =  0.0f;
        currentFrame       =     0;
        frameSpeed         =  6.5f;
        animCompleted      =  true;
        animState          = ANIMSTATE::GLIDING;
        velocityY          = 0.0f;
    }
    void  CleanUP    () { cgame.image.unload (&bird); }
    void  Update     (float dt, bool running) 
    {
        if (running)
        {
            if (cgame.key.just_pressed (cgame.K_SPACE) && animState == ANIMSTATE::GLIDING)
            {
                velocityY = flapStrength;
                animState = ANIMSTATE::FLAPPING;
            }
            velocityY           += gravity * dt;
            birdCollisionRect.y += velocityY * dt;
            if (animState == ANIMSTATE::FLAPPING)
            {
                time             += dt;
                currentFrame      = int (time * frameSpeed);
                if (currentFrame >= frames) { currentFrame = 0; time = 0.0f; animState = ANIMSTATE::GLIDING; }
                birdRect          =         { (float)currentFrame * FrameWidth, 0.0f, FrameWidth, FrameHeight};
            }
        }
    }
    void  Draw       ()         
    {
        cgame.image.draw_advanced (&bird, (int)birdCollisionRect.x, (int)birdCollisionRect.y - 4, (int)birdRect.x, (int)birdRect.y, (int)birdRect.w, (int)birdRect.h);
    }
    rect  GetBirdCollisionBox  ()
    {
        return birdCollisionRect;
    }
    rect  GetScoreCollisionBox ()
    {
        return scoreCollisionRect;
    }
private:
    CGameImage         bird;
    rect           birdRect;
    rect  birdCollisionRect;
    rect scoreCollisionRect;
    float              time;
    int        currentFrame;
    float        frameSpeed;  
    const float  FrameWidth  = 64.0f;
    const float  FrameHeight = 64.0f;
    const int         frames =     4; 
    bool      animCompleted;
    ANIMSTATE     animState;
    float         velocityY;             // vertical velocity
    const float      gravity =  450.0f;   // pixels per second^2
    const float flapStrength = -225.0f;  // upward impulse
};
class Window
{
public :
    Window  (int width, int height) 
    {
        this -> width  = width ;
        this -> height = height;
        running        = true  ;
        gameRunning    = true  ;
        bgImageX       = 0.0f  ;
        speed          = 8.0f  ;
        scoreCollision = false ;
        score          = 0     ;
        warmOrange     = {255, 200,   0};
        goldenYellow   = {255, 174,  66};
        cgame.init ();
        screen = cgame.display.set_mode (this->width, this->height, CGAME_FLAG_DPI_AWARE);
        (void)screen;
        cgame.display.center            ();
        cgame.time.set_fps              (60.0f);
        cgame.display.set_title         ("Flappy Bird (CGame Edition)");
        cgame.display.set_icon          ("resources/logo.ico");
        bgImage = cgame.image.load      ("resources/bg1.png" );
        bgImage = cgame.image.resize    (&bgImage, 720, 720);
        bird  .Initialize               ();
        pillar.Initialize               ();
        Run                             ();
    }
    ~Window ()
    {
        bird  .CleanUP     ();
        pillar.CleanUP     ();
        cgame.image.unload (&bgImage);
        cgame.quit         ();
    }
private:
    int   width, height, score;
    CGameScreen  screen;
    bool        running; // window running
    bool    gameRunning; // game   running
    CGameImage  bgImage;
    float            dt;
    float      bgImageX;
    float         speed;
    bool scoreCollision;
    Bird           bird;
    Pillar       pillar;
    Color    warmOrange, goldenYellow;
    void Run ()
    {
        while (running)
        {
            if (cgame.event.get () == cgame.QUIT) { running = false; continue; } 
            Update ();
            Draw   ();
        }
    }
    void Update ()
    {
        dt = cgame.time.get_dt ();
        bgImageX -= (dt * speed);
        if (bgImageX <= -720.0f) { bgImageX = 0.0f; }

        pillar.Update            (dt, gameRunning);
        bird  .Update            (dt, gameRunning);
        
        if (pillar.CheckCollision (bird.GetBirdCollisionBox  ())) { gameRunning = false; }
        if (pillar.CheckCollision (bird.GetScoreCollisionBox ()) && !scoreCollision) 
        {
            scoreCollision = true;
            score         +=    1;
        }
        else if (!pillar.CheckCollision (bird.GetScoreCollisionBox ()) && scoreCollision) { scoreCollision = false; }
        RestartGame (); // restart logic
    }
    void Draw   ()
    {
        cgame.display.set_bgcolor (245, 245, 250);
        cgame.display.clear       ();
        cgame.image.draw          (&bgImage, (int) bgImageX, 0);       // first background buffer
        cgame.image.draw          (&bgImage, (int) bgImageX + 719, 0); // second background buffer
        pillar.Draw               ();
        bird  .Draw               ();
        if (gameRunning == false) 
        { 
            cgame.text.draw_complex ("GAME OVER!", 78, 320, L"Arial", 50.0f, goldenYellow.r, goldenYellow.g, goldenYellow.b); 
            cgame.text.draw_complex ("PRESS SPACE TO RESTART", 0, 390, L"Arial", 34.0f, goldenYellow.r, goldenYellow.g, goldenYellow.b); 
        }
        cgame.text.draw_complex   ("SCORE:", 275, 10, L"Arial", 30.0f, warmOrange.r, warmOrange.g, warmOrange.b);                         // score drawing
        cgame.text.draw_complex   (std::to_string (score).c_str (), 400, 10, L"Arial",  30.0f, warmOrange.r, warmOrange.g, warmOrange.b); // score drawing
        cgame.display.flip        ();
    }
    void RestartGame ()
    {
        if (cgame.key.just_pressed (cgame.K_SPACE) && !gameRunning) 
        { 
            speed          = 10.0f ;
            scoreCollision = false ;
            score          = 0     ;
            gameRunning    = true  ; 
            bird  .Initialize ()   ; 
            pillar.Initialize ()   ; 
        }
    }
};
int main () { Window window (480, 720); } // main functions 🎮👍 
