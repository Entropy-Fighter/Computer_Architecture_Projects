#include <string.h>

#include "lcd/lcd.h"
#include "utils.h"

//#######################################################################
//我自己加入的头文件
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "img.h"

//#######################################################################
void paintStartMenu_S();
void paintDinoBlack1_S(int x, int y, unsigned char*);
void paintDinoBlack2_S(int x, int y, unsigned char*);
void paintScore_S(char*,int x);
void paintEndMenuText_S();
void paintEndMenuScore_S(char*,int x);
void paintEndMenu_S(char*,int x);

//#######################################################################

void Inp_init(void) {
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    RCU_CFG0 |= (0b10 << 14) | (1 << 28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0) |= ADC_CTL1_ADCON;
}

void IO_init(void) {
    Inp_init();  // inport init
    Adc_init();  // A/D init
    Lcd_Init();  // LCD init
}

//########################################################################

/**
 * 这个函数是助教后来提供的，用于打印自定义大小的图片;
 *
 *  the image is shown at [x1,x2]×[y1,y2];
 *
 * 据说这个函数无法满足高刷新率的要求
 *
 * @see https://piazza.com/class/ky06dwortmv48g?cid=313
 *
 */
void LCD_ShowPicCustomed(u16 x1, u16 y1, u16 x2, u16 y2, u8* img) {
    LCD_Address_Set(x1, y1, x2, y2);
    int w = x2 - x1 + 1, h = y2 - y1 + 1;
    for (int i = 0; i < w*h*2; i++) {
        LCD_WR_DATA8(img[i]);
    }
}
//########################################################################

//########################################################################
// the codes below is to decorate the ground(using points and points2)
int pointx = 109;
int pointx2 = 149;

void drawPoints(){
    int i;
    for(i = 0; i < 40; i++){
        int j;
        for(j = 0; j < 10; j++){
            if(points[i][j]){
                LCD_DrawPoint(i + pointx,j+70,WHITE);
            }
            
        }
    }
};

void drawPoints2(){
    int i;
    for(i = 0; i < 40; i++){
        int j;
        for(j = 0; j < 10; j++){
            if(points2[i][j]){
                LCD_DrawPoint(i + pointx2,j+70,WHITE);
            }
            
        }
    }
};

void movePoints(){
    int i;
    for(i = 0; i < 40; i++){
        int j;
        for(j = 0; j < 10; j++){
            if(points[i][j]){
                LCD_DrawPoint(i + pointx,j+70,BLACK);
            }
            
        }
    }
    pointx -= 6;
    if(pointx <= 80){
        pointx = 149;
    }
}

void movePoints2(){
    int i;
    for(i = 0; i < 40; i++){
        int j;
        for(j = 0; j < 10; j++){
            if(points2[i][j]){
                LCD_DrawPoint(i + pointx2,j+70,BLACK);
            }
            
        }
    }
    pointx2 -= 6;
    if(pointx2 <= 80){
        pointx2 = 149;
    }
}
// below are my codes to implement chrome dino in c
//########################################################################

//########################################################################

// GameView中的静态属性

int FRAME_LENGTH = 160;
int FRAME_WIDTH = 80;

//########################################################################

// GamePanel中的静态属性

int groundx = 0;
int groundY = 65;    //地面在坐标系的y坐标
int groundWidth = 10;  //地面图片的宽度

int OBSTACLE_NUM = 2;  //一共有几种障碍物

int START_MENU = 0;
int GAMING = 1;
int END_MENU = 2;

//########################################################################
// Dino中的静态属性

int cnt = 0;// use this later in main() to keep dino down 

int DINO_STAND_LENGTH = 20;  // dino站立时图片的尺寸
int DINO_STAND_WIDTH = 20;

int DINO_BELOW_LENGTH = 27;  // dino蹲下时图片的尺寸
int DINO_BELOW_WIDTH = 20;

