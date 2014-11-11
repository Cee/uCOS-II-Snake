
#include "config.h"
#include <stdlib.h>
//=======================================
#include "app_cfg.h"
#include "Printf.h"
#include "ucos_ii.h"

#include "gui.h"
#include "math.h"
#include "GUI_Protected.h"
#include "GUIDEMO.H"
#include "WM.h"
#include "Dialog.h"
#include "LISTBOX.h"
#include "EDIT.h"
#include "SLIDER.h"
#include "FRAMEWIN.h" 

#define N 200 //蛇的最大节数

/*全局变量定义*/
int i,delay;
U8 key;
int score = 0;//游戏得分
int gamespeed = 5000;//游戏速度(速度越大蛇移动越慢)
int state = 0;

/*食物结构体定义*/
struct Food
{
   int x;  //食物的横坐标
   int y;  //食物的纵坐标
   int yes;//判断是否要出现食物的变量
}food;     //食物的结构体

/*蛇结构体定义*/
struct Snake
{
  int x[N];
  int y[N];
  int node;//蛇的节数
  int direction;//蛇移动方向
  int life;//蛇的生命，0活着，1死亡
}snake;

/*函数声明*/
void Drawk(void);//开始画面
void GameOver(void);//游戏结束
void GamePlay(void);//游戏过程
void ShowScore(void);//输出游戏成绩
void ShowGameSpeed(void);

U8 Key_Scan( void )
{
	Delay( 8000 ) ;
	rGPBDAT &=~(1<<6);
	rGPBDAT |=1<<7;

	if(      (rGPFDAT&(1<< 0)) == 0 )		return 1 ;
	else if( (rGPFDAT&(1<< 2)) == 0 )		return 3 ;
	else if( (rGPGDAT&(1<< 3)) == 0 )		return 5 ;
	else if( (rGPGDAT&(1<<11)) == 0 )		return 7 ;

	rGPBDAT &=~(1<<7);
	rGPBDAT |=1<<6;
	if(      (rGPFDAT&(1<< 0)) == 0 )		return 2 ;
	else if( (rGPFDAT&(1<< 2)) == 0 )		return 4 ;
	else if( (rGPGDAT&(1<< 3)) == 0 )		return 6 ;
	else if( (rGPGDAT&(1<<11)) == 0 )		return 8 ;
	else return 0xff ;
	
}




#define WIDTH 620
#define HEIGHT 460
#define CELL 10
void _MainTask(void)//应用程序开始
{

 Drawk();
 ShowScore();
 ShowGameSpeed();
	do {
		key = Key_Scan();
		if (key == 7) {
			gamespeed += 100;
			gamespeed = gamespeed > 9999 ? 9999 : gamespeed;
		}
		if (key == 5) {
			gamespeed -= 100;
			gamespeed = gamespeed < 1000 ? 1000 : gamespeed;
		}
		ShowGameSpeed();
	}while (key != 1);
 GamePlay();
}


