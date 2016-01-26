#include <GL/glut.h>
#include <math.h>
#include "Keyboard.h"
#define BLOCK_SIZE 11
#define LINE 9
#define OBS_SIZE 1.5
#define MEM_SIZE 1.5
//方向
#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3
extern int chose,sky;
extern GLfloat  light1_att;
extern bool light1_open;
extern int output[LINE][LINE];//迷宫矩阵
extern int winlose;
int path = 1;
struct member
{
	int outx;//矩阵位置
	int outy;
	GLfloat posx;//场景位置
	GLfloat posy;
	int ismove;
	int dir;//方向(来的方向)
};
extern struct member mem[4];
extern GLfloat eye[3],forward[3],flo[3];
int mode=0;
void Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'e':chose += 1;chose = chose % 5;break;//纹理切换
	case 'w':sky = (sky + 1) % 2;if(sky==1){mem[0].dir=DIR_UP;
forward[0] = 0;forward[1] = 0;forward[2] = -1;flo[0] = 0;flo[2] = 40;}break;
	case 'z':if (light1_att > 0.0)
		light1_att -= 0.1;break;
	case 'x':if (light1_att < 1.0)
		light1_att += 0.1;break;
	case 'c':if (light1_open)
		glDisable(GL_LIGHT1);
			 else
				 glEnable(GL_LIGHT1);
		light1_open = !light1_open;
		break;
	case 'q':exit(0);break;
	default:
		break;
	}
}
void ProcessSpecialKeyboard(int key, int x, int y) {
	double nextx, nexty,cirx,ciry;
	double dis,newpath,click;
	int flag,i;
	flag = 0;
	cirx = (mem[0].outy + 0.5)*BLOCK_SIZE;
	ciry = (mem[0].outx + 0.5)*BLOCK_SIZE;
	switch (key)
	{
		//小球移动
	case GLUT_KEY_END:mode += 1;mode = mode % 2;break;
	case GLUT_KEY_UP:
		if (mode == 0) {
			switch (mem[0].dir)
			{
				//1.该格中是否有圆柱
				//2.前进后是否进入另一格，是的话另一格是否为墙
			case DIR_UP:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy - path;
				//撞球
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4*MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy + newpath;
						flag = 1;
						mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)){//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty-MEM_SIZE) < mem[0].outx*BLOCK_SIZE) {
						if (mem[0].outx == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx - 1][mem[0].outy] == 1)
							||(mem[0].outy<(LINE-1)&&(nextx+MEM_SIZE)>BLOCK_SIZE*(mem[0].outy+1)&& output[mem[0].outx - 1][mem[0].outy+1] == 1)
							||(mem[0].outy>0&& (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx - 1][mem[0].outy - 1]==1)
							) {//撞墙
							mem[0].posy = mem[0].outx*BLOCK_SIZE+MEM_SIZE;
							flag = 1;
						}
						else {//可以进去
							mem[0].outx--;
						}
					}
				}
				}
				if(flag==0){ mem[0].posy -= path; }
				eye[2] = mem[0].posy;break;
			case DIR_DOWN:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy + path;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy - newpath;
						flag = 1;
						mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty + MEM_SIZE) > (mem[0].outx+1)*BLOCK_SIZE) {
						if (mem[0].outx == (LINE-1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx + 1][mem[0].outy] == 1)
							|| (mem[0].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[0].outy + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outy>0 && (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx + 1][mem[0].outy - 1] == 1)
						){//撞墙
							mem[0].posy = (mem[0].outx+1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else {//可以进去
							mem[0].outx++;
						}
					}
				}}
				if (flag == 0) { mem[0].posy += path; }
				eye[2] = mem[0].posy;break;
			case DIR_LEFT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx+path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx - newpath;
						flag = 1;
						mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx + MEM_SIZE) > (mem[0].outy + 1)*BLOCK_SIZE) {
						if (mem[0].outy == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy + 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy + 1] == 1)
							) {//撞墙
							mem[0].posx = (mem[0].outy + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy++;
						}
					}
				}}
				if (flag == 0) { mem[0].posx += path; }
				eye[0] = mem[0].posx;break;
			case DIR_RIGHT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx - path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx + newpath;
						flag = 1;
						mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx - MEM_SIZE) < mem[0].outy*BLOCK_SIZE) {
						if (mem[0].outy == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy - 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy - 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy - 1] == 1)
						){//撞墙
							mem[0].posx = mem[0].outy *BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy--;
						}
					}
				}}
				if (flag == 0) { mem[0].posx -= path; }
				eye[0] = mem[0].posx;break;
			default:
				break;
			}
		}
		else {
			path++;
			path = path % 5;//最高速度
		}break;
	case GLUT_KEY_DOWN:
		if (mode == 0) {
			switch (mem[0].dir)
			{
			case DIR_UP:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy + path;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy - newpath;
						flag = 1;
						mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty + MEM_SIZE) > (mem[0].outx + 1)*BLOCK_SIZE) {
						if (mem[0].outx == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx + 1][mem[0].outy] == 1)
							|| (mem[0].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[0].outy + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outy>0 && (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx + 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posy = (mem[0].outx + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outx++;
						}
					}
				}}
				if (flag == 0) { mem[0].posy += path; }
				eye[2] = mem[0].posy;break;
			case DIR_DOWN:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy - path;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy + newpath;
						flag = 1;
						mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty - MEM_SIZE) < mem[0].outx*BLOCK_SIZE) {
						if (mem[0].outx == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx - 1][mem[0].outy] == 1)
							|| (mem[0].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[0].outy + 1) && output[mem[0].outx - 1][mem[0].outy + 1] == 1)
							|| (mem[0].outy>0 && (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx - 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posy = mem[0].outx*BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outx--;
						}
					}
				}}
				if (flag == 0) { mem[0].posy -= path; }
				eye[2] = mem[0].posy;break;
			case DIR_LEFT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx - path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx + newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx - MEM_SIZE) < mem[0].outy*BLOCK_SIZE) {
						if (mem[0].outy == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy + 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy + 1] == 1)
							) {//撞墙
							mem[0].posx = mem[0].outy *BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy--;
						}
					}
				}}
				if (flag == 0) { mem[0].posx -= path; }
				eye[0] = mem[0].posx;break;
			case DIR_RIGHT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx + path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx - newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty- ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx + MEM_SIZE) > (mem[0].outy + 1)*BLOCK_SIZE) {
						if (mem[0].outy == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy - 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy - 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posx = (mem[0].outy + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy++;
						}
					}
				}}
				if (flag == 0) { mem[0].posx += path; }
				eye[0] = mem[0].posx;break;
			default:
				break;
			}
		}
		else {
			if (path > 1) {
				path--;
			}
		}break;
	case GLUT_KEY_LEFT:
		if (mode == 0) {
			switch (mem[0].dir)
			{
			case DIR_LEFT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy - path;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy + newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty - MEM_SIZE) < mem[0].outx*BLOCK_SIZE) {
						if (mem[0].outx == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx - 1][mem[0].outy] == 1)
							|| (mem[0].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[0].outy + 1) && output[mem[0].outx - 1][mem[0].outy + 1] == 1)
							|| (mem[0].outy>0 && (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx - 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posy = mem[0].outx*BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outx--;
						}
					}
				}}
				if (flag == 0) { mem[0].posy -= path; }
				eye[2] = mem[0].posy;break;
			case DIR_RIGHT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy + path;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy - newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty + MEM_SIZE) > (mem[0].outx + 1)*BLOCK_SIZE) {
						if (mem[0].outx == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx + 1][mem[0].outy] == 1)
							|| (mem[0].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[0].outy + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outy>0 && (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx + 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posy = (mem[0].outx + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outx++;
						}
					}
				}}
				if (flag == 0) { mem[0].posy += path; }
				eye[2] = mem[0].posy;break;
			case DIR_DOWN:
				if(mem[0].ismove==1){
				nextx = mem[0].posx + path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx - newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx + MEM_SIZE) > (mem[0].outy + 1)*BLOCK_SIZE) {
						if (mem[0].outy == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy + 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy + 1] == 1)
							){//撞墙
							mem[0].posx = (mem[0].outy + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy++;
						}
					}
				}}
				if (flag == 0) { mem[0].posx += path; }
				eye[0] = mem[0].posx;break;
			case DIR_UP:
				if(mem[0].ismove==1){
				nextx = mem[0].posx - path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx + newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx - MEM_SIZE) < mem[0].outy*BLOCK_SIZE) {
						if (mem[0].outy == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy - 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy - 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posx = mem[0].outy *BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy--;
						}
					}
				}}
				if (flag == 0) { mem[0].posx -= path; }
				eye[0] = mem[0].posx;break;
			default:
				break;
			}
		}
		else if(sky==0){
			switch (mem[0].dir)
			{
			case DIR_RIGHT:mem[0].dir = DIR_DOWN;forward[0] = 0;forward[1] = 0;forward[2] = 1;
				flo[0] = 0;flo[2] = -40;break;
			case DIR_LEFT:mem[0].dir = DIR_UP;forward[0] = 0;forward[1] = 0;forward[2] = -1;
				flo[0] = 0;flo[2] = 40;break;
			case DIR_UP:mem[0].dir = DIR_RIGHT;forward[0] = -1;forward[1] = 0;forward[2] = 0;
				flo[0] = 40;flo[2] = 0;break;
			case DIR_DOWN:mem[0].dir = DIR_LEFT;forward[0] = 1;forward[1] = 0;forward[2] = 0;
				flo[0] = -40;flo[2] = 0;break;
			default:
				break;
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (mode == 0) {
			switch (mem[0].dir)
			{
			case DIR_RIGHT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy - path;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy + newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty - MEM_SIZE) < mem[0].outx*BLOCK_SIZE) {
						if (mem[0].outx == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx - 1][mem[0].outy] == 1)
							|| (mem[0].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[0].outy + 1) && output[mem[0].outx - 1][mem[0].outy + 1] == 1)
							|| (mem[0].outy>0 && (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx - 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posy = mem[0].outx*BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outx--;
						}
					}
				}}
				if (flag == 0) { mem[0].posy -= path; }
				eye[2] = mem[0].posy;break;
			case DIR_LEFT:
				if(mem[0].ismove==1){
				nextx = mem[0].posx;
				nexty = mem[0].posy + path;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nextx - mem[i].posx)*(nextx - mem[i].posx);
						newpath = sqrt(newpath);
						mem[0].posy = mem[i].posy - newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[0].posy = ciry - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nexty + MEM_SIZE) > (mem[0].outx + 1)*BLOCK_SIZE) {
						if (mem[0].outx == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx + 1][mem[0].outy] == 1)
							|| (mem[0].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[0].outy + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outy>0 && (nextx - MEM_SIZE)<BLOCK_SIZE*mem[0].outy&&output[mem[0].outx + 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posy = (mem[0].outx + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outx++;
						}
					}
				}}
				if (flag == 0) { mem[0].posy += path; }
				eye[2] = mem[0].posy;break;
			case DIR_UP:
				if(mem[0].ismove==1){
				nextx = mem[0].posx + path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx - newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx - newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx + MEM_SIZE) > (mem[0].outy + 1)*BLOCK_SIZE) {
						if (mem[0].outy == (LINE - 1)) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy + 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy + 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy + 1] == 1)
							) {//撞墙
							mem[0].posx = (mem[0].outy + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy++;
						}
					}
				}}
				if (flag == 0) { mem[0].posx += path; }
				eye[0] = mem[0].posx;break;
			case DIR_DOWN:
				if(mem[0].ismove==1){
				nextx = mem[0].posx - path;
				nexty = mem[0].posy;
				for (i = 1;i < 4;i++) {
					click = (nextx - mem[i].posx)*(nextx - mem[i].posx) + (nexty - mem[i].posy)*(nexty - mem[i].posy);
					click = sqrt(click);
					if (mem[i].ismove==1&&click <= (2 * MEM_SIZE)) {
						newpath = 4 * MEM_SIZE*MEM_SIZE - (nexty - mem[i].posy)*(nexty - mem[i].posy);
						newpath = sqrt(newpath);
						mem[0].posx = mem[i].posx + newpath;
						flag = 1;mem[0].ismove = 0;
						break;
					}
				}
				if (output[mem[0].outx][mem[0].outy] == 2) {
					//该格内存在圆柱
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//撞圆柱
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[0].posx = cirx + newpath;
					}
				}
				if (flag == 0) {
					//是否进入下一格
					if ((nextx - MEM_SIZE) < mem[0].outy*BLOCK_SIZE) {
						if (mem[0].outy == 0) { flag = 1; }//不能出去
						else if ((output[mem[0].outx][mem[0].outy - 1] == 1)
							|| (mem[0].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[0].outx + 1) && output[mem[0].outx + 1][mem[0].outy - 1] == 1)
							|| (mem[0].outx>0 && (nexty - MEM_SIZE)<BLOCK_SIZE*mem[0].outx&&output[mem[0].outx - 1][mem[0].outy - 1] == 1)
							) {//撞墙
							mem[0].posx = mem[0].outy *BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else{//可以进去
							mem[0].outy--;
						}
					}
				}}
				if (flag == 0) { mem[0].posx -= path; }
				eye[0] = mem[0].posx;break;
			default:
				break;
			}
		}
		else if(sky==0){
			switch (mem[0].dir)
			{
			case DIR_LEFT:mem[0].dir = DIR_DOWN;forward[0] = 0;forward[1] = 0;forward[2] = 1;
				flo[0] = 0;flo[2] = -40;break;
			case DIR_RIGHT:mem[0].dir = DIR_UP;forward[0] = 0;forward[1] = 0;forward[2] = -1;
				flo[0] = 0;flo[2] = 40;break;
			case DIR_DOWN:mem[0].dir = DIR_RIGHT;forward[0] = -1;forward[1] = 0;forward[2] = 0;
				flo[0] = 40;flo[2] = 0;break;
			case DIR_UP:mem[0].dir = DIR_LEFT;forward[0] = 1;forward[1] = 0;forward[2] = 0;
				flo[0] = -40;flo[2] = 0;break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}

}
