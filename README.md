# Game Project Report

## 1. Introduction 

This project presents the design and implementation of a 2D survivor-style game developed in C++ using the *GamesEngineeringBase* framework. The game features a hero character controlled by the player alongside four distinct NPC characters generated randomly. The goal is to survive as long as possible and get higher scores by defeating waves of enemies while avoiding bullets. The game comprises two levels: the first is an infinite map mode, the second a fixed-boundary map mode. Each stage lasts for two minutes.The implemented system incorporates multiple game mechanics including a dynamic camera, four unique types of non-player characters (NPCs), collision detection, player attack systems, two distinct map modes (infinite and fixed boundaries), and a complete runtime system that supports saving and loading. This report details each subsystem, discusses its theoretical background, describes implementation strategies, and evaluates performance and limitations.

Players control the hero's movement up, down, left and right using the WASD keys. The hero will automatically fire bullets at the nearest enemy. Pressing the Q key unleashes an AOE attack, while pressing P saves the game state (displaying "GAME SAVE" at the top centre of the screen upon successful save). Pressing L loads the last saved game state to continue (displaying "GAME LOAD" at the top centre upon successful load). Power-up items spawn randomly across the map: blue items increase bullet attack speed, while pink items expand AoE damage radius. Each level lasts two minutes, with two levels total. The first level features an infinite map, while the second level employs a fixed map layout.

## 2. Technologies implement

### 2.1 Virtual Camera System

In the code,there is a structure named `Camera` which stores `x, y,width,height  `.

```cpp
struct Camera {
    int x; 
    int y;
    unsigned int width;
    unsigned int height;  
};
```

The virtual camera  follows the player-controlled (hero) character at its centre and defines the portion of a large world which is visible on the screen.It maps world coordinates to screen coordinates, allowing for a scrolling view.The relationship between the world and the screen is defined as:

```cpp
screenX = worldX - cameraX;
screenY = worldY - cameraY;
```

The main loop continuously updates camera position to follow the hero.

```cpp
cam.x = hero.x + hero.sprite.width / 2 - canvas.getWidth() / 2;
cam.y = hero.y + hero.sprite.height / 2 - canvas.getHeight() / 2;
```

Two operational modes were developed:

1. **Infinite World Mode**: The map wraps around both horizontally and vertically using modular arithmetic:

   ```cpp
   x = (x + mapW) % mapW;
   y = (y + mapH) % mapH;
   ```

   As this calculation will be frequently used, it is encapsulated as an inline function  `wrap()`:

   ```cpp
   inline float wrap(float v, float max) {
       while (v < 0) v += max;
       while (v >= max) v -= max;
       return v;
   }
   ```

2. **Fixed Boundary Mode**: The camera clamps to world edges to prevent revealing empty space outside the map.

### 2.2  NPC System

#### 2.2.1 NPC class 

Using **object-oriented programming**, 4 different enemy types inherit from a base class `BaseNPC`, supporting polymorphism.

`BaseNPC` is an enemy base class (non-player character) encapsulating position, health, speed, collision radius, sprite sheet, and an inherent projectile array (Projectile bullets[MAX_BULLETS]), alongside generic functions for handling bullet updates, rendering, hits, and damage. Four subclasses implement distinct behaviours: standard pursuit, rapid pursuit, tank-like slow movement with high health, and stationary turrets firing projectiles at the hero.

This design places "generic NPC behaviour and data" within the base class, reserving "specific behaviours" for implementation within subclass update() methods. This facilitates polymorphic management of a group of `BaseNPC` instances or arrays within the game.

```cpp
virtual void update(float dt, float heroX, float heroY, int mapW, int mapH, bool inf) = 0;
virtual void draw(GamesEngineeringBase::Window& canvas, int camX, int camY, int mapW, int mapH);
```

