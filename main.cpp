/*******************************************************************************************
 *
 *   raylib [core] example - Basic window
 *
 *   Welcome to raylib!
 *
 *   To test examples, just press F6 and execute raylib_compile_execute script
 *   Note that compiled executable is placed in the same folder as .c file
 *
 *   You can find all basic examples on C:\raylib\raylib\examples folder or
 *   raylib official webpage: www.raylib.com
 *
 *   Enjoy using raylib. :)
 *
 *   This example has been created using raylib 1.0 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h
 *for details)
 *
 *   Copyright (c) 2013-2016 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

// #define __EMSCRIPTEN__
#include "raylib.h"
#include "raymath.h"
#include <map>
#include <string>

const int NUM_POWERUP_TYPES = 9;
enum powerup_types {
  SHOT_SPEED,
  SHOT_RELOAD,
  SHOT_POWER,
  SHOT_SPREAD,
  SHIELD_REPAIR,
  SHIELD_MAX,
  HULL_REPAIR,
  HULL_MAX,
  HULL_STRENGTH
};

typedef enum {
  keyMoveUp,
  keyMoveDown,
  keyMoveLeft,
  keyMoveRight,
  keyShoot,
  keyToggleAutoshoot
} keys;

typedef enum {
  mouseShoot,
} mouseKeys;

std::map<keys, KeyboardKey> keymap = {
  {keyMoveUp, KEY_W},
  {keyMoveDown, KEY_S},
  {keyMoveLeft, KEY_A},
  {keyMoveRight, KEY_D},
  {keyShoot, KEY_SPACE},
  {keyToggleAutoshoot, KEY_I}
};

std::map<mouseKeys, MouseButton> mouseKeymap = {
  {mouseShoot, MOUSE_BUTTON_LEFT},
};

const std::pair<int, int> upgrade_matrix[NUM_POWERUP_TYPES] = {
    {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}};

const int NUM_BADDIE_TYPES = 36;

const int NUM_SHOT_STYLES = 30;

const int NUM_BOSS_TYPES = 6;
const int NUM_BOSS_SHOT_STYLES = 18;
/* const int NUM_BOSS_SPECIAL_ATTACKS = 4; */

const int NUM_METALS = 4;
enum metals { IRON, NICKEL, COBALT, SILICON };

const int NUM_ACHIEVEMENTS = 5;

// -----------------------------------------------------------
// Bounding boxes for every object that has a collision.
// These are arrays of Rectangle objects that contain the
// appropriate offsets for each subitem within the spritemap
// area.
//
// { object.x, object.y, object.width, object.height }
// -----------------------------------------------------------
const Rectangle shot_size_matrix[NUM_SHOT_STYLES] = {
    {4, 4, 1, 1}, {3, 4, 1, 2}, {3, 3, 1, 3}, {3, 3, 2, 3}, {3, 3, 3, 2},
    {2, 3, 4, 2}, {2, 1, 3, 6}, {2, 1, 3, 6}, {0, 0, 8, 8}, {1, 1, 6, 6},
    {1, 1, 6, 4}, {0, 0, 8, 5}, {2, 1, 3, 5}, {2, 1, 3, 6}, {3, 3, 2, 2},
    {2, 2, 4, 4}, {3, 1, 3, 4}, {2, 1, 4, 5}, {2, 1, 3, 6}, {2, 1, 3, 6},
    {3, 1, 3, 6}, {2, 3, 3, 3}, {1, 1, 6, 6}, {2, 1, 4, 5}, {2, 1, 5, 5},
    {0, 0, 8, 8}, {1, 1, 6, 6}, {1, 1, 6, 6}, {3, 3, 3, 3}, {2, 1, 3, 6}};

const Rectangle boss_shot_size_matrix[NUM_BOSS_SHOT_STYLES] = {
    {3, 0, 3, 8}, {2, 0, 4, 8},   {1, 0, 6, 8},
    {2, 0, 6, 8}, {2, 1, 6, 5},   {2, 1, 6, 5},

    {3, 1, 1, 6}, {0, 0, 8, 5},   {2, 0, 5, 8},
    {3, 0, 3, 8}, {2, 1, 3, 6},   {2, 1, 2, 6},

    {0, 0, 8, 8}, {-1, 0, 10, 4}, {1, 1, 6, 5},
    {0, 0, 8, 6}, {2, 1, 4, 6},   {3, 1, 3, 6},
};

const Rectangle baddie_size_matrix[NUM_BADDIE_TYPES] = {
    {2, 2, 5, 5}, {0, 2, 7, 5}, {1, 2, 5, 5},
    {1, 3, 6, 5}, {2, 2, 5, 4}, {1, 2, 7, 5},

    {1, 2, 6, 4}, {1, 2, 6, 4}, {1, 2, 5, 5},
    {1, 2, 6, 5}, {1, 2, 6, 5}, {2, 2, 4, 5},

    {1, 1, 7, 6}, {0, 1, 8, 6}, {1, 2, 6, 6},
    {1, 2, 5, 6}, {0, 2, 8, 5}, {1, 2, 6, 5},

    {1, 1, 6, 6}, {1, 1, 6, 6}, {1, 1, 6, 6},
    {1, 1, 5, 6}, {1, 1, 6, 5}, {0, 1, 8, 6},

    {0, 0, 8, 8}, {0, 0, 8, 8}, {0, 0, 8, 8},
    {0, 1, 8, 7}, {1, 1, 6, 6}, {0, 1, 8, 6},

    {0, 1, 8, 6}, {1, 0, 6, 8}, {1, 1, 6, 6},
    {1, 1, 6, 6}, {1, 1, 6, 6}, {1, 0, 6, 7},

};

struct Baddie {
  bool boss;
  int type;
  int shot_style;
  int bullets_per_shot;
  bool active;
  Vector2 position;
  Vector2 direction;
  Vector2 size;
  float lifetime;
  Color tint;
  bool player_bullet;
  float shot_timer;
  int damage;
  int hull;
  int hull_max;
  float shield;
  int shield_max;
  int hull_strength;
  float flash_timer;
  bool shield_activating;
  bool shield_deactivating;
  float shield_regen_amt;
  float shield_regen_timer;
  float shield_frame_timer;
  float shield_frame;
  float exploding;
  float exp_frame;
  float exp_timer;
};

struct Bullet {
  bool boss;
  int type;
  int shot_style;
  bool active;
  Vector2 position;
  Vector2 direction;
  Vector2 size;
  float lifetime;
  Color tint;
  bool player_bullet;
  float shot_timer;
  int damage;
};

struct Powerup {
  int type;
  bool active;
  Vector2 position;
  Vector2 direction;
  float lifetime;
};

struct Asteroid {
  bool active;
  float lifetime;
  int type;
  Vector2 position;
  Vector2 direction;
  float size;
  int durability;
  int durability_max;
  float rotation_speed;
  float rotation;
  int metal;
  int metal_amount;
};

struct Popup {
  bool visible;
  float lifetime;
  Vector2 position;
  Vector2 size;
  Color border_color;
  int border_thickness;
  float anim_total;
  float anim_current;
};

struct Achievement {
  std::string title;
  std::string desc;
  bool achieved;
  int logo;
};

Achievement achievement_info[NUM_ACHIEVEMENTS] = {
    {"Slayer I", "Slay 100 enemies!", false, 0},
    {"Slayer II", "Slay 500 enemies!", false, 0},
    {"Slayer III", "Slay 1000 enemies!", false, 0},
    {"Winner", "Win the Game!", false, 0},
    {"Gun Nut", "Max out a single gun stat!", false, 0},
};

#define MAX_BADDIES 100
#define MAX_BULLETS 1000
#define MAX_POWERUPS 10
#define MAX_ASTEROIDS 5
#define MAX_FRAGMENTS 100

Vector2 screenSize = {800, 1200};

Baddie Baddies[MAX_BADDIES];
Bullet Bullets[MAX_BULLETS];
Powerup Powerups[MAX_POWERUPS];
Asteroid Asteroids[MAX_ASTEROIDS];
Asteroid Fragments[MAX_FRAGMENTS];

Popup achievement;
Popup death = {false, 20.0f, Vector2{screenSize.x * 0.5f, screenSize.y * 0.5f}, Vector2{300, 50}, RED, 1, 0.25f, 0};

Vector2 GetScreenSize(void);
Vector2 GetShotSpeed(int totalShots, int currentShot, float speed, float direction);
int NewBaddie(void);
void UpdatePlayer(void);
void UpdatePowerup(int id);
void UpdateBaddie(int id);
void UpdateBullet(int id);
void UpdateAsteroid(int id);
void UpdateFragment(int id);
void StopAllMusic(void);
void PerformHitPlayer(Bullet &bullet);
void PerformHitBaddie(Bullet &bullet, Baddie &baddie);
void PerformHitAsteroid(Bullet &bullet, Asteroid &hit);
void PerformKillBaddie(Baddie *baddie);
void PerformKillBullet(Bullet *bullet);
void PerformKillAsteroid(Asteroid *asteroid);
void PerformKillPowerup(Powerup *powerup);
void PerformFragmentAsteroid(Asteroid *asteroid);
void ResetBaddie(Baddie *baddie);
void ResetBullet(Bullet *bullet);
void ResetAsteroid(Asteroid *asteroid);
void ResetPowerup(Powerup *powerup);
Rectangle getBulletRec(Bullet &bullet);
Rectangle getAsteroidRec(Asteroid &asteroid);
Rectangle getPlayerRec(void);
Rectangle getBaddieRec(Baddie &baddie);
Vector2 getBaddieCenter(Baddie &baddie);
float getBaddieShieldRadius(Baddie &baddie);
Vector2 getPlayerCenter(void);
float getPlayerShieldRadius(void);
void DrawBackground(void);
void DrawPlayer(void);
void DrawStats(void);
void DrawHullBar(void);
void DrawShieldBar(void);
void DrawExpBar(void);
void DrawPlayerFreezeTimer(void);
void ProcessPlayerState(void);
void UpdateAndDraw(void);
void DrawIntro(void);
void DrawMainMenu(void);
void DrawSettingsMenu(void);
void DrawShipSetupMenu(void);
void DrawPauseMenu(void);
void DrawDeathScreen(void);
void DrawWinScreen(void);
void UpdateBackground(void);