void Drawk(void)//以矩形填充的方式，画出一个围墙
{
 GUI_SetColor(GUI_RED);
 GUI_SetPenSize(CELL);
 GUI_AA_DrawLine(5,CELL+5,WIDTH,CELL+5);
 GUI_AA_DrawLine(5,HEIGHT,WIDTH,HEIGHT);
 GUI_AA_DrawLine(5,CELL+5,5,HEIGHT);
 GUI_AA_DrawLine(WIDTH,CELL+5,WIDTH,HEIGHT);
}
/*游戏主要部分*/
void GamePlay(void)
{
	state = 1;
srand(food.x);
   food.yes = 1;//1表示需要出现新食物，0表示已经存在食物
   snake.life = 0;//活着
   snake.direction = 1;//方向向右
   snake.x[0] = 50;snake.y[0] = 50;//蛇头，初始为两节
   snake.x[1] = 55;snake.y[1] = 50;
   snake.node = 2;//蛇的节数
   ShowScore();//输出得分
   while(1)
   {
      do
   {
      key = Key_Scan();//接收按键
   if(food.yes == 1)//需要出现新食物
   {
      food.x = rand()%(WIDTH-2*CELL)+CELL+5;//新出现的食物放在随机位置
   food.y = rand()%(HEIGHT-2*CELL)+CELL+5;//
   while(food.x%CELL!=0)//食物随机出现后必须让食物能够在整格内，这样才可以让蛇吃到
    food.x++;
   while(food.y%CELL!=0)
    food.y++;
   food.yes = 0;//画面上有食物了
   }
   if(food.yes == 0)//画面上有食物了就要显示
   {
    GUI_SetColor(GUI_DARK_BLUE);
             GUI_FillRect(food.x,food.y,food.x+CELL,food.y+CELL);

   }
   /*蛇的每个环节往前移动，也就是贪吃蛇的关键算法*/
   for(i = snake.node-1;i>0;i--)
   {
     snake.x[i] = snake.x[i-1];
     snake.y[i] = snake.y[i-1];

   }
   /*1,2,3,4表示右，左，上，下四个方向，通过这个判断来移动蛇头*/
         switch(snake.direction)//拐过来

   {
          case 1: snake.x[0] += CELL;break;
          case 2: snake.x[0] -= CELL;break;
    	  case 3: snake.y[0] += CELL;break;
   		  case 4: snake.y[0] -= CELL;break;
   }
   for(i = 3;i<snake.node;i++)//从蛇的第四节开始判断是否撞到自己了，因为蛇头为第二节，第三节不可能拐过来
   {
   if(snake.x[i] == snake.x[0]&&snake.y[i] == snake.y[0])
   {
     GameOver();//显示失败
     snake.life = 1;
     break;
   }
     
   }
   
   if(snake.x[0]<CELL+5 || snake.x[0]>WIDTH-CELL-5 ||snake.y[0]<CELL+5 ||snake.y[0]>HEIGHT-CELL-5)//蛇是否撞到墙壁
   {
       GameOver();//本次游戏结束
    snake.life = 1;//蛇死
   }
   if(snake.life == 1)//以上两种判断以后，如果蛇死就跳出内循环，重新开始
    break;
   if(snake.x[0] == food.x && snake.y[0] == food.y)//吃到食物后
   {
      GUI_SetColor(GUI_WHITE);//把画面上的食物东西吃掉（以背景色填充）
   GUI_FillRect(food.x,food.y,food.x+CELL,food.y+CELL);
   snake.x[snake.node] = -10;snake.y[snake.node] = +10;/*新的一节先放在看不见的位置，下次循环就取前一节的位置*/
   snake.node++;//蛇的身体长长一节
   food.yes = 1;//画面上需要出现新的食物
   score += 10;//得分加10
   ShowScore();//输出得分
   }
   GUI_SetColor(GUI_BLACK);//画出蛇
   for(i = 0;i<snake.node;i++)
    GUI_FillRect(snake.x[i],snake.y[i],snake.x[i]+CELL,snake.y[i]+CELL);
    
   //GUI_Delay(1);
   //OSTimeDlyHMSM(0,0,0,1000);
   //Delay(8000);
   delay = 40000;
   while (delay) {
    GUI_Exec();
    delay--;
  }
  //Sleep(200);
   
   GUI_SetColor(GUI_WHITE);//用白色去除蛇的最后一节
      GUI_FillRect(snake.x[snake.node-1],snake.y[snake.node-1],snake.x[snake.node-1]+CELL,snake.y[snake.node-1]+CELL);

   }while((key!= 1)&&(key != 3)&&(key != 5)&&(key != 7));//在没有按键的情况下，蛇自己移动身体
   if(snake.life == 1)//如果蛇死机跳出循环
    break;
   if(key == GUI_KEY_ESCAPE)//按ESC键退出
    break;
   else
    /*判断是否往相反的方向移动*/
    switch(key)
   {
      case 7: //往上
    if(snake.direction!=3)  snake.direction = 4;
    break;
   case 1:		//往右
    if(snake.direction != 2) snake.direction = 1;
    break;
   case 3:		//往左
    if(snake.direction != 1)  snake.direction = 2;
    break;
   case 5:		//往下
    if(snake.direction != 4)  snake.direction = 3;
    break;
   }
}
}
void GameOver(void)//游戏结束
{
  GUI_SetColor(GUI_WHITE);
  GUI_FillRect(0,0,WIDTH+10,HEIGHT+10);
  ShowScore();
  GUI_SetColor(GUI_RED);
  GUI_SetBkColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font32_ASCII);
  GUI_DispStringAt("GAME OVER",150,80);
  GUI_SetFont(&GUI_Font32_ASCII);
  GUI_SetColor(GUI_BLUE);
  GUI_DispStringAt("Try again, press RIGHT key",80,180);
  	// play again
	do {
		key = Key_Scan();
		if (key == 7) {
			gamespeed += 100;
			gamespeed = gamespeed > 9999 ? 9999 : gamespeed;
		}
		if (key == 5) {
			gamespeed -= 100;
			gamespeed = gamespeed < 1000 ? 1000 : gamespeed;
		}
		ShowGameSpeed();
	}while (key != 1);
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);
 	GUI_FillRect(0,0,GUI_XMAX,GUI_YMAX);
	Drawk();
	ShowGameSpeed();
 	GamePlay();
}
void ShowScore()
{
   char str[10];
   GUI_SetColor(GUI_YELLOW);
   GUI_SetBkColor(GUI_BLACK);
   sprintf(str,"score:%d",score);
  GUI_SetFont(&GUI_Font24_ASCII);
   GUI_DispStringAt(str,640,200);
}
void ShowGameSpeed(){
	char str[40];
	int i = 0;
	for (i = 0; i < 40; i++){
		str[i] = '\0';
	}
   GUI_SetColor(GUI_GREEN);
   GUI_SetBkColor(GUI_BLACK);
   sprintf(str,"gamespeed:%d",gamespeed);
  GUI_SetFont(&GUI_Font24_ASCII);
   GUI_DispStringAt(str,640,280);
}	
//主函数
int Main(void)
 {

 GUI_Init();
 GUI_SetBkColor(GUI_WHITE);
 GUI_Clear(); 
 GUI_SetColor(GUI_WHITE);
 GUI_FillRect(0,0,GUI_XMAX,GUI_YMAX);
 _MainTask();
 
 return 0; 
}

