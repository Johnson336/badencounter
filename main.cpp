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
  keyToggleAutoshoot,
  keyGodMode,
  keyAbility,
  keyPause,
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
  {keyToggleAutoshoot, KEY_I},
  {keyGodMode, KEY_G},
  {keyAbility, KEY_LEFT_SHIFT},
  {keyPause, KEY_P},
};

std::map<mouseKeys, MouseButton> mouseKeymap = {
  {mouseShoot, MOUSE_BUTTON_LEFT},
};

const std::pair<int, int> upgrade_matrix[NUM_POWERUP_TYPES] = {
    {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}};

const int NUM_ENEMY_TYPES= 36;

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

const Rectangle enemy_size_matrix[NUM_ENEMY_TYPES] = {
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

struct Player {
  bool autoShoot = false;
  bool GOD_MODE = false;
  Vector2 pos = {0};
  Vector2 target = {0};
  Vector2 movement = {0};
  float rotation = 0;
  Vector2 prevPos = {0};
  Vector2 size = {32.0f, 32.0f};
  bool dead = false;
  bool won = false;
  Vector2 speed = {150.0f, 0.0f};
  int hull = 3;
  int hullMax = 3;
  int hullStrength = 1; // mitigates damage
  int shield = 0;
  int shieldMax = 0;
  int shotSpeed = 100;
  float shotReload = 10.0f / 60.0f;
  float shotTimer = 0;
  bool canShoot = true;
  int shotPower = 1;
  int shotSpread = 1;
  int shotStyle = 0;

  float shieldRegenTime = 2.0f;
  float shieldRegenTimer = 0;
  float shieldRegenAmt = 0.02f;

  // animation variables
  float shieldFrameTime = 0.25f; // secs between shield frame transitions
  float shieldFrameTimer = 0.0f;
  int shieldFrames = 4;
  int shieldFrame = 0;
  bool shieldActivating = false;
  bool shieldDeactivating = false;

  // player exploding variables
  bool exploding = false;
  int expFrame = 0;
  float expTimer = 0;

  // player alert variables
  float alertTimer = 0;
  float flashTime = 0.5f;
  float flashTimer = 0;

  double gameTime = 0;
  double endTime = 0;

  // player time freeze variables
  float freezeTime = 1.0f;
  float freezeTimer = freezeTime;
  bool timeFrozen = false;
  int Score = 0;
};

struct Enemy {
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

#define MAX_ENEMIES 100
#define MAX_BULLETS 1000
#define MAX_POWERUPS 10
#define MAX_ASTEROIDS 5
#define MAX_FRAGMENTS 100

Vector2 screenSize = {800, 1200};

Enemy Enemies[MAX_ENEMIES];
Bullet Bullets[MAX_BULLETS];
Powerup Powerups[MAX_POWERUPS];
Asteroid Asteroids[MAX_ASTEROIDS];
Asteroid Fragments[MAX_FRAGMENTS];
Player player; // main player structure

Popup achievement;
Popup death = {false, 20.0f, Vector2{screenSize.x * 0.5f, screenSize.y * 0.5f}, Vector2{300, 50}, RED, 1, 0.25f, 0};

Vector2 GetScreenSize(void);
Vector2 GetShotSpeed(int totalShots, int currentShot, float speed, float direction);
int NewEnemy(void);
void UpdatePlayer(void);
void UpdatePowerup(int id);
void UpdateEnemy(int id);
void UpdateBullet(int id);
void UpdateAsteroid(int id);
void UpdateFragment(int id);
void StopAllMusic(void);
void PerformHitPlayer(Bullet &bullet);
void PerformHitEnemy(Bullet &bullet, Enemy &enemy);
void PerformHitAsteroid(Bullet &bullet, Asteroid &hit);
void PerformKillEnemy(Enemy *enemy);
void PerformKillBullet(Bullet *bullet);
void PerformKillAsteroid(Asteroid *asteroid);
void PerformKillPowerup(Powerup *powerup);
void PerformFragmentAsteroid(Asteroid *asteroid);
void ResetEnemy(Enemy *enemy);
void ResetBullet(Bullet *bullet);
void ResetAsteroid(Asteroid *asteroid);
void ResetPowerup(Powerup *powerup);
Rectangle getBulletRec(Bullet &bullet);
Rectangle getAsteroidRec(Asteroid &asteroid);
Rectangle getPlayerRec(void);
Rectangle getEnemyRec(Enemy &enemy);
Vector2 getEnemyCenter(Enemy &enemy);
float getEnemyShieldRadius(Enemy &enemy);
Vector2 getPlayerCenter(void);
float getPlayerShieldRadius(void);
void DrawBackground(void);
void DrawPlayer(void);
void DrawPlayerTarget(void);
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



// // PLAYER VARIABLES
// bool autoShoot = false;
// bool GOD_MODE = false;
// Vector2 playerPos = {0};
// Vector2 playerTarget = {0};
// float playerRotation = 0.0f;
// Vector2 playerPreviousPos = {0};
// Vector2 playerSize = {32.0f, 32.0f};
// bool playerDead = false;
// bool playerWon = false;
//
// float playerSpeed = 150.0f; // pixels per frame
//                            // start with 3 hull
// int playerHull = 3;
// int playerHullMax = 3;
// int playerHullStrength = 1; // mitigates 1 damage
//                             // start with no shields
// float playerShield = 0;
// float playerShieldMax = 0;
// int playerShotSpeed = 100;
// float playerShotReload = 10.0f / 60.0f;
// float playerShotTimer = 0;
// bool playerCanShoot = true;
// int playerShotPower = 1;
// int playerShotSpread = 1;
// int playerShotStyle = 0;
//
// float playerShieldRegenTime = 2.0f;
// float playerShieldRegenTimer = 0;
// float playerShieldRegenAmt = 0.02f;
//
// // shield animation VARIABLES
// float playerShieldFrameTime = 0.25f; // sec between shield frame transitions
// float playerShieldFrameTimer = 0.0f;
// int playerShieldFrames = 4;
// int playerShieldFrame = 0;
// bool playerShieldActivating = false;
// bool playerShieldDeactivating = false;
//
// // player exploding variables
// bool playerExploding = false;
// int playerExpFrame = 0;
// float playerExpTimer = 0;
//
// // player alert variables
// float playerAlertTimer = 0;
//
// float playerFlashTime = 0.5f;
// float playerFlashTimer = 0;
// double gameTime = 0;
// double endTime = 0;
//
// // player time freeze variables
// float playerFreezeTime = 1.0f;
// float playerFreezeTimer = playerFreezeTime;
// bool playerTimeFrozen = false;

float AsteroidSpeed = 100.0f;
float EnemyShotSpeed = 200.0f;
float ShotLifetime = 2.0f;
Vector2 EnemySize = {48.0f, 48.0f};
Vector2 BulletSize = {48.0f, 48.0f};
Vector2 BossSize = {96.0f, 96.0f};
float EnemySpeed = 100.0f;
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

void ResetPlayer(void) {
  player.autoShoot = false;
  player.GOD_MODE = false;
  player.pos = {0};
  player.target = {0};
  player.rotation = 0;
  player.prevPos = {0};
  player.size = {32.0f, 32.0f};
  player.dead = false;
  player.won = false;
  player.speed = {150.0f, 0.0f};
  player.hull = 3;
  player.hullMax = 3;
  player.hullStrength = 1; // mitigates damage
  player.shield = 0;
  player.shieldMax = 0;
  player.shotSpeed = 100;
  player.shotReload = 10.0f / 60.0f;
  player.shotTimer = 0;
  player.canShoot = true;
  player.shotPower = 1;
  player.shotSpread = 1;
  player.shotStyle = 0;

  player.shieldRegenTime = 2.0f;
  player.shieldRegenTimer = 0;
  player.shieldRegenAmt = 0.02f;

  // animation variables
  player.shieldFrameTime = 0.25f; // secs between shield frame transitions
  player.shieldFrameTimer = 0.0f;
  player.shieldFrames = 4;
  player.shieldFrame = 0;
  player.shieldActivating = false;
  player.shieldDeactivating = false;

  // player exploding variables
  player.exploding = false;
  player.expFrame = 0;
  player.expTimer = 0;

  // player alert variables
  player.alertTimer = 0;
  player.flashTime = 0.5f;
  player.flashTimer = 0;

  player.gameTime = 0;
  player.endTime = 0;

  // player time freeze variables
  player.freezeTime = 1.0f;
  player.freezeTimer = player.freezeTime;
  player.timeFrozen = false;
  player.Score = 0;
  player.gameTime = GetTime();
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

  ResetPlayer();

  movingBackgroundPosY = 0;
  musicPlayed = 0;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    Enemies[i].active = false;
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
    NewEnemy();
  }

  player.pos = {screenSize.x / 2.0f, screenSize.y - (100.0f + player.size.y)};
  SetMousePosition(player.pos.x, player.pos.y);
}

void DrawShipTexture(int x, int y, Vector2 coords, float rotation, Color hitColor) {
  DrawTexturePro(textures[SHIPS], Rectangle{8.0f * x, 8.0f * y, 8, 8},
                 Rectangle{coords.x+(player.size.x*0.5f), coords.y+(player.size.y*0.5f), player.size.x, player.size.y},
                 {player.size.x*0.5f, player.size.y*0.5f}, rotation, hitColor);
  // DrawRectangleLines(coords.x, coords.y, playerSize.x, playerSize.y, WHITE);
}

void DrawEnemyTexture(int x, int y, Vector2 coords, Color hitColor) {
  DrawTexturePro(textures[SHIPS],
                 Rectangle{8.0f * 4 + (8.0f * x), 8.0f * y, 8.0f, 8.0f},
                 Rectangle{coords.x, coords.y, EnemySize.x, EnemySize.y},
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

int FindEmptyEnemy(Enemy *list) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
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

Vector2 calculateBulletTrajectory(const Vector2& shipPos, const Vector2& shipVelocity, const Vector2& targetPos, float bulletSpeed) {
  // calculate direction from ship to target
  float dx = targetPos.x - shipPos.x;
  float dy = targetPos.y - shipPos.y;
  float distance = std::sqrt(dx * dx + dy * dy);

  // handle case where ship is at target position
  if (distance == 0) {
    return shipVelocity; // just inherit the ship velocity
  }

  // normalize direction vector
  Vector2 normalized = {dx / distance, dy / distance};

  // calculate bullet velocity relative to ship
  Vector2 bulletVelRelative = Vector2Scale(normalized, bulletSpeed);

  // add ship's velocity to get absolute bullet velocity
  Vector2 bulletVelAbsolute = Vector2Add(bulletVelRelative, shipVelocity);

  return bulletVelAbsolute; 
}

int NewPlayerBullet(Vector2 position, int style, float lifetime, Color tint,
                    bool boss) {
  int id = -1;
  for (int i = 0; i < player.shotSpread; i++) {
    id = FindEmptyBullet(Bullets);
    if (id == -1)
      return -1; // nope
    
    Bullets[id].active = true;
    Bullets[id].boss = boss;
    Bullets[id].position = getPlayerCenter();
    Bullets[id].lifetime = lifetime;
    Bullets[id].tint = tint;
    Bullets[id].player_bullet = true;

    Bullets[id].direction = calculateBulletTrajectory(Bullets[id].position, Vector2Scale(player.movement, player.speed.x), player.target, player.shotSpeed);
        // using enhanced shot targeting
        // GetShotSpeed(playerShotSpread, i, playerShotSpeed, -1.0f);
    Bullets[id].damage = player.shotPower;
    Bullets[id].type = style;
    Bullets[id].size = Vector2{48.0f, 48.0f};
  }
  return id;
}

int NewEnemyBullet(Enemy &enemy, float lifetime, Color tint) {
  int id = -1;
  for (int i = 0; i < enemy.bullets_per_shot; i++) {
    id = FindEmptyBullet(Bullets);
    if (id == -1)
      return -1; // nope

    Bullets[id].active = true;
    Bullets[id].boss = enemy.boss;
    Bullets[id].position = Vector2{enemy.position.x + (enemy.size.x * 0.5f),
                                   enemy.position.y + (enemy.size.x * 0.5f)};
    Bullets[id].lifetime = lifetime;
    Bullets[id].tint = tint;
    Bullets[id].player_bullet = false;
    Bullets[id].direction =
        GetShotSpeed(enemy.bullets_per_shot, i, EnemyShotSpeed, 1.0f);
    Bullets[id].damage =
        (enemy.boss ? GetRandomValue(5, 10) : GetRandomValue(1, 3));
    Bullets[id].type = enemy.shot_style;
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
  if (player.exploding || player.dead || player.won)
    return;
  if ((player.shield < player.shieldMax)) {
    if (player.shieldRegenTimer == 0)
      player.shield += player.shieldRegenAmt * GetFrameTime();
    else {
      player.shieldRegenTimer -= GetFrameTime();
      if (player.shieldRegenTimer <= 0) {
        player.shieldRegenTimer = 0;
        player.shieldActivating = true;
        PlaySound(sounds[SHIELD_RAISE]);
      }
    }
  }
  if (player.shieldActivating) {
    player.shieldFrameTimer += GetFrameTime();
    if (player.shieldFrameTimer >= player.shieldFrameTime) {
      player.shieldFrameTimer = 0;
      player.shieldFrame++;
      if (player.shieldFrame >= player.shieldFrames - 1) {
        player.shieldFrame = 3;
        player.shieldActivating = false;
      }
    }
  } else if (player.shieldDeactivating) {
    player.shieldFrameTimer += GetFrameTime();
    if (player.shieldFrameTimer >= player.shieldFrameTime) {
      player.shieldFrameTimer = 0;
      player.shieldFrame--;
      if (player.shieldFrame <= 0) {
        player.shieldFrame = 0;
        player.shieldDeactivating = false;
      }
    }
  }
}

void UpdateEnemyShield(Enemy &enemy) {
  if (enemy.shield < enemy.shield_max) {
    if (enemy.shield_regen_timer <= 0)
      enemy.shield += enemy.shield_regen_amt * GetFrameTime();
    else {
      enemy.shield_regen_timer -= GetFrameTime();
      if (enemy.shield_regen_timer <= 0) {
        enemy.shield_regen_timer = 0;
        enemy.shield_activating = true;
      }
    }
  }
  if (enemy.shield_activating) {
    enemy.shield_frame_timer += GetFrameTime();
    if (enemy.shield_frame_timer >= player.shieldFrameTime) {
      enemy.shield_frame_timer = 0;
      enemy.shield_frame++;
      if (enemy.shield_frame >= player.shieldFrames - 1) {
        enemy.shield_frame = 3;
        enemy.shield_activating = false;
      }
    }
  } else if (enemy.shield_deactivating) {
    enemy.shield_frame_timer += GetFrameTime();
    if (enemy.shield_frame_timer >= player.shieldFrameTime) {
      enemy.shield_frame_timer = 0;
      enemy.shield_frame--;
      if (enemy.shield_frame <= 0) {
        enemy.shield_frame = 0;
        enemy.shield_deactivating = false;
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

// Function to calculate the angle (in radians) for point1 to face toward point2
// Returns angle in radians, measured counterclockwise from the positive X-axis
float getRotationAngle(const Vector2& from, const Vector2& to) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    return std::atan2(dy, dx);
}

// Alternative version that returns angle in degrees
float getRotationAngleDegrees(const Vector2& from, const Vector2& to) {
    float radians = getRotationAngle(from, to);
    float angle = radians * 180.0f / M_PI;
    angle += 90.0f;
    if (angle > 360.0f) angle -= 360.0f;
    return angle;
}

void DrawPlayer() {
  if (player.exploding || player.dead) {
    DrawMiscTexture(
        Rectangle{(9 * 8.0f) + (8.0f * player.expFrame), 6 * 8.0f, 8.0f, 8.0f},
        Rectangle{player.pos.x - (player.size.x * 0.5f),
                  player.pos.y - (player.size.y * 0.5f), player.size.x * 2.0f,
                  player.size.y * 2.0f},
        WHITE);
    return;
  }
  Color hitColor = WHITE;
  if (player.flashTimer > 0)
    player.flashTimer -= GetFrameTime();
  if (player.flashTimer <= 0)
    hitColor = WHITE;
  else
    hitColor = YELLOW;
  int shipdirection;
  if (player.prevPos.x == player.pos.x)
    shipdirection = 1;
  else if (player.prevPos.x - player.pos.x > 1)
    shipdirection = 0;
  else if (player.prevPos.x - player.pos.x < -1)
    shipdirection = 2;
  else
    shipdirection = 1;

  // draw player ship with direction
  DrawShipTexture(shipdirection, 0, player.pos, player.rotation, hitColor);
  // draw shield shields
  if (player.GOD_MODE) {
    DrawShieldTexture(player.shieldFrames - 1, player.pos, player.size, 0, RED);
  } else if ((player.shield > 0) || player.shieldActivating ||
             player.shieldDeactivating) {
    DrawShieldTexture(player.shieldFrame, player.pos, player.size,
                      player.shield / player.shieldMax, WHITE);
    // DrawCircleV(Vector2{playerPos.x+(playerSize.x*0.5f),
    // playerPos.y+(playerSize.y*0.5f)}, (playerSize.x*0.5f)+playerShield,
    // YELLOW);
  }
  DrawPlayerFreezeTimer();
}

void DrawPlayerTarget() {

  DrawLine(player.target.x-5, player.target.y, player.target.x+5, player.target.y, RED);
  DrawLine(player.target.x, player.target.y-5, player.target.x, player.target.y+5, RED);
  DrawLineV(player.target, getPlayerCenter(), GRAY);
}

void DrawPlayerFreezeTimer(void) {
  DrawRing(Vector2{player.pos.x - (player.size.x * 0.1f),
                   player.pos.y + (player.size.y * 0.5f)},
           player.size.x - 3.0f, player.size.x, 135.0f,
           135.0f + (player.freezeTimer * 90.0f), 5, RAYWHITE);
}

void DrawEnemy(int id) {
  if (Enemies[id].exploding) {
    DrawMiscTexture(Rectangle{9 * 8.0f + (8.0f * Enemies[id].exp_frame),
                              6 * 8.0f, 8.0f, 8.0f},
                    Rectangle{Enemies[id].position.x, Enemies[id].position.y,
                              Enemies[id].boss ? BossSize.x : EnemySize.x,
                              Enemies[id].boss ? BossSize.y : EnemySize.y},
                    WHITE);
    return;
  }
  int bx = 0;
  int by = 0;
  int maxX = 6;

  if (Enemies[id].boss)
    maxX = 3;
  for (int i = 0; i < Enemies[id].type; i++) {
    bx++;
    if (bx >= maxX) {
      bx = 0;
      by++;
    }
  }
  Color hitColor = WHITE;

  if (Enemies[id].flash_timer > 0)
    Enemies[id].flash_timer -= GetFrameTime();

  if (Enemies[id].flash_timer <= 0) {
    hitColor = (player.timeFrozen ? SKYBLUE : WHITE);
  } else
    hitColor = YELLOW;

  if (Enemies[id].boss) {
    DrawBossTexture(bx, by, Enemies[id].position, hitColor);
  } else {
    DrawEnemyTexture(bx, by, Enemies[id].position, hitColor);
  }

  if (Enemies[id].shield > 0 || Enemies[id].shield_activating ||
      Enemies[id].shield_deactivating) {
    DrawShieldTexture(Enemies[id].shield_frame, Enemies[id].position,
                      Enemies[id].size,
                      Enemies[id].shield / Enemies[id].shield_max, WHITE);
  }

  // Draw bounding box for debug
  // Rectangle rec = getEnemyRec(Enemies[id]);
  // DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, WHITE);
  //
  // Draw hull/shield values for debug
  /* DrawText(TextFormat("%2.2f/%d", Enemies[id].shield, */
  /* Enemies[id].shield_max), */
  /*           Enemies[id].position.x, Enemies[id].position.y - 20, 10, WHITE);
   */
  /*  DrawText(TextFormat("%d/%d", Enemies[id].hull, Enemies[id].hull_max), */
  /*           Enemies[id].position.x, Enemies[id].position.y - 10, 10, WHITE);
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
                      player.timeFrozen ? SKYBLUE : WHITE);
  // Drawing bounding box for debug
  // Rectangle rec = getAsteroidRec(Asteroids[id]);
  // DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, WHITE);
}

void DrawFragment(int id) {
  DrawAsteroidTexture(Fragments[id].type, Fragments[id].position,
                      Fragments[id].size, Fragments[id].rotation,
                      player.timeFrozen ? SKYBLUE : WHITE);
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
    if (player.dead || player.won)
      InitGame();
  }

  if (IsKeyPressed(keymap[keyPause])) {
    if (player_game_state == GAME_STATE_PAUSED) {
      player_game_state = GAME_STATE_PLAYING;
    } else if (player_game_state == GAME_STATE_PLAYING) {
      player_game_state = GAME_STATE_PAUSED;
    }
  }

  if (player_game_state != GAME_STATE_PLAYING || player.exploding ||
      player.dead || player.won)
    return; // abort input handling when not in a valid playing state

  // handle freeze time
  if (IsKeyDown(keymap[keyAbility])) {
    player.timeFrozen = (player.freezeTimer > 0.0f);
  } else if (player.timeFrozen) {
    player.timeFrozen = false;
  }

  if (IsKeyPressed(keymap[keyToggleAutoshoot]))
    player.autoShoot = !player.autoShoot;

  if (IsKeyPressed(keymap[keyGodMode])) {
    player.GOD_MODE = !player.GOD_MODE;
  }

  if (IsKeyPressed(KEY_EQUAL)) {
    player.shotStyle++;
  }
  if (IsKeyPressed(KEY_MINUS)) {
    player.shotStyle--;
  }

  // player movement and shooting
  float speed = player.speed.x * GetFrameTime();
  if (IsKeyDown(keymap[keyShoot]) || IsMouseButtonDown(mouseKeymap[mouseShoot]) ||
      player.autoShoot) {
    if (player.canShoot) {
      player.canShoot = false;

      NewPlayerBullet(player.pos, player.shotStyle, ShotLifetime, GREEN, false);
    }
  } else
    speed *= 2;

  // perform player rotation
  // this needs to happen before player movement
  // so we know which direction to move the player if using directional_movement
  Vector2 mouseCoords = GetMousePosition();
  //playerPos = mouseCoords;
  player.target = mouseCoords;
  player.rotation = getRotationAngleDegrees(getPlayerCenter(), player.target);
 
  player.prevPos = player.pos;

  player.movement = Vector2Zero();
  bool FOUR_AXIS_MOVEMENT = false;

  if (FOUR_AXIS_MOVEMENT) {
    // movement is locked to four axis

    if (IsKeyDown(KEY_UP) || IsKeyDown(keymap[keyMoveUp]))
      player.movement.y = -1;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(keymap[keyMoveDown]))
      player.movement.y = 1;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(keymap[keyMoveLeft]))
      player.movement.x = -1;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(keymap[keyMoveRight]))
      player.movement.x = 1;

    player.pos = Vector2Add(player.pos, Vector2Scale(Vector2Normalize(player.movement), speed));
  } else {
    // directional movement relative to player target
    Vector2 playerCenter = getPlayerCenter();
    float dx = player.target.x - playerCenter.x;
    float dy = player.target.y - playerCenter.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    float normDx = dx / dist;
    float normDy = dy / dist;

    if (IsKeyDown(KEY_UP) || IsKeyDown(keymap[keyMoveUp]))
      player.movement = Vector2Add(player.movement, {normDx, normDy});
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(keymap[keyMoveDown]))
      player.movement = Vector2Add(player.movement, {-normDx, -normDy});
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(keymap[keyMoveLeft]))
      player.movement = Vector2Add(player.movement, {normDy, -normDx});
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(keymap[keyMoveRight]))
      player.movement = Vector2Add(player.movement, {-normDy, normDx});

    player.pos = Vector2Add(player.pos, Vector2Scale(player.movement, speed));
    
    
  }

  // clamps player position within screen space
  player.pos = Vector2Clamp(player.pos, 
                           Vector2{1.0f, 1.0f}, Vector2{screenSize.x - 1.0f - player.size.x, screenSize.y - 1.0f - player.size.y});
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
  if (player_game_state == GAME_STATE_PLAYING && !player.exploding &&
      !player.dead && !player.won) {
    // Update background movement state
    UpdateBackground();
    // bullets
    for (i = 0; i < MAX_BULLETS; i++) {
      if (!Bullets[i].active)
        continue;
      UpdateBullet(i);
    }
    // Enemies
    for (i = 0; i < MAX_ENEMIES; i++) {
      if (!Enemies[i].active)
        continue;
      UpdateEnemy(i);
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
  DrawPlayerTarget();

  // draw end line for Enemies to cross
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

  for (i = 0; i < MAX_ENEMIES; i++) {
    if (!Enemies[i].active)
      continue;
    DrawEnemy(i);
  }

  for (i = 0; i < MAX_POWERUPS; i++) {
    if (!Powerups[i].active)
      continue;
    DrawPowerup(i);
  }

  DrawText("Mouse to Move, Click to Fire", 10, 10, 20, LIGHTGRAY);
  DrawText(TextFormat("Score %d", Score), 30, 30, 20, RED);
  DrawFPS(screenSize.x - 80, 20);

  if (player.dead)
    DrawDeathScreen();
  else if (player.won)
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
                      Score, player.endTime - player.gameTime),
           15, 405, 30, RAYWHITE);
}

void DrawWinScreen(void) {
  DrawRectangle(10, 400, screenSize.x - 20, 200, BLACK);
  DrawRectangleLines(9, 399, screenSize.x - 19, 202, GREEN);

  DrawText(TextFormat("You have WON!!\n\nFinal score: %d\n\nWon in %1.0f "
                      "seconds!\n\nClick or tap to fly again",
                      Score, player.endTime - player.gameTime),
           15, 405, 30, RAYWHITE);
}

// spawn a new asteroid of a given type with all random properties
int NewAsteroid(int type) {
  int id = FindEmptyAsteroid(Asteroids);
  if (id == -1)
    return -1; // didn't find a valid slot
  int scoreMult = (fmaxf(Score, 0) / 100); // make sure we don't have a negative scoreMult, that would make enemies impossible
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

int NewEnemy() {
  int id = FindEmptyEnemy(Enemies);
  if (id == -1)
    return -1; // nope
  int scoreMult = (fmaxf(Score, 0) / 100);
  bool boss = (GetRandomValue(0, 100) < scoreMult);

  Enemies[id].boss = boss; // 5% chance to spawn a boss
  Enemies[id].size = (boss ? BossSize : EnemySize);
  float xPos = GetRandomValue(0, screenSize.x - Enemies[id].size.x);
  float yPos = 0;
  Enemies[id].type =
      GetRandomValue(0, (boss ? NUM_BOSS_TYPES - 1 : NUM_ENEMY_TYPES - 1));
  Enemies[id].shot_style = GetRandomValue(
      0, (boss ? NUM_BOSS_SHOT_STYLES - 1 : NUM_SHOT_STYLES - 1));
  Enemies[id].bullets_per_shot =
      (boss ? GetRandomValue(2, 4) : 1); // boss gets 1-3 shots
  Enemies[id].active = true;
  Enemies[id].position = Vector2{xPos, yPos};
  Enemies[id].direction = Vector2{
      0,
      (float)(boss ? GetRandomValue(EnemySpeed * 0.15, EnemySpeed * 0.35)
                   : GetRandomValue(EnemySpeed * 0.5f, EnemySpeed * 1.5f))};
  Enemies[id].lifetime = 60.0f;
  Enemies[id].tint = WHITE;
  Enemies[id].player_bullet = false;
  Enemies[id].shot_timer = 0;
  Enemies[id].hull = Enemies[id].hull_max =
      (boss ? GetRandomValue(20, (20 + (scoreMult * 5)))
            : GetRandomValue(1, (scoreMult ? (scoreMult * 2) : 1)));
  Enemies[id].shield = Enemies[id].shield_max =
      (boss ? GetRandomValue(40, (40 + (scoreMult * 10)))
            : GetRandomValue(0, (scoreMult ? (scoreMult * 4) : 0)));
  // leave Enemy hull strength at 0 until I implement increased player damage
  Enemies[id].hull_strength = (boss ? 0 : 0);
  Enemies[id].flash_timer = 0;
  Enemies[id].exploding = false;
  Enemies[id].exp_frame = 0;
  Enemies[id].exp_timer = 0;
  if (Enemies[id].shield > 0) {
    Enemies[id].shield_activating = true;
    Enemies[id].shield_frame_timer = player.shieldFrameTime;
    Enemies[id].shield_frame = 0;
    Enemies[id].shield_regen_amt = 0.5f;
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
    if (!player.GOD_MODE) {
      if (player.shield > 0) {
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
    for (int i = 0; i < MAX_ENEMIES; i++) {
      if (!Enemies[i].active)
        continue;
      if (Enemies[i].shield > 0) {
        // check if bullet hits Enemy shield
        if (CheckCollisionCircleRec(getEnemyCenter(Enemies[i]),
                                    getEnemyShieldRadius(Enemies[i]),
                                    getBulletRec(Bullets[id]))) {
          PerformHitEnemy(Bullets[id], Enemies[i]);
          break;
        }
      } else {
        // check if bullet hits Enemy ship
        if (CheckCollisionRecs(getBulletRec(Bullets[id]),
                               getEnemyRec(Enemies[i]))) {
          PerformHitEnemy(Bullets[id], Enemies[i]);
          break;
        }
      }
    }
  }
}

void UpdatePlayer(void) {
  if (Score >= 10000 && !player.GOD_MODE && !player.won) {
    PlaySound(sounds[WIN_TRUMPETS]);
    player.endTime = GetTime();
    player.won = true;
    StopAllMusic();
    nowPlaying = &music[WIN];
    PlayMusicStream(*nowPlaying);
    return;
  }
  if (player.hull <= 0 && player.exploding) {
    player.expTimer += GetFrameTime();
    if (player.expTimer >= 0.2f) {
      player.expTimer = 0;
      player.expFrame++;
      if (player.expFrame > 3) {
        player.exploding = false;
        player.dead = true;
      }
    }
    return;
  } else if (!player.dead && player.hull <= 1) {
    player.alertTimer += GetFrameTime();
    if (player.alertTimer >= 2.0f) {
      player.alertTimer = 0;
      PlaySound(sounds[ALERT_LOW_HULL]);
    }
  }

  UpdatePlayerShield();

  // update shot timer
  if (!player.canShoot) {
    player.shotTimer -= GetFrameTime();
    if (player.shotTimer <= 0) {
      player.canShoot = true;
      player.shotTimer = player.shotReload;
    }
  }

  // update player freeze timer
  if (player.timeFrozen) {
    player.freezeTimer -= GetFrameTime();
    if (player.freezeTimer <= 0) {
      player.timeFrozen = false;
    }
  } else if (player.freezeTimer < player.freezeTime && IsKeyUp(KEY_LEFT_SHIFT)) {
    player.freezeTimer += GetFrameTime();
    player.freezeTimer = Clamp(player.freezeTimer, 0.0f, player.freezeTime);
  }
}
void ResetEnemy(Enemy *enemy) {
  enemy->active = false;
  NewEnemy();
  // memset(enemy, '\0', sizeof(&enemy)); // fill every property with NULL
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
  player.hull = 0;
  player.shield = 0;
  player.exploding = true;
  player.expFrame = 0;
  player.expTimer = 0;
  player.alertTimer = 0;
  player.shieldActivating = false;
  player.shieldDeactivating = false;
  PlaySound(sounds[EXPLOSION]);
  player.endTime = GetTime();
  StopAllMusic();
  nowPlaying = &music[DEATH];
  PlayMusicStream(*nowPlaying);
}
void PerformKillEnemy(Enemy *enemy) {
  enemy->hull = 0;
  enemy->shield = 0;
  enemy->exploding = true;
  enemy->exp_frame = 0;
  enemy->exp_timer = 0;
  enemy->shield_activating = false;
  enemy->shield_deactivating = false;
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
  Score -= Clamp((bullet.damage - player.hullStrength), 0, bullet.damage);
  if (player.shield > 0) {
    player.shield -= fminf(player.shield, bullet.damage); // prevent shields going negative
    if (player.shield == 0) {
      player.shieldDeactivating = true;
      PlaySound(sounds[SHIELD_DROP]);
      // hit took shields offline, begin regen timer
      player.flashTimer = player.flashTime; // flash player ship for a short duration
      player.shieldRegenTimer = player.shieldRegenTime; // shield took a hit, initiate shield regen
                                 // delay
    }
  } else if (player.hull > (bullet.damage - player.hullStrength - 1)) {
    player.hull -= Clamp((bullet.damage - player.hullStrength), 0, player.hull); // prevent damage going negative (healing player)
    player.flashTimer = player.flashTime;
  } else {
    PerformKillPlayer();
  }
}

void PerformHitEnemy(Bullet &bullet, Enemy &enemy) {
  PerformKillBullet(&bullet);
  if (enemy.shield > 0) {
    enemy.shield -= fminf(enemy.shield, bullet.damage);
    if (enemy.shield == 0) {
      enemy.shield_deactivating = true;

      enemy.flash_timer = player.flashTime;
      enemy.shield_regen_timer = player.shieldRegenTime;
    }
  } else if (enemy.hull > 0) {
    enemy.hull -= fminf(enemy.hull, bullet.damage);
    enemy.flash_timer = player.flashTime;
    if (enemy.hull <= 0) {
      PerformKillEnemy(&enemy);
      if (GetRandomValue(1, 100) < (enemy.boss ? 20 : 5)) { // 20% : 5% chance for a powerup
        NewPowerup(enemy.position, Vector2{0, (float)PowerupSpeed}, 5.0f);
      }
      if (GetRandomValue(1, 100) < 10) {
        NewAsteroid(0);
      }
      int value = (enemy.hull_max);
      if (enemy.boss)
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
    if (GetRandomValue(1, 100) < 33) { // 33% chance to drop a resource collectible
      NewPowerup(hit.position, Vector2{0, (float)PowerupSpeed}, 5.0f);
      // skip the resource drop for now, just give some bonus score
      // NewResource(hit.position, Vector2{ 0, GetRandomValue(0, 50) },
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
      player.shotReload -= 1.0f / 60.0f; // increase shots per second
      break;
    case SHOT_SPEED:
      PlaySound(sounds[POWERUP_COLLECT]);
      player.shotSpeed += 100.0f;
      break;
    case SHOT_POWER:
      if (player.shotPower < 5) {
        player.shotPower++;
      }
      if (player.shotStyle < NUM_SHOT_STYLES - 1) {
        PlaySound(sounds[POWERUP_DAMAGE_INCREASE]);
        player.shotStyle++;
      }
      break;
    case SHOT_SPREAD:
      if (player.shotSpread < 5) {
        PlaySound(sounds[POWERUP_SPREAD_INCREASE]);
        player.shotSpread++;
      } else {
        PlaySound(sounds[NEGATIVE]);
      }
      break;
    case SHIELD_REPAIR:
      if (player.shieldMax == 0) {
        // Only increase shieldmax if we have no shields
        player.shieldMax++;
      }
      if (player.shield == 0) {
        PlaySound(sounds[SHIELD_RAISE]);
        player.shieldActivating = true;
      } else {
        PlaySound(sounds[NEGATIVE]);
      }
      player.shield = player.shieldMax;
      break;
    case SHIELD_MAX:
      player.shieldMax++;
      if (player.shield == 0) {
        PlaySound(sounds[SHIELD_RAISE]);
        player.shieldActivating = true;
      } else {
        PlaySound(sounds[NEGATIVE]);
      }
      player.shield = player.shieldMax;
      break;
    case HULL_REPAIR:
      PlaySound(sounds[POWERUP_HULL_RESTORE]);
      player.hull = player.hullMax;
      break;
    case HULL_MAX:
      PlaySound(sounds[POWERUP_HULL_INCREASE]);
      player.hullMax++;
      player.hull = player.hullMax;
      break;
    case HULL_STRENGTH:
      if (player.hullStrength < 5) {
        PlaySound(sounds[POWERUP_HULL_STRENGTH]);
        player.hullStrength++;
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
  return Rectangle{player.pos.x, player.pos.y, player.size.x, player.size.y};
}
Rectangle getEnemyRec(Enemy &enemy) {
  const Rectangle *mat = &enemy_size_matrix[enemy.type];
  Vector2 scale = {enemy.size.x * 0.125f, enemy.size.y * 0.125f};
  return Rectangle{enemy.position.x + (mat->x * scale.x),
                   enemy.position.y + (mat->y * scale.y), mat->width * scale.x,
                   mat->height * scale.y};
}

Vector2 getEnemyCenter(Enemy &enemy) {
  return Vector2{enemy.position.x + (enemy.size.x * 0.5f),
                 enemy.position.y + (enemy.size.y * 0.5f)};
}

float getEnemyShieldRadius(Enemy &enemy) {
  if (enemy.shield <= 0)
    return 0.0f;
  return enemy.size.x * 0.5f + (20.0f * (enemy.shield / enemy.shield_max));
}

Vector2 getPlayerCenter(void) {
  return Vector2{player.pos.x + (player.size.x * 0.5f),
                 player.pos.y + (player.size.y * 0.5f)};
}

float getPlayerShieldRadius(void) {
  if (player.shield <= 0)
    return 0.0f;
  return player.size.x * 0.5f + (20.0f * (player.shield / player.shieldMax));
}

void UpdateBackground(void) {
  movingBackgroundPosY += 50 * GetFrameTime();
  if (movingBackgroundPosY >= screenSize.y)
    movingBackgroundPosY = 0;
}

void UpdateEnemy(int id) {
  if (Enemies[id].hull <= 0 && Enemies[id].exploding) {
    Enemies[id].exp_timer += GetFrameTime();
    if (Enemies[id].exp_timer >= 0.2f) {
      Enemies[id].exp_timer = 0;
      Enemies[id].exp_frame++;
      if (Enemies[id].exp_frame > 3) {
        Enemies[id].exploding = false;

        ResetEnemy(&Enemies[id]);
      }
    }
    return;
  }

  if (!player.timeFrozen) {
    Enemies[id].position = Vector2Add(Enemies[id].position, Vector2Scale(Enemies[id].direction, GetFrameTime()));
  }

  if (Enemies[id].position.y + EnemySize.y >= (screenSize.y - 100.0f)) {
    // they got through
    ResetEnemy(&Enemies[id]);
    Score -= 10;
    return;
  }

  UpdateEnemyShield(Enemies[id]);

  if (Enemies[id].shield > 0) {
    // perform shield collisions
    if (player.shield > 0) {
      if (CheckCollisionCircles(getEnemyCenter(Enemies[id]),
                                getEnemyShieldRadius(Enemies[id]),
                                getPlayerCenter(), getPlayerShieldRadius())) {
        // player with shield runs into enemy with shield
        Enemies[id].shield = 0;
        Enemies[id].shield_deactivating = true;
        if (!player.GOD_MODE) {
          player.shield = 0;
          player.shieldDeactivating = true;
          player.shieldRegenTimer = player.shieldRegenTime;
          PlaySound(sounds[SHIELD_DROP]);
        }
        return;
      }
    } else {
      if (CheckCollisionCircleRec(getEnemyCenter(Enemies[id]),
                                  getEnemyShieldRadius(Enemies[id]),
                                  getPlayerRec())) {
        // player without shield runs into enemy with shield
        Vector2 collision_angle = {player.pos.x - Enemies[id].position.x,
                                   player.pos.y - Enemies[id].position.y};
        // bounce player off the enemy shield
        player.pos = Vector2Add(player.pos, Vector2Scale(collision_angle, GetFrameTime()));

        player.pos = Vector2Clamp(player.pos, Vector2{1.0f, 1.0f},
                                 Vector2{screenSize.x - 1.0f - player.size.x,
                                         screenSize.y - 1.0f - player.size.y});

        Enemies[id].shield = 0;
        Enemies[id].shield_deactivating = true;
        Enemies[id].shield_regen_timer = player.shieldRegenTime;
        PlaySound(sounds[SHIELD_BOUNCE]);
        return;
      }
    }
  } else {
    // perform ship collisions
    if (player.shield > 0) {
      if (CheckCollisionCircleRec(getPlayerCenter(), getPlayerShieldRadius(),
                                  getEnemyRec(Enemies[id]))) {
        // perform player shield to enemy ship collision
        Vector2 collision_angle = {player.pos.x - Enemies[id].position.x,
                                   player.pos.y - Enemies[id].position.y};
        // bounce enemy off player shield
        Enemies[id].position =
            Vector2Add(Enemies[id].position,
                       Vector2Scale(collision_angle, GetFrameTime()));
        Enemies[id].position =
            Vector2Clamp(Enemies[id].position, Vector2{1.0f, 1.0f},
                         Vector2{screenSize.x - 1.0f - Enemies[id].size.x,
                                 screenSize.y - 1.0f - Enemies[id].size.y});

        if (!player.GOD_MODE) {
          player.shield = 0;
          player.shieldDeactivating = true;
          player.shieldRegenTimer = player.shieldRegenTime;
          PlaySound(sounds[SHIELD_DROP]);
        }
        return;
      }
    } else {
      if (CheckCollisionRecs(getEnemyRec(Enemies[id]), getPlayerRec())) {
        // perform player ship to enemy ship collision
        // player crashed into enemy, nobody wins with the headbutt!!
        PerformKillEnemy(&Enemies[id]);
        if (!player.GOD_MODE) {
          PerformKillPlayer();
        }
        return;
      }
    }
  }

  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (!Asteroids[i].active)
      continue;
    if (CheckCollisionRecs(getEnemyRec(Enemies[id]),
                           getAsteroidRec(Asteroids[i]))) {
      // enemy crashed into asteroid, enemy loses
      PerformKillEnemy(&Enemies[id]);
      return;
    }
  }

  for (int i = 0; i < MAX_FRAGMENTS; i++) {
    if (!Fragments[i].active)
      continue;
    if (CheckCollisionRecs(getEnemyRec(Enemies[id]),
                           getAsteroidRec(Fragments[i]))) {
      // enemy crashed into fragment, enemy loses
      PerformKillEnemy(&Enemies[id]);
      return;
    }
  }

  Enemies[id].shot_timer -= GetFrameTime();
  if (Enemies[id].shot_timer <= 0 && GetRandomValue(1, 100) < 10) {
    // make enemy bullet
    Enemies[id].shot_timer = GetRandomValue(1, 4);
    NewEnemyBullet(Enemies[id], ShotLifetime * 2, RED);
  }
}

void UpdateAsteroid(int id) {

  Asteroids[id].lifetime -= GetFrameTime();
  if (Asteroids[id].lifetime <= 0) {
    ResetAsteroid(&Asteroids[id]);
    return;
  }

  if (!player.timeFrozen) {
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

  if (!player.GOD_MODE && CheckCollisionRecs(asteroidRect, getPlayerRec())) {
    // player ran into asteroid, destroy asteroid
    PerformKillAsteroid(&Asteroids[id]);

    if (player.shield > 0) {
      // player had shields, deactivate shield
      player.shield = 0;
      player.shieldDeactivating = true;
      player.shieldRegenTimer = player.shieldRegenTime;
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
  if (!player.timeFrozen) {
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

  if (!player.GOD_MODE && CheckCollisionRecs(fragmentRect, getPlayerRec())) {
    // player ran into asteroid, destroy asteroid
    PerformKillAsteroid(&Fragments[id]);

    if (player.shield > 0) {
      // player had a shield up, deactivate shields
      player.shield = 0;
      player.shieldDeactivating = true;
      player.shieldRegenTimer = player.shieldRegenTime;
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
  DrawText(TextFormat("Damage: ", player.shotPower), 5, screenSize.y - 98, 20,
           WHITE);
  for (int i = 0; i < player.shotPower; i++) {
    DrawUpgradeTexture(
        3, 0,
        Vector2{MeasureText("Damage: ", 20) + (i * 25.0f), screenSize.y - 98},
        Vector2{20, 20}, WHITE);
  }
  DrawText(TextFormat("Armor: %d", player.hullStrength), screenSize.x * 0.5f,
           screenSize.y - 98, 20, WHITE);
  for (int i = 0; i < player.hullStrength; i++) {
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
  if (player.hull < 0 || player.hullMax <= 0)
    return;

  float percent = (float)player.hull / player.hullMax;

  DrawRing(Vector2{player.pos.x + player.size.x + (player.size.x * 0.1f),
                   player.pos.y + (player.size.y * 0.5f)},
           player.size.x - 3.0f, player.size.x, 45.0f, 45.0f - (percent * 90.0f),
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

  float percent = (player.shieldMax > 0 ? player.shield / player.shieldMax : 0);

  DrawRing(Vector2{player.pos.x + player.size.x + 5 + (player.size.x * 0.1f),
                   player.pos.y + (player.size.y * 0.5f)},
           player.size.x - 3.0f, player.size.x, 45.0f, 45.0f - (percent * 90.0f),
           5, YELLOW);

  DrawRectangle(bar.x, bar.y, bar.width, bar.height, GRAY);
  DrawText("Shield:", bar.x - MeasureText("Shield:", 20), bar.y + 2, 20,
           RAYWHITE);
  if (player.shield > 0 || player.shieldMax == 0) {
    DrawRectangle(bar.x, bar.y, bar.width * percent, bar.height, YELLOW);
    DrawText(TextFormat("%2.0f%%", percent * 100), bar.x + bar.width * 0.5f,
             bar.y + 2, 20, BLACK);
  } else {
    float percent = player.shieldRegenTimer / player.shieldRegenTime;
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
  SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor())); // Set our game
  SetWindowState(FLAG_WINDOW_UNDECORATED);

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