// PLAYER VARIABLES
bool autoShoot = false;
bool GOD_MODE = false;
Vector2 playerPos = {0};
Vector2 playerPreviousPos = {0};
Vector2 playerSize = {32.0f, 32.0f};
bool playerDead = false;
bool playerWon = false;

float playerSpeed = 10.0f; // pixels per frame
                           // start with 3 hull
int playerHull = 3;
int playerHullMax = 3;
int playerHullStrength = 1; // mitigates 1 damage
                            // start with no shields
float playerShield = 0;
float playerShieldMax = 0;
int playerShotSpeed = 100;
float playerShotReload = 10.0f / 60.0f;
float playerShotTimer = 0;
bool playerCanShoot = true;
int playerShotPower = 1;
int playerShotSpread = 1;
int playerShotStyle = 0;

float playerShieldRegenTime = 2.0f;
float playerShieldRegenTimer = 0;
float playerShieldRegenAmt = 0.02f;

// shield animation VARIABLES
float playerShieldFrameTime = 0.25f; // sec between shield frame transitions
float playerShieldFrameTimer = 0.0f;
int playerShieldFrames = 4;
int playerShieldFrame = 0;
bool playerShieldActivating = false;
bool playerShieldDeactivating = false;

// player exploding variables
bool playerExploding = false;
int playerExpFrame = 0;
float playerExpTimer = 0;

// player alert variables
float playerAlertTimer = 0;

float playerFlashTime = 0.5f;
float playerFlashTimer = 0;
double gameTime = 0;
double endTime = 0;

// player time freeze variables
float playerFreezeTime = 1.0f;
float playerFreezeTimer = playerFreezeTime;
bool playerTimeFrozen = false;

float AsteroidSpeed = 100.0f;
float EnemyShotSpeed = 200.0f;
float ShotLifetime = 2.0f;
Vector2 BaddieSize = {48.0f, 48.0f};
Vector2 BulletSize = {48.0f, 48.0f};
Vector2 BossSize = {96.0f, 96.0f};
float BaddieSpeed = 100.0f;
int Score = 0;
Vector2 PowerupSize = {25.0f, 25.0f};
float PowerupSpeed = 300.0f;
float PowerupLifetime = 2.0f;

float movingBackgroundPosY = 0;
float musicPlayed = 0;

enum game_states {
  GAME_STATE_INTRO,
  GAME_STATE_MAIN_MENU,
  GAME_STATE_SETTINGS,
  GAME_STATE_SHIP_SETUP,
  GAME_STATE_PLAYING,
  GAME_STATE_PAUSED,
  GAME_STATE_DEAD,
  GAME_STATE_WIN
};
int player_game_state = GAME_STATE_PLAYING;

const int NUM_TEXTURES = 7;
enum texture_names {
  SHIPS,
  PROJECTILES,
  BACKGROUND,
  MOVING_BACKGROUND,
  UI,
  MISC,
  UPGRADES
};
const char *texture_paths[NUM_TEXTURES]{
    "assets/SpaceShooterAssetPack_Ships.png",
    "assets/SpaceShooterAssetPack_Projectiles.png",
    "assets/SpaceBackground.png",
    "assets/SpaceShooterAssetPack_BackGrounds.png",
    "assets/SpaceShooterAssetPack_IU.png",
    "assets/SpaceShooterAssetPack_Miscellaneous.png",
    "assets/ship_upgrades.png"};
Texture2D textures[NUM_TEXTURES];

void load_textures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    textures[i] = LoadTexture(texture_paths[i]);
  }
}

void unload_textures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    UnloadTexture(textures[i]);
  }
}

const int NUM_MUSIC = 7;
enum music_names { DEATH, GAME1, GAME2, GAME3, GAME4, MENU1, WIN };
const char *music_paths[NUM_MUSIC]{
    "assets/music_death01.wav", "assets/music_game01.wav",
    "assets/music_game02.wav",  "assets/music_game03.mp3",
    "assets/music_game04.wav",  "assets/music_menu01.mp3",
    "assets/music_win01.wav"};
Music music[NUM_MUSIC];
Music *nowPlaying;
int numMusicRepeats = 0;
int currentMusicRepeats = 0;
const int NUM_GAME_MUSIC = 4;
int game_music[NUM_GAME_MUSIC] = {GAME1, GAME2, GAME3, GAME4};

const int NUM_SOUNDS = 28;
enum sound_names {
  ALARM,
  ALERT_LOW_HULL,
  ASTEROID_HIT,
  ATTACK_HYPER_SHOT,
  ATTACK_ROCKET,
  BOSS_ATTACK_1,
  BOSS_ATTACK_2,
  EXPLOSION,
  HULL_HIT,
  MENU_HOVER,
  MENU_SELECT,
  NEGATIVE,
  NAVIGATION_BUOY,
  POWERUP_ATTACK_SPEED,
  POWERUP_COLLECT,
  POWERUP_DAMAGE_INCREASE,
  POWERUP_DROP,
  POWERUP_HULL_INCREASE,
  POWERUP_HULL_STRENGTH,
  POWERUP_HULL_RESTORE,
  POWERUP_SHIELD_INCREASE,
  POWERUP_SPREAD_INCREASE,
  SHIELD_BOUNCE,
  SHIELD_DROP,
  SHIELD_HIT,
  SHIELD_RAISE,
  SUCCESS,
  WIN_TRUMPETS
};
const char *sound_paths[NUM_SOUNDS]{"assets/sounds/alarm.wav",
                                    "assets/sounds/alert_low_hull.wav",
                                    "assets/sounds/asteroid_hit.ogg",
                                    "assets/sounds/attack_hyper_shot.ogg",
                                    "assets/sounds/attack_rocket.wav",
                                    "assets/sounds/boss_attack_1.wav",
                                    "assets/sounds/boss_attack_2.wav",
                                    "assets/sounds/explosion.wav",
                                    "assets/sounds/hull_hit.ogg",
                                    "assets/sounds/menu_hover.ogg",
                                    "assets/sounds/menu_select.ogg",
                                    "assets/sounds/negative.wav",
                                    "assets/sounds/navigation_buoy.wav",
                                    "assets/sounds/powerup_attack_speed.wav",
                                    "assets/sounds/powerup_collect.wav",
                                    "assets/sounds/powerup_damage_increase.ogg",
                                    "assets/sounds/powerup_drop.ogg",
                                    "assets/sounds/powerup_hull_increase.ogg",
                                    "assets/sounds/powerup_hull_restore.wav",
                                    "assets/sounds/powerup_hull_strength.wav",
                                    "assets/sounds/powerup_shield_increase.ogg",
                                    "assets/sounds/powerup_spread_increase.ogg",
                                    "assets/sounds/shield_bounce.ogg",
                                    "assets/sounds/shield_drop.wav",
                                    "assets/sounds/shield_hit.wav",
                                    "assets/sounds/shield_raise.wav",
                                    "assets/sounds/success.wav",
                                    "assets/sounds/win_trumpets.wav"};
Sound sounds[NUM_SOUNDS];

void load_music(void) {
  for (int i = 0; i < NUM_MUSIC; i++) {
    music[i] = LoadMusicStream(music_paths[i]);
  }
}

void unload_music(void) {
  for (int i = 0; i < NUM_MUSIC; i++) {
    UnloadMusicStream(music[i]);
  }
}

void load_sounds(void) {
  for (int i = 0; i < NUM_SOUNDS; i++) {
    sounds[i] = LoadSound(sound_paths[i]);
  }
}

void unload_sounds(void) {
  for (int i = 0; i < NUM_SOUNDS; i++) {
    UnloadSound(sounds[i]);
  }
}

void PlayBackgroundMusic(void) {
  StopAllMusic();
  currentMusicRepeats++;
  if (!numMusicRepeats || currentMusicRepeats > numMusicRepeats) {
    int newTrack = GetRandomValue(0, NUM_GAME_MUSIC - 1);
    currentMusicRepeats = 0;
    numMusicRepeats = GetRandomValue(1, 3);
    nowPlaying = &music[game_music[newTrack]];
    nowPlaying->looping = false;
  }
  PlayMusicStream(*nowPlaying);

  musicPlayed = 0.0f;
}

Vector2 GetScreenSize(void) {
  int monitor = GetCurrentMonitor();
  float minHeight = fminf(screenSize.y, GetMonitorHeight(monitor));
  return Vector2{minHeight * 0.66f, minHeight};
  // Vector2 out = {fminf(screenSize.x, (float)GetMonitorWidth(monitor)),
  //                fminf(screenSize.y, (float)GetMonitorHeight(monitor))};
  // return out;
}

void InitGame(void) {

  screenSize = GetScreenSize();

  SetWindowSize(screenSize.x, screenSize.y);
  SetWindowState(FLAG_WINDOW_UNDECORATED);

  for (int i = 0; i < NUM_TEXTURES; i++) {
    if (!IsTextureReady(textures[i])) {
      TraceLog(LOG_ERROR, "Error loading texture %d", i);
    }
  }
  for (int i = 0; i < NUM_MUSIC; i++) {
    if (!IsMusicReady(music[i])) {
      TraceLog(LOG_ERROR, "Error loading music track %d", i);
    }
  }

  numMusicRepeats = 0;
  currentMusicRepeats = 0;

  if (IsAudioDeviceReady())
    PlayBackgroundMusic();

  autoShoot = false;
  GOD_MODE = false;
  playerPos = {0};
  playerPreviousPos = {0};
  playerDead = false;
  playerWon = false;

  playerSpeed = 10.0f; // pixels per frame
                       // start with 3 hull
  playerHull = 3;
  playerHullMax = 3;
  playerHullStrength = 1; // mitigates 1 damage
                          // start with no shields
  playerShield = 0;
  playerShieldMax = 0;

  // shield animation variables
  playerShieldFrame = 0;
  playerShieldActivating = false;
  playerShieldDeactivating = false;

  // player explosion variables
  playerExploding = false;
  playerExpFrame = 0;
  playerExpTimer = 0;

  // player alert variable
  playerAlertTimer = 0;

  playerShotSpeed = 100;
  playerShotReload = 10.0f / 60.0f;
  playerShotTimer = 0;
  playerCanShoot = true;
  playerShotPower = 1;
  playerShotSpread = 1;
  playerShotStyle = 0;
  Score = 0;
  gameTime = GetTime();
  movingBackgroundPosY = 0;
  musicPlayed = 0;

  for (int i = 0; i < MAX_BADDIES; i++) {
    Baddies[i].active = false;
  }
  for (int i = 0; i < MAX_BULLETS; i++) {
    Bullets[i].active = false;
  }
  for (int i = 0; i < MAX_POWERUPS; i++) {
    Powerups[i].active = false;
  }
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    Asteroids[i].active = false;
  }
  for (int i = 0; i < MAX_FRAGMENTS; i++) {
    Fragments[i].active = false;
  }

  int count = 5; // GetRandomValue(10, MAX_ENTITIES / 10);
  for (int i = 0; i < count; i++) {
    NewBaddie();
  }

  playerPos = {screenSize.x / 2.0f, screenSize.y - (100.0f + playerSize.y)};
  SetMousePosition(playerPos.x, playerPos.y);
}