-  `update()` is a pure virtual function that must be implemented by subclasses: updating position, bullet firing logic, etc. Parameter meanings: dt represents frame time, heroX/heroY denote hero position, mapW/mapH indicate map dimensions (used for infinite maps or wrapping logic), inf specifies whether in "infinite/wrapping map" mode (true enables coordinate wrapping in infinite mode).
- `draw()` has a default implementation that renders sprites and their bullets. It may be overridden by subclasses to implement more specialised rendering .

```c++
void updateBullets(float dt, int mapW, int mapH, bool inf) {
    for (int i = 0; i < bulletCount; ++i) {
        if (!bullets[i].alive) continue;
        bullets[i].x += bullets[i].vx * dt;
        bullets[i].y += bullets[i].vy * dt;
        if (inf) {
            bullets[i].x = wrap(bullets[i].x, mapW);
            bullets[i].y = wrap(bullets[i].y, mapH);
        }
        else {
            if (bullets[i].x < 0 || bullets[i].x >= mapW ||
                bullets[i].y < 0 || bullets[i].y >= mapH)
                bullets[i].alive = false;
        }
    }
}
```

- `updateBullets()` is used to update the bullets from the NPCs.For each active bullet, update its position by multiplying its velocity vx, vy by dt. If the map is infinitely looping (inf == true), wrap the coordinates; otherwise, mark the bullet as inactive (alive = false) when it leaves the map boundaries.

The NPC subsystem implements four enemy archetypes, each inheriting from a`BaseNPC` class:

- Normal NPC: Standard chaser with moderate health and speed.
- Runner NPC: Fast-moving but fragile enemy.
- Tank NPC: High-health, slow-moving type.
- Turret NPC: Static enemy that fires projectiles toward the hero.

Each subclass overrides the `update()` method to define its specific behavior. The `chase()` method calculates direction vectors toward the hero using normalization:

```cpp
dx = heroX - x;
dy = heroY - y;
len = sqrt(dx * dx + dy * dy);
x += (dx / len) * speed * dt;
y += (dy / len) * speed * dt;
```

The Turret NPC introduces projectile-based behavior using the `Projectile` class, where bullets have velocity components `(vx, vy)` updated every frame. Spawning frequency and difficulty increase over time to meet coursework requirements for dynamic challenge scaling.

#### 2.2.2 NPCManager class

It is one of the core components of the game logic, responsible for the creation, updating, rendering, destruction, and attack detection of enemies.

NPCManager is responsible for:

- Managing the lifecycle of all enemies (creation / updating / deletion).

- Generating NPCs from off-screen (spawnOutsideCamera).

  - Core Logic:
    1. Locate available slot: Call findFree().

       2.    Determine spawn edge direction: Randomly select up, down, left, or right (rand() % 4).
        2.    Calculate spawn coordinates:
        -  If infinite map mode (inf=true), wrap coordinates using wrap();
        -  If fixed-boundary map, restrict within map limits.
    4.    Create specific enemy object (based on type enumeration):
        - 0 → NormalNPC
        - 1 → RunnerNPC
        - 2 → TankNPC
        - 3 → TurretNPC

    5. Initialise enemy and register into array.

```cpp
//generating NPCs from off-screen
void NPCManager::spawnOutsideCamera(int type, int camX, int camY, int camW, int camH,int mapW, int mapH, bool inf)
```

- Invoking each enemy's behaviour logic (updateAll).
- Rendering all enemies (drawAll).
- Detecting collisions between hero projectiles and enemies (checkHeroProjectilesHit).

### **2.3 Collision System**

Collision detection combines circular and tile-based approaches. Circular collision is used for dynamic entities (hero, NPCs, projectiles), while tile-based collision ensures movement constraints.

**Circular Collision Formula:**

```cpp
(dx * dx + dy * dy) < (r1 + r2) * (r1 + r2)
```

where `(dx, dy)` is the distance between two entity centers, and `r1`, `r2` are their radius.

#### 2.3.1 Hero projectiles vs NPCs 

```cpp
void NPCManager::checkHeroProjectilesHit(Projectile* heroBullets, int& heroBulletCount);
```

