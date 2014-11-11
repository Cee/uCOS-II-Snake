
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

#define N 200 //�ߵ�������

/*ȫ�ֱ�������*/
int i,delay;
U8 key;
int score = 0;//��Ϸ�÷�
int gamespeed = 5000;//��Ϸ�ٶ�(�ٶ�Խ�����ƶ�Խ��)
int state = 0;

/*ʳ��ṹ�嶨��*/
struct Food
{
   int x;  //ʳ��ĺ�����
   int y;  //ʳ���������
   int yes;//�ж��Ƿ�Ҫ����ʳ��ı���
}food;     //ʳ��Ľṹ��

/*�߽ṹ�嶨��*/
struct Snake
{
  int x[N];
  int y[N];
  int node;//�ߵĽ���
  int direction;//���ƶ�����
  int life;//�ߵ�������0���ţ�1����
}snake;

/*��������*/
void Drawk(void);//��ʼ����
void GameOver(void);//��Ϸ����
void GamePlay(void);//��Ϸ����
void ShowScore(void);//�����Ϸ�ɼ�
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
void _MainTask(void)//Ӧ�ó���ʼ
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


void Drawk(void)//�Ծ������ķ�ʽ������һ��Χǽ
{
 GUI_SetColor(GUI_RED);
 GUI_SetPenSize(CELL);
 GUI_AA_DrawLine(5,CELL+5,WIDTH,CELL+5);
 GUI_AA_DrawLine(5,HEIGHT,WIDTH,HEIGHT);
 GUI_AA_DrawLine(5,CELL+5,5,HEIGHT);
 GUI_AA_DrawLine(WIDTH,CELL+5,WIDTH,HEIGHT);
}
/*��Ϸ��Ҫ����*/
void GamePlay(void)
{
	state = 1;
srand(food.x);
   food.yes = 1;//1��ʾ��Ҫ������ʳ�0��ʾ�Ѿ�����ʳ��
   snake.life = 0;//����
   snake.direction = 1;//��������
   snake.x[0] = 50;snake.y[0] = 50;//��ͷ����ʼΪ����
   snake.x[1] = 55;snake.y[1] = 50;
   snake.node = 2;//�ߵĽ���
   ShowScore();//����÷�
   while(1)
   {
      do
   {
      key = Key_Scan();//���հ���
   if(food.yes == 1)//��Ҫ������ʳ��
   {
      food.x = rand()%(WIDTH-2*CELL)+CELL+5;//�³��ֵ�ʳ��������λ��
   food.y = rand()%(HEIGHT-2*CELL)+CELL+5;//
   while(food.x%CELL!=0)//ʳ��������ֺ������ʳ���ܹ��������ڣ������ſ������߳Ե�
    food.x++;
   while(food.y%CELL!=0)
    food.y++;
   food.yes = 0;//��������ʳ����
   }
   if(food.yes == 0)//��������ʳ���˾�Ҫ��ʾ
   {
    GUI_SetColor(GUI_DARK_BLUE);
             GUI_FillRect(food.x,food.y,food.x+CELL,food.y+CELL);

   }
   /*�ߵ�ÿ��������ǰ�ƶ���Ҳ����̰���ߵĹؼ��㷨*/
   for(i = snake.node-1;i>0;i--)
   {
     snake.x[i] = snake.x[i-1];
     snake.y[i] = snake.y[i-1];

   }
   /*1,2,3,4��ʾ�ң����ϣ����ĸ�����ͨ������ж����ƶ���ͷ*/
         switch(snake.direction)//�չ���

   {
          case 1: snake.x[0] += CELL;break;
          case 2: snake.x[0] -= CELL;break;
    	  case 3: snake.y[0] += CELL;break;
   		  case 4: snake.y[0] -= CELL;break;
   }
   for(i = 3;i<snake.node;i++)//���ߵĵ��Ľڿ�ʼ�ж��Ƿ�ײ���Լ��ˣ���Ϊ��ͷΪ�ڶ��ڣ������ڲ����ܹչ���
   {
   if(snake.x[i] == snake.x[0]&&snake.y[i] == snake.y[0])
   {
     GameOver();//��ʾʧ��
     snake.life = 1;
     break;
   }
     
   }
   
   if(snake.x[0]<CELL+5 || snake.x[0]>WIDTH-CELL-5 ||snake.y[0]<CELL+5 ||snake.y[0]>HEIGHT-CELL-5)//���Ƿ�ײ��ǽ��
   {
       GameOver();//������Ϸ����
    snake.life = 1;//����
   }
   if(snake.life == 1)//���������ж��Ժ����������������ѭ�������¿�ʼ
    break;
   if(snake.x[0] == food.x && snake.y[0] == food.y)//�Ե�ʳ���
   {
      GUI_SetColor(GUI_WHITE);//�ѻ����ϵ�ʳ�ﶫ���Ե����Ա���ɫ��䣩
   GUI_FillRect(food.x,food.y,food.x+CELL,food.y+CELL);
   snake.x[snake.node] = -10;snake.y[snake.node] = +10;/*�µ�һ���ȷ��ڿ�������λ�ã��´�ѭ����ȡǰһ�ڵ�λ��*/
   snake.node++;//�ߵ����峤��һ��
   food.yes = 1;//��������Ҫ�����µ�ʳ��
   score += 10;//�÷ּ�10
   ShowScore();//����÷�
   }
   GUI_SetColor(GUI_BLACK);//������
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
   
   GUI_SetColor(GUI_WHITE);//�ð�ɫȥ���ߵ����һ��
      GUI_FillRect(snake.x[snake.node-1],snake.y[snake.node-1],snake.x[snake.node-1]+CELL,snake.y[snake.node-1]+CELL);

   }while((key!= 1)&&(key != 3)&&(key != 5)&&(key != 7));//��û�а���������£����Լ��ƶ�����
   if(snake.life == 1)//�������������ѭ��
    break;
   if(key == GUI_KEY_ESCAPE)//��ESC���˳�
    break;
   else
    /*�ж��Ƿ����෴�ķ����ƶ�*/
    switch(key)
   {
      case 7: //����
    if(snake.direction!=3)  snake.direction = 4;
    break;
   case 1:		//����
    if(snake.direction != 2) snake.direction = 1;
    break;
   case 3:		//����
    if(snake.direction != 1)  snake.direction = 2;
    break;
   case 5:		//����
    if(snake.direction != 4)  snake.direction = 3;
    break;
   }
}
}
void GameOver(void)//��Ϸ����
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
//������
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

