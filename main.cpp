#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "glm.h"
#include "Display.h"
#include "Keyboard.h"
#include "Obj.h"
#define BMP_Header_Length 54
#define LINE 9
#define POINT_NUM 16 //(LINE/2)^2
#define BLOCK_SIZE 11
#define OBS_SIZE 1.5
#define MEM_SIZE 1.5
//方向
#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define GLUT_WHEEL_UP 3      //向上滚动鼠标
#define GLUT_WHEEL_DOWN 4
#define PUSH_EMPTY 0         //鼠标按住的键
#define	PUSH_LEFT 1
#define PUSH_RIGHT 2
#define MOVE_PRECISION  100  //视线转动灵敏度
#define SCROLL_PRECISION 0.1 //视线滚动速度
//纹理加载
GLuint wall[5],ground;//墙壁纹理和地面纹理
int chose;//纹理选择
int output[LINE][LINE];//迷宫矩阵
int endx, endy,stpox,stpoy,enddir;
int winlose;//0normal,1win,2lose
GLuint win, lose,start,end;
struct poin
{
	int x;
	int y;
	int known;
}po[POINT_NUM];//点结构(构造迷宫用)
struct member
{
	int outx;//矩阵位置
	int outy;
	GLfloat posx;//场景位置
	GLfloat posy;
	int ismove;
	int dir;//方向(来的方向)
}mem[4];
//视线
GLfloat eye[3], forward[3],up[3],flo[3],right[3],Eye[3],Forward[3];
int sky;//天眼
int old[2];
int push_side;
GLfloat angle[2];
//light
GLfloat light1_att = 0.0;     //LIGHT1的衰减系数
bool light1_open = true;    //LIGHT1是否开启
GLfloat light1_position[] = { 0.0, 40.0, 0, 1.0 };       //LIGHT1的位置坐标
GLfloat point_light1[] = { 1.0, 1.0, 0.5, 0.5 };		  //LIGHT1的光照参数

GLuint drawOBJ(char * filename);      //load the obj model
GLuint LoadTexture(const char* file);
void Reshape(GLsizei w, GLsizei h);
void InitMaze();
void GetMaze();
void onMouseMove(int x, int y);
void MouseFunc(int button, int state, int x, int y);
void updateDirection();
void timer(int value);