int STAND_FULL = 0;      //双脚在地
int LEFT_UP = 1;         //左脚在空中
int RIGHT_UP = 2;        //右脚在空中
int BELOW_LEFT_UP = 3;   //左脚在空中，且蹲下
int BELOW_RIGHT_UP = 4;  //右脚在空中，且蹲下

int UP = 5;    //上升
int DOWN = 6;  //下降

/**
 * 能起跳的最大高度
 */
int JUMP_HEIGHT = 35;

//站立时左上角y的坐标
int STAND_Y = 0;
//下蹲时左上角y的坐标
int BELOW_Y = 0;

int DINO_IMAGE_NUM = 5;
unsigned char* DinoImages[5] = {dinoStandImage,dinoUpLeftImage,dinoUpRightImage,dinoDownLeftImage,dinoDownRightImage};

//########################################################################
// Dino 类
void paintDinoBlack1();
void paintDinoBlack2();

typedef struct Dino {
    int x;
    int y;
    int state;
    int jumpSpeed;
    int jumping;
} Dino;

Dino DinoConstructor(Dino dino) {
    dino.x = 20;
    dino.y = STAND_Y;
    dino.state = LEFT_UP;
    dino.jumpSpeed = 5;
    dino.jumping = 0;
    return dino;
}

Dino jump(Dino dino) {
    //检查是否处于空中
    if (!(dino.state == UP || dino.state == DOWN) && !dino.jumping){
        return dino;
    }
    //suddenly down if dino is in the jumping and the player pushes the 'down' button
    if (!(dino.state == UP || dino.state == DOWN) && dino.jumping){
        paintDinoBlack1_S(dino.x, dino.y, black1Image);
        dino.jumping = 0;
        dino.y = STAND_Y + 1;
        cnt = 0;
        return dino;
    }
        
    dino.jumping = 1;

    if (dino.state == DOWN) {
        if (dino.y >= groundY - (DINO_STAND_WIDTH)) {
            dino.state = LEFT_UP;
            dino.jumping = 0;
        } else {
            paintDinoBlack2_S(dino.x, dino.y, black2Image);
            dino.y += dino.jumpSpeed;
            
        }
    } else {  // state == UP
        if (dino.y <= groundY - (DINO_STAND_WIDTH) - JUMP_HEIGHT) {
            dino.state = DOWN;
        } else {
            paintDinoBlack2_S(dino.x, dino.y, black2Image);
            dino.y -= dino.jumpSpeed;
            
        }
    }
    return dino;
}

Dino stand(Dino dino){

    paintDinoBlack2_S(dino.x, dino.y, black2Image);
    dino.state = LEFT_UP;
    return dino;
}

Dino walk(Dino dino) {
    if (dino.state == UP || dino.state == DOWN) {
        return dino;
    }
    //换一只脚继续走路
    if (dino.state == LEFT_UP) {
        dino.state = RIGHT_UP;
    } else if (dino.state == RIGHT_UP) {
        dino.state = LEFT_UP;
    } else if (dino.state == BELOW_LEFT_UP) {
        dino.state = BELOW_RIGHT_UP;
    } else if (dino.state == BELOW_RIGHT_UP) {
        dino.state = BELOW_LEFT_UP;
    }
    return dino;
}

Dino setState(Dino dino, int state) {
    dino.state = state;
    return dino;
}

int getLengthDino(Dino dino) {
    if (dino.state == BELOW_LEFT_UP || dino.state == BELOW_RIGHT_UP) {
        return DINO_BELOW_LENGTH;
    }
    return DINO_STAND_LENGTH;
}

int getWidthDino(Dino dino) {
    if (dino.state == BELOW_LEFT_UP || dino.state == BELOW_RIGHT_UP) {
        return DINO_BELOW_WIDTH;
    }
    return DINO_STAND_WIDTH;
}

//########################################################################
// Obstacle中的静态属性

unsigned char* obstacleImages[4] = {cactusImage,cactus2,pterUpImage,pterDownImage};

//控制地面和障碍物的移动速度
int MOVE_SPEED = 6;