void DrawShipTexture(int x, int y, Vector2 coords, Color hitColor) {
  DrawTexturePro(textures[SHIPS], Rectangle{8.0f * x, 8.0f * y, 8, 8},
                 Rectangle{coords.x, coords.y, playerSize.x, playerSize.y},
                 Vector2Zero(), 0, hitColor);
  // DrawRectangleLines(coords.x, coords.y, playerSize.x, playerSize.y, WHITE);
}

void DrawBaddieTexture(int x, int y, Vector2 coords, Color hitColor) {
  DrawTexturePro(textures[SHIPS],
                 Rectangle{8.0f * 4 + (8.0f * x), 8.0f * y, 8.0f, 8.0f},
                 Rectangle{coords.x, coords.y, BaddieSize.x, BaddieSize.y},
                 Vector2Zero(), 0, hitColor);
}

void DrawBossTexture(int x, int y, Vector2 coords, Color hitColor) {
  DrawTexturePro( textures[SHIPS],
      Rectangle{8.0f * 4 + (16.0f * x), 8.0f * 6 + (16.0f * y), 16.0f, 16.0f},
      Rectangle{coords.x, coords.y, BossSize.x, BossSize.y}, Vector2Zero(), 0, hitColor);
  // DrawRectangleLines(coords.x, coords.y, BossSize.x, BossSize.y, WHITE);
}

void DrawProjectileTexture(int x, int y, Vector2 coords, Vector2 size,
                           float rotation, Color tint) {
  DrawTexturePro(textures[PROJECTILES],
                 Rectangle{8.0f * x, 8.0f * y, 8.0f, 8.0f},
                 Rectangle{coords.x, coords.y, size.x, size.y},
                 Vector2{size.x * 0.5f, size.y * 0.5f}, rotation, tint);
}

void DrawBossProjectileTexture(int x, int y, Vector2 coords, Vector2 size,
                               float rotation, Color tint) {
  DrawTexturePro(textures[PROJECTILES],
                 Rectangle{8.0f * x, 8.0f * 5 + (8.0f * y), 8.0f, 8.0f},
                 Rectangle{coords.x, coords.y, size.x, size.y},
                 Vector2{size.x * 0.5f, size.y * 0.5f}, rotation, tint);
}

void DrawUpgradeTexture(int x, int y, Vector2 coords, Vector2 size,
                        Color tint) {
  DrawTexturePro(textures[UPGRADES],
                 Rectangle{ 16.0f * x, 16.0f * y, 16.0f, 16.0f, },
                 Rectangle{coords.x, coords.y, size.x, size.y}, 
                 Vector2Zero(), 0.0f, tint);
}

void DrawMiscTexture(Rectangle source, Rectangle output, Color tint) {
  DrawTexturePro(textures[MISC], source, output, Vector2Zero(), 0.0f, tint);
}

void DrawShieldTexture(int frame, Vector2 coords, Vector2 size, float amt,
                       Color tint) {
  DrawTexturePro(
      textures[MISC],
      Rectangle{(8.0f * 5) + (16.0f * frame), 8.0f * 4, 16.0f, 16.0f},
      Rectangle{coords.x - (size.x * 0.25f) - (20 * amt) * 0.5f,
                coords.y - (size.y * 0.25f) - (20 * amt) * 0.5f,
                (size.x * 1.5f) + (20 * amt), (size.y * 1.5f) + (20 * amt)},
      Vector2Zero(), 0.0f, tint);
}

void DrawAsteroidTexture(int type, Vector2 coords, float size, float rotation,
                         Color tint) {
  int x = 1;
  int y = 3;
  if (type == 2) {
    x = 2;
    y = 2;
  }
  DrawTexturePro(textures[MISC],
                 Rectangle{(8.0f * x), (8.0f * y), (type == 1 ? 8.0f : 16.0f),
                           (type == 1 ? 8.0f : 16.0f)},
                 Rectangle{coords.x, coords.y, size, size},
                 Vector2{size * 0.5f, size * 0.5f}, rotation, tint);
}

int FindEmptyBaddie(Baddie *list) {
  for (int i = 0; i < MAX_BADDIES; i++) {
    if (!list[i].active)
      return i;
  }

  return -1;
}
int FindEmptyBullet(Bullet *list) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!list[i].active)
      return i;
  }
  return -1;
}

int FindEmptyPowerup(Powerup *list) {
  for (int i = 0; i < MAX_POWERUPS; i++) {
    if (!list[i].active)
      return i;
  }
  return -1;
}

int FindEmptyAsteroid(Asteroid *list) {
  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (!list[i].active)
      return i;
  }
  return -1;
}

int FindEmptyFragment(Asteroid *list) {
  for (int i = 0; i < MAX_FRAGMENTS; i++) {
    if (!list[i].active)
      return i;
  }
  return -1;
}

int NewPlayerBullet(Vector2 position, int style, float lifetime, Color tint,
                    bool boss) {
  int id = -1;
  for (int i = 0; i < playerShotSpread; i++) {
    id = FindEmptyBullet(Bullets);
    if (id == -1)
      return -1; // nope

    Bullets[id].active = true;
    Bullets[id].boss = boss;
    Bullets[id].position =
        Vector2{position.x + (playerSize.x * 0.5f), position.y};
    Bullets[id].lifetime = lifetime;
    Bullets[id].tint = tint;
    Bullets[id].player_bullet = true;
    Bullets[id].direction =
        GetShotSpeed(playerShotSpread, i, playerShotSpeed, -1.0f);
    Bullets[id].damage = playerShotPower;
    Bullets[id].type = style;
    Bullets[id].size = Vector2{48.0f, 48.0f};
  }
  return id;
}

int NewBaddieBullet(Baddie &baddie, float lifetime, Color tint) {
  int id = -1;
  for (int i = 0; i < baddie.bullets_per_shot; i++) {
    id = FindEmptyBullet(Bullets);
    if (id == -1)
      return -1; // nope

    Bullets[id].active = true;
    Bullets[id].boss = baddie.boss;
    Bullets[id].position = Vector2{baddie.position.x + (baddie.size.x * 0.5f),
                                   baddie.position.y + (baddie.size.x * 0.5f)};
    Bullets[id].lifetime = lifetime;
    Bullets[id].tint = tint;
    Bullets[id].player_bullet = false;
    Bullets[id].direction =
        GetShotSpeed(baddie.bullets_per_shot, i, EnemyShotSpeed, 1.0f);
    Bullets[id].damage =
        (baddie.boss ? GetRandomValue(5, 10) : GetRandomValue(1, 3));
    Bullets[id].type = baddie.shot_style;
    Bullets[id].size = Vector2{32.0f, 32.0f};
  }

  return id;
}

Vector2 GetShotSpeed(int totalShots, int currentShot, float speed,
                     float direction) {
  Vector2 dir = {0};
  switch (totalShots) {
  case 1:
    dir = Vector2{0, 1};
    break;
  case 2:
    switch (currentShot) {
    case 0:
      dir = Vector2{-0.15f, 1};
      break;
    case 1:
      dir = Vector2{0.15f, 1};
      break;
    }
    break;
  case 3:
    switch (currentShot) {
    case 0:
      dir = Vector2{-0.25f, 1};
      break;
    case 1:
      dir = Vector2{0, 1};
      break;
    case 2:
      dir = Vector2{0.25f, 1};
      break;
    }
    break;
  case 4:
    switch (currentShot) {
    case 0:
      dir = Vector2{-0.35f, 1};
      break;
    case 1:
      dir = Vector2{-0.15f, 1};
      break;
    case 2:
      dir = Vector2{0.15f, 1};
      break;
    case 3:
      dir = Vector2{0.35f, 1};
      break;
    }
    break;
  case 5:
    switch (currentShot) {
    case 0:
      dir = Vector2{-0.45f, 1};
      break;
    case 1:
      dir = Vector2{-0.25f, 1};
      break;
    case 2:
      dir = Vector2{0, 1};
      break;
    case 3:
      dir = Vector2{0.25f, 1};
      break;
    case 4:
      dir = Vector2{0.45f, 1};
      break;
    }
    break;
  }
  return Vector2Multiply(Vector2Scale(Vector2Normalize(dir), speed), Vector2{1.0f, direction});
}

void UpdatePlayerShield(void) {
  if (playerExploding || playerDead || playerWon)
    return;
  if ((playerShield < playerShieldMax)) {
    if (playerShieldRegenTimer == 0)
      playerShield += playerShieldRegenAmt * GetFrameTime();
    else {
      playerShieldRegenTimer -= GetFrameTime();
      if (playerShieldRegenTimer <= 0) {
        playerShieldRegenTimer = 0;
        playerShieldActivating = true;
        PlaySound(sounds[SHIELD_RAISE]);
      }
    }
  }
  if (playerShieldActivating) {
    playerShieldFrameTimer += GetFrameTime();
    if (playerShieldFrameTimer >= playerShieldFrameTime) {
      playerShieldFrameTimer = 0;
      playerShieldFrame++;
      if (playerShieldFrame >= playerShieldFrames - 1) {
        playerShieldFrame = 3;
        playerShieldActivating = false;
      }
    }
  } else if (playerShieldDeactivating) {
    playerShieldFrameTimer += GetFrameTime();
    if (playerShieldFrameTimer >= playerShieldFrameTime) {
      playerShieldFrameTimer = 0;
      playerShieldFrame--;
      if (playerShieldFrame <= 0) {
        playerShieldFrame = 0;
        playerShieldDeactivating = false;
      }
    }
  }
}