The above function`checkHeroProjectilesHit()` is responsible for detecting whether bullets fired by the player strike enemies, inflicting damage upon impact and subsequently destroying the bullet. It constitutes one of the most fundamental interactive logics within the entire combat system. The function's execution process can be understood as a frame-by-frame "collision detection" procedure, traversing all active hero bullets within the current game scene and calculating distances with every surviving enemy. For each bullet, the programme first skips those that have become invalid or are out of bounds. It then examines every existing enemy object. Should an enemy be deceased or its corresponding storage slot be empty, it is also ignored to prevent redundant calculations. When both a bullet and an enemy are in valid states, the programme calculates the horizontal and vertical coordinate differences between them on a two-dimensional plane, thereby obtaining the squared Euclidean distance between the two. This distance is then compared against the enemy's collision radius. If the distance falls below the defined hit range (typically twice the enemy's radius), the bullet is deemed to have struck that enemy. Upon a hit, the system invokes the enemy's takeDamage() function to reduce its health points, whilst simultaneously marking the bullet as invalid to prevent it from participating in subsequent frame detection. To prevent a single bullet from striking multiple targets, the programme immediately exits the inner loop after a hit is confirmed.

#### 2.3.2 NPC projectiles vs Hero

```cpp
bool BaseNPC::hitHero(float heroX, float heroY, float heroR);
```

The function `hitHero()` serves to detect whether enemy-fired bullets have struck the player character, forming the core component of the enemy attack detection system. It traverses all bullets currently possessed by the enemy, calculating the distance between each surviving bullet (i.e., still in flight) and the player's position. Specifically, the programme first ignores all invalidated bullets to avoid redundant calculations. It then computes the horizontal and vertical coordinate differences between the player and the current bullet in two-dimensional space, thereby determining the square of their distance. Subsequently, the function compares this squared distance with the square of the player's collision radius. Should the distance prove less than the player's radius squared, it signifies the bullet has entered the player's detection range, thus constituting a hit event. Upon detecting a hit, the system marks the bullet as invalid to prevent repeated detection in subsequent checks, while immediately returning true to indicate the player has been struck. Should none of the bullets land a hit, the function ultimately returns false.

#### 2.3.3 Hero vs NPCs 

```cpp
for (int i = 0; i < npcManager.count; i++) {
		BaseNPC* e = npcManager.slots[i];
  	if (!e || !e->isAlive()) continue;
  	float dx = heroWorldX - e->getX();
  	float dy = heroWorldY - e->getY();
  	float distSq = dx * dx + dy * dy;
  	float rSum = hero.radius + e->getRadius();
  	if (distSq < rSum * rSum) {
  			hero.takeDamage(1);
  	}	
}
```

This detection method constitutes a typical circular collision detection algorithm, suitable for most two-dimensional action or shooter games. It determines whether two objects intersect by comparing the distance between them against the collision radius, offering both simplicity and efficiency. Employing square values for comparison avoids square root operations, thereby maintaining high performance during real-time calculations.

#### 2.3.4 Hero vs impassable terrain

Hero will be stuck by water area.When reading the file, initialise the two-dimensional array passable. Elements 14 to 22 represent impassable water areas and are set to false.

```cpp
 bool isPassable(int X, int Y, int width, int height) {
        
        // Check the four corners of the hero rectangle
        int corners[4][2] = {
            {X, Y},
            {X + width, Y},
            {X, Y + height},
            {X + width, Y + height}
        };


        for (int i = 0; i < 4; ++i) {
            int cx = corners[i][0];
            int cy = corners[i][1];
            if (!w.Passable(cx, cy))
                return false; 
          // If any corner is on a water tile, it is deemed impassable.
        }
        return true;

    }
```



### **2.4 Hero Attacks (Linear & AOE)**

The hero’s attacks demonstrate two fundamental  strategies:

- **Auto-aim linear attack:** constantly targets the **closest** enemy.
- **AOE (Area of Effect):** damages multiple enemies within a radius or based on their health.

#### 2.4.1 **Linear Attack:**

The programme first checks the firing conditions: if the player's shooting cooldown time `heroShootCD` is less than or equal to zero (indicating another attack is possible), and the current number of bullets in the scene has not reached the maximum limit `MAX_HERO_PROJECTILES`, it proceeds to the shooting logic. Next, the system initialises a variable best with its maximum value, used to record the square of the distance to the nearest enemy currently found. Simultaneously, targetX and targetY are defined to store the coordinates of the nearest enemy. It then iterates through all potential enemy objects in the scene, obtaining a pointer to each enemy via the npcManager's slots array. If the enemy does not exist or is dead, it is skipped; otherwise, the relative distance between the enemy and the player is calculated. The `shortestDelta()` function is employed here, which accounts for whether the map is a looping world to correctly calculate the shortest path distance on infinite maps. The computed `dx` and `dy` represent the horizontal and vertical distance differences respectively, from which the squared distance `d² = dx² + dy²` is derived. If this distance is less than the currently recorded `best` value, the nearest enemy's coordinates and minimum distance are updated.

Upon completing the traversal, if a valid target exists (best < 1e8f), the programme commences creating a new bullet. First, it recalculates the direction vector (dx, dy) between the player and the target enemy, normalising it to a length of 1. Next, a new Projectile object is generated based on the preset bullet speed bulletSpeed and damage value bulletDamage. This is added to the player's bullet array heroBullets, while the bullet count heroBulletCount is incremented. Finally, the shooting cooldown time heroShootCD is reset to a fixed interval value heroShootInterval, thereby preventing the player from firing continuously without pause.

The specific implementation is as follows:

```cpp
if (heroShootCD <= 0.f && heroBulletCount < MAX_HERO_PROJECTILES) {
		float best = 1e30f;
    float targetX = 0, targetY = 0;

    // choose the nearest enemy
    for (int i = 0; i < MAX_NPCS; ++i) {
        BaseNPC* e = npcManager.slots[i];
        if (!e || !e->isAlive()) continue;
        float dx = shortestDelta(e->getX(), heroWorldX, mapW);
        float dy = shortestDelta(e->getY(), heroWorldY, mapH);
        float d2 = dx * dx + dy * dy;
				if (d2 < best) { best = d2; targetX = e->getX(); targetY = e->getY(); }
		}

		if (best < 1e8f) {
				// shoot from the center of the hero
        float dx = shortestDelta(targetX, heroWorldX, mapW);
        float dy = shortestDelta(targetY, heroWorldY, mapH);
        float len = sqrtf(dx * dx + dy * dy);
        if (len > 0.001f) { dx /= len; dy /= len; }

				heroBullets[heroBulletCount++] =
            Projectile(heroWorldX, heroWorldY, dx * bulletSpeed, dy * bulletSpeed, 										 bulletDamage, true);

        heroShootCD = heroShootInterval;
                
    }
}
```

#### 2.4.2 **AOE Attack:**

- Triggered with **Q key**.
- Damages top `N` highest-HP enemies within range.
  - Here, a simple selection sort algorithm is employed to arrange enemies in descending order of maximum health points. The significance of this sorting lies in:
    When multiple enemies fall within a skill's range, the system prioritises targeting the enemy with the highest health, thereby enhancing the tactical value and visual impact of the skill.
- Adds a **visual explosion circle** in `Manager::draw()`.

```cpp
void Manager::castAOE();
```

### 2.5 Tile-based World

The map system in this project is built upon a **tile-based rendering principle**, a well-established method used in 2D games to construct large and efficient worlds from small, reusable image components called *tiles*.In essence, the entire game world is divided into a grid, where each cell corresponds to one tile. Each tile represents a small piece of terrain such as grass, sand, rock, or water. By combining thousands of these small images, the engine can form a large, visually complex environment while keeping memory consumption and draw calls under control.Its principal advantages are:

- High rendering efficiency.
- Simplified terrain logic evaluation (such as pathfinding).
- Memory reusability (the same image tile can be reused).

The map system has been split into three classes:

- `tile`:represents a single image unit that can be drawn on the screen.
- `tileSet`:stores and manages a collection of tile images, acting as a reusable resource library.
- `world`:represents the entire map layout, responsible for managing tile data, loading map files, determining terrain passability, and drawing only the visible region.

This hierarchical separation allows clear division of concerns: rendering, data management, and gameplay logic are decoupled.

#### Core Implement:

1. Two-dimensional representation of map data

   The map is represented by a two-dimensional array:**`data[y][x]`**.Each cell stores the number of the current terrain.

2. File IO

   In the constructor, data is read from the tile file, processed, and subsequently stored in the corresponding variables.

   The core function of this constructor is to load and initialise the entire game world from a map file, providing the visual terrain and path logic foundation for gameplay. It first completes a series of memory allocation tasks, dynamically creating the two-dimensional array structures `data` and `isPassable` to store map data and passability information. This enables rapid access and modification of each map cell during runtime. Subsequently, to prevent game crashes caused by failed file reads, the programme generates a default map using randomised values. This ensures the game functions correctly even when external map files are absent.

   Upon successfully opening the file, the programme parses its contents line-by-line, distinguishing between the metadata section and the map data section. The metadata section primarily records fundamental structural information about the map, such as its width, height, and the pixel dimensions of each tile. This data determines the map's display scale within the game's visuals and the method for coordinate calculations. Upon entering the data section, the programme reads each tile ID line by line, converts it to an integer, and stores it within the two-dimensional array. These IDs represent different types of terrain textures, such as grass, sand, water surfaces, and so forth.

   Simultaneously, the programme automatically determines the passability of each terrain based on its tile ID: if the ID falls within a specific range (e.g., 14 to 22), it is designated as water or an obstacle and marked as impassable; otherwise, it is set as a walkable area. Thus, upon completion of the map loading, it not only possesses visual information but also carries physical attributes for logical decision-making. Finally, the file is closed, and the entire world's two-dimensional terrain data along with its passability information is formally loaded into memory.

   ```cpp
   world(const std::string& filename) : ts() {
           tilesWide = worldSize;
           tilesHigh = worldSize;
           tileWidth = worldSize;
           tileHeight = worldSize;
           
           data = new unsigned int* [worldSize];
           isPassable = new bool* [worldSize];
   
           for (int i = 0; i < worldSize; ++i) {
               data[i] = new unsigned int[worldSize];
               isPassable[i] = new bool[worldSize];
           }
           for (int i = 0; i < worldSize; i++) {
               for (int j = 0; j < worldSize; j++) {
                   data[i][j] = rand() % tileNum;
                   isPassable[i][j] = true;
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
                   std::stringstream ss(line);
                   std::string key;
                   ss >> key;
   
                   if (key == "tileswide") ss >> tilesWide;
                   else if (key == "tileshigh") ss >> tilesHigh;
                   else if (key == "tilewidth") ss >> tileWidth;
                   else if (key == "tileheight") ss >> tileHeight;
               }
               else {
                   std::stringstream ss(line);
                   string value;
   
                   while (std::getline(ss, value, ',') && value != "") {
                       data[index1][index2] = std::stoi(value);
                       if (stoi(value) >= 14 && stoi(value) <= 22) {
                           //water impassable
                           isPassable[index1][index2] = false;
                       }
                       else {
                           isPassable[index1][index2] = true;
                       }
                       //cout << value << endl;
                       index2++;
                   }
                   index1++;
                   index2 = 0;
               }
           }
           fin.close();
       }
   ```

3. Camera Culling

   Only a small portion of the map is visible on screen at any given time.Therefore, only tiles **within the current camera's field** of view are rendered to reduce computational overhead.

   1.    Calculate the starting and ending tile indices based on the camera position;

   2. For each tile within the current range:

      - Retrieve the tile ID;

      - Compute the drawing position;

      - Call `ts[id].draw()`.

   ​	Advantages: Significantly reduces the number of drawing operations, thereby improving frame rate.

4. Passability

   Different terrains on the map (such as water surfaces and rocks) require marking whether they are passable.The programme stores passability information via a Boolean two-dimensional array **`isPassable[y][x]`**.
   Movement logic can be implemented through:`if (world.Passable(x, y)) move();`to determine whether the character can walk.

   ```cpp
   //Convert pixel coordinates to tile indices and return whether the tile is walkable.
   bool Passable(unsigned int worldX, unsigned int worldY) {
       int tileX = worldX / tileWidth % tilesWide;
       int tileY = worldY / tileHeight % tilesHigh;
       return isPassable[tileY][tileX];
   }
   ```

5. Infinite World

   Employ **modulo arithmetic** to wrap the coordinates, ensuring the map edges connect seamlessly.

   ```cpp
   tileX = (tx % width + width) % width;
   tileY = (ty % height + height) % height;
   ```

### 2.6 Game Levels and Modes

#### 2.6.1 Two Game Levels

Employ a timer or cumulative time variable to monitor the level's duration. Upon exceeding two minutes (120 seconds), trigger the level termination logic.

```cpp
// Within the frame update logic
float dt = timer.dt();   // Obtain the time interval (in seconds) for the current frame
levelTimer += dt;        // Cumulative level runtime
```

#### 2.6.2 Two different Map

- A fixed map is a map of finite size (e.g. 100x100 tiles) that does not loop when the player or camera reaches its boundaries.In this mode, the map's coordinates do not wrap around. Players will not appear on the opposite side when moving to the edge; the entire world is finite and enclosed.

- The infinite map is achieved through a wrap-around mechanism, allowing players to move seamlessly in a loop.The `wrap()` function will wrap coordinates that exceed the boundaries back to the opposite side.

  ```cpp
  float wrap(float value, int max) {
      while (value < 0) value += max;
      while (value >= max) value -= max;
      return value;
  }
  ```

Distinguish between different modes via the loadLevel(int level) function, and control game logic via the Boolean variable infiniteMode, such as enemy spawning, map boundary detection, and camera loop rendering.

For example, in the update logic for the hero:

```cpp
if (infiniteMode){
	x = wrap(x, mapW);
	y = wrap(y, mapH);
}
else{
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(x + sprite.width > mapW) x = mapW - sprite.width;
	if(y + sprite.height > mapH) y = map - sprite.height;
}
```

#### 2.6.3 Score and Frame Rate Display

Frame rate (FPS) is calculated by recording time intervals:

```cpp
float fps = 1.0f / dt;
```

The final score and FPS will be displayed in the top-right corner of the screen.The effect is as follows:

![852b00fd171fb633a3a067afac21a106](assets/852b00fd171fb633a3a067afac21a106.png)

### 2.7 Game Progress Saving and Loading

In the Manager class, add the `saveGame()` and `loadGame()` functions.Their primary function is to preserve the current game state in its entirety within a binary file, enabling players to resume gameplay at different points by reloading this data when required. The system achieves complete state restoration by directly manipulating data structures in memory, serialising the core attributes of game objects—including protagonists, enemies, bullets, and level information—into a binary stream, then subsequently deserialising them back into their original form.`saveGame()` is responsible for writing the current memory state to a file, whilst `loadGame()` performs the inverse operation, restoring the data from the file to the respective object states during game execution.

Here, C++ binary file streams (`std::ofstream` and `std::ifstream`) are employed to achieve efficient data access. Compared to text-based methods, binary writing directly preserves structure data in memory without requiring formatting conversions, resulting in higher efficiency and reduced storage space.

## 3. Evaluation

![4d092a9b968fab5b58a37638f4807656](assets/4d092a9b968fab5b58a37638f4807656.png)



During gameplay, real-time frame rate measurements were collected by logging the FPS and the corresponding milliseconds per frame for each update cycle. The observed FPS values fluctuated between approximately 45 and 75, corresponding to a frame time range of 13–22 ms per frame**. **The average frame rate remained close to 60 FPS, which is generally considered the threshold for smooth interactive gameplay.Performance dips (e.g., down to ~46 FPS) were primarily observed during intensive computation events, such as when multiple NPCs were active or when collision and projectile calculations occurred (as indicated by "Hero hit!" events). This demonstrates that the performance monitoring system effectively captures the dynamic load changes in real-time.

Overall, the results confirm that the rendering and update loops are optimized enough to maintain fluid gameplay even under moderate computational stress. The game engine shows good scalability, with performance degradation remaining within acceptable limits as scene complexity increases.



------

## 4. Limitations 

- The NPC system, managed by the NPCManager class, updates and draws all enemies in a single loop. This design works but limits scalability, as all NPCs share the same update rate and behaviour logic.
-  The AI movement is very simple — in `updateAll()` enemies always move directly toward the hero using linear distance, without any obstacle avoidance or pathfinding. 
- The collision detection in `checkHeroProjectilesHit()` and `heroHitByBullet()` only uses circular radius checks, which is fast but not precise when sprites have irregular shapes. The projectile system stores bullets in fixed-size arrays (`heroBullets[MAX_HERO_PROJECTILES]`), which may waste memory and lacks dynamic allocation or pooling. 
- In the rendering part, everything is drawn pixel-by-pixel on the CPU using canvas.draw() and drawString(), which restricts performance when many projectiles or NPCs are visible. 
- The `saveGame()` and `loadGame()` functions rely on binary serialization of C++ structs. This is efficient but risky — if the class structure changes, the saved data becomes invalid. 
- The camera and tile map systems lack parallax scrolling and smooth transitions, making the visual experience relatively basic. These limitations make the game technically solid but still closer to a prototype than a complete, optimized product.

## 5. Conclusions

In conclusion, this project successfully demonstrates the design and implementation of a 2D top-down shooting game using object-oriented programming and core game development techniques. The system integrates multiple technologies including procedural world generation, camera control, collision detection, AI-based enemy behavior, power-ups, and a save/load mechanism. Functional and non-functional testing verified that the system runs stably at more than 60 FPS in both infinite and fixed maps. Although some advanced visual effects and AI improvements could be further developed, the current implementation fully meets the functional goals and provides a smooth and responsive gameplay experience. Through this project, I have gained practical understanding of how to manage complex C++ systems, optimize performance, and apply theoretical concepts of algorithms and data structures to real-time game development.

## 6. Improvement 

If I were to start this project again from scratch, I would plan the architecture more carefully and adopt a modular, data-driven design from the beginning. One major issue in the current version is that the coordinate systems are somewhat inconsistent. If I were to rewrite the project, I would start by defining a clear and unified coordinate framework, including precise conversion rules between world, camera, and screen coordinates.

In the early stages, most of the code evolved naturally as new features were added. Systems such as NPC behaviour, collision handling, and map rendering were developed separately, which made later integration more complex. Looking back, I would introduce clearer layers between the game logic, rendering, and resource management so that each part of the system could evolve independently.

I would also consider adopting an Entity-Component System to manage all dynamic objects like the hero, enemies, and projectiles. This would make it easier to mix and match behaviours and to expand the game with new features such as power-ups or environmental effects.

From a performance perspective, I would start profiling much earlier in the process to locate bottlenecks, particularly in the drawing functions that handle per-pixel rendering. Early optimisation could have improved frame stability and reduced unnecessary processing.

Lastly, I would add simple unit tests for critical systems such as saving/loading and collision detection. This would make the project more robust and easier to maintain as it grows. Overall, this project has shown me how important it is to combine creativity with solid engineering principles. Careful planning at the start can make the later stages of development much smoother and more rewarding.

## 7.Screenshot of the Game in Operation

![68e723cb4a6ece051affb7f65d61ebab](assets/68e723cb4a6ece051affb7f65d61ebab.png)

![0f4e531f52da5de7a10010e30f30419c](assets/0f4e531f52da5de7a10010e30f30419c.png)

![0377cab18dca131d199075add5d70407](assets/0377cab18dca131d199075add5d70407.png)

![a6c71bdcc8ad9856859a4958679c7166](assets/a6c71bdcc8ad9856859a4958679c7166.png)

