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
//����
#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define GLUT_WHEEL_UP 3      //���Ϲ������
#define GLUT_WHEEL_DOWN 4
#define PUSH_EMPTY 0         //��갴ס�ļ�
#define	PUSH_LEFT 1
#define PUSH_RIGHT 2
#define MOVE_PRECISION  100  //����ת��������
#define SCROLL_PRECISION 0.1 //���߹����ٶ�
//�������
GLuint wall[5],ground;//ǽ������͵�������
int chose;//����ѡ��
int output[LINE][LINE];//�Թ�����
int endx, endy,stpox,stpoy,enddir;
int winlose;//0normal,1win,2lose
GLuint win, lose,start,end;
struct poin
{
	int x;
	int y;
	int known;
}po[POINT_NUM];//��ṹ(�����Թ���)
struct member
{
	int outx;//����λ��
	int outy;
	GLfloat posx;//����λ��
	GLfloat posy;
	int ismove;
	int dir;//����(���ķ���)
}mem[4];
//����
GLfloat eye[3], forward[3],up[3],flo[3],right[3],Eye[3],Forward[3];
int sky;//����
int old[2];
int push_side;
GLfloat angle[2];
//light
GLfloat light1_att = 0.0;     //LIGHT1��˥��ϵ��
bool light1_open = true;    //LIGHT1�Ƿ���
GLfloat light1_position[] = { 0.0, 40.0, 0, 1.0 };       //LIGHT1��λ������
GLfloat point_light1[] = { 1.0, 1.0, 0.5, 0.5 };		  //LIGHT1�Ĺ��ղ���

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
	//��ʼ������
	glutInit(&argc, argv);//��ʼ��
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);//��ʾģʽ
	glutInitWindowPosition(200, 50);//����λ��
	glutInitWindowSize(700, 700);//���ڴ�С
	glutCreateWindow("Maze");//��������
	InitMaze();
	glEnable(GL_DEPTH_TEST);//��ȼ��
	//���ܺ���
	glutDisplayFunc(&Display);//����
	glutReshapeFunc(&Reshape);//reshape
	glutKeyboardFunc(&Keyboard);
	glutSpecialFunc(&ProcessSpecialKeyboard);
	glutMouseFunc(&MouseFunc);//�����Ӧx
	glutMotionFunc(onMouseMove);//����϶�x
	glutTimerFunc(100, timer, 1);//��ʱ�� ms,point,value,���ں������ٵ���
	glutMainLoop();//����glut�¼�����ѭ��
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
	glClearColor(0.0, 0.0, 0.0, 1.0);    //����ɫ
	glEnable(GL_DEPTH_TEST);             //��Ȳ���
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);              //������λ��
	glEnable(GL_COLOR_MATERIAL);         //������ɫ׷��
	glEnable(GL_LIGHTING);//�ƹ�
	glEnable(GL_LIGHT1);
	//�������
	ground = LoadTexture("ground.bmp");
	wall[0] = LoadTexture("wall1.bmp");
	wall[1] = LoadTexture("wall2.bmp");
	wall[2] = LoadTexture("wall3.bmp");
	wall[3] = LoadTexture("wall4.bmp");
	wall[4] = LoadTexture("wall5.bmp");
	loadOBJ();
	chose = 0;
	GetMaze();
	angle[0] = 1.57;       //��ʼ���߼н�
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
//�Թ�����
void GetMaze() {
	//��С���������Թ�
	int countpo, knownum, i, j, k, next;//�������,��֪�����,ѭ����,��ǽ��
	int Known[POINT_NUM];//�Ƿ���֪
	int viewpo, cou, wall[4];
	int pointli, posget,thisdir, endpos;//ÿ��������������ʱ���㣩
	pointli = (LINE-1) / 2;
	countpo = knownum = viewpo = cou = k = next = 0;
	//1.start:�Թ�����
	for (i = 0;i<POINT_NUM;i++) {
		Known[i] = -1;
	}
	srand((unsigned)time(NULL));
	//��ʼ����1����ǽ��0������ͨ�������ʼ��Ϊ0
	for (j = 0;j<LINE;j++) {
		for (i = 0;i<LINE;i++) {
			if (i % 2 == 1 && j % 2 == 1) {
				//�����ʼ��(����ǽ������),֮��Ҫ��ͨ��Щ����
				output[i][j] = 0;
				po[countpo].x = i;
				po[countpo].y = j;
				po[countpo].known = 0;
				countpo++;
			}
			else {
				output[i][j] = 1;//���������ǽ
			}
		}
	}
	//����
	Known[0] = rand() % POINT_NUM;//��ʼ��
	knownum = 1;//��֪����Ŀ��ȫ����֪ʱͣ�£�
	po[Known[0]].known = 1;//�����֪
	wall[0] = wall[1] = wall[2] = wall[3] = 0;//�ĸ������ǽ
	do {
		//��Ϊÿ����Ȩֵ����1���������֪��ѡ
		viewpo = rand() % knownum;//����֪�������ѡȡ
		k = Known[viewpo];//���ţ����Ͽ�ʼ������
						  //����ǽ
		cou = 0;
		//��ȡ��k���ܵ�ǽ��״̬��ǽ�����Ҵ�ͨǽ����Ǹ��㻹û����ͨ��
		if (k>(pointli - 1) && output[po[k].x][po[k].y - 1] == 1 && po[k - pointli].known == 0) { wall[cou] = -7;cou++; }//��
		if (k<(POINT_NUM - pointli) && output[po[k].x][po[k].y + 1] == 1 && po[k + pointli].known == 0) { wall[cou] = 7;cou++; }
		if (k%pointli != 0 && output[po[k].x - 1][po[k].y] == 1 && po[k - 1].known == 0) { wall[cou] = -1;cou++; }
		if (k%pointli != (pointli - 1) && output[po[k].x + 1][po[k].y] == 1 && po[k + 1].known == 0) { wall[cou] = 1;cou++; }
		//��ǽ
		if (cou != 0) {
			next = rand() % cou;//��һ��ͨ��
			switch (wall[next]) {
			case -7:output[po[k].x][po[k].y - 1] = 0;po[k - pointli].known = 1;Known[knownum] = k - pointli;knownum++;break;
			case 7:output[po[k].x][po[k].y + 1] = 0;po[k + pointli].known = 1;Known[knownum] = k + pointli;knownum++;break;
			case -1:output[po[k].x - 1][po[k].y] = 0;po[k - 1].known = 1;Known[knownum] = k - 1;knownum++;break;
			case 1:output[po[k].x + 1][po[k].y] = 0;po[k + 1].known = 1;Known[knownum] = k + 1;knownum++;break;
			default:break;
			}
		}
	} while (knownum != POINT_NUM);
	//1.end:�Թ��������
	//2.start:·�����ɣ��������·�ϣ�Բ����
	for (j = 2;j < LINE-2;j++) {
		for (i = 2;i < LINE-2;i++) {
			if (output[i][j] == 0 && (rand() % 4) == 0) {//1/4��������·��
				output[i][j] = 2;
			}
		}
	}
	//2.end:·��
	//3.start:�Թ������յ�(���������)
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
	//3.end:�Թ������յ�
	//4.start:���־�*3���
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
	//4.end:���־�*3���
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
			if (button == GLUT_WHEEL_UP) {       //����
				eye[0] += SCROLL_PRECISION * cos(angle[0])*cos(angle[1]);
				eye[1] += SCROLL_PRECISION * sin(angle[0])*cos(angle[1]);
				eye[2] += SCROLL_PRECISION * sin(angle[1]);
			}
			else if (button == GLUT_WHEEL_DOWN) { //��Զ
				eye[0] -= SCROLL_PRECISION * cos(angle[0])*cos(angle[1]);
				eye[1] -= SCROLL_PRECISION * sin(angle[0])*cos(angle[1]);
				eye[2] -= SCROLL_PRECISION * sin(angle[1]);
			}

		}
		glutPostRedisplay();
	}
}
//����ƶ�
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
			if (abs(dx) < 50) {      //�����ƶ�����
				angle[0] += (GLfloat)dx / MOVE_PRECISION;
			}

			if (abs(dy) < 50) {
				angle[1] += (GLfloat)dy / MOVE_PRECISION;
			}

			updateDirection();     //ת�����ߺ�Ҫ����������

			old[0] = x;            //����ԭ��λ��
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

	//���������ǰ�����õ�������
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
				//1.�ø����Ƿ���Բ��
				//2.ǰ�����Ƿ������һ���ǵĻ���һ���Ƿ�Ϊǽ
			case DIR_UP:
				nextx = mem[j].posx;
				nexty = mem[j].posy - 1;
				//ײ��
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
					//�ø��ڴ���Բ��
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//ײԲ��
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[j].posy = ciry + newpath;
					}
				}
				if (flag == 0) {
					//�Ƿ������һ��
					if ((nexty - MEM_SIZE) < mem[j].outx*BLOCK_SIZE) {
						if (mem[j].outx == 0) { flag = 1; }//���ܳ�ȥ
						else if ((output[mem[j].outx - 1][mem[j].outy] == 1)
							|| (mem[j].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[j].outy + 1) && output[mem[j].outx - 1][mem[j].outy + 1] == 1)
							|| (mem[j].outy > 0 && (nextx - MEM_SIZE) < BLOCK_SIZE*mem[j].outy&&output[mem[j].outx - 1][mem[j].outy - 1] == 1)
							) {//ײǽ
							mem[j].posy = mem[j].outx*BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else {//���Խ�ȥ
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
					//�ø��ڴ���Բ��
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//ײԲ��
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nextx - cirx)*(nextx - cirx);
						newpath = sqrt(newpath);
						mem[j].posy = ciry - newpath;
					}
				}
				if (flag == 0) {
					//�Ƿ������һ��
					if ((nexty + MEM_SIZE) > (mem[j].outx + 1)*BLOCK_SIZE) {
						if (mem[j].outx == (LINE - 1)) { flag = 1; }//���ܳ�ȥ
						else if ((output[mem[j].outx + 1][mem[j].outy] == 1)
							|| (mem[j].outy<(LINE - 1) && (nextx + MEM_SIZE)>BLOCK_SIZE*(mem[j].outy + 1) && output[mem[j].outx + 1][mem[j].outy + 1] == 1)
							|| (mem[j].outy > 0 && (nextx - MEM_SIZE) < BLOCK_SIZE*mem[j].outy&&output[mem[j].outx + 1][mem[j].outy - 1] == 1)
							) {//ײǽ
							mem[j].posy = (mem[j].outx + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else {//���Խ�ȥ
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
					//�ø��ڴ���Բ��
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//ײԲ��
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[j].posx = cirx - newpath;
					}
				}
				if (flag == 0) {
					//�Ƿ������һ��
					if ((nextx + MEM_SIZE) > (mem[j].outy + 1)*BLOCK_SIZE) {
						if (mem[j].outy == (LINE - 1)) { flag = 1; }//���ܳ�ȥ
						else if ((output[mem[j].outx][mem[j].outy + 1] == 1)
							|| (mem[j].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[j].outx + 1) && output[mem[j].outx + 1][mem[j].outy + 1] == 1)
							|| (mem[j].outx > 0 && (nexty - MEM_SIZE) < BLOCK_SIZE*mem[j].outx&&output[mem[j].outx - 1][mem[j].outy + 1] == 1)
							) {//ײǽ
							mem[j].posx = (mem[j].outy + 1)*BLOCK_SIZE - MEM_SIZE;
							flag = 1;
						}
						else {//���Խ�ȥ
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
					//�ø��ڴ���Բ��
					dis = (nextx - cirx)*(nextx - cirx) + (nexty - ciry)*(nexty - ciry);
					if (sqrt(dis) < (OBS_SIZE + MEM_SIZE)) {//ײԲ��
						flag = 1;
						newpath = (OBS_SIZE + MEM_SIZE)*(OBS_SIZE + MEM_SIZE) - (nexty - ciry)*(nexty - ciry);
						newpath = sqrt(newpath);
						mem[j].posx = cirx + newpath;
					}
				}
				if (flag == 0) {
					//�Ƿ������һ��
					if ((nextx - MEM_SIZE) < mem[j].outy*BLOCK_SIZE) {
						if (mem[j].outy == 0) { flag = 1; }//���ܳ�ȥ
						else if ((output[mem[j].outx][mem[j].outy - 1] == 1)
							|| (mem[j].outx<(LINE - 1) && (nexty + MEM_SIZE)>BLOCK_SIZE*(mem[j].outx + 1) && output[mem[j].outx + 1][mem[j].outy - 1] == 1)
							|| (mem[j].outx > 0 && (nexty - MEM_SIZE) < BLOCK_SIZE*mem[j].outx&&output[mem[j].outx - 1][mem[j].outy - 1] == 1)
							) {//ײǽ
							mem[j].posx = mem[j].outy *BLOCK_SIZE + MEM_SIZE;
							flag = 1;
						}
						else {//���Խ�ȥ
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