void UpdateBaddieShield(Baddie &baddie) {
  if (baddie.shield < baddie.shield_max) {
    if (baddie.shield_regen_timer <= 0)
      baddie.shield += baddie.shield_regen_amt * GetFrameTime();
    else {
      baddie.shield_regen_timer -= GetFrameTime();
      if (baddie.shield_regen_timer <= 0) {
        baddie.shield_regen_timer = 0;
        baddie.shield_activating = true;
      }
    }
  }
  if (baddie.shield_activating) {
    baddie.shield_frame_timer += GetFrameTime();
    if (baddie.shield_frame_timer >= playerShieldFrameTime) {
      baddie.shield_frame_timer = 0;
      baddie.shield_frame++;
      if (baddie.shield_frame >= playerShieldFrames - 1) {
        baddie.shield_frame = 3;
        baddie.shield_activating = false;
      }
    }
  } else if (baddie.shield_deactivating) {
    baddie.shield_frame_timer += GetFrameTime();
    if (baddie.shield_frame_timer >= playerShieldFrameTime) {
      baddie.shield_frame_timer = 0;
      baddie.shield_frame--;
      if (baddie.shield_frame <= 0) {
        baddie.shield_frame = 0;
        baddie.shield_deactivating = false;
      }
    }
  }
}

void DrawBackground() {
  DrawTexturePro(textures[BACKGROUND], Rectangle{0, 0, 500, 1000},
                 Rectangle{0, 0, screenSize.x, screenSize.y}, Vector2Zero(), 0, WHITE);

  DrawTexturePro(textures[MOVING_BACKGROUND],
                 Rectangle{129 * 2, 256 * 1, 129, 256},
                 Rectangle{0, movingBackgroundPosY - screenSize.y, screenSize.x, screenSize.y},
                 Vector2Zero(), 0, WHITE);
  DrawTexturePro(textures[MOVING_BACKGROUND],
                 Rectangle{129 * 2, 256 * 1, 129, 256},
                 Rectangle{0, movingBackgroundPosY, screenSize.x, screenSize.y}, Vector2Zero(), 0, WHITE);
}

void DrawPlayer() {
  if (playerExploding || playerDead) {
    DrawMiscTexture(
        Rectangle{(9 * 8.0f) + (8.0f * playerExpFrame), 6 * 8.0f, 8.0f, 8.0f},
        Rectangle{playerPos.x - (playerSize.x * 0.5f),
                  playerPos.y - (playerSize.y * 0.5f), playerSize.x * 2.0f,
                  playerSize.y * 2.0f},
        WHITE);
    return;
  }
  Color hitColor = WHITE;
  if (playerFlashTimer > 0)
    playerFlashTimer -= GetFrameTime();
  if (playerFlashTimer <= 0)
    hitColor = WHITE;
  else
    hitColor = YELLOW;
  int shipdirection;
  if (playerPreviousPos.x == playerPos.x)
    shipdirection = 1;
  else if (playerPreviousPos.x - playerPos.x > 1)
    shipdirection = 0;
  else if (playerPreviousPos.x - playerPos.x < -1)
    shipdirection = 2;
  else
    shipdirection = 1;

  // draw player ship with direction
  DrawShipTexture(shipdirection, 0, playerPos, hitColor);
  // draw shield shields
  if (GOD_MODE) {
    DrawShieldTexture(playerShieldFrames - 1, playerPos, playerSize, 0, RED);
  } else if ((playerShield > 0) || playerShieldActivating ||
             playerShieldDeactivating) {
    DrawShieldTexture(playerShieldFrame, playerPos, playerSize,
                      playerShield / playerShieldMax, WHITE);
    // DrawCircleV(Vector2{playerPos.x+(playerSize.x*0.5f),
    // playerPos.y+(playerSize.y*0.5f)}, (playerSize.x*0.5f)+playerShield,
    // YELLOW);
  }
  DrawPlayerFreezeTimer();
}

void DrawPlayerFreezeTimer(void) {
  DrawRing(Vector2{playerPos.x - (playerSize.x * 0.1f),
                   playerPos.y + (playerSize.y * 0.5f)},
           playerSize.x - 3.0f, playerSize.x, 135.0f,
           135.0f + (playerFreezeTimer * 90.0f), 5, RAYWHITE);
}

void DrawBaddie(int id) {
  if (Baddies[id].exploding) {
    DrawMiscTexture(Rectangle{9 * 8.0f + (8.0f * Baddies[id].exp_frame),
                              6 * 8.0f, 8.0f, 8.0f},
                    Rectangle{Baddies[id].position.x, Baddies[id].position.y,
                              Baddies[id].boss ? BossSize.x : BaddieSize.x,
                              Baddies[id].boss ? BossSize.y : BaddieSize.y},
                    WHITE);
    return;
  }
  int bx = 0;
  int by = 0;
  int maxX = 6;

  if (Baddies[id].boss)
    maxX = 3;
  for (int i = 0; i < Baddies[id].type; i++) {
    bx++;
    if (bx >= maxX) {
      bx = 0;
      by++;
    }
  }
  Color hitColor = WHITE;

  if (Baddies[id].flash_timer > 0)
    Baddies[id].flash_timer -= GetFrameTime();

  if (Baddies[id].flash_timer <= 0) {
    hitColor = (playerTimeFrozen ? SKYBLUE : WHITE);
  } else
    hitColor = YELLOW;

  if (Baddies[id].boss) {
    DrawBossTexture(bx, by, Baddies[id].position, hitColor);
  } else {
    DrawBaddieTexture(bx, by, Baddies[id].position, hitColor);
  }

  if (Baddies[id].shield > 0 || Baddies[id].shield_activating ||
      Baddies[id].shield_deactivating) {
    DrawShieldTexture(Baddies[id].shield_frame, Baddies[id].position,
                      Baddies[id].size,
                      Baddies[id].shield / Baddies[id].shield_max, WHITE);
  }

  // Draw bounding box for debug
  // Rectangle rec = getBaddieRec(Baddies[id]);
  // DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, WHITE);
  //
  // Draw hull/shield values for debug
  /* DrawText(TextFormat("%2.2f/%d", Baddies[id].shield, */
  /* Baddies[id].shield_max), */
  /*           Baddies[id].position.x, Baddies[id].position.y - 20, 10, WHITE);
   */
  /*  DrawText(TextFormat("%d/%d", Baddies[id].hull, Baddies[id].hull_max), */
  /*           Baddies[id].position.x, Baddies[id].position.y - 10, 10, WHITE);
   */
}

void DrawBullet(int id) {

  // find bullet shot style
  int bx = 0;
  int by = 0;
  for (int a = 0; a < Bullets[id].type; a++) {
    bx++;
    if (bx >= 6) {
      bx = 0;
      by++;
    }
  }
  if (Bullets[id].boss) {
    DrawBossProjectileTexture(bx, by, Bullets[id].position, Bullets[id].size,
                              135.0f, WHITE);
  } else {
    DrawProjectileTexture(bx, by, Bullets[id].position, Bullets[id].size,
                          180.0f * (!Bullets[id].player_bullet), WHITE);
  }

  // draw bullet bounding box for debug

  // Rectangle bulletRec = getBulletRec(Bullets[i]);
  // DrawRectangleLines(bulletRec.x, bulletRec.y, bulletRec.width,
  // bulletRec.height, WHITE);
  //  DrawCircleV(Bullets[i].position, 2*Bullets[i].damage, Bullets[i].Tint);
}

void DrawAsteroid(int id) {
  DrawAsteroidTexture(Asteroids[id].type, Asteroids[id].position,
                      Asteroids[id].size, Asteroids[id].rotation,
                      playerTimeFrozen ? SKYBLUE : WHITE);
  // Drawing bounding box for debug
  // Rectangle rec = getAsteroidRec(Asteroids[id]);
  // DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, WHITE);
}

void DrawFragment(int id) {
  DrawAsteroidTexture(Fragments[id].type, Fragments[id].position,
                      Fragments[id].size, Fragments[id].rotation,
                      playerTimeFrozen ? SKYBLUE : WHITE);
  // Drawing bounding box for debug
  // Rectangle rec = getAsteroidRec(Fragments[id]);
  // DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, WHITE);
}

void DrawPowerup(int id) {
  DrawUpgradeTexture(upgrade_matrix[Powerups[id].type].first,
                     upgrade_matrix[Powerups[id].type].second,
                     Powerups[id].position, PowerupSize,
                     (Powerups[id].lifetime <= 0.5f ? RED : GREEN));
}

void StopAllMusic(void) {
  for (int i = 0; i < NUM_MUSIC; i++) {
    if (IsMusicStreamPlaying(music[i]))
      StopMusicStream(music[i]);
  }
}

void ProcessPlayerInput(void) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (playerDead || playerWon)
      InitGame();
  }

  if (IsKeyPressed(KEY_P)) {
    if (player_game_state == GAME_STATE_PAUSED) {
      player_game_state = GAME_STATE_PLAYING;
    } else if (player_game_state == GAME_STATE_PLAYING) {
      player_game_state = GAME_STATE_PAUSED;
    }
  }

  if (player_game_state != GAME_STATE_PLAYING || playerExploding ||
      playerDead || playerWon)
    return; // abort input handling when not in a valid playing state

  // handle freeze time
  if (IsKeyDown(KEY_LEFT_SHIFT)) {
    playerTimeFrozen = (playerFreezeTimer > 0.0f);
  } else if (playerTimeFrozen) {
    playerTimeFrozen = false;
  }

  if (IsKeyPressed(keymap[keyToggleAutoshoot]))
    autoShoot = !autoShoot;

  if (IsKeyPressed(KEY_G)) {
    GOD_MODE = !GOD_MODE;
  }

  if (IsKeyPressed(KEY_EQUAL)) {
    playerShotStyle++;
  }
  if (IsKeyPressed(KEY_MINUS)) {
    playerShotStyle--;
  }

  // player movement and shooting
  float speed = 150 * GetFrameTime();
  if (IsKeyDown(keymap[keyShoot]) || IsMouseButtonDown(mouseKeymap[mouseShoot]) ||
      autoShoot) {
    if (playerCanShoot) {
      playerCanShoot = false;

      NewPlayerBullet(playerPos, playerShotStyle, ShotLifetime, GREEN, false);
    }
  } else
    speed *= 2;

  playerPreviousPos = playerPos;

  if (IsKeyDown(KEY_UP) || IsKeyDown(keymap[keyMoveUp]))
    playerPos.y -= speed;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(keymap[keyMoveDown]))
    playerPos.y += speed;
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(keymap[keyMoveLeft]))
    playerPos.x -= speed;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(keymap[keyMoveRight]))
    playerPos.x += speed;

  Vector2 mouseCoords = GetMousePosition();
  playerPos = mouseCoords;
  // clamps player position within screen space
  playerPos = Vector2Clamp(playerPos, Vector2{1.0f, 1.0f},
                           Vector2{screenSize.x - 1.0f - playerSize.x,
                                   screenSize.y - 1.0f - playerSize.y});
}

