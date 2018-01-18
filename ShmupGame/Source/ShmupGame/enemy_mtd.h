#define ENEMY_POSITION_RATIO 1024

#define BULLET_COLOR_NUM 4
#define BULLET_TYPE_NUM 3

extern int processSpeedDownBulletsNum;
extern int nowait;

void initFoes();
void closeFoes();
void moveFoes();
void clearFoes();
void clearFoesZako();
void wipeBullets(FVector *pos, int width);
void drawBulletsWake();
void drawFoes();
void drawBullets();