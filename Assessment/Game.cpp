#include "iostream"
#include <algorithm>
#include "GamesEngineeringBase.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;




// ================== 公共：Projectile（如你已有可复用） ==================
struct Projectile {
    float x, y, vx, vy;
    int damage;
    bool fromHero, alive;
    Projectile() : x(0), y(0), vx(0), vy(0), damage(1), fromHero(false), alive(false) {}
    Projectile(float X, float Y, float VX, float VY, int dmg, bool hero)
        : x(X), y(Y), vx(VX), vy(VY), damage(dmg), fromHero(hero), alive(true) {
    }
};

// ================== 父类：BaseNPC（抽象） ==================
class BaseNPC {
protected:
    float x, y;
    float speed;
    int hp, maxHP;
    float radius;
    bool alive;
    GamesEngineeringBase::Image sprite;

public:
    BaseNPC() : x(0), y(0), speed(0), hp(1), maxHP(1), radius(16), alive(false) {}
    virtual ~BaseNPC() {}

    virtual void init(float startX, float startY) { x = startX; y = startY; alive = true; }
    virtual void update(float dt, float heroX, float heroY, Projectile* bullets, int& bulletCount, int MAX_PROJECTILES) = 0;
    virtual void draw(GamesEngineeringBase::Window& canvas, int camX, int camY) {
        if (!alive) return;
        int sx = (int)(x - camX), sy = (int)(y - camY);
        for (unsigned yy = 0; yy < sprite.height; ++yy)
            for (unsigned xx = 0; xx < sprite.width; ++xx) {
                if (sprite.alphaAtUnchecked(xx, yy) == 0) continue;
                int X = sx + xx, Y = sy + yy;
                if (0 <= X && X < (int)canvas.getWidth() && 0 <= Y && Y < (int)canvas.getHeight())
                    canvas.draw(X, Y, sprite.atUnchecked(xx, yy));
            }
    }

    virtual void takeDamage(int dmg) { hp -= dmg; if (hp <= 0) { hp = 0; alive = false; } }
    bool isAlive() const { return alive; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getRadius() const { return radius; }

protected:
    // 通用“直线追踪”行为
    void chase(float dt, float heroX, float heroY) {
        float dx = heroX - x, dy = heroY - y;
        float len = (float)sqrt(dx * dx + dy * dy);
        if (len > 0.001f) { dx /= len; dy /= len; x += dx * speed * dt; y += dy * speed * dt; }
    }
    void loadSprite(const char* file) {
        if (!sprite.load(file)) { /* 可加日志 */ }
    }
};

// ================== 子类 1：Normal ==================
class NormalNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 60; hp = maxHP = 3; radius = 16; loadSprite("Resources/npc_normal.png");
    }
    void update(float dt, float heroX, float heroY, Projectile*, int&, int) override {
        chase(dt, heroX, heroY);
    }
};

// ================== 子类 2：Runner ==================
class RunnerNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 120; hp = maxHP = 2; radius = 14; loadSprite("Resources/npc_runner.png");
    }
    void update(float dt, float heroX, float heroY, Projectile*, int&, int) override {
        chase(dt, heroX, heroY);
    }
};

// ================== 子类 3：Tank ==================
class TankNPC : public BaseNPC {
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 35; hp = maxHP = 6; radius = 20; loadSprite("Resources/npc_tank.png");
    }
    void update(float dt, float heroX, float heroY, Projectile*, int&, int) override {
        chase(dt, heroX, heroY);
    }
};

// ================== 子类 4：Turret（静止发射弹幕） ==================
class TurretNPC : public BaseNPC {
    float shootCD;
public:
    void init(float startX, float startY) override {
        BaseNPC::init(startX, startY);
        speed = 0; hp = maxHP = 4; radius = 18; loadSprite("Resources/npc_turret.png");
        shootCD = 1.2f;
    }
    void update(float dt, float heroX, float heroY, Projectile* bullets, int& bulletCount, int MAX_PROJECTILES) override {
        // 不移动，只发射子弹
        shootCD -= dt;
        if (shootCD <= 0.f && bulletCount < MAX_PROJECTILES) {
            shootCD = 1.2f;
            float dx = heroX - x, dy = heroY - y;
            float len = (float)sqrt(dx * dx + dy * dy); if (len < 0.001f) return;
            dx /= len; dy /= len;
            bullets[bulletCount++] = Projectile(x, y, dx * 200.f, dy * 200.f, 1, false);
        }
    }
};