void ProcessPlayerState(void) {
  switch (player_game_state) {
  case GAME_STATE_INTRO:
    DrawIntro();
    break;
  case GAME_STATE_MAIN_MENU:
    DrawMainMenu();
    break;
  case GAME_STATE_SETTINGS:
    DrawSettingsMenu();
    break;
  case GAME_STATE_SHIP_SETUP:
    DrawShipSetupMenu();
    break;
  case GAME_STATE_PLAYING:
    UpdateAndDraw();
    break;
  case GAME_STATE_PAUSED:
    // DrawPauseMenu();
    UpdateAndDraw();
    break;
  case GAME_STATE_DEAD:
    // DrawDeathScreen();
    UpdateAndDraw();
    break;
  case GAME_STATE_WIN:
    // DrawWinScreen();
    UpdateAndDraw();
    break;
  }
}

void UpdateAndDraw(void) {

  UpdateMusicStream(*nowPlaying);
  // get normalized time played for current music stream
  musicPlayed =
      GetMusicTimePlayed(*nowPlaying) / GetMusicTimeLength(*nowPlaying);
  if (musicPlayed >= 0.99f) {
    PlayBackgroundMusic();
  }
  int i;

  UpdatePlayer();
  // update game objects
  if (player_game_state == GAME_STATE_PLAYING && !playerExploding &&
      !playerDead && !playerWon) {
    // Update background movement state
    UpdateBackground();
    // bullets
    for (i = 0; i < MAX_BULLETS; i++) {
      if (!Bullets[i].active)
        continue;
      UpdateBullet(i);
    }
    // baddies
    for (i = 0; i < MAX_BADDIES; i++) {
      if (!Baddies[i].active)
        continue;
      UpdateBaddie(i);
    }
    // powerups
    for (i = 0; i < MAX_POWERUPS; i++) {
      if (!Powerups[i].active)
        continue;
      UpdatePowerup(i);
    }
    // Asteroids
    for (i = 0; i < MAX_ASTEROIDS; i++) {
      if (!Asteroids[i].active)
        continue;
      UpdateAsteroid(i);
    }
    // Fragments
    for (i = 0; i < MAX_FRAGMENTS; i++) {
      if (!Fragments[i].active)
        continue;
      UpdateFragment(i);
    }
  }

  // Draw
  //--------------------------------//
  BeginDrawing();

  ClearBackground(BLACK);

  DrawBackground();

  DrawPlayer();

  // draw end line for baddies to cross
  DrawRectangle(0, screenSize.y - 100, screenSize.x, 3, BLACK);
  DrawLineV(Vector2{0.0f, screenSize.y - 100.0f},
            Vector2{screenSize.x, screenSize.y - 100.0f}, YELLOW);

  DrawStats();
  DrawHullBar();
  DrawShieldBar();
  DrawExpBar();

  for (i = 0; i < MAX_BULLETS; i++) {
    if (!Bullets[i].active)
      continue;
    DrawBullet(i);
  }

  for (i = 0; i < MAX_ASTEROIDS; i++) {
    if (!Asteroids[i].active)
      continue;
    DrawAsteroid(i);
  }

  for (i = 0; i < MAX_FRAGMENTS; i++) {
    if (!Fragments[i].active)
      continue;
    DrawFragment(i);
  }

  for (i = 0; i < MAX_BADDIES; i++) {
    if (!Baddies[i].active)
      continue;
    DrawBaddie(i);
  }

  for (i = 0; i < MAX_POWERUPS; i++) {
    if (!Powerups[i].active)
      continue;
    DrawPowerup(i);
  }

  DrawText("Mouse to Move, Click to Fire", 10, 10, 20, LIGHTGRAY);
  DrawText(TextFormat("Score %d", Score), 30, 30, 20, RED);
  DrawFPS(screenSize.x - 80, 20);

  if (playerDead)
    DrawDeathScreen();
  else if (playerWon)
    DrawWinScreen();
  else if (player_game_state == GAME_STATE_PAUSED) {
    DrawPauseMenu();
  }

  EndDrawing();
}

void DrawMainMenu(void) {}

void DrawSettingsMenu(void) {}

void DrawPauseMenu(void) {
  Rectangle rec = {10, screenSize.y / 2 - 100, screenSize.x - 20, 200};
  DrawRectangleRec(rec, BLACK);
  DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, RAYWHITE);
  int textSize = MeasureText("Paused", 20);
  DrawText("Paused", screenSize.x * 0.5f - textSize * 0.5f,
           screenSize.y * 0.5f - 10, 20, RAYWHITE);
}

void DrawShipSetupMenu(void) {}

void DrawIntro(void) {}

void DrawPopup(void) {
  if (death.anim_current < death.anim_total) {
    death.anim_current += GetFrameTime();
  }
  float perc = death.anim_current / death.anim_total;
  Rectangle popRec = {death.position.x - (death.size.x * perc * 0.5f),
                      death.position.y, death.size.x * perc, death.size.y};

  DrawRectangleRec(popRec, BLACK);
  DrawRectangleLinesEx(popRec, death.border_thickness, death.border_color);
  if (death.size.x * perc >
      MeasureText(TextFormat("%s", achievement_info[0].title.c_str()), 20))
    DrawText(TextFormat("%s", achievement_info[0].title.c_str()),
             death.position.x - (death.size.x * perc * 0.5f) + 10,
             death.position.y + 10, 20, WHITE);
}
void DrawDeathScreen(void) {
  DrawRectangle(10, 400, screenSize.x - 20, 200, BLACK);
  DrawRectangleLines(9, 399, screenSize.x - 19, 202, RED);

  DrawText(TextFormat("You are DEAD\n\nFinal score: %d\n\nYou lasted %1.0f "
                      "seconds.\n\nClick or tap to fly again.",
                      Score, endTime - gameTime),
           15, 405, 30, RAYWHITE);
}

void DrawWinScreen(void) {
  DrawRectangle(10, 400, screenSize.x - 20, 200, BLACK);
  DrawRectangleLines(9, 399, screenSize.x - 19, 202, GREEN);

  DrawText(TextFormat("You have WON!!\n\nFinal score: %d\n\nWon in %1.0f "
                      "seconds!\n\nClick or tap to fly again",
                      Score, endTime - gameTime),
           15, 405, 30, RAYWHITE);
}

// spawn a new asteroid of a given type with all random properties
int NewAsteroid(int type) {
  int id = FindEmptyAsteroid(Asteroids);
  if (id == -1)
    return -1; // didn't find a valid slot
  int scoreMult = (Score / 100);
  Asteroids[id].type = type ? type : GetRandomValue(1, 2);
  Asteroids[id].size = ((Asteroids[id].type == 1) ? 32.0f : 64.0f);
  Asteroids[id].active = true;
  Asteroids[id].position =
      Vector2{(float)GetRandomValue(0, screenSize.x - Asteroids[id].size),
              (float)GetRandomValue(0, Asteroids[id].size)};
  Asteroids[id].direction =
      Vector2Scale(Vector2Normalize(Vector2{GetRandomValue(-5, 5) / 10.0f, 1}),
                   AsteroidSpeed);
  Asteroids[id].lifetime = 20;
  Asteroids[id].durability = Asteroids[id].durability_max =
      ((Asteroids[id].type == 1) ? GetRandomValue(20, 20 + (scoreMult * 10))
                                 : GetRandomValue(40, 40 + (scoreMult * 20)));
  Asteroids[id].rotation_speed = (GetRandomValue(20, 40) / 10.0f) * 50;
  Asteroids[id].metal = GetRandomValue(1, NUM_METALS - 1);
  Asteroids[id].metal_amount = GetRandomValue(0, 1 + (scoreMult * 2));

  return id;
}

// spawn a fragment from an asteroid with specific properties
int NewFragment(int type, int size, Vector2 position, int durability, int metal,
                int metal_amt) {
  int id = FindEmptyFragment(Fragments);
  if (id == -1)
    return -1; // didn'ot find a fragment slot open
  Fragments[id].type = type;
  Fragments[id].size = size;
  Fragments[id].active = true;
  Fragments[id].position = position;
  Fragments[id].direction =
      Vector2Scale(Vector2Normalize(Vector2{GetRandomValue(-5, 5) / 10.0f, 1}),
                   AsteroidSpeed);
  Fragments[id].lifetime = 20;
  Fragments[id].durability = Fragments[id].durability_max = durability;
  Fragments[id].rotation_speed = (GetRandomValue(20, 40) / 10.0f) * 50;
  Fragments[id].metal = metal;
  Fragments[id].metal_amount = metal_amt;

  return id;
}