// Obstacle类（本来只是个接口，不管这些OOP的细节了）

typedef struct Obstacle {
    int x;
    int y;
    int picWidth;  //图片的长宽
    int picLength;
    int type;      //是仙人掌还是翼龙，只能为0或2
    int picState;  //使用哪张照片，只能为0或1
} Obstacle;

void paintObstacleBlack1_S(int x, int y, Obstacle obstacle, unsigned char* image);
void paintObstacleBlack2_S(int x, int y, Obstacle obstacle, unsigned char* image);
void paintObstacle_S(int x, int y, Obstacle obstacle, unsigned char* image);

int getX(Obstacle obstacle) {
    return obstacle.x;
}

int getY(Obstacle obstacle) {
    return obstacle.y;
}

int getLengthObstacle(Obstacle obstacle) {
    return obstacle.picLength;
}

int getWidthObstacle(Obstacle obstacle) {
    return obstacle.picWidth;
}

unsigned char* getImage(Obstacle obstacle) {
    //这是我定义这两个属性的目的
    return obstacleImages[obstacle.type + obstacle.picState];
}

Obstacle moveLeft(Obstacle obstacle) {
    obstacle.x -= MOVE_SPEED;
    return obstacle;
}

//以上的方法为Obstacle接口中本来应该实现的方法

Obstacle ObstacleConstructor(Obstacle obstacle) {
    int choice = rand();

    //随机生成一种障碍物
    obstacle.type = (choice % 3);
    obstacle.picState = 0;

    //仙人掌
    if (obstacle.type == 0 || obstacle.type == 1) {
        obstacle.picLength = 12;
        obstacle.picWidth = 20;
    } else if (obstacle.type == 2) {  //翼龙
        obstacle.picLength = 20;
        obstacle.picWidth = 20;  //图片的参数同样写死，懒得设置常量了
    }

    obstacle.y = groundY - obstacle.picWidth;
    obstacle.x = FRAME_LENGTH - obstacle.picLength - 10;

    // //如果是翼龙，飞的高度是
    if (obstacle.type == 2) {
        obstacle.y -= 20;
    }
    return obstacle;
}

Obstacle fly(Obstacle obstacle){
    if (obstacle.type == 0 || obstacle.type == 1) {
        return obstacle;
    }
    //换一只脚继续走路
    if (obstacle.type == 2 && obstacle.picState == 0) {
        obstacle.picState = 1;
    } else if (obstacle.type == 2 && obstacle.picState == 1) {
        obstacle.picState = 0;
    }
    return obstacle;
}

//####################################################################
// GamePabel类的普通属性，为方便，做成全局变量

// time_t lastObstacleCreated = 0;
// time_t obstacleCreatedInterval = 1;  //隔多少 秒 生成一个障碍物
int score = 0;
int gameState = 0;
Dino dino;

int OBSTACLE_CAPACITY = 2;  //最多生成多少个障碍
int obstacleNum = 0;            //当前有多少个障碍


Obstacle obstacles[2] = {};       //存放障碍的容器

//####################################################################
//静态属性初始化

void staticConstrucotr() {
    //站立时左上角y的坐标
    STAND_Y = groundY - (DINO_STAND_WIDTH);
    //下蹲时左上角y的坐标
    BELOW_Y = groundY - (DINO_BELOW_WIDTH);

    //lastObstacleCreated = time(NULL);
    //直接写死了，不写常量来确定图片的数量
    //故意放两张仙人掌的照片，为了和翼龙扇翅膀需要两张照片对应
}

//静态属性销毁
void staticDestructor() {
    // free(DinoImages);
    // free(obstacleImages);
    // free(obstacles);
}

//####################################################################
//绘图函数先声明，再定义

void paint();
void paintStartMenu();
void paintGround();
void paintDino();
void paintScore();
void paintObstacle();
void paintEndMenu();

//####################################################################
//与碰撞相关的函数

