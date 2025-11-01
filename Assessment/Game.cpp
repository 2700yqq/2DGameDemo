#include "iostream"
#include <algorithm>
#include "GamesEngineeringBase.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;


class tile {
    GamesEngineeringBase::Image sprite;
public:
    tile() {}
    void load(std::string filename) {
        sprite.load(filename);
    }
    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (unsigned int i = 0; i < sprite.height; i++)
            // bounds checking goes here
            if (i + y > 0 && i + y < canvas.getHeight()) {
                for (unsigned int j = 0; j < sprite.width; j++) {
                    if (j + x > 0 && j + x < canvas.getWidth()) {
                        canvas.draw(x + j, y + i, sprite.atUnchecked(j, i));
                    }

                }
            }


    }
    unsigned int getHeight() { return sprite.height; }
    unsigned int getWidth() { return sprite.width; }
    GamesEngineeringBase::Image& getSprite() { return sprite; }

};


const unsigned int tileNum = 24;
class tileSet {
    tile t[tileNum];
public:
    // create and load tiles here
    tileSet(std::string pre = "") {

        for (unsigned int i = 0; i < tileNum; i++) {
            std::string filename;
            filename = "Resources/" + pre + std::to_string(i) + ".png";
            t[i].load(filename);
        }
    }
    // access individual tile here
    tile& operator[](unsigned int index) { return t[index]; }
};


const int worldSize = 1000;
class world {
    tileSet ts;
    unsigned int tilesWide = 0;
    unsigned int tilesHigh = 0;
    unsigned int tileWidth = 0;
    unsigned int tileHeight = 0;
    unsigned int** data;



public:
    world() : ts() {
        data = new unsigned int* [worldSize];
        for (int i = 0; i < worldSize; ++i)
            data[i] = new unsigned int[worldSize];


        tilesWide = worldSize;
        tilesHigh = worldSize;
        tileWidth = worldSize;
        tileHeight = worldSize;
        for (int i = 0; i < worldSize; i++) {
            for (int j = 0; j < worldSize; j++) {
                data[i][j] = rand() % tileNum;
            }
        }

    }

    //- File handling start

    world(const std::string& filename) : ts() {
        data = new unsigned int* [worldSize];
        for (int i = 0; i < worldSize; ++i)
            data[i] = new unsigned int[worldSize];


        tilesWide = worldSize;
        tilesHigh = worldSize;
        tileWidth = worldSize;
        tileHeight = worldSize;
        for (int i = 0; i < worldSize; i++) {
            for (int j = 0; j < worldSize; j++) {
                data[i][j] = rand() % tileNum;
            }
        }

        std::ifstream fin(filename);

        if (!fin.is_open()) {
            std::cerr << "Failed to open tile file: " << filename << std::endl;
            return;
        }

        std::string line;
        bool readingData = false;
        int index1 = 0;
        int index2 = 0;
        while (std::getline(fin, line)) {
            if (line.empty()) continue;

            if (line.rfind("layer", 0) == 0) {
                readingData = true;
                continue;
            }

            if (!readingData) {
                // 读取元数据部分
                std::stringstream ss(line);
                std::string key;
                ss >> key;

                if (key == "tileswide") ss >> tilesWide;
                else if (key == "tileshigh") ss >> tilesHigh;
                else if (key == "tilewidth") ss >> tileWidth;
                else if (key == "tileheight") ss >> tileHeight;

                //cout << "tilesWide: " << tilesWide << ";tilesHigh: " << tilesHigh << ";tileWidth: " << tileWidth << ";tileHeight: " << tileHeight << endl;

            }
            else {
                // 读取地图数据部分
                std::stringstream ss(line);
                string value;

                while (std::getline(ss, value, ',') && value != "") {
                    //data[0][0] = std::stoi(value);
                    data[index1][index2] = std::stoi(value);
                    //cout << value << endl;
                    index2++;
                }

                index1++;
                index2 = 0;
            }


        }

        fin.close();

        /*cout << index1 << " " << index2 << endl;
        for (int i = 0; i < index1; i++) {
            for (int j = 0; j < tilesHigh; j++) {
                cout << data[i][j] << " ";
            }
            cout << endl;
        }*/



    }

    int getPixelWidth()  const { return tilesWide * tileWidth; }
    int getPixelHeight() const { return tilesHigh * tileHeight; }