int NewBaddie() {
  int id = FindEmptyBaddie(Baddies);
  if (id == -1)
    return -1; // nope
  int scoreMult = (Score / 100);
  bool boss = (GetRandomValue(0, 100) < scoreMult);

  Baddies[id].boss = boss; // 5% chance to spawn a boss
  Baddies[id].size = (boss ? BossSize : BaddieSize);
  float xPos = GetRandomValue(0, screenSize.x - Baddies[id].size.x);
  float yPos = 0;
  Baddies[id].type =
      GetRandomValue(0, (boss ? NUM_BOSS_TYPES - 1 : NUM_BADDIE_TYPES - 1));
  Baddies[id].shot_style = GetRandomValue(
      0, (boss ? NUM_BOSS_SHOT_STYLES - 1 : NUM_SHOT_STYLES - 1));
  Baddies[id].bullets_per_shot =
      (boss ? GetRandomValue(2, 4) : 1); // boss gets 1-3 shots
  Baddies[id].active = true;
  Baddies[id].position = Vector2{xPos, yPos};
  Baddies[id].direction = Vector2{
      0,
      (float)(boss ? GetRandomValue(BaddieSpeed * 0.15, BaddieSpeed * 0.35)
                   : GetRandomValue(BaddieSpeed * 0.5f, BaddieSpeed * 1.5f))};
  Baddies[id].lifetime = 60.0f;
  Baddies[id].tint = WHITE;
  Baddies[id].player_bullet = false;
  Baddies[id].shot_timer = 0;
  Baddies[id].hull = Baddies[id].hull_max =
      (boss ? GetRandomValue(20, (20 + (scoreMult * 5)))
            : GetRandomValue(1, (scoreMult ? (scoreMult * 2) : 1)));
  Baddies[id].shield = Baddies[id].shield_max =
      (boss ? GetRandomValue(40, (40 + (scoreMult * 10)))
            : GetRandomValue(0, (scoreMult ? (scoreMult * 4) : 0)));
  // leave baddie hull strength at 0 until I implement increased player damage
  Baddies[id].hull_strength = (boss ? 0 : 0);
  Baddies[id].flash_timer = 0;
  Baddies[id].exploding = false;
  Baddies[id].exp_frame = 0;
  Baddies[id].exp_timer = 0;
  if (Baddies[id].shield > 0) {
    Baddies[id].shield_activating = true;
    Baddies[id].shield_frame_timer = playerShieldFrameTime;
    Baddies[id].shield_frame = 0;
    Baddies[id].shield_regen_amt = 0.5f;
  }
  return id;
}

int NewPowerup(Vector2 position, Vector2 direction, float lifetime) {
  int id = FindEmptyPowerup(Powerups);
  if (id == -1)
    return -1;

  Powerups[id].active = true;
  Powerups[id].position = position;
  Powerups[id].direction = direction;
  Powerups[id].lifetime = lifetime;
  Powerups[id].type = GetRandomValue(0, NUM_POWERUP_TYPES - 1);
  PlaySound(sounds[POWERUP_DROP]);

  return id;
}

void UpdateBullet(int id) {
  Bullets[id].lifetime -= GetFrameTime(); // expire shots
  if (Bullets[id].lifetime <= 0) {
    ResetBullet(&Bullets[id]);
    return;
  }
  Bullets[id].position =
      Vector2Add(Bullets[id].position,
                 Vector2Scale(Bullets[id].direction, GetFrameTime()));

  if (Bullets[id].position.x <= 0 || Bullets[id].position.x >= screenSize.x ||
      Bullets[id].position.y <= 0 ||
      Bullets[id].position.y >= screenSize.y - 100.0f) {
    PerformKillBullet(&Bullets[id]);
    return;
  }

  if (!Bullets[id].player_bullet) {
    if (!GOD_MODE) {
      if (playerShield > 0) {
        if (CheckCollisionCircleRec(getPlayerCenter(), getPlayerShieldRadius(),
                                    getBulletRec(Bullets[id]))) {
          PerformHitPlayer(Bullets[id]);
          PlaySound(sounds[SHIELD_HIT]);
        }
      } else {
        if (CheckCollisionRecs(getPlayerRec(), getBulletRec(Bullets[id]))) {
          PerformHitPlayer(Bullets[id]);
          PlaySound(sounds[HULL_HIT]);
        }
      }
    }
  } else {
    for (int i = 0; i < MAX_BADDIES; i++) {
      if (!Baddies[i].active)
        continue;
      if (Baddies[i].shield > 0) {
        // check if bullet hits baddie shield
        if (CheckCollisionCircleRec(getBaddieCenter(Baddies[i]),
                                    getBaddieShieldRadius(Baddies[i]),
                                    getBulletRec(Bullets[id]))) {
          PerformHitBaddie(Bullets[id], Baddies[i]);
          break;
        }
      } else {
        // check if bullet hits baddie ship
        if (CheckCollisionRecs(getBulletRec(Bullets[id]),
                               getBaddieRec(Baddies[i]))) {
          PerformHitBaddie(Bullets[id], Baddies[i]);
          break;
        }
      }
    }
  }
}

void UpdatePlayer(void) {
  if (Score >= 10000 && !GOD_MODE && !playerWon) {
    PlaySound(sounds[WIN_TRUMPETS]);
    endTime = GetTime();
    playerWon = true;
    StopAllMusic();
    nowPlaying = &music[WIN];
    PlayMusicStream(*nowPlaying);
    return;
  }
  if (playerHull <= 0 && playerExploding) {
    playerExpTimer += GetFrameTime();
    if (playerExpTimer >= 0.2f) {
      playerExpTimer = 0;
      playerExpFrame++;
      if (playerExpFrame > 3) {
        playerExploding = false;
        playerDead = true;
      }
    }
    return;
  } else if (!playerDead && playerHull <= 1) {
    playerAlertTimer += GetFrameTime();
    if (playerAlertTimer >= 2.0f) {
      playerAlertTimer = 0;
      PlaySound(sounds[ALERT_LOW_HULL]);
    }
  }

  UpdatePlayerShield();

  // update shot timer
  if (!playerCanShoot) {
    playerShotTimer -= GetFrameTime();
    if (playerShotTimer <= 0) {
      playerCanShoot = true;
      playerShotTimer = playerShotReload;
    }
  }

  // update player freeze timer
  if (playerTimeFrozen) {
    playerFreezeTimer -= GetFrameTime();
    if (playerFreezeTimer <= 0) {
      playerTimeFrozen = false;
    }
  } else if (playerFreezeTimer < playerFreezeTime && IsKeyUp(KEY_LEFT_SHIFT)) {
    playerFreezeTimer += GetFrameTime();
    playerFreezeTimer = Clamp(playerFreezeTimer, 0.0f, playerFreezeTime);
  }
}
void ResetBaddie(Baddie *baddie) {
  baddie->active = false;
  NewBaddie();
  // memset(baddie, '\0', sizeof(&baddie)); // fill every property with NULL
}
void ResetBullet(Bullet *bullet) {
  bullet->active = false;
  // memset(bullet, '\0', sizeof(&bullet));
}
void ResetAsteroid(Asteroid *asteroid) {
  asteroid->active = false;
  // memset(asteroid, '\0', sizeof(&asteroid));
}
void ResetPowerup(Powerup *powerup) {
  powerup->active = false;
  // memset(powerup, '\0', sizeof(&powerup));
}
void PerformKillPlayer(void) {
  playerHull = 0;
  playerShield = 0;
  playerExploding = true;
  playerExpFrame = 0;
  playerExpTimer = 0;
  playerAlertTimer = 0;
  playerShieldActivating = false;
  playerShieldDeactivating = false;
  PlaySound(sounds[EXPLOSION]);
  endTime = GetTime();
  StopAllMusic();
  nowPlaying = &music[DEATH];
  PlayMusicStream(*nowPlaying);
}
void PerformKillBaddie(Baddie *baddie) {
  baddie->hull = 0;
  baddie->shield = 0;
  baddie->exploding = true;
  baddie->exp_frame = 0;
  baddie->exp_timer = 0;
  baddie->shield_activating = false;
  baddie->shield_deactivating = false;
  SetSoundVolume(sounds[EXPLOSION], 0.5f);
  PlaySound(sounds[EXPLOSION]);
}
void PerformKillBullet(Bullet *bullet) { bullet->lifetime = -1; }
void PerformKillAsteroid(Asteroid *asteroid) { asteroid->lifetime = -1; }
void PerformKillPowerup(Powerup *powerup) { powerup->lifetime = -1; }

void PerformFragmentAsteroid(Asteroid *asteroid) {
  // check if there's enough metal to spread between smaller Asteroids
  int newFragments = GetRandomValue(2, 4);
  for (int i = 0; i < newFragments; i++) {
    // break into random equally sized pieces
    // @TODO -- make this more random with the possibility of spawning
    // both large and small fragments that are proportional in size to
    // the parent asteroid
    NewFragment(1, asteroid->size / newFragments, asteroid->position,
                asteroid->durability_max / newFragments, asteroid->metal,
                asteroid->metal_amount / newFragments);
  }
}

void PerformHitPlayer(Bullet &bullet) {
  // we are passing in the bullet so we can do damage calculating... eventually
  PerformKillBullet(&bullet);
  Score -= Clamp((bullet.damage - playerHullStrength), 0, bullet.damage);
  if (playerShield > 0) {
    playerShield -=
        fminf(playerShield, bullet.damage); // prevent shields going negative
    if (playerShield == 0) {
      playerShieldDeactivating = true;
      PlaySound(sounds[SHIELD_DROP]);
      // hit took shields offline, begin regen timer
      playerFlashTimer =
          playerFlashTime; // flash player ship for a short duration
      playerShieldRegenTimer =
          playerShieldRegenTime; // shield took a hit, initiate shield regen
                                 // delay
    }
  } else if (playerHull > (bullet.damage - playerHullStrength - 1)) {
    playerHull -=
        Clamp((bullet.damage - playerHullStrength), 0,
              playerHull); // prevent damage going negative (healing player)
    playerFlashTimer = playerFlashTime;
  } else {
    PerformKillPlayer();
  }
}

void PerformHitBaddie(Bullet &bullet, Baddie &baddie) {
  PerformKillBullet(&bullet);
  if (baddie.shield > 0) {
    baddie.shield -= fminf(baddie.shield, bullet.damage);
    if (baddie.shield == 0) {
      baddie.shield_deactivating = true;

      baddie.flash_timer = playerFlashTime;
      baddie.shield_regen_timer = playerShieldRegenTime;
    }
  } else if (baddie.hull > 0) {
    baddie.hull -= fminf(baddie.hull, bullet.damage);
    baddie.flash_timer = playerFlashTime;
    if (baddie.hull <= 0) {
      PerformKillBaddie(&baddie);
      if (GetRandomValue(1, 100) <
          (baddie.boss ? 20 : 5)) { // 20% : 5% chance for a powerup
        NewPowerup(baddie.position, Vector2{0, (float)PowerupSpeed}, 5.0f);
      }
      if (GetRandomValue(1, 100) < 10) {
        NewAsteroid(0);
      }
      int value = (baddie.hull_max);
      if (baddie.boss)
        value *= 2;
      Score += value;
    }
  }
}

