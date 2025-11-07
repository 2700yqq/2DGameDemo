//#include <cmath>
//#include <iostream>
//#include "GamesEngineeringBase.h"
//#include "iostream"
//#include <algorithm>
//#include "GamesEngineeringBase.h"
//#include <vector>
//#include <cstdlib>
//#include <ctime>
//#include <fstream>
//#include <sstream>
//
//using namespace std;
//
//// ======== 坐标工具函数 ========
//inline float wrap(float v, float max) {
//    while (v < 0) v += max;
//    while (v >= max) v -= max;
//    return v;
//}
//
//inline float shortestDelta(float a, float b, float range) {
//    float d = a - b;
//    if (d > range / 2) d -= range;
//    if (d < -range / 2) d += range;
//    return d;
//}
//
//// ======== Camera ========
//struct Camera {
//    float x, y;
//    int width, height;
//
//    void follow(float heroX, float heroY, int spriteW, int spriteH, int mapW, int mapH, bool infinite) {
//        x = heroX + spriteW / 2 - width / 2;
//        y = heroY + spriteH / 2 - height / 2;
//
//        if (!infinite) {
//            if (x < 0) x = 0;
//            if (y < 0) y = 0;
//            if (x + width > mapW)  x = mapW - width;
//            if (y + height > mapH) y = mapH - height;
//        }
//        else {
//            x = wrap(x, mapW);
//            y = wrap(y, mapH);
//        }
//    }
//};
//
//// ======== Projectile ========
//struct Projectile {
//    float x, y, vx, vy;
//    int damage;
//    bool alive;
//    Projectile() : x(0), y(0), vx(0), vy(0), damage(1), alive(false) {}
//    Projectile(float X, float Y, float VX, float VY, int dmg)
//        : x(X), y(Y), vx(VX), vy(VY), damage(dmg), alive(true) {
//    }
//};
//
//// ======== NPC 基类 ========
//class BaseNPC {
//protected:
//    float x, y;
//    float speed;
//    unsigned int hp, maxHP;
//    bool alive;
//    float radius;
//
//
//    GamesEngineeringBase::Image sprite;
//
//public:
//    BaseNPC() : x(0), y(0), speed(0), hp(1), maxHP(1), alive(false) {}
//    virtual ~BaseNPC() {}
//
//    virtual void init(float startX, float startY) { x = startX; y = startY; alive = true; }
//    virtual void update(float dt, float heroX, float heroY, int mapW, int mapH, bool infiniteMode) = 0;
//
//    void draw(GamesEngineeringBase::Window& canvas, Camera& cam) {
//        if (!alive) return;
//        int sx = (int)(x - cam.x);
//        int sy = (int)(y - cam.y);
//        for (unsigned int yy = 0; yy < sprite.height; ++yy)
//            for (unsigned int xx = 0; xx < sprite.width; ++xx) {
//                if (sprite.alphaAtUnchecked(xx, yy) == 0) continue;
//                int X = sx + xx, Y = sy + yy;
//                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
//                    canvas.draw(X, Y, sprite.atUnchecked(xx, yy));
//            }
//    }
//
//    void takeDamage(int dmg) { if (alive) { hp -= dmg; if (hp <= 0) alive = false; } }
//    bool isAlive() const { return alive; }
//    float getX() const { return x; }
//    float getY() const { return y; }
//    
//    float getRadius() const { return radius; }
//
//protected:
//    void loadSprite(const char* file) { sprite.load(file); }
//
//    void chase(float dt, float heroX, float heroY, int mapW, int mapH, bool infiniteMode) {
//        float dx = shortestDelta(heroX, x, mapW);
//        float dy = shortestDelta(heroY, y, mapH);
//        float len = sqrt(dx * dx + dy * dy);
//        if (len > 0.001f) {
//            dx /= len; dy /= len;
//            x += dx * speed * dt;
//            y += dy * speed * dt;
//        }
//        if (infiniteMode) { x = wrap(x, mapW); y = wrap(y, mapH); }
//    }
//};
//
//// ======== NPC 子类 ========
//class NormalNPC : public BaseNPC {
//public:
//    void init(float sx, float sy) override {
//        BaseNPC::init(sx, sy);
//        loadSprite("Resources/npc1.png");
//        speed = 60; hp = maxHP = 3;
//    }
//    void update(float dt, float hx, float hy, int mapW, int mapH, bool inf) override {
//        chase(dt, hx, hy, mapW, mapH, inf);
//    }
//};
//
//class RunnerNPC : public BaseNPC {
//public:
//    void init(float sx, float sy) override {
//        BaseNPC::init(sx, sy);
//        loadSprite("Resources/npc2.png");
//        speed = 120; hp = maxHP = 2;
//    }
//    void update(float dt, float hx, float hy, int mapW, int mapH, bool inf) override {
//        chase(dt, hx, hy, mapW, mapH, inf);
//    }
//};
//
//class TankNPC : public BaseNPC {
//public:
//    void init(float sx, float sy) override {
//        BaseNPC::init(sx, sy);
//        loadSprite("Resources/npc3.png");
//        speed = 40; hp = maxHP = 6;
//    }
//    void update(float dt, float hx, float hy, int mapW, int mapH, bool inf) override {
//        chase(dt, hx, hy, mapW, mapH, inf);
//    }
//};
//
//// 静止炮塔
//class TurretNPC : public BaseNPC {
//    float shootCD;
//public:
//    void init(float sx, float sy) override {
//        BaseNPC::init(sx, sy);
//        loadSprite("Resources/npc4.png");
//        speed = 0; hp = maxHP = 4; shootCD = 1.5f;
//    }
//    void update(float dt, float hx, float hy, int mapW, int mapH, bool inf) override {
//        chase(dt, hx, hy, mapW, mapH, inf);
//        shootCD -= dt;
//        if (shootCD <= 0.f) {
//            shootCD = 1.5f;
//            cout << "Turret at (" << x << "," << y << ") fires projectile!\n";
//        }
//    }
//};
//
//// ======== NPC 管理器 ========
//#define MAX_NPCS 200
//class NPCManager {
//public:
//    BaseNPC* slots[MAX_NPCS];
//    int count;
//
//    NPCManager() : count(0) { for (int i = 0; i < MAX_NPCS; ++i) slots[i] = nullptr; }
//
//    int findFree() {
//        for (int i = 0; i < MAX_NPCS; ++i) if (slots[i] == nullptr) return i;
//        return -1;
//    }
//
//    void spawnOutsideCamera(int type, int camX, int camY, int camW, int camH, int mapW, int mapH, bool inf) {
//        int idx = findFree();
//        if (idx < 0) return;
//
//        float sx = 0, sy = 0;
//        int margin = 120;
//        int side = rand() % 4;
//
//        switch (side) {
//        case 0: sx = camX + rand() % camW; sy = camY - margin; break;
//        case 1: sx = camX + rand() % camW; sy = camY + camH + margin; break;
//        case 2: sx = camX - margin; sy = camY + rand() % camH; break;
//        case 3: sx = camX + camW + margin; sy = camY + rand() % camH; break;
//        }
//
//        if (inf) { sx = wrap(sx, mapW); sy = wrap(sy, mapH); }
//
//        BaseNPC* p = nullptr;
//        if (type == 0) p = new NormalNPC();
//        else if (type == 1) p = new RunnerNPC();
//        else if (type == 2) p = new TankNPC();
//        else p = new TurretNPC();
//
//        p->init(sx, sy);
//        slots[idx] = p;
//        if (idx + 1 > count) count = idx + 1;
//
//        cout << "Spawn NPC type " << type << " at WORLD (" << sx << "," << sy << ")\n";
//    }
//
//    void updateAll(float dt, float heroX, float heroY, int mapW, int mapH, bool inf) {
//        for (int i = 0; i < count; ++i) {
//            if (!slots[i]) continue;
//            if (!slots[i]->isAlive()) { delete slots[i]; slots[i] = nullptr; continue; }
//            slots[i]->update(dt, heroX, heroY, mapW, mapH, inf);
//        }
//    }
//
//    void drawAll(GamesEngineeringBase::Window& canvas, Camera& cam) {
//        for (int i = 0; i < count; ++i)
//            if (slots[i] && slots[i]->isAlive())
//                slots[i]->draw(canvas, cam);
//    }
//};
//
//class tile {
//    GamesEngineeringBase::Image sprite;
//public:
//    tile() {}
//    void load(std::string filename) {
//        sprite.load(filename);
//    }
//    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
//        for (unsigned int i = 0; i < sprite.height; i++)
//            // bounds checking goes here
//            if (i + y > 0 && i + y < canvas.getHeight()) {
//                for (unsigned int j = 0; j < sprite.width; j++) {
//                    if (j + x > 0 && j + x < canvas.getWidth()) {
//                        canvas.draw(x + j, y + i, sprite.atUnchecked(j, i));
//                    }
//
//                }
//            }
//
//
//    }
//    unsigned int getHeight() { return sprite.height; }
//    unsigned int getWidth() { return sprite.width; }
//    GamesEngineeringBase::Image& getSprite() { return sprite; }
//
//};
//
//
//const unsigned int tileNum = 24;
//class tileSet {
//    tile t[tileNum];
//public:
//    // create and load tiles here
//    tileSet(std::string pre = "") {
//
//        for (unsigned int i = 0; i < tileNum; i++) {
//            std::string filename;
//            filename = "Resources/" + pre + std::to_string(i) + ".png";
//            t[i].load(filename);
//        }
//    }
//    // access individual tile here
//    tile& operator[](unsigned int index) { return t[index]; }
//};
//
//
//
//const int worldSize = 1000;
//class world {
//    tileSet ts;
//    unsigned int tilesWide = 0;
//    unsigned int tilesHigh = 0;
//    unsigned int tileWidth = 0;
//    unsigned int tileHeight = 0;
//    unsigned int** data;
//    bool** isPassable;
//
//
//
//
//public:
//    unsigned int xIndex;
//    unsigned int yIndex;
//
//    world() : ts() {
//
//        tilesWide = worldSize;
//        tilesHigh = worldSize;
//        tileWidth = worldSize;
//        tileHeight = worldSize;
//
//        data = new unsigned int* [worldSize];
//        isPassable = new  bool* [worldSize];
//
//        for (int i = 0; i < worldSize; ++i) {
//            data[i] = new unsigned int[worldSize];
//            isPassable[i] = new bool[worldSize];
//
//        }
//
//        for (int i = 0; i < worldSize; i++) {
//            for (int j = 0; j < worldSize; j++) {
//                data[i][j] = rand() % tileNum;
//                isPassable[i][j] = true;
//            }
//        }
//
//
//
//
//
//    }
//
//    //- File handling start
//
//    world(const std::string& filename) : ts() {
//        tilesWide = worldSize;
//        tilesHigh = worldSize;
//        tileWidth = worldSize;
//        tileHeight = worldSize;
//
//        data = new unsigned int* [worldSize];
//        isPassable = new bool* [worldSize];
//
//        for (int i = 0; i < worldSize; ++i) {
//            data[i] = new unsigned int[worldSize];
//            isPassable[i] = new bool[worldSize];
//
//        }
//
//        for (int i = 0; i < worldSize; i++) {
//            for (int j = 0; j < worldSize; j++) {
//                data[i][j] = rand() % tileNum;
//                isPassable[i][j] = true;
//            }
//        }
//
//
//        std::ifstream fin(filename);
//
//        if (!fin.is_open()) {
//            std::cerr << "Failed to open tile file: " << filename << std::endl;
//            return;
//        }
//
//        std::string line;
//        bool readingData = false;
//        int index1 = 0;
//        int index2 = 0;
//        while (std::getline(fin, line)) {
//            if (line.empty()) continue;
//
//            if (line.rfind("layer", 0) == 0) {
//                readingData = true;
//                continue;
//            }
//
//            if (!readingData) {
//                // 读取元数据部分
//                std::stringstream ss(line);
//                std::string key;
//                ss >> key;
//
//                if (key == "tileswide") ss >> tilesWide;
//                else if (key == "tileshigh") ss >> tilesHigh;
//                else if (key == "tilewidth") ss >> tileWidth;
//                else if (key == "tileheight") ss >> tileHeight;
//
//                //cout << "tilesWide: " << tilesWide << ";tilesHigh: " << tilesHigh << ";tileWidth: " << tileWidth << ";tileHeight: " << tileHeight << endl;
//
//            }
//            else {
//                // 读取地图数据部分
//                std::stringstream ss(line);
//                string value;
//
//                while (std::getline(ss, value, ',') && value != "") {
//                    //data[0][0] = std::stoi(value);
//
//                    data[index1][index2] = std::stoi(value);
//
//                    if (stoi(value) >= 14 && stoi(value) <= 22) {
//                        //water impassable
//                        isPassable[index1][index2] = false;
//                    }
//                    else {
//                        isPassable[index1][index2] = true;
//                    }
//
//                    //cout << value << endl;
//                    index2++;
//                }
//
//                index1++;
//                index2 = 0;
//            }
//
//
//        }
//
//        fin.close();
//
//    }
//    int getPixelWidth()  const { return tilesWide * tileWidth; }
//    int getPixelHeight() const { return tilesHigh * tileHeight; }
//
//
//    // 根据像素坐标判断位置是否可走
//    bool Passable(unsigned int worldX, unsigned int worldY) {
//        int tileX = worldX / tileWidth % tilesWide;
//        int tileY = worldY / tileHeight % tilesHigh;
//        //cout << tileX << ", " << tileY << endl;
//
//        //// 限制范围
//        //if (tileX < 0 || tileY < 0 || tileX >= (int)tilesWide || tileY >= (int)tilesHigh)
//        //    return false; // 出界都视为不可通行
//
//        return isPassable[tileY][tileX];
//    }
//
//    ~world() {
//        for (int i = 0; i < worldSize; ++i)
//            delete[] data[i];
//        delete[] data;
//
//        for (int i = 0; i < worldSize; ++i)
//            delete[] isPassable[i];
//        delete[] isPassable;
//    }
//
//    //- File Handling end
//
//    void draw(GamesEngineeringBase::Window& canvas, unsigned int camX, unsigned int camY) {
//
//        int offsetX = camX % tileWidth;
//        int offsetY = camY % tileHeight;
//
//        xIndex = camX / tileWidth % tilesWide;
//        yIndex = camY / tileHeight % tilesHigh;
//
//        //cout << "start with" << xIndex << ", " << yIndex << endl;
//
//        for (int i = 0; i < tilesHigh; i++) {
//            for (int j = 0; j < tilesWide; j++) {
//
//                unsigned int x = (j + camX / tileWidth) % tilesWide;
//                unsigned int y = (i + camY / tileHeight) % tilesHigh;
//
//                unsigned int tileID = data[y][x];
//
//                // 计算绘制位置
//
//                int yPos = i * tileHeight - offsetY;
//                int xPos = j * tileWidth - offsetX;
//
//                //cout << xPos << ", " << yPos << endl;
//                // 绘制该 tile
//                //ts[tileID].setPosition(xPos, yPos);
//                ts[tileID].draw(canvas, xPos, yPos);
//
//                // Debug输出
//                //std::cout << "Draw tile ID=" << tileID << " at (" << xPos << "," << yPos << ")\n";
//            }
//        }
//
//    }
//
//
//
//
//
//
//
//    /* void drawAlphas(GamesEngineeringBase::Window& canvas, unsigned int y) {
//
//         unsigned int offset = y % 384;
//         unsigned int Y = y / 384;
//
//         alphas[tarray[Y % size]].draw(canvas, (canvas.getHeight() / 2) + offset);
//         alphas[tarray[(Y + 1) % size]].draw(canvas, offset);
//         alphas[tarray[(Y + 2) % size]].draw(canvas, offset - (canvas.getHeight() / 2));
//     }*/
//
//
//     // this a simple first collision that just draws the line of collision
//    /* void collision(GamesEngineeringBase::Window& canvas, Plane& h, unsigned int y) {
//
//         int Y = y / 384;
//         tile& T = alphas[tarray[Y % size]];
//
//         unsigned int yCoord = T.getHeight() - ((h.getYCollide() + y) % T.getHeight());
//
//         for (unsigned int x = 0; x < canvas.getWidth(); x++)
//             if (T.getSprite().at(x, yCoord, 0) < 1)
//                 canvas.draw(x, canvas.getHeight() - h.getYCollide(), 255, 0, 0);
//     }
//
//     void collision2(GamesEngineeringBase::Window& canvas, Plane& h, unsigned int y) {
//
//         int Y = y / 384;
//         tile& T = alphas[tarray[Y % size]];
//
//         unsigned int yCoord = T.getHeight() - ((h.getYCollide() + y) % T.getHeight());
//
//         for (unsigned int x = 0; x < h.getWidth(); x++)
//             if (T.getSprite().at(x + h.getX(), yCoord, 0) < 1)
//                 canvas.draw(x + h.getX(), canvas.getHeight() - h.getYCollide(), 255, 0, 0);
//             else
//                 canvas.draw(x + h.getX(), canvas.getHeight() - h.getYCollide(), 0, 255, 0);
//     }*/
//};
//
//
//#define MAX_HERO_PROJECTILES 300
//
//class Hero {
//public:
//    int x, y;
//    int speed;
//    int health;
//    int maxHealth;
//    int radius;
//    GamesEngineeringBase::Image sprite;
//
//    Hero(GamesEngineeringBase::Window& canvas, std::string filename) {
//        sprite.load(filename);
//
//        if (!sprite.load(filename)) {
//            std::cerr << "Failed to load hero" << std::endl;
//        }
//        else {
//            std::cerr << "Load hero! -- " << filename << endl;
//        }
//
//        x = (canvas.getWidth() - sprite.width) / 2;
//        y = (canvas.getHeight() - sprite.height) / 2;
//        //cout << "Hero: " << x << ", " << y << endl;
//        speed = 300;
//
//        maxHealth = 100;
//        health = maxHealth;
//        radius = sprite.width / 2;  // 碰撞半径
//
//    }
//
//
//    void back(GamesEngineeringBase::Window& canvas, int _x, int _y, int mapW, int mapH) {
//        x -= _x;
//        y -= _y;
//
//        if (x < 0) x += mapW;
//        if (y < 0) y += mapH;
//        if (x >= mapW) x -= mapW;
//        if (y >= mapH) y -= mapH;
//
//    }
//
//    void update(GamesEngineeringBase::Window& canvas, int _x, int _y, int mapW, int mapH) {
//
//        x += _x;
//        y += _y;
//        if (x < 0) x += mapW;
//        if (y < 0) y += mapH;
//        if (x >= mapW) x -= mapW;
//        if (y >= mapH) y -= mapH;
//
//        // 限制在窗口范围内
//       /* if (x < 0) x = 0;
//        if (x > (int)canvas.getWidth() - (int)sprite.width)
//            x = (int)canvas.getWidth() - (int)sprite.width;
//
//        if (y < 0) y = 0;
//        if (y > (int)canvas.getHeight() - (int)sprite.height)
//            y = (int)canvas.getHeight() - (int)sprite.height;*/
//
//    }
//
//    void draw(GamesEngineeringBase::Window& canvas) {
//        for (unsigned int yy = 0; yy < sprite.height; yy++) {
//            for (unsigned int xx = 0; xx < sprite.width; xx++) {
//
//                if (sprite.alphaAtUnchecked(xx, yy) == 0)
//                    continue; // 跳过透明区域
//
//                int screenX = x + xx;
//                int screenY = y + yy;
//
//                //防止越界写入
//                if (screenX >= 0 && screenX < (int)canvas.getWidth() &&
//                    screenY >= 0 && screenY < (int)canvas.getHeight()) {
//                    canvas.draw(screenX, screenY, sprite.atUnchecked(xx, yy));
//
//
//                }
//            }
//        }
//
//    }
//
//    void drawAt(GamesEngineeringBase::Window& canvas, int sx, int sy) {
//        for (unsigned yy = 0; yy < sprite.height; ++yy)
//            for (unsigned xx = 0; xx < sprite.width; ++xx) {
//                if (sprite.alphaAtUnchecked(xx, yy) == 0)
//                    continue; // 跳过透明区域
//                int X = sx + xx, Y = sy + yy;
//                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
//                    canvas.draw(X, Y, sprite.atUnchecked(xx, yy));
//            }
//
//        //cout << "DrawAt" << endl;
//    }
//
//
//    //hero受到伤害（碰撞 子弹）
//    void takeDamage(int dmg) {
//        health -= dmg;
//
//        if (health < 0) health = 0;
//        std::cout << "Hero hit! HP = " << health << std::endl;
//    }
//
//    bool isDead() const { return health <= 0; }
//
//
//};
//
//
//class Manager {
//    Hero hero;
//    world w;
//    unsigned int x = 0;
//    unsigned int y = 0;
//
//    Camera cam;
//    bool infiniteMode = true;
//    int score; // 分数统计
//
//
//
//    int mapW;
//    int mapH;
//
//
//    //NPC-related
//    NPCManager npcs;
//
//    //Projectile heroProjectiles[MAX_PROJECTILES];
//    float npcSpawnTimer;
//    float npcSpawnInterval;
//    unsigned int gameTime;
//
//    //herobullets
//    Projectile heroBullets[MAX_HERO_PROJECTILES];
//    int heroBulletCount = 0;
//    float heroShootCD = 0.f;
//    float heroShootInterval = 0.8f; // 英雄射速
//
//
//public:
//
//    bool gameOver;
//    float gameOverTimer;
//
//    Manager(GamesEngineeringBase::Window& canvas) : hero(canvas, "Resources/plane.png"), w() {}
//    Manager(GamesEngineeringBase::Window& canvas, string filename) : hero(canvas, "Resources/hero1.png"), w(filename), score(0), gameOver(false), gameOverTimer(0.0f) {
//        cam.width = canvas.getWidth();
//        cam.height = canvas.getHeight();
//        /*cam.x = hero.x - w.getPixelWidth() / 2;
//        cam.y = hero.y - w.getPixelHeight() / 2;*/
//        cam.x = 0;
//        cam.y = 0;
//
//
//        heroBulletCount = 0;
//        heroShootCD = 0;
//        npcSpawnTimer = 0;
//        npcSpawnInterval = 3.0f; // 初始3秒生成敌人
//
//        gameTime = 0;
//        mapW = w.getPixelWidth();
//        mapH = w.getPixelHeight();
//    }
//    Manager(GamesEngineeringBase::Window& canvas, string filename, float _dt) : hero(canvas, "Resources/hero1.png"), w(filename) {
//
//
//    }
//
//    int getScore() {
//        return score;
//    }
//
//    int getHp() {
//        //cout << hero.health;
//        return hero.health;
//    }
//
//    int getMaxHp() {
//        return hero.maxHealth;
//    }
//
//
//    void switchMapMode(GamesEngineeringBase::Window& canvas) {
//        static bool switchPressed = false;
//        if (canvas.keyPressed('M')) {
//            if (!switchPressed) { // 防止连按
//                infiniteMode = !infiniteMode;
//                cout << "Map mode switched to : "
//                    << (infiniteMode ? "Infinite" : "Fixed Boundary") << endl;
//            }
//            switchPressed = true;
//        }
//        else switchPressed = false;
//    }
//
//    bool isPassable(int X, int Y, int width, int height) {
//
//        // 检查英雄矩形的四个角
//        int corners[4][2] = {
//            {X, Y},
//            {X + width, Y},
//            {X, Y + height},
//            {X + width, Y + height}
//        };
//
//
//        for (int i = 0; i < 4; ++i) {
//            int cx = corners[i][0];
//            int cy = corners[i][1];
//            if (!w.Passable(cx, cy))
//                return false; // 只要有一个角在水 tile，就视为不可通行
//        }
//        return true;
//
//    }
//
//    int updateX(int _x) {
//        // 无限循环模式
//        /*mapW = w.getPixelWidth();
//        mapH = w.getPixelHeight();*/
//
//        if (_x < 0) _x += mapW;
//        //if (_y < 0) _y += mapH;
//        if (_x >= mapW) _x -= mapW;
//        //if (_y > mapH) _y -= mapH;
//
//        return _x;
//
//    }
//
//    int updateY(int _y) {
//        // 无限循环模式
//        /*mapW = w.getPixelWidth();
//        mapH = w.getPixelHeight();*/
//
//        //if (_x < 0) _x += mapW;
//        if (_y < 0) _y += mapH;
//        //if (_x > mapW) _x -= mapW;
//        if (_y >= mapH) _y -= mapH;
//
//        return _y;
//
//    }
//
//    void updateHeroShoot(float dt, float heroX, float heroY, Projectile heroBullets[], int& heroBulletCount, int MAX_BULLETS, NPCManager& npcManager, float mapW, float mapH, bool infiniteMode) {
//        static float heroShootCD = 0.f;
//        heroShootCD -= dt;
//
//        if (heroShootCD <= 0.f) {
//            float minDist2 = 1e18f;
//            float targetX = 0, targetY = 0;
//
//            for (int i = 0; i < MAX_NPCS; ++i) {
//                BaseNPC* e = npcManager.slots[i];
//                if (!e || !e->isAlive()) continue;
//
//                float dx = shortestDelta(e->getX(), heroX, mapW);
//                float dy = shortestDelta(e->getY(), heroY, mapH);
//                float d2 = dx * dx + dy * dy;
//                if (d2 < minDist2) { minDist2 = d2; targetX = e->getX(); targetY = e->getY(); }
//            }
//
//            if (minDist2 < 1e17f && heroBulletCount < MAX_BULLETS) {
//                float dx = shortestDelta(targetX, heroX, mapW);
//                float dy = shortestDelta(targetY, heroY, mapH);
//                float len = sqrtf(dx * dx + dy * dy);
//                if (len > 0.001f) { dx /= len; dy /= len; }
//
//                int slot = -1;
//                for (int i = 0; i < MAX_BULLETS; ++i) {
//                    if (!heroBullets[i].alive) { slot = i; break; }
//                }
//                if (slot >= 0) {
//                    //heroBullets[slot] = Projectile(heroX, heroY, dx * 400.f, dy * 400.f, 1, true);
//                    if (slot >= heroBulletCount) heroBulletCount = slot + 1;
//                    heroShootCD = 0.6f;
//                    std::cout << "Hero bullet WORLD start: " << heroX << ", " << heroY << "\n";
//                }
//            }
//        }
//    }
//
//
//
//
//    void update(GamesEngineeringBase::Window& canvas, float dt) {
//        // 英雄移动
//        int dx = 0, dy = 0;
//        if (canvas.keyPressed('W')) dy -= 1;
//        if (canvas.keyPressed('S')) dy += 1;
//        if (canvas.keyPressed('A')) dx -= 1;
//        if (canvas.keyPressed('D')) dx += 1;
//
//        hero.x += dx * hero.speed * dt;
//        hero.y += dy * heroS * dt;
//        if (infinite) { heroX = wrap(heroX, mapW); heroY = wrap(heroY, mapH); }
//
//        // 相机跟随
//        cam.follow(heroX, heroY, heroImg.width, heroImg.height, mapW, mapH, infinite);
//
//        // 敌人生成
//        spawnTimer += dt;
//        if (spawnTimer >= spawnInterval) {
//            int t = rand() % 4;
//            npcs.spawnOutsideCamera(t, cam.x, cam.y, cam.width, cam.height, mapW, mapH, infinite);
//            spawnTimer = 0;
//        }
//
//        // 更新敌人
//        npcs.updateAll(dt, heroX, heroY, mapW, mapH, infinite);
//
//        //std::cout << "Hero.x=" << hero.x << " Cam.x=" << cam.x << " BulletStartX=" << heroWorldX << std::endl;
//
//    }
//
//
//    // ===== 绘制虚拟相机边框 =====
//    void drawCameraBox(GamesEngineeringBase::Window& canvas, Camera cam) {
//        int cx = 0;                // 相机在屏幕左上角（视图原点）
//        int cy = 0;
//        int cw = cam.width;
//        int ch = cam.height;
//
//        // 边框颜色（蓝色）
//        unsigned char R = 50, G = 120, B = 255;
//
//        // 画外框
//        for (int x = 0; x < cw; ++x) {
//            if (cy >= 0 && cy < (int)canvas.getHeight()) {
//                canvas.draw(cx + x, cy, R, G, B);             // 上边
//                canvas.draw(cx + x, cy + ch - 1, R, G, B);    // 下边
//            }
//        }
//        for (int y = 0; y < ch; ++y) {
//            if (cx >= 0 && cx < (int)canvas.getWidth()) {
//                canvas.draw(cx, cy + y, R, G, B);             // 左边
//                canvas.draw(cx + cw - 1, cy + y, R, G, B);    // 右边
//            }
//        }
//
//        // 中心十字线（表示相机中心）
//        int centerX = cw / 2;
//        int centerY = ch / 2;
//        for (int i = -10; i <= 10; ++i) {
//            if (centerX + i >= 0 && centerX + i < (int)canvas.getWidth())
//                canvas.draw(centerX + i, centerY, 255, 255, 0); // 黄线横
//            if (centerY + i >= 0 && centerY + i < (int)canvas.getHeight())
//                canvas.draw(centerX, centerY + i, 255, 255, 0); // 黄线竖
//        }
//    }
//
//
//    void draw(GamesEngineeringBase::Window& canvas) {
//
//
//        // 绘制英雄
//        int hx = (int)(hero.x - cam.x);
//        int hy = (int)(hero.y - cam.y);
//        for (unsigned int yy = 0; yy < hero.sprite.height; yy++)
//            for (unsigned int xx = 0; xx < hero.sprite.width; xx++) {
//                if (hero.sprite.alphaAtUnchecked(xx, yy) == 0) continue;
//                int X = hx + xx, Y = hy + yy;
//                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
//                    canvas.draw(X, Y, hero.sprite.atUnchecked(xx, yy));
//            }
//
//        // 绘制敌人
//        npcs.drawAll(canvas, cam);
//
//        //drawCameraBox(canvas, cam);
//
//    }
//
//    void drawUI(GamesEngineeringBase::Window& canvas, int score, float fps, int heroHP, int heroMaxHP) {
//        // =============================
//        // 🔹 1. 血量条区域设置
//        // =============================
//        int barX = 20;
//        int barY = 20;
//        int barWidth = 200;
//        int barHeight = 16;
//
//        // 背景条（暗红）
//        for (int y = 0; y < barHeight; ++y) {
//            for (int x = 0; x < barWidth; ++x)
//                canvas.draw(barX + x, barY + y, 60, 0, 0);
//        }
//
//        // =============================
//        // 🔹 2. 当前血量条宽度计算
//        // =============================
//        float ratio = (float)heroHP / (float)heroMaxHP;
//        if (ratio < 0) ratio = 0;
//        if (ratio > 1) ratio = 1;
//        int filled = (int)(barWidth * ratio);
//
//        // =============================
//        // 🔹 3. 动态颜色过渡（绿→黄→红）
//        // =============================
//        int r, g;
//        if (ratio > 0.5f) {           // 绿色到黄色
//            r = (int)(255 * (1.0f - ratio) * 2.0f);
//            g = 255;
//        }
//        else {                      // 黄色到红色
//            r = 255;
//            g = (int)(255 * ratio * 2.0f);
//        }
//
//        // =============================
//        // 🔹 4. 绘制当前血量条
//        // =============================
//        for (int y = 0; y < barHeight; ++y) {
//            for (int x = 0; x < filled; ++x)
//                canvas.draw(barX + x, barY + y, r, g, 0);
//        }
//
//        // 边框（浅灰线）
//        for (int x = 0; x < barWidth; ++x) {
//            canvas.draw(barX + x, barY, 180, 180, 180);
//            canvas.draw(barX + x, barY + barHeight - 1, 180, 180, 180);
//        }
//        for (int y = 0; y < barHeight; ++y) {
//            canvas.draw(barX, barY + y, 180, 180, 180);
//            canvas.draw(barX + barWidth - 1, barY + y, 180, 180, 180);
//        }
//
//        // =============================
//        // 🔹 5. 血量文字显示
//        // =============================
//
//        char hpText[32];
//        sprintf_s(hpText, sizeof(hpText), "HP: %d / %d", heroHP, heroMaxHP);
//        //drawString(canvas, barX + 5, barY + 22, hpText, 255, 255, 255, 2, 1);
//
//        // =============================
//        // 🔹 6. 显示 “Score”
//        // =============================
//        char bufScore[32];
//        int s = score, digits[12], dcnt = 0;
//        if (s == 0) digits[dcnt++] = 0;
//        while (s > 0 && dcnt < 12) { digits[dcnt++] = s % 10; s /= 10; }
//        int idx = 0;
//        bufScore[idx++] = 'S'; bufScore[idx++] = 'c'; bufScore[idx++] = 'o';
//        bufScore[idx++] = 'r'; bufScore[idx++] = 'e'; bufScore[idx++] = ':';
//        bufScore[idx++] = ' ';
//        for (int i = dcnt - 1; i >= 0; --i) bufScore[idx++] = '0' + digits[i];
//        bufScore[idx] = '\0';
//        //drawString(canvas, canvas.getWidth() - 280, 20, bufScore, 255, 255, 255, 2, 2);
//
//        // =============================
//        // 🔹 7. 显示 “FPS”
//        // =============================
//        int ifps = (int)(fps + 0.5f);
//        int fd[6]; int fcnt = 0; if (ifps == 0) fd[fcnt++] = 0;
//        while (ifps > 0 && fcnt < 6) { fd[fcnt++] = ifps % 10; ifps /= 10; }
//        char bufFPS[24]; idx = 0;
//        bufFPS[idx++] = 'F'; bufFPS[idx++] = 'P'; bufFPS[idx++] = 'S';
//        bufFPS[idx++] = ':'; bufFPS[idx++] = ' ';
//        for (int i = fcnt - 1; i >= 0; --i) bufFPS[idx++] = '0' + fd[i];
//        bufFPS[idx] = '\0';
//        //drawString(canvas, canvas.getWidth() - 280, 50, bufFPS, 255, 220, 40, 2, 2);
//    }
//
//
//
//
//    void gameOverScreen(GamesEngineeringBase::Window& canvas, float dt) {
//        // 半透明黑背景
//        for (int y = 0; y < (int)canvas.getHeight(); y++)
//            for (int x = 0; x < (int)canvas.getWidth(); x++)
//                canvas.draw(x, y, 0, 0, 100);  // RGBA简写，若不支持透明，就直接黑色
//
//        // 居中绘制 GAME OVER 字样（用 drawString 函数）
//        int centerX = (int)canvas.getWidth() / 2 - 120;
//        int centerY = (int)canvas.getHeight() / 2 - 20;
//        //drawString(canvas, centerX, centerY, "GAME OVER", 255, 0, 0, 4, 2);
//
//        // 显示提示
//        //drawString(canvas, centerX - 60, centerY + 60, "Press ESC to Exit", 255, 255, 255, 2, 1);
//
//        // 倒计时退出
//        gameOverTimer -= dt;
//    }
//
//
//
//
//private:
//    // === 工具函数 ===
//    int findClosestEnemy() {
//        float hx = cam.x + hero.x;
//        float hy = cam.y + hero.y;
//        float bestDist = 1e9;
//        int bestIdx = -1;
//        for (int i = 0; i < MAX_NPCS; ++i) {
//            if (!npcManagerSlotAlive(i)) continue;
//            float dx = npcManagerEnemyX(i) - hx;
//            float dy = npcManagerEnemyY(i) - hy;
//            float dist = dx * dx + dy * dy;
//            if (dist < bestDist) { bestDist = dist; bestIdx = i; }
//        }
//        return bestIdx;
//    }
//
//    bool npcManagerSlotAlive(int i) {
//        return npcManager.slots[i] && npcManager.slots[i]->isAlive();
//    }
//    float npcManagerEnemyX(int i) { return npcManager.slots[i]->getX(); }
//    float npcManagerEnemyY(int i) { return npcManager.slots[i]->getY(); }
//    void npcManagerDamage(int i, int dmg) { npcManager.slots[i]->takeDamage(dmg); }
//
//
//};
//
//
//
//// ======== 示例主循环 ========
//int main() {
//    GamesEngineeringBase::Window canvas;
//    canvas.create(1024, 768, "World Coordinate System Demo");
//    GamesEngineeringBase::Timer timer;
//
//    float dt = 0;
//    bool infinite = true;
//    int mapW = 4096, mapH = 4096;
//
//    Camera cam{ 0, 0, 1024, 768 };
//    NPCManager npcs;
//
//    /*float heroX = 2000, heroY = 2000;
//    float heroSpeed = 200;
//    GamesEngineeringBase::Image heroImg;
//    heroImg.load("Resources/hero1.png");*/
//
//    float spawnTimer = 0, spawnInterval = 2.f;
//    Manager m(canvas, "Resources/tiles.txt");
//    while (true) {
//        dt = timer.dt();
//        canvas.checkInput();
//        canvas.clear();
//        
//        m.update(canvas, dt);
//        m.draw(canvas, npcs);
//       
//       
//
//        canvas.present();
//        if (canvas.keyPressed(VK_ESCAPE)) break;
//    }
//    return 0;
//}
//
//
//int main() {
//
//    srand(static_cast<unsigned int>(time(nullptr)));
//    // Create a canvas window with dimensions 1024x768 and title "Example"
//    GamesEngineeringBase::Window canvas;
//    canvas.create(1024, 1024, "Assessment");
//
//    // Timer object to manage time-based events, such as movement speed
//    GamesEngineeringBase::Timer timer;
//    float dt = timer.dt();
//
//    float fps = 0.0f;
//
//    Manager m(canvas, "Resources/tiles.txt");
//    while (true)
//    {
//        // Check for input (key presses or window events)
//        canvas.checkInput();
//        // Clear the window for the next frame rendering
//        canvas.clear();
//
//        if (canvas.keyPressed(VK_ESCAPE)) break;
//
//        dt = timer.dt();
//        if (dt > 0.0f) {
//            fps = 1.0f / dt;
//        }
//
//        if (!m.gameOver) {
//            m.update(canvas, dt);
//            m.draw(canvas);
//
//            m.drawUI(canvas, /*score=*/m.getScore(), fps, /*HP=*/m.getHp(), /*MaxHP=*/m.getMaxHp());
//        }
//        else {
//            m.gameOverScreen(canvas, dt);
//        }
//
//
//        canvas.present();
//    }
//    return 0;
//
//}