// ================== NPCManager（固定数组，不用 STL） ==================
#define MAX_NPCS 200
#define MAX_PROJECTILES 500

class NPCManager {
    BaseNPC* slots[MAX_NPCS];   // 指向多态对象
    int count;

public:
    Projectile enemyBullets[MAX_PROJECTILES];
    int bulletCount;

    NPCManager() : count(0), bulletCount(0) {
        for (int i = 0; i < MAX_NPCS; ++i) slots[i] = 0;
        for (int i = 0; i < MAX_PROJECTILES; ++i) enemyBullets[i].alive = false;
    }

    ~NPCManager() { clear(); }

    void clear() {
        for (int i = 0; i < MAX_NPCS; ++i) { if (slots[i]) { delete slots[i]; slots[i] = 0; } }
        count = 0; bulletCount = 0;
    }

    // 在相机外随机生成：type = 0..3（Normal/Runner/Tank/Turret）
    void spawnOutsideCamera(int type, int camX, int camY, int camW, int camH) {
        int idx = findFree();
        if (idx < 0) return;

        float spawnX, spawnY;
        int margin = 100;
        int side = rand() % 4;
        switch (side) {
        case 0: spawnX = camX + (rand() % camW); spawnY = camY - margin; break;            // 上
        case 1: spawnX = camX + (rand() % camW); spawnY = camY + camH + margin; break;      // 下
        case 2: spawnX = camX - margin; spawnY = camY + (rand() % camH); break;             // 左
        default:spawnX = camX + camW + margin; spawnY = camY + (rand() % camH); break;      // 右
        }

        BaseNPC* p = 0;
        if (type == 0) p = new NormalNPC();
        else if (type == 1) p = new RunnerNPC();
        else if (type == 2) p = new TankNPC();
        else                p = new TurretNPC();

        p->init(spawnX, spawnY);
        slots[idx] = p;
        if (idx + 1 > count) count = idx + 1;
    }

    void updateAll(float dt, float heroWorldX, float heroWorldY) {
        for (int i = 0; i < count; ++i) {
            if (!slots[i]) continue;
            if (!slots[i]->isAlive()) { delete slots[i]; slots[i] = 0; continue; }
            slots[i]->update(dt, heroWorldX, heroWorldY, enemyBullets, bulletCount, MAX_PROJECTILES);
        }
        // 更新子弹
        for (int i = 0; i < bulletCount; ++i) {
            if (!enemyBullets[i].alive) continue;
            enemyBullets[i].x += enemyBullets[i].vx * dt;
            enemyBullets[i].y += enemyBullets[i].vy * dt;
            // 超出可简单失效（或做边界循环）
            if (enemyBullets[i].x < -1000 || enemyBullets[i].y < -1000) enemyBullets[i].alive = false;
        }
    }

    void drawAll(GamesEngineeringBase::Window& canvas, int camX, int camY) {
        // 敌人
        for (int i = 0; i < count; ++i) if (slots[i]) slots[i]->draw(canvas, camX, camY);
        // 敌人子弹（简单画点）
        for (int i = 0; i < bulletCount; ++i) {
            if (!enemyBullets[i].alive) continue;
            int sx = (int)(enemyBullets[i].x - camX);
            int sy = (int)(enemyBullets[i].y - camY);
            if (0 <= sx && sx < (int)canvas.getWidth() && 0 <= sy && sy < (int)canvas.getHeight())
                canvas.draw(sx, sy, 255, 0, 0);
        }
    }

    // Hero 被子弹击中（圆形碰撞）
    bool heroHitByBullet(float heroX, float heroY, float heroR) {
        bool hit = false;
        for (int i = 0; i < bulletCount; ++i) {
            if (!enemyBullets[i].alive) continue;
            float dx = heroX - enemyBullets[i].x, dy = heroY - enemyBullets[i].y;
            if (dx * dx + dy * dy < heroR * heroR) { enemyBullets[i].alive = false; hit = true; }
        }
        return hit;
    }

private:
    int findFree() {
        for (int i = 0; i < MAX_NPCS; ++i) if (slots[i] == 0) return i;
        return -1;
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