void PerformHitAsteroid(Bullet &bullet, Asteroid &hit) {
  PerformKillBullet(&bullet);
  if (hit.durability > (bullet.damage - 1)) {
    hit.durability -= Clamp((bullet.damage), 0, hit.durability);
    PlaySound(sounds[ASTEROID_HIT]);
  } else {
    PerformKillAsteroid(&hit);
    if (hit.type == 2) {
      // fragent larger asteroid into several smaller ones
      PerformFragmentAsteroid(&hit);
    }
    // PlaySound(sounds[SUCCESS]);
    if (GetRandomValue(1, 100) <
        33) { // 33% chance to drop a resource collectible
      NewPowerup(hit.position, Vector2{0, (float)PowerupSpeed}, 5.0f);
      // skip the resource drop for now, just give some bonus score
      // NewResource(hit.position, Vector2{ 0, (float)PowerupSpeed },
      // GetRandomValue(0, 1000) / 1000.0f );
    }

    Score += (hit.durability_max);
    if (GetRandomValue(1, 100) < 10) {
      NewAsteroid(0);
    }
  }
}

void UpdatePowerup(int id) {
  Powerups[id].lifetime -= GetFrameTime();
  if (Powerups[id].lifetime <= 0) {
    ResetPowerup(&Powerups[id]);
    return;
  }
  Powerups[id].position =
      Vector2Add(Powerups[id].position,
                 Vector2Scale(Powerups[id].direction, GetFrameTime()));

  if (Powerups[id].position.y + PowerupSize.y > (screenSize.y - 100.0f)) {
    // ran into the endzone and disappeared
    PerformKillPowerup(&Powerups[id]);
    return;
  }
  Rectangle powerupRec = {Powerups[id].position.x, Powerups[id].position.y,
                          PowerupSize.x, PowerupSize.y};

  if (CheckCollisionRecs(powerupRec, getPlayerRec())) {
    PerformKillPowerup(&Powerups[id]);
    // perform player powerups here
    switch (Powerups[id].type) {
    case SHOT_RELOAD:
      PlaySound(sounds[POWERUP_COLLECT]);
      playerShotReload -= 1.0f / 60.0f; // increase shots per second
      break;
    case SHOT_SPEED:
      PlaySound(sounds[POWERUP_COLLECT]);
      playerShotSpeed += 100.0f;
      break;
    case SHOT_POWER:
      if (playerShotPower < 5) {
        playerShotPower++;
      }
      if (playerShotStyle < NUM_SHOT_STYLES - 1) {
        PlaySound(sounds[POWERUP_DAMAGE_INCREASE]);
        playerShotStyle++;
      }
      break;
    case SHOT_SPREAD:
      if (playerShotSpread < 5) {
        PlaySound(sounds[POWERUP_SPREAD_INCREASE]);
        playerShotSpread++;
      } else {
        PlaySound(sounds[NEGATIVE]);
      }
      break;
    case SHIELD_REPAIR:
      if (playerShieldMax == 0) {
        // Only increase shieldmax if we have no shields
        playerShieldMax++;
      }
      if (playerShield == 0) {
        PlaySound(sounds[SHIELD_RAISE]);
        playerShieldActivating = true;
      } else {
        PlaySound(sounds[NEGATIVE]);
      }
      playerShield = playerShieldMax;
      break;
    case SHIELD_MAX:
      playerShieldMax++;
      if (playerShield == 0) {
        PlaySound(sounds[SHIELD_RAISE]);
        playerShieldActivating = true;
      } else {
        PlaySound(sounds[NEGATIVE]);
      }
      playerShield = playerShieldMax;
      break;
    case HULL_REPAIR:
      PlaySound(sounds[POWERUP_HULL_RESTORE]);
      playerHull = playerHullMax;
      break;
    case HULL_MAX:
      PlaySound(sounds[POWERUP_HULL_INCREASE]);
      playerHullMax++;
      playerHull = playerHullMax;
      break;
    case HULL_STRENGTH:
      if (playerHullStrength < 5) {
        PlaySound(sounds[POWERUP_HULL_STRENGTH]);
        playerHullStrength++;
      } else {
        PlaySound(sounds[NEGATIVE]);
      }
      break;
    default:
      break;
    }

    return;
  }
}

Rectangle getBulletRec(Bullet &bullet) {
  const Rectangle *mat = (bullet.boss ? &boss_shot_size_matrix[bullet.type]
                                      : &shot_size_matrix[bullet.type]);
  Vector2 scale = {bullet.size.x * 0.125f, bullet.size.y * 0.125f};
  Rectangle bulletRec;
  if (bullet.player_bullet) {
    bulletRec = {
        bullet.position.x - (bullet.size.x * 0.5f) + (mat->x * scale.x),
        bullet.position.y - (bullet.size.y * 0.5f) + (mat->y * scale.y),
        mat->width * scale.x, mat->height * scale.y};
  } else {
    Vector2 rotated = {8.0f - (mat->x + mat->width),
                       8.0f - (mat->y + mat->height)};
    bulletRec = {
        bullet.position.x - (bullet.size.x * 0.5f) + (rotated.x * scale.x),
        bullet.position.y - (bullet.size.x * 0.5f) + (rotated.y * scale.y),
        mat->width * scale.x, mat->height * scale.y};
  }
  return bulletRec;
}

Rectangle getAsteroidRec(Asteroid &asteroid) {
  return Rectangle{asteroid.position.x - asteroid.size * 0.5f,
                   asteroid.position.y - asteroid.size * 0.5f, asteroid.size,
                   asteroid.size};
}
Rectangle getPlayerRec(void) {
  return Rectangle{playerPos.x, playerPos.y, playerSize.x, playerSize.y};
}
Rectangle getBaddieRec(Baddie &baddie) {
  const Rectangle *mat = &baddie_size_matrix[baddie.type];
  Vector2 scale = {baddie.size.x * 0.125f, baddie.size.y * 0.125f};
  return Rectangle{baddie.position.x + (mat->x * scale.x),
                   baddie.position.y + (mat->y * scale.y), mat->width * scale.x,
                   mat->height * scale.y};
}

Vector2 getBaddieCenter(Baddie &baddie) {
  return Vector2{baddie.position.x + (baddie.size.x * 0.5f),
                 baddie.position.y + (baddie.size.y * 0.5f)};
}

float getBaddieShieldRadius(Baddie &baddie) {
  if (baddie.shield <= 0)
    return 0.0f;
  return baddie.size.x * 0.5f + (20.0f * (baddie.shield / baddie.shield_max));
}

Vector2 getPlayerCenter(void) {
  return Vector2{playerPos.x + (playerSize.x * 0.5f),
                 playerPos.y + (playerSize.y * 0.5f)};
}

float getPlayerShieldRadius(void) {
  if (playerShield <= 0)
    return 0.0f;
  return playerSize.x * 0.5f + (20.0f * (playerShield / playerShieldMax));
}

void UpdateBackground(void) {
  movingBackgroundPosY += 50 * GetFrameTime();
  if (movingBackgroundPosY >= screenSize.y)
    movingBackgroundPosY = 0;
}

void UpdateBaddie(int id) {
  if (Baddies[id].hull <= 0 && Baddies[id].exploding) {
    Baddies[id].exp_timer += GetFrameTime();
    if (Baddies[id].exp_timer >= 0.2f) {
      Baddies[id].exp_timer = 0;
      Baddies[id].exp_frame++;
      if (Baddies[id].exp_frame > 3) {
        Baddies[id].exploding = false;

        ResetBaddie(&Baddies[id]);
      }
    }
    return;
  }

  if (!playerTimeFrozen) {
    Baddies[id].position =
        Vector2Add(Baddies[id].position,
                   Vector2Scale(Baddies[id].direction, GetFrameTime()));
  }

  if (Baddies[id].position.y + BaddieSize.y >= (screenSize.y - 100.0f)) {
    // they got through
    ResetBaddie(&Baddies[id]);
    Score -= 10;
    return;
  }

  UpdateBaddieShield(Baddies[id]);

  if (Baddies[id].shield > 0) {
    // perform shield collisions
    if (playerShield > 0) {
      if (CheckCollisionCircles(getBaddieCenter(Baddies[id]),
                                getBaddieShieldRadius(Baddies[id]),
                                getPlayerCenter(), getPlayerShieldRadius())) {
        // player with shield runs into baddie with shield
        Baddies[id].shield = 0;
        Baddies[id].shield_deactivating = true;
        if (!GOD_MODE) {
          playerShield = 0;
          playerShieldDeactivating = true;
          playerShieldRegenTimer = playerShieldRegenTime;
          PlaySound(sounds[SHIELD_DROP]);
        }
        return;
      }
    } else {
      if (CheckCollisionCircleRec(getBaddieCenter(Baddies[id]),
                                  getBaddieShieldRadius(Baddies[id]),
                                  getPlayerRec())) {
        // player without shield runs into baddie with shield
        Vector2 collision_angle = {playerPos.x - Baddies[id].position.x,
                                   playerPos.y - Baddies[id].position.y};
        // bounce player off the baddie shield
        playerPos = Vector2Add(playerPos,
                               Vector2Scale(collision_angle, GetFrameTime()));

        playerPos = Vector2Clamp(playerPos, Vector2{1.0f, 1.0f},
                                 Vector2{screenSize.x - 1.0f - playerSize.x,
                                         screenSize.y - 1.0f - playerSize.y});

        Baddies[id].shield = 0;
        Baddies[id].shield_deactivating = true;
        Baddies[id].shield_regen_timer = playerShieldRegenTime;
        PlaySound(sounds[SHIELD_BOUNCE]);
        return;
      }
    }
  } else {
    // perform ship collisions
    if (playerShield > 0) {
      if (CheckCollisionCircleRec(getPlayerCenter(), getPlayerShieldRadius(),
                                  getBaddieRec(Baddies[id]))) {
        // perform player shield to baddie ship collision
        Vector2 collision_angle = {playerPos.x - Baddies[id].position.x,
                                   playerPos.y - Baddies[id].position.y};
        // bounce baddie off player shield
        Baddies[id].position =
            Vector2Add(Baddies[id].position,
                       Vector2Scale(collision_angle, GetFrameTime()));
        Baddies[id].position =
            Vector2Clamp(Baddies[id].position, Vector2{1.0f, 1.0f},
                         Vector2{screenSize.x - 1.0f - Baddies[id].size.x,
                                 screenSize.y - 1.0f - Baddies[id].size.y});

        if (!GOD_MODE) {
          playerShield = 0;
          playerShieldDeactivating = true;
          playerShieldRegenTimer = playerShieldRegenTime;
          PlaySound(sounds[SHIELD_DROP]);
        }
        return;
      }
    } else {
      if (CheckCollisionRecs(getBaddieRec(Baddies[id]), getPlayerRec())) {
        // perform player ship to baddie ship collision
        // player crashed into baddie, nobody wins with the headbutt!!
        PerformKillBaddie(&Baddies[id]);
        if (!GOD_MODE) {
          PerformKillPlayer();
        }
        return;
      }
    }
  }

  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (!Asteroids[i].active)
      continue;
    if (CheckCollisionRecs(getBaddieRec(Baddies[id]),
                           getAsteroidRec(Asteroids[i]))) {
      // baddie crashed into asteroid, baddie loses
      PerformKillBaddie(&Baddies[id]);
      return;
    }
  }

  for (int i = 0; i < MAX_FRAGMENTS; i++) {
    if (!Fragments[i].active)
      continue;
    if (CheckCollisionRecs(getBaddieRec(Baddies[id]),
                           getAsteroidRec(Fragments[i]))) {
      // baddie crashed into fragment, baddie loses
      PerformKillBaddie(&Baddies[id]);
      return;
    }
  }

  Baddies[id].shot_timer -= GetFrameTime();
  if (Baddies[id].shot_timer <= 0 && GetRandomValue(1, 100) < 10) {
    // make enemy bullet
    Baddies[id].shot_timer = GetRandomValue(1, 4);
    NewBaddieBullet(Baddies[id], ShotLifetime * 2, RED);
  }
}