    //void draw(GamesEngineeringBase::Window& canvas, int camX, int camY) {
    //    for (int i = 0; i < tilesHigh; ++i) {
    //        for (int j = 0; j < tilesWide; ++j) {
    //            const unsigned int tileID = data[i][j];
    //            const int xPos = j * tileWidth - camX;
    //            const int yPos = i * tileHeight - camY;
    //            if (xPos + (int)tileWidth <= 0 || yPos + (int)tileHeight <= 0 ||
    //                xPos >= (int)canvas.getWidth() || yPos >= (int)canvas.getHeight()) {
    //                continue;
    //            }

    //            ts[tileID].draw(canvas, xPos, yPos);

    //            //std::cout << "Draw tile ID=" << tileID << " at (" << xPos << "," << yPos << ")\n";
    //        }
    //    }
    //}

    ~world() {
        for (int i = 0; i < worldSize; ++i)
            delete[] data[i];
        delete[] data;
    }

    //- File Handling end


    void draw(GamesEngineeringBase::Window& canvas, unsigned int x, unsigned int y) {

        unsigned int offset = tileWidth / 2;
        unsigned int X = x / 384;
        unsigned int Y = y / 384;

        for (int i = 0; i < tilesHigh; i++) {
            for (int j = 0; j < tilesWide; j++) {
                unsigned int tileID = data[i][j];

                // 计算绘制位置
                int yPos = i * tileHeight;
                int xPos = j * tileWidth;


                // 绘制该 tile
                //ts[tileID].setPosition(xPos, yPos);
                ts[tileID].draw(canvas, xPos, yPos);

                // Debug输出
                 //std::cout << "Draw tile ID=" << tileID << " at (" << xPos << "," << yPos << ")\n";
            }
        }
        /* ts[data[Y % tileHeight]].draw(canvas, (canvas.getHeight() / 2) + offset);
         ts[tarray[(Y + 1) % size]].draw(canvas, offset);
         ts[tarray[(Y + 2) % size]].draw(canvas, offset - (canvas.getHeight() / 2));*/
    }

    /* void drawAlphas(GamesEngineeringBase::Window& canvas, unsigned int y) {

         unsigned int offset = y % 384;
         unsigned int Y = y / 384;

         alphas[tarray[Y % size]].draw(canvas, (canvas.getHeight() / 2) + offset);
         alphas[tarray[(Y + 1) % size]].draw(canvas, offset);
         alphas[tarray[(Y + 2) % size]].draw(canvas, offset - (canvas.getHeight() / 2));
     }*/


     // this a simple first collision that just draws the line of collision
    /* void collision(GamesEngineeringBase::Window& canvas, Plane& h, unsigned int y) {

         int Y = y / 384;
         tile& T = alphas[tarray[Y % size]];

         unsigned int yCoord = T.getHeight() - ((h.getYCollide() + y) % T.getHeight());

         for (unsigned int x = 0; x < canvas.getWidth(); x++)
             if (T.getSprite().at(x, yCoord, 0) < 1)
                 canvas.draw(x, canvas.getHeight() - h.getYCollide(), 255, 0, 0);
     }

     void collision2(GamesEngineeringBase::Window& canvas, Plane& h, unsigned int y) {

         int Y = y / 384;
         tile& T = alphas[tarray[Y % size]];

         unsigned int yCoord = T.getHeight() - ((h.getYCollide() + y) % T.getHeight());

         for (unsigned int x = 0; x < h.getWidth(); x++)
             if (T.getSprite().at(x + h.getX(), yCoord, 0) < 1)
                 canvas.draw(x + h.getX(), canvas.getHeight() - h.getYCollide(), 255, 0, 0);
             else
                 canvas.draw(x + h.getX(), canvas.getHeight() - h.getYCollide(), 0, 255, 0);
     }*/
};


class Hero {
public:
    int x, y;
    int speed;
    GamesEngineeringBase::Image sprite;

    Hero(GamesEngineeringBase::Window& canvas, std::string filename) {
        sprite.load(filename);

        if (!sprite.load(filename)) {
            std::cerr << "Failed to load hero" << std::endl;
        }
        else {
            std::cerr << "Load hero!" << endl;
        }

        x = 640; y = 640;
        speed = 300;
    }



    void update(GamesEngineeringBase::Window& canvas, int _x, int _y) {
        x += _x;
        x = max(0, x);
        x = min(static_cast<int>(canvas.getWidth()) - static_cast<int>(sprite.width), x);

        y += _y;
        y = max(0, y);
        y = min(static_cast<int>(canvas.getHeight()) - static_cast<int>(sprite.height), y);

        // 限制在窗口范围内
       /* if (x < 0) x = 0;
        if (x > (int)canvas.getWidth() - (int)sprite.width)
            x = (int)canvas.getWidth() - (int)sprite.width;

        if (y < 0) y = 0;
        if (y > (int)canvas.getHeight() - (int)sprite.height)
            y = (int)canvas.getHeight() - (int)sprite.height;*/

    }

    void draw(GamesEngineeringBase::Window& canvas) {
        for (unsigned int yy = 0; yy < sprite.height; yy++) {
            for (unsigned int xx = 0; xx < sprite.width; xx++) {
                int screenX = x + xx;
                int screenY = y + yy;

                //防止越界写入
                if (screenX >= 0 && screenX < (int)canvas.getWidth() &&
                    screenY >= 0 && screenY < (int)canvas.getHeight()) {
                    canvas.draw(screenX, screenY, sprite.atUnchecked(xx, yy));
                }
            }
        }

    }

    void drawAt(GamesEngineeringBase::Window& canvas, int sx, int sy) {
        for (unsigned yy = 0; yy < sprite.height; ++yy)
            for (unsigned xx = 0; xx < sprite.width; ++xx) {
                int X = sx + xx, Y = sy + yy;
                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
                    canvas.draw(X, Y, sprite.atUnchecked(xx, yy));
            }

        //cout << "DrawAt" << endl;
    }

};


//struct Enemy {
//    int x, y;
//    float speed;
//    GamesEngineeringBase::Image sprite;
//
//    Enemy(GamesEngineeringBase::Window& w) {
//        sprite.load("Resources/enemy.png");
//
//        if (!sprite.load("Resources/enemy.png")) {
//            std::cerr << "Failed to load enemy.png" << std::endl;
//        }
//        else {
//            std::cerr << "Load enemy!" << endl;
//        }
//
//        x =(w.getWidth() - sprite.width) > 0 ?  rand() % (w.getWidth() - sprite.width) : rand();
//
//        y = -static_cast<int>(sprite.height);
//        speed = static_cast<float>(100 + rand() % 200);
//    }
//
//    void update(float dt, GamesEngineeringBase::Window& w) {
//        y += static_cast<int>(speed * dt);
//        if (y > w.getHeight()) { // 回到顶部
//            y = -static_cast<int>(sprite.height);
//            x = rand() % (w.getWidth() - sprite.width);
//            speed = static_cast<float>(100 + rand() % 200);
//        }
//    }
//
//    void draw(GamesEngineeringBase::Window& w) {
//        for (unsigned int yy = 0; yy < sprite.height; yy++) {
//            for (unsigned int xx = 0; xx < sprite.width; xx++) {
//                int screenX = x + xx;
//                int screenY = y + yy;
//
//                //防止越界写入
//                if (screenX >= 0 && screenX < (int)w.getWidth() &&
//                    screenY >= 0 && screenY < (int)w.getHeight()) {
//                        w.draw(screenX, screenY, sprite.atUnchecked(xx, yy));
//                }
//            }
//        }
//    }
//};

//class Game {
//public:
//    GamesEngineeringBase::Window canvas;
//    GamesEngineeringBase::Timer timer;
//    bool running = true;
//    Hero hero;
//    std::vector<Enemy> enemies;
//    GamesEngineeringBase::Image background;
//
//
//    Game() {
//        srand(static_cast<unsigned int>(time(0)));
//        canvas.create(1024, 768, "WM908 Game Framework");
//
//        background.load("Resources/landscape.png");
//        player = Hero();
//
//       /* enemies.push_back(Enemy(canvas));
//
//        enemies.push_back(Enemy(canvas));*/
//
//        /*for (int i = 0; i < 2; i++) {
//            enemies.push_back(Enemy(canvas));
//        }*/
//
//    }
//    void draw(GamesEngineeringBase::Window & canvas) {
//        for (int y = 0; y < canvas.getHeight(); y++) {
//            for (int x = 0; x < canvas.getWidth(); x++) {
//                // 计算背景图对应像素坐标（浮点比例）
//                int srcX = x * background.width / canvas.getWidth();
//                int srcY = y * background.height / canvas.getHeight();
//                canvas.draw(x, y, background.atUnchecked(srcX, srcY));
//            }
//        }
//    }
//
//    void run() {
//        while (running) {
//            float dt = timer.dt();
//            handleInput();
//            update(dt);
//            render();
//        }
//    }
//
//
//private:
//    void handleInput() {
//        // 可以在这里检测键盘输入，例如：
//        if (canvas.keyPressed(VK_ESCAPE))
//            running = false;
//    };
//
//    void update(float dt) {
//        player.update(canvas, dt);
//        for (auto& e : enemies) e.update(dt, canvas);
//    };
//
//    void render() {
//        canvas.clear();
//
//        draw(canvas);
//       
//        
//        /*for (auto& e : enemies){
//            e.draw(canvas);
//        }*/
//
//        player.draw(canvas);
//        canvas.present();
//    };
//};