void createObstacle() {
    // use delay_1ms to control time and don't consider it here
    if (obstacleNum >= OBSTACLE_CAPACITY)
        return;

    Obstacle newObstacle;
    newObstacle = ObstacleConstructor(newObstacle);

    obstacles[obstacleNum] = newObstacle;
    obstacleNum++;
}

int collided(Obstacle obstacle);
int pointInDino(int x, int y);

/**
 * 检查dino是否和任意障碍物相撞
 */
void checkCollision() {
    for (int i = 0; i < obstacleNum; i++) {
        if (collided(obstacles[i])) {
            gameState = END_MENU;
            break;
        }
    }
}

/**
 * 检查dino是否和某一障碍物相撞
 *
 * @param obstacle 待比较的障碍物
 * @return 如果相撞，返回true,否则返回false
 */
int collided(Obstacle obstacle) {
    int x = getX(obstacle);
    int y = getY(obstacle);
    int length = getLengthObstacle(obstacle);
    int width = getWidthObstacle(obstacle);

    // 检查的方法是看障碍物图片的四个角的点是否在dino图片中
    if (pointInDino(x, y))
        return 1;
    else if (pointInDino(x + length, y))
        return 1;
    else if (pointInDino(x + length, y + width))
        return 1;
    else if (pointInDino(x, y + width))
        return 1;
    return 0;
}

/**
 * (x, y)是否在dino图片中
 */
int pointInDino(int x, int y) {
    return dino.x <= x && x <= dino.x + getLengthDino(dino) && dino.y <= y && y <= dino.y + getWidthDino(dino);
}

//####################################################################
//程序入口
void ObstacleMove();

int main(void) {
    
    int cnt1 = 0; //use this to control the time of the obstacles' occurence

    IO_init();  // init OLED

    staticConstrucotr();
    dino = DinoConstructor(dino);  //初始化dino对象

    LCD_Clear(BLACK);
    BACK_COLOR=BLACK;

    paint();
    while(1){
        if(Get_Button(0) || Get_Button(1)){
            gameState = GAMING;
            delay_1ms(500);
            break;
        }
    }
    LCD_Clear(BLACK);
    
    // GamePanel::run方法
    while (1) {


        //decorate the ground
        drawPoints();
        drawPoints2();

        //use this to control the time of the obstacles' occurence
        cnt1 = (cnt1 + 1) % 20;

        delay_1ms(100);
        //LCD_Clear(BLACK);
        if (gameState == GAMING) {
            
            //score
            score++;

            //decorate the ground, make it move
            movePoints();
            movePoints2();

            //use this to control the time of the obstacles' occurence
            if(!cnt1){  
                createObstacle(); //create
            }

            dino = walk(dino);
           
            //jump button
            if(Get_Button(0) && !dino.jumping){
                dino = setState(dino, UP);   
            }

            //down button
            if(Get_Button(1) && !cnt){
                dino = setState(dino, BELOW_LEFT_UP);
                dino.y++;
            }else if(Get_Button(1) && cnt){
                cnt = 0;
            }

            //move obstacles
            ObstacleMove();
            for (int i = 0; i < obstacleNum; i++) {
                obstacles[i] = fly(obstacles[i]); //make pter fly
            }

            //check jump and down
            dino = jump(dino);
            if(dino.state == BELOW_LEFT_UP || dino.state == BELOW_RIGHT_UP){
                cnt++;
            }
            if(cnt == 3){
                cnt = 0;
                dino.y--;
                dino = stand(dino);
            }

            checkCollision();
        }
        paint();
        if (gameState == END_MENU){
            break;
        }
    }

    staticDestructor();

    return 0;
}

//接管所有绘图行为
void paint() {
    if (gameState == START_MENU) {
        paintStartMenu_S();
    } else if (gameState == GAMING) {
        paintGround();
        paintDino();
        paintScore_S("%d",score);
        // paintScore();
        paintObstacle();
    } else if (gameState == END_MENU) {
        paintEndMenu_S("Score is: %d", score);
    }
}