void UpdateAsteroid(int id) {

  Asteroids[id].lifetime -= GetFrameTime();
  if (Asteroids[id].lifetime <= 0) {
    ResetAsteroid(&Asteroids[id]);
    return;
  }

  if (!playerTimeFrozen) {
    Asteroids[id].position =
        Vector2Add(Asteroids[id].position,
                   Vector2Scale(Asteroids[id].direction, GetFrameTime()));
    Asteroids[id].rotation += Asteroids[id].rotation_speed * GetFrameTime();
  }
  if (Asteroids[id].rotation >= 360.0f) {
    Asteroids[id].rotation -= 360.0f;
  }

  if (Asteroids[id].position.y + Asteroids[id].size >=
      (screenSize.y - 100.0f)) {
    // asteroid made it through
    PerformKillAsteroid(&Asteroids[id]);
    Score -= 20;
    return;
  }
  if (Asteroids[id].position.x + Asteroids[id].size <= 0 ||
      Asteroids[id].position.x >= screenSize.x) {
    // asteroid went off the side of screen, no penalty but it still disappears
    PerformKillAsteroid(&Asteroids[id]);
    return;
  }

  Rectangle asteroidRect = getAsteroidRec(Asteroids[id]);

  if (!GOD_MODE && CheckCollisionRecs(asteroidRect, getPlayerRec())) {
    // player ran into asteroid, destroy asteroid
    PerformKillAsteroid(&Asteroids[id]);

    if (playerShield > 0) {
      // player had shields, deactivate shield
      playerShield = 0;
      playerShieldDeactivating = true;
      playerShieldRegenTimer = playerShieldRegenTime;
      PlaySound(sounds[SHIELD_DROP]);
    } else {
      // player had no shields, player dies
      PerformKillPlayer();
    }
    return;
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    if (Bullets[i].active && Bullets[i].player_bullet) {
      if (CheckCollisionRecs(getBulletRec(Bullets[i]), asteroidRect)) {
        PerformHitAsteroid(Bullets[i], Asteroids[id]);
      }
    }
  }
}

void UpdateFragment(int id) {
  Fragments[id].lifetime -= GetFrameTime();
  if (Fragments[id].lifetime <= 0) {
    ResetAsteroid(&Fragments[id]);
    return;
  }
  if (!playerTimeFrozen) {
    Fragments[id].position =
        Vector2Add(Fragments[id].position,
                   Vector2Scale(Fragments[id].direction, GetFrameTime()));
    Fragments[id].rotation += Fragments[id].rotation_speed * GetFrameTime();
  }
  if (Fragments[id].rotation >= 360.0f) {
    Fragments[id].rotation -= 360.0f;
  }
  if (Fragments[id].position.y + Fragments[id].size >=
      (screenSize.y - 100.0f)) {
    PerformKillAsteroid(&Fragments[id]);
    Score -= 10;
    return;
  }
  if ((Fragments[id].position.x + Fragments[id].size <= 0) ||
      (Fragments[id].position.x >= screenSize.x)) {
    PerformKillAsteroid(&Fragments[id]);
    return;
  }
  Rectangle fragmentRect = getAsteroidRec(Fragments[id]);

  if (!GOD_MODE && CheckCollisionRecs(fragmentRect, getPlayerRec())) {
    // player ran into asteroid, destroy asteroid
    PerformKillAsteroid(&Fragments[id]);

    if (playerShield > 0) {
      // player had a shield up, deactivate shields
      playerShield = 0;
      playerShieldDeactivating = true;
      playerShieldRegenTimer = playerShieldRegenTime;
      PlaySound(sounds[SHIELD_DROP]);
    } else {
      // player had no shields, player dies
      PerformKillPlayer();
    }
    return;
  }
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (Bullets[i].active && Bullets[i].player_bullet) {
      if (CheckCollisionRecs(getBulletRec(Bullets[i]), fragmentRect)) {
        PerformHitAsteroid(Bullets[i], Fragments[id]);
      }
    }
  }
}

void DrawStats() {
  DrawText(TextFormat("Damage: ", playerShotPower), 5, screenSize.y - 98, 20,
           WHITE);
  for (int i = 0; i < playerShotPower; i++) {
    DrawUpgradeTexture(
        3, 0,
        Vector2{MeasureText("Damage: ", 20) + (i * 25.0f), screenSize.y - 98},
        Vector2{20, 20}, WHITE);
  }
  DrawText(TextFormat("Armor: %d", playerHullStrength), screenSize.x * 0.5f,
           screenSize.y - 98, 20, WHITE);
  for (int i = 0; i < playerHullStrength; i++) {
    DrawUpgradeTexture(
        0, 1,
        Vector2{screenSize.x * 0.5f + MeasureText("Armor: ", 20) + (i * 25.0f),
                screenSize.y - 98},
        Vector2{20, 20}, WHITE);
  }
  // draw gametime for debug
  // DrawText(TextFormat("%1.0f - %1.0f: %1.0f", endTime, gameTime, GetTime()),
  // 5, screenSize.y - 110, 10, WHITE);
}

void DrawHullBar() {
  if (playerHull < 0 || playerHullMax <= 0)
    return;

  float percent = (float)playerHull / playerHullMax;

  DrawRing(Vector2{playerPos.x + playerSize.x + (playerSize.x * 0.1f),
                   playerPos.y + (playerSize.y * 0.5f)},
           playerSize.x - 3.0f, playerSize.x, 45.0f, 45.0f - (percent * 90.0f),
           5, RED);

  Rectangle bar = {100, screenSize.y - 70, screenSize.x - 100, 20};

  DrawRectangle(bar.x, bar.y, bar.width, bar.height, GRAY);
  DrawText("Hull:", bar.x - MeasureText("Hull:", 20), bar.y + 2, 20, RAYWHITE);
  DrawRectangle(bar.x, bar.y, bar.width * percent, bar.height, RED);
  DrawText(TextFormat("%2.0f%%", percent * 100), bar.x + bar.width * 0.5f,
           bar.y + 2, 20, BLACK);
}

void DrawShieldBar() {
  // if (playerShieldMax <= 0) return;
  Rectangle bar = {100, screenSize.y - 50, screenSize.x - 100, 20};

  float percent = (playerShieldMax > 0 ? playerShield / playerShieldMax : 0);

  DrawRing(Vector2{playerPos.x + playerSize.x + 5 + (playerSize.x * 0.1f),
                   playerPos.y + (playerSize.y * 0.5f)},
           playerSize.x - 3.0f, playerSize.x, 45.0f, 45.0f - (percent * 90.0f),
           5, YELLOW);

  DrawRectangle(bar.x, bar.y, bar.width, bar.height, GRAY);
  DrawText("Shield:", bar.x - MeasureText("Shield:", 20), bar.y + 2, 20,
           RAYWHITE);
  if (playerShield > 0 || playerShieldMax == 0) {
    DrawRectangle(bar.x, bar.y, bar.width * percent, bar.height, YELLOW);
    DrawText(TextFormat("%2.0f%%", percent * 100), bar.x + bar.width * 0.5f,
             bar.y + 2, 20, BLACK);
  } else {
    float percent = playerShieldRegenTimer / playerShieldRegenTime;
    DrawRectangle(100, screenSize.y - 50, ((screenSize.x - 100) * percent), 20,
                  GRAY);
    DrawText(
        TextFormat("Recharging..."),
        (bar.x + bar.width * 0.5f - (MeasureText("Recharging...", 20) * 0.5f)),
        bar.y + 2, 20, RAYWHITE);
  }
}

void DrawExpBar() {
  Rectangle bar = {100, screenSize.y - 30, screenSize.x - 100, 20};
  float percent = (float)Score / 10000;
  DrawRectangle(bar.x, bar.y, bar.width, bar.height, GRAY);
  DrawText("Progress:", bar.x - MeasureText("Progress:", 20), bar.y + 2, 20,
           RAYWHITE);

  DrawRectangle(bar.x, bar.y, bar.width * percent, bar.height, GREEN);
  DrawText(TextFormat("%2.0f%%", percent * 100), bar.x + bar.width * 0.5f,
           bar.y + 2, 20, BLACK);
}

int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------

  InitWindow(screenSize.x, screenSize.y, "Bad Encounter");

  const char *app_dir = GetApplicationDirectory();
  ChangeDirectory(app_dir);

  InitAudioDevice();

  DisableCursor();

  load_textures();
  load_music();
  load_sounds();

  InitGame();

#ifndef __EMSCRIPTEN__

  // SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor())); // Set our game
  // to run at v-sync

#endif

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    ProcessPlayerInput();

    ProcessPlayerState();
  }
  // De-Initialization
  //--------------------------------------------------------------------------------------
  unload_music();
  unload_sounds();
  unload_textures();
  CloseAudioDevice();
  CloseWindow(); // Close window and OpenGL context
                 //--------------------------------------------------------------------------------------

  return 0;
}