struct Camera {
    int x; // top-left corner of what we see in world space
    int y;
    int width;
    int height;
};


// 用于子弹
struct Projectile {
    float x, y;         // 位置
    float vx, vy;       // 速度
    int damage;         // 伤害
    bool fromHero;      // 是否英雄子弹
    bool alive;         // 是否有效

    Projectile(float X, float Y, float VX, float VY, int dmg, bool hero)
        : x(X), y(Y), vx(VX), vy(VY), damage(dmg), fromHero(hero), alive(true) {
    }
};

// NPC类型枚举 
// TODO:修改
enum class NPCType { Normal, Runner, Tank, Turret };

class NPC {
public:
    float x, y;          // 世界坐标位置
    float speed;         // 移动速度
    int hp, maxHP;       // 当前与最大生命
    float radius;        // 碰撞半径
    NPCType type;        // 类型
    bool alive;          // 是否存活
    float shootCooldown; // 炮台型使用
    GamesEngineeringBase::Image sprite; // 精灵图像

public:
    NPC() : x(0), y(0), speed(0), hp(1), maxHP(1), radius(16),
        type(NPCType::Normal), alive(true), shootCooldown(0) {
    }

    // 生成不同类型NPC
    void init(NPCType t, float startX, float startY) {
        type = t;
        x = startX;
        y = startY;
        alive = true;

        switch (type) {
        case NPCType::Normal:
            sprite.load("Resources/npc_normal.png");
            speed = 60;
            hp = maxHP = 3;
            radius = 16;
            break;
        case NPCType::Runner:
            sprite.load("Resources/npc_runner.png");
            speed = 110;
            hp = maxHP = 2;
            radius = 14;
            break;
        case NPCType::Tank:
            sprite.load("Resources/npc_tank.png");
            speed = 35;
            hp = maxHP = 6;
            radius = 20;
            break;
        case NPCType::Turret:
            sprite.load("Resources/npc_turret.png");
            speed = 0;
            hp = maxHP = 4;
            radius = 18;
            shootCooldown = 1.0f;
            break;
        }
    }

    // 更新NPC逻辑
    void update(float dt, float heroX, float heroY, std::vector<Projectile>& enemyBullets) {
        if (!alive) return;

        if (type == NPCType::Turret) {
            // 炮台不移动，只发射子弹
            shootCooldown -= dt;
            if (shootCooldown <= 0.f) {
                shootCooldown = 1.2f; // 射速
                float dx = heroX - x;
                float dy = heroY - y;
                float len = sqrtf(dx * dx + dy * dy);
                dx /= len;
                dy /= len;
                enemyBullets.emplace_back(x, y, dx * 200.f, dy * 200.f, 1, false);
            }
        }
        else {
            // 其他类型NPC自动追踪英雄
            float dx = heroX - x;
            float dy = heroY - y;
            float len = sqrtf(dx * dx + dy * dy);
            if (len > 0.001f) {
                dx /= len;
                dy /= len;
                x += dx * speed * dt;
                y += dy * speed * dt;
            }
        }
    }

    // 绘制
    void draw(GamesEngineeringBase::Window& canvas, int camX, int camY) {
        if (!alive) return;

        int screenX = (int)(x - camX);
        int screenY = (int)(y - camY);

        // 超出屏幕不绘制（提高性能）
        if (screenX + (int)sprite.width < 0 || screenY + (int)sprite.height < 0 ||
            screenX >= (int)canvas.getWidth() || screenY >= (int)canvas.getHeight())
            return;

        for (unsigned int yy = 0; yy < sprite.height; yy++) {
            for (unsigned int xx = 0; xx < sprite.width; xx++) {
                int drawX = screenX + xx;
                int drawY = screenY + yy;
                if (drawX >= 0 && drawX < (int)canvas.getWidth() &&
                    drawY >= 0 && drawY < (int)canvas.getHeight()) {
                    canvas.draw(drawX, drawY, sprite.atUnchecked(xx, yy));
                }
            }
        }
    }