int main(int argc,char *argv[])
{
	//初始化设置
	glutInit(&argc, argv);//初始化
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);//显示模式
	glutInitWindowPosition(200, 50);//窗口位置
	glutInitWindowSize(700, 700);//窗口大小
	glutCreateWindow("Maze");//建立窗口
	InitMaze();
	glEnable(GL_DEPTH_TEST);//深度检测
	//功能函数
	glutDisplayFunc(&Display);//绘制
	glutReshapeFunc(&Reshape);//reshape
	glutKeyboardFunc(&Keyboard);
	glutSpecialFunc(&ProcessSpecialKeyboard);
	glutMouseFunc(&MouseFunc);//鼠标响应x
	glutMotionFunc(onMouseMove);//鼠标拖动x
	glutTimerFunc(100, timer, 1);//定时器 ms,point,value,需在函数中再调用
	glutMainLoop();//进入glut事件处理循环
    return 0;
}
GLuint LoadTexture(const char* file_name) {
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLint last_texture_ID = 0;
	GLuint texture_ID = 0;
	FILE * pFile;
	pFile=fopen( file_name, "rb");
	if (pFile == 0){
		printf("load file failed %s\n",file_name);
		return 0;
	}
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}
	pixels = (GLubyte*)malloc(total_bytes);  	if (pixels == 0)
	{
		fclose(pFile);  	 	return 0;
	}
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}


	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);

	free(pixels);
	return texture_ID;
}
void InitMaze() {
	winlose = 0;
	glClearColor(0.0, 0.0, 0.0, 1.0);    //背景色
	glEnable(GL_DEPTH_TEST);             //深度测试
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);              //向量单位化
	glEnable(GL_COLOR_MATERIAL);         //材质颜色追踪
	glEnable(GL_LIGHTING);//灯光
	glEnable(GL_LIGHT1);
	//纹理加载
	ground = LoadTexture("ground.bmp");
	wall[0] = LoadTexture("wall1.bmp");
	wall[1] = LoadTexture("wall2.bmp");
	wall[2] = LoadTexture("wall3.bmp");
	wall[3] = LoadTexture("wall4.bmp");
	wall[4] = LoadTexture("wall5.bmp");
	loadOBJ();
	chose = 0;
	GetMaze();
	angle[0] = 1.57;       //初始视线夹角
	angle[1] = -1.57;
	flo[0] = 0;
	flo[1] = 40;
	flo[2] = 40;
	forward[0]=0;
	forward[1]=0;
	forward[2]=-1;
	eye[0] = mem[0].posx;
	Eye[0] = eye[0] + flo[0];
	eye[1] = BLOCK_SIZE / 2;
	Eye[1] = eye[1] + flo[1]+30;
	eye[2] = mem[0].posy;
	Eye[2] = eye[2] + flo[2];
	updateDirection();
	sky = 0;
}
void Reshape(GLsizei w, GLsizei h) {
	if (h == 0) { h = 1; }
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, w / h, 20, 2000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
//迷宫生成
void GetMaze() {
	//最小生成树建迷宫
	int countpo, knownum, i, j, k, next;//顶点计数,已知点计数,循环用,打墙用
	int Known[POINT_NUM];//是否已知
	int viewpo, cou, wall[4];
	int pointli, posget,thisdir, endpos;//每条边网格数，临时（点）
	pointli = (LINE-1) / 2;
	countpo = knownum = viewpo = cou = k = next = 0;
	//1.start:迷宫生成
	for (i = 0;i<POINT_NUM;i++) {
		Known[i] = -1;
	}
	srand((unsigned)time(NULL));
	//初始化，1代表墙，0代表连通，顶点初始化为0
	for (j = 0;j<LINE;j++) {
		for (i = 0;i<LINE;i++) {
			if (i % 2 == 1 && j % 2 == 1) {
				//顶点初始化(不是墙的网格),之后要连通这些网格
				output[i][j] = 0;
				po[countpo].x = i;
				po[countpo].y = j;
				po[countpo].known = 0;
				countpo++;
			}
			else {
				output[i][j] = 1;//隔离网格的墙
			}
		}
	}
	//连接
	Known[0] = rand() % POINT_NUM;//起始点
	knownum = 1;//已知点数目（全部已知时停下）
	po[Known[0]].known = 1;//起点已知
	wall[0] = wall[1] = wall[2] = wall[3] = 0;//四个方向的墙
	do {
		//因为每条边权值都是1，随机从已知点选
		viewpo = rand() % knownum;//在已知点中随机选取
		k = Known[viewpo];//点编号（左上开始计数）
						  //计数墙
		cou = 0;
		//获取点k四周的墙的状态（墙还在且打通墙后的那个点还没有连通）
		if (k>(pointli - 1) && output[po[k].x][po[k].y - 1] == 1 && po[k - pointli].known == 0) { wall[cou] = -7;cou++; }//上
		if (k<(POINT_NUM - pointli) && output[po[k].x][po[k].y + 1] == 1 && po[k + pointli].known == 0) { wall[cou] = 7;cou++; }
		if (k%pointli != 0 && output[po[k].x - 1][po[k].y] == 1 && po[k - 1].known == 0) { wall[cou] = -1;cou++; }
		if (k%pointli != (pointli - 1) && output[po[k].x + 1][po[k].y] == 1 && po[k + 1].known == 0) { wall[cou] = 1;cou++; }
		//打墙
		if (cou != 0) {
			next = rand() % cou;//下一个通道
			switch (wall[next]) {
			case -7:output[po[k].x][po[k].y - 1] = 0;po[k - pointli].known = 1;Known[knownum] = k - pointli;knownum++;break;
			case 7:output[po[k].x][po[k].y + 1] = 0;po[k + pointli].known = 1;Known[knownum] = k + pointli;knownum++;break;
			case -1:output[po[k].x - 1][po[k].y] = 0;po[k - 1].known = 1;Known[knownum] = k - 1;knownum++;break;
			case 1:output[po[k].x + 1][po[k].y] = 0;po[k + 1].known = 1;Known[knownum] = k + 1;knownum++;break;
			default:break;
			}
		}
	} while (knownum != POINT_NUM);
	//1.end:迷宫生成完成
	//2.start:路障生成，随机生成路障（圆柱）
	for (j = 2;j < LINE-2;j++) {
		for (i = 2;i < LINE-2;i++) {
			if (output[i][j] == 0 && (rand() % 4) == 0) {//1/4概率生成路障
				output[i][j] = 2;
			}
		}
	}
	//2.end:路障
	//3.start:迷宫起点和终点(起点在正面)
	posget = rand() % pointli;
	posget = 2 * posget + 1;
	mem[0].outx = LINE - 1;
	mem[0].outy = posget;
	output[LINE - 1][posget] = 0;
	mem[0].dir = DIR_UP;
	mem[0].posx = BLOCK_SIZE*(posget + 0.5);
	mem[0].posy = BLOCK_SIZE*(LINE - 0.5);
	stpox=mem[0].posx;
	stpoy=mem[0].posy;
	mem[0].ismove = 1;
	endpos = rand() % pointli;
	endpos = 2 * endpos + 1;
	enddir = rand() % 3 + 1;
	switch (enddir)
	{
	case 1:output[endpos][LINE - 1] = 0;endx = endpos;endy = LINE - 1;break;//right
	case 2:output[0][endpos] = 0;endx = 0;endy = endpos;break;//down
	case 3:output[endpos][0] = 0;endx = endpos;endy = 0;break;//left
	default:break;
	}
	//3.end:迷宫起点和终点
	//4.start:对手君*3起点
	for (i = 1;i <= 3;i++) {
		do {
			posget = rand() % pointli;
			posget = 2 * posget + 1;
			thisdir = i;
		} while ((thisdir == enddir && posget == endpos)||(thisdir==DIR_DOWN&&(posget==1||posget==(2*pointli-1))));
		switch (thisdir)
		{
		case 1:mem[i].dir = DIR_RIGHT;mem[i].outx = posget;mem[i].outy = LINE - 2;
			mem[i].posx = BLOCK_SIZE*(LINE-1.5);mem[i].posy = BLOCK_SIZE*(posget + 0.5);
			break;
		case 2:mem[i].dir = DIR_DOWN;mem[i].outx = 1;mem[i].outy = posget;
			mem[i].posx = BLOCK_SIZE*(posget + 0.5);mem[i].posy = BLOCK_SIZE*1.5;
			break;
		case 3:mem[i].dir = DIR_LEFT;mem[i].outx = posget;mem[i].outy = 1;
			mem[i].posx = BLOCK_SIZE*1.5;mem[i].posy = BLOCK_SIZE*(posget + 0.5);
			break;
		default:break;
		}
		mem[i].ismove= 1;
	}
	//4.end:对手君*3起点
}

void MouseFunc(int button, int state, int x, int y) {
	if (sky == 1) {
		if (state == GLUT_DOWN) {
			if (button == GLUT_LEFT_BUTTON) {
				push_side = PUSH_LEFT;
				old[0] = x;
				old[1] = y;
			}
			else if (button == GLUT_RIGHT_BUTTON) {
				push_side = PUSH_RIGHT;
				old[0] = x;
				old[1] = y;
			}
		}
		if (state == GLUT_UP) {
			push_side = PUSH_EMPTY;
			if (button == GLUT_WHEEL_UP) {       //拉近
				eye[0] += SCROLL_PRECISION * cos(angle[0])*cos(angle[1]);
				eye[1] += SCROLL_PRECISION * sin(angle[0])*cos(angle[1]);
				eye[2] += SCROLL_PRECISION * sin(angle[1]);
			}
			else if (button == GLUT_WHEEL_DOWN) { //拉远
				eye[0] -= SCROLL_PRECISION * cos(angle[0])*cos(angle[1]);
				eye[1] -= SCROLL_PRECISION * sin(angle[0])*cos(angle[1]);
				eye[2] -= SCROLL_PRECISION * sin(angle[1]);
			}

		}
		glutPostRedisplay();
	}
}
//鼠标移动
void onMouseMove(int x, int y) {
	if (sky == 1) {
		if (push_side == 1) {
			GLfloat dx = old[0] - x;
			GLfloat dy = old[1] - y;

			dx /= MOVE_PRECISION;
			dy /= MOVE_PRECISION;

			Eye[0] -= dx * right[0];
			Eye[1] -= dx * right[1];
			Eye[2] -= dx * right[2];

			Eye[0] -= dy * up[0];
			Eye[1] -= dy * up[1];
			Eye[2] -= dy * up[2];

			old[0] = x;
			old[1] = y;
			glutPostRedisplay();
		}
		else if (push_side == 2) {
			int dx = old[0] - x;
			int dy = old[1] - y;
			if (abs(dx) < 50) {      //限制移动幅度
				angle[0] += (GLfloat)dx / MOVE_PRECISION;
			}

			if (abs(dy) < 50) {
				angle[1] += (GLfloat)dy / MOVE_PRECISION;
			}

			updateDirection();     //转动视线后要更新右向量

			old[0] = x;            //更新原先位置
			old[1] = y;
			glutPostRedisplay();
		}
	}
}
void updateDirection() {
	Forward[0] = cos(angle[0])*cos(angle[1]);//0
	Forward[1] = sin(angle[0])*cos(angle[1]);//0
	Forward[2] = sin(angle[1]);//-1

	up[0] = cos(3.14 + angle[0])*cos(1.57 - angle[1]);
	up[1] = sin(3.14 + angle[0])*cos(1.57 - angle[1]);
	up[2] = sin(1.57 - angle[1]);

	//上向量叉乘前向量得到右向量
	right[0] = up[1] * Forward[2] - up[2] * Forward[1];
	right[1] = up[2] * Forward[0] - up[0] * Forward[2];
	right[2] = up[0] * Forward[1] - up[1] * Forward[0];
}
void timer(int value) {
	int i,j;
	double nextx, nexty, cirx, ciry;
	double dis, newpath, click;
	int flag;
	flag = 0;
	if (mem[0].ismove == 0) {
		//lose
		if(winlose==0)
		winlose = 2;
		//printf("You lose\n");
	}
	for (j = 0;j < 4;j++) {
		if (mem[j].posy < BLOCK_SIZE || mem[j].posx<BLOCK_SIZE || mem[j].posx>BLOCK_SIZE*(LINE - 1)) {
			if (j == 0) {
				if(winlose==0)
				winlose = 1;
				mem[0].ismove=0;
		//		printf("You win\n");
			}
			else {
				if(winlose==0)
				winlose = 2;
				mem[0].ismove=0;
		//		printf("You lose\n");
			}
		}
	}
	for (j = 1;j < 4;j++) {
		if (mem[j].ismove == 1) {
			cirx = (mem[j].outy + 0.5)*BLOCK_SIZE;
			ciry = (mem[j].outx + 0.5)*BLOCK_SIZE;
			switch (rand() % 4)
			{
				//1.该格中是否有圆柱
				//2.前进后是否进入另一格，是的话另一格是否为墙
			case DIR_UP:
				nextx = mem[j].posx;
				nexty = mem[j].posy - 1;
				//撞球
				for (i = 0;i < 4;i++) {
					if (i != j&&mem[i].ismove==1) {
						click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
						click = sqrt(click);
						if (click <= (2 * MEM_SIZE)) {
							newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
							newpath = sqrt(newpath);
							mem[j].posy = mem[i].posy + newpath;
							flag = 1;
							if (i == 0)
								mem[0].ismove = 0;
							else {
								if (rand() % 2 == 0)mem[j].ismove = 0;
								else mem[i].ismove = 0;
							}
							break;
						}
					}
				}
				if (output[mem[j].outx][mem[j].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[j].posy = ciry + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty - MEM_SIZE) < mem[j].outx*BLOCK_SIZE) {
						if (mem[j].outx == 0) { flag = 1; }//不能出去
						else if ((output[mem[j].outx - 1][mem[j].outy] == 1)
							|| (mem[j].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[j].outy + 1) && output[mem[j].outx - 1][mem[j].outy + 1] == 1)
							|| (mem[j].outy > 0 && (nextx - MEM_SIZE) < BLOCK_SIZE*mem[j].outy&&output[mem[j].outx - 1][mem[j].outy - 1] == 1)
							) {//撞墙
							mem[j].posy = mem[j].outx*BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else {//可以进去
							mem[j].outx--;
						}
					}
				}
				if (flag == 0) { mem[j].posy -= 1; }break;
			case DIR_DOWN:
				nextx = mem[j].posx;
				nexty = mem[j].posy + 1;
				for (i = 0;i < 4;i++) {
					if (i != j&&mem[i].ismove==1) {
						click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
						click = sqrt(click);
						if (click <= (2 * MEM_SIZE)) {
							newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
							newpath = sqrt(newpath);
							mem[j].posy = mem[i].posy - newpath;
							flag = 1;
							if (i == 0)
								mem[0].ismove = 0;
							else {
								if (rand() % 2 == 0)mem[j].ismove = 0;
								else mem[i].ismove = 0;
							}
							break;
						}
					}
				}
				if (output[mem[j].outx][mem[j].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[j].posy = ciry - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty + MEM_SIZE) > (mem[j].outx + 1)*BLOCK_SIZE) {
						if (mem[j].outx == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[j].outx + 1][mem[j].outy] == 1)
							|| (mem[j].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[j].outy + 1) && output[mem[j].outx + 1][mem[j].outy + 1] == 1)
							|| (mem[j].outy > 0 && (nextx - MEM_SIZE) < BLOCK_SIZE*mem[j].outy&&output[mem[j].outx + 1][mem[j].outy - 1] == 1)
							) {//撞墙
							mem[j].posy = (mem[j].outx + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else {//可以进去
							mem[j].outx++;
						}
					}
				}
				if (flag == 0) { mem[j].posy += 1; }break;
			case DIR_LEFT:
				nextx = mem[j].posx + 1;
				nexty = mem[j].posy;
				for (i = 0;i < 4;i++) {
					if (i != j&&mem[i].ismove==1) {
						click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
						click = sqrt(click);
						if (click <= (2 * MEM_SIZE)) {
							newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
							newpath = sqrt(newpath);
							mem[j].posx = mem[i].posx - newpath;
							flag = 1;
							if (i == 0)
								mem[0].ismove = 0;
							else {
								if (rand() % 2 == 0)mem[j].ismove = 0;
								else mem[i].ismove = 0;
							}
							break;
						}
					}
				}
				if (output[mem[j].outx][mem[j].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[j].posx = cirx - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx + MEM_SIZE) > (mem[j].outy + 1)*BLOCK_SIZE) {
						if (mem[j].outy == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[j].outx][mem[j].outy + 1] == 1)
							|| (mem[j].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[j].outx + 1) && output[mem[j].outx + 1][mem[j].outy + 1] == 1)
							|| (mem[j].outx > 0 && (nexty - MEM_SIZE) < BLOCK_SIZE*mem[j].outx&&output[mem[j].outx - 1][mem[j].outy + 1] == 1)
							) {//撞墙
							mem[j].posx = (mem[j].outy + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else {//可以进去
							mem[j].outy++;
						}
					}
				}
				if (flag == 0) { mem[j].posx += 1; }break;
			case DIR_RIGHT:
				nextx = mem[j].posx - 1;
				nexty = mem[j].posy;
				for (i = 0;i < 4;i++) {
					if (i != j&&mem[i].ismove==1) {
						click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
						click = sqrt(click);
						if (click <= (2 * MEM_SIZE)) {
							newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
							newpath = sqrt(newpath);
							mem[j].posx = mem[i].posx + newpath;
							flag = 1;
							if (i == 0)
								mem[0].ismove = 0;
							else {
								if (rand() % 2 == 0)mem[j].ismove = 0;
								else mem[i].ismove = 0;
							}
							break;
						}
					}
				}
				if (output[mem[j].outx][mem[j].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[j].posx = cirx + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx - MEM_SIZE) < mem[j].outy*BLOCK_SIZE) {
						if (mem[j].outy == 0) { flag = 1; }//不能出去
						else if ((output[mem[j].outx][mem[j].outy - 1] == 1)
							|| (mem[j].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[j].outx + 1) && output[mem[j].outx + 1][mem[j].outy - 1] == 1)
							|| (mem[j].outx > 0 && (nexty - MEM_SIZE) < BLOCK_SIZE*mem[j].outx&&output[mem[j].outx - 1][mem[j].outy - 1] == 1)
							) {//撞墙
							mem[j].posx = mem[j].outy *BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else {//可以进去
							mem[j].outy--;
						}
					}
				}
				if (flag == 0) { mem[j].posx -= 1; }break;
			default:
				break;
			}

		}
	}
	glutPostRedisplay();
	glutTimerFunc(100, timer, 1);
}