void paintStartMenu() {
    LCD_ShowString(FRAME_LENGTH / 5 + 5, FRAME_WIDTH / 4, (u8*)"Chrome Dino", WHITE);
    LCD_ShowString(FRAME_LENGTH / 5 + 13, FRAME_WIDTH / 2, (u8*)"play now!", WHITE);
}

void paintGround() {
    //不动的地面加上会动的障碍物很诡异 -_-
    LCD_ShowPicCustomed(groundx, groundY, groundx+ FRAME_LENGTH - 1, groundY+groundWidth - 1, ground1Image);
    if(groundx == -90){
        groundx = 0;
    } else{
        groundx -= MOVE_SPEED;
    }
}

void paintDino() {
    int picIndex = dino.state;

    //如果dino在空中，使用的图片和站在地上的是同一张
    if (dino.state == UP || dino.state == DOWN) {
        picIndex = STAND_FULL;
    }
    int picLength = DINO_STAND_LENGTH;
    int picWidth = DINO_STAND_WIDTH;

    // dino蹲下和站立时的图片尺寸是不一样的
    if (dino.state == BELOW_LEFT_UP || dino.state == BELOW_RIGHT_UP) {
        picLength = DINO_BELOW_LENGTH;
        picWidth = DINO_BELOW_WIDTH;
    }

    // state同时是所使用的的图片的下标

    LCD_ShowPicCustomed(dino.x, dino.y,
                        dino.x + picLength - 1, dino.y + picWidth - 1, DinoImages[picIndex]);
}

void paintDinoBlack1() {
    int picLength = DINO_STAND_LENGTH;
    int picWidth = DINO_STAND_WIDTH;

    LCD_ShowPicCustomed(dino.x, dino.y,
                        dino.x + picLength - 1, dino.y + picWidth - 1, black1Image);
}

void paintDinoBlack2() {
    int picLength = DINO_BELOW_LENGTH;
    int picWidth = DINO_BELOW_WIDTH;

    LCD_ShowPicCustomed(dino.x, dino.y,
                        dino.x + picLength - 1, dino.y + picWidth - 1, black2Image);
}

void paintScore() {
    char scoreString[20];
    sprintf(scoreString, "%d", score);
    LCD_ShowString(FRAME_LENGTH - 40, 5, (u8*)scoreString, WHITE);
    //score++;
}

void paintObstacle() {
    for (int i = 0; i < obstacleNum; i++) {
        Obstacle obstacle = obstacles[i];

        if (getX(obstacle) <= 0) {
            //标准的list的remove方法
            for (int j = i + 1; j < obstacleNum; ++j) {
                obstacles[j - 1] = obstacles[j];
            }

            obstacleNum--;
            i--;
            continue;
        }

        unsigned char* image = getImage(obstacle);

        int x = getX(obstacle);
        int y = getY(obstacle);
        // LCD_ShowPicCustomed(x, y, x + getLengthObstacle(obstacle) - 1, y + getWidthObstacle(obstacle) - 1, image);   
        paintObstacle_S(x, y, obstacle, image);
    }
}

void ObstacleMove(){
    for (int i = 0; i < obstacleNum; i++) {
        Obstacle obstacle = obstacles[i];
        int x = getX(obstacle);
        int y = getY(obstacle);
        if(obstacle.type == 0){
            paintObstacleBlack1_S(x, y, obstacle, black3Image);
            //LCD_ShowPicCustomed(x, y, x + getLengthObstacle(obstacle) - 1, y + getWidthObstacle(obstacle) - 1, black3Image);
        }else{
            paintObstacleBlack2_S(x, y, obstacle, black1Image);
            //LCD_ShowPicCustomed(x, y, x + getLengthObstacle(obstacle) - 1, y + getWidthObstacle(obstacle) - 1, black1Image);
        }
        obstacles[i] = moveLeft(obstacle);
    }
}

void paintEndMenu() {
    paintEndMenuText_S();
    paintEndMenuScore_S("Score is: %d", score);
}