    // 被攻击
    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp <= 0) {
            alive = false;
            hp = 0;
        }
    }

    // 与英雄的简单碰撞检测
    bool collidesWith(float heroX, float heroY, float heroRadius) const {
        float dx = heroX - x;
        float dy = heroY - y;
        float r = radius + heroRadius;
        return dx * dx + dy * dy < r * r;
    }
};


class Manager {
    Hero hero;
    world w;
    unsigned int x = 0;
    unsigned int y = 0;
    float dt;
    Camera cam;

    //bool drawAlpha = false;
public:
    Manager(GamesEngineeringBase::Window& canvas) : hero(canvas, "Resources/L.png"), w() {}
    Manager(GamesEngineeringBase::Window& canvas, string filename) : hero(canvas, "Resources/L.png"), w(filename) {
        cam.width = canvas.getWidth();
        cam.height = canvas.getHeight();
        cam.x = hero.x - cam.width / 2;
        cam.y = hero.y - cam.height / 2;
    }
    Manager(GamesEngineeringBase::Window& canvas, string filename, float dt) : hero(canvas, "Resources/L.png"), w(filename), dt() {
        cam.width = canvas.getWidth();
        cam.height = canvas.getHeight();
        cam.x = hero.x - cam.width / 2;
        cam.y = hero.y - cam.height / 2;
    }


    void update(GamesEngineeringBase::Window& canvas) {
        /*int x = 0;
        y = 0;*/
        //scroll
        //y += 2;
        //drawAlpha = false;
        /*if (canvas.keyPressed(VK_UP)) y += 5;
        if (canvas.keyPressed(VK_DOWN)) y -= 1;
        if (canvas.keyPressed(VK_LEFT)) x -= 1;
        if (canvas.keyPressed(VK_RIGHT)) x += 1;*/
        //if (canvas.keyPressed('A')) drawAlpha = true;
        // 
        int x = 0, y = 0;
        if (canvas.keyPressed('W')) y -= 2;
        if (canvas.keyPressed('S')) y += 2;
        if (canvas.keyPressed('A')) x -= 2;
        if (canvas.keyPressed('D')) x += 2;

        hero.update(canvas, x, y);

        // Camera follows hero
        cam.x = hero.x - cam.width / 2;
        cam.y = hero.y - cam.height / 2;

        //cout << "Hero: " << hero.x << ", " << hero.y << endl;

        //cout << "Camera: " << cam.x << ", " << cam.y << endl;
        // //Clamp camera inside world bounds
        //if (cam.x < 0) cam.x = 0;
        //if (cam.y < 0) cam.y = 0;
        //if (cam.x + cam.width > w.getPixelWidth())
        //    cam.x = w.getPixelWidth() - cam.width;
        //if (cam.y + cam.height > w.getPixelHeight())
        //    cam.y = w.getPixelHeight() - cam.height;



    }
    void draw(GamesEngineeringBase::Window& canvas) {

        w.draw(canvas, x, y);
        hero.draw(canvas);

        //// 英雄始终画在屏幕中心
        //w.draw(canvas, cam.x, cam.y);
        //hero.drawAt(canvas, cam.width / 2 - hero.sprite.width / 2,
        //    cam.height / 2 - hero.sprite.height / 2);


    }

};

//int main() {
//
//    srand(static_cast<unsigned int>(time(nullptr)));
//    // Create a canvas window with dimensions 1024x768 and title "Example"
//    GamesEngineeringBase::Window canvas;
//    canvas.create(1344, 1344, "Assessment");
//
//    // Timer object to manage time-based events, such as movement speed
//    GamesEngineeringBase::Timer timer;
//    float dt = timer.dt();
//
//    //Manager m(canvas);
//    Manager m(canvas, "Resources/tiles.txt", dt);
//    while (true)
//    {
//        // Check for input (key presses or window events)
//        canvas.checkInput();
//        // Clear the window for the next frame rendering
//        canvas.clear();
//
//        if (canvas.keyPressed(VK_ESCAPE)) break;
//
//        m.update(canvas);
//        m.draw(canvas);
//
//        canvas.present();
//    }
//    return 0;
//
//}