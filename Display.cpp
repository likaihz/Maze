#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include "Display.h"
#define FLOORR 2000
#define BLOCK_SIZE 11
#define LINE 9
#define OBS_SIZE 1.5
#define MEM_SIZE 1.5
const GLfloat Pi = 3.1415926536f;
extern GLuint ground, wall[5];//纹理
extern int output[LINE][LINE];//迷宫矩阵
extern int endx, endy,stpox,stpoy,enddir;
extern int chose,sky,winlose;
extern GLfloat eye[3], forward[3],up[3],flo[3],Eye[3],Forward[3];
extern GLfloat light1_att;
extern bool light1_open;
extern GLfloat light1_position[4], point_light1[4];
extern GLuint win, lose,start,end;
GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
GLfloat mat_diffuse[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_diffusex[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat no_shininess[] = { 0.0 };
GLfloat low_shininess[] = { 5.0 };
GLfloat high_shininess[] = { 100.0 };
GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};
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
void Display() {
	int i, j,g;
	GLfloat pocube[8][3] = {
		0,0,0,
		BLOCK_SIZE,0,0,
		0,BLOCK_SIZE,0,
		BLOCK_SIZE,BLOCK_SIZE,0,
		0,0,BLOCK_SIZE,
		BLOCK_SIZE,0,BLOCK_SIZE,
		0,BLOCK_SIZE,BLOCK_SIZE,
		BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE
	};
 static const	GLint list[6][4] = {
	 0, 2, 3, 1,
	 0, 4, 6, 2,
	 0, 1, 5, 4,
	 4, 5, 7, 6,
	 1, 3, 7, 5,
	 2, 6, 7, 3,
	};
	//initial
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空颜色缓冲,深度缓冲
	glShadeModel(GL_SMOOTH);
	glLoadIdentity(); //除去之前矩阵的影响
	if (sky == 0) {
		gluLookAt(eye[0] + flo[0], eye[1] + flo[1], eye[2] + flo[2], eye[0] + forward[0], eye[1] + forward[1], eye[2] + forward[2], 0, 1, 0);
	}
	else {
		gluLookAt(Eye[0], Eye[1], Eye[2], Eye[0] + Forward[0], Eye[1] + Forward[1], Eye[2] + Forward[2], up[0], up[1], up[2]);
	}
	//设置光源1
	glPushMatrix();
	light1_position[0]=eye[0]+flo[0];
	light1_position[1]=eye[0]+flo[1];
	light1_position[2]=eye[0]+flo[2];
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, point_light1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, point_light1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, point_light1);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.1 + light1_att * 2);  //衰减设置
	glPopMatrix();
	//绘制地板
	GLfloat gropo[4][3] = {-FLOORR,0,FLOORR,FLOORR,0,FLOORR,FLOORR,0,-FLOORR,-FLOORR,0,-FLOORR};
	drawPolygon(gropo[0], gropo[1], gropo[2], gropo[3],ground);
	//end1
	//迷宫绘制
	for (i = 0;i < LINE;i++) {
		glPushMatrix();
		glTranslated(0,0,BLOCK_SIZE*i);
		for (j = 0;j < LINE;j++) {
			if (output[i][j] == 1) {
				glPushMatrix();
				glTranslated(BLOCK_SIZE*j,0,0);
				for (g = 0;g < 6;g++) {
					drawPolygon(pocube[list[g][0]], pocube[list[g][1]], pocube[list[g][2]], pocube[list[g][3]], wall[chose]);
				}
				glPopMatrix();
			}
			else if (output[i][j] == 2) {
				glPushMatrix();
				glTranslated(BLOCK_SIZE*j, 0, 0);
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffusex);
				glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //镜面反射颜色（白）
				glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);  //镜面反射指数
				glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); //发射光
				drawCylin();
				glPopMatrix();
			}
		}
		glPopMatrix();
	}
	//end2
	//主人公和球们
	for (i = 0;i < 4;i++) {
		if (mem[i].ismove == 1) {
			glPushMatrix();
			glTranslated(mem[i].posx, BLOCK_SIZE / 2, mem[i].posy);
			switch (i)
			{
			case 0:glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);break;
			case 1:mat_diffuse[0]=1.0;glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);mat_diffuse[0]=0.0;break;
			case 2:mat_diffuse[1]=1.0;glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);mat_diffuse[1]=0.0;break;
			case 3:mat_diffuse[2]=1.0;glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);mat_diffuse[2]=0.0;break;
			default:
				break;
			}
			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //镜面反射颜色（白）
			glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);  //镜面反射指数
			glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); //发射光
			glutSolidSphere(MEM_SIZE, 100, 100);
			glPopMatrix();
		}
	}
	//glColor3f(1.0, 1.0, 1.0);
	//end3
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffusex);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //镜面反射颜色（白）
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);  //镜面反射指数
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); //发射光			
		glTranslated(stpox,15,stpoy);
		glRotated(45,1,0,0);
		glCallList(start);
		glPopMatrix();
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffusex);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //镜面反射颜色（白）
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);  //镜面反射指数
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); //发射光			
		glTranslated(BLOCK_SIZE*(endy+0.5),15,BLOCK_SIZE*(endx+0.5));
		if(enddir==1)
			glRotated(-90,0,1,0);
		else if(enddir==3)
			glRotated(90,0,1,0);
		glRotated(45,1,0,0);
		glCallList(end);
		glPopMatrix();
	if (winlose == 1) {
		//win
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffusex);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //镜面反射颜色（白）
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);  //镜面反射指数
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); //发射光			
		glTranslated(mem[0].posx,20,mem[0].posy);
		if(mem[0].dir==1)glRotated(90,0,1,0);
		else if(mem[0].dir==3)glRotated(-90,0,1,0);
		else if(mem[0].dir==2)glRotated(-180,0,1,0);
		glRotated(45,1,0,0);
		glCallList(win);
		glPopMatrix();
	}
	else if(winlose==2){
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffusex);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //镜面反射颜色（白）
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);  //镜面反射指数
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat); //发射光			
		glTranslated(mem[0].posx,20,mem[0].posy);
		if(mem[0].dir==1)glRotated(90,0,1,0);
		else if(mem[0].dir==3)glRotated(-90,0,1,0);
		else if(mem[0].dir==2)glRotated(-180,0,1,0);
		glRotated(45,1,0,0);
		glCallList(lose);
		glPopMatrix();
	}
	glutSwapBuffers();

}
//面绘制
void drawPolygon(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat d[3],GLuint textu)//根据四个点画一个面
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textu);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 1.0f);//左上
	glVertex3fv(a);
	glTexCoord2f(1.0f, 1.0f);//右上
	glVertex3fv(b);
	glTexCoord2f(1.0f, 0.0f);//右下
	glVertex3fv(c);
	glTexCoord2f(0.0f, 0.0f);//左下
	glVertex3fv(d);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void drawCylin() {
	int count;
	GLUquadricObj *newcy = gluNewQuadric();
	glPushMatrix();
	glTranslated(BLOCK_SIZE / 2, 0, BLOCK_SIZE / 2);
	glRotated(-90, 1, 0, 0);
	gluCylinder(newcy, OBS_SIZE, OBS_SIZE, BLOCK_SIZE, 100,100);
	glPopMatrix();
	//顶面
	glBegin(GL_TRIANGLE_FAN);
	for (count = 0;count < 200;count++) {
		glVertex3f(BLOCK_SIZE/2+OBS_SIZE*cosf(count*Pi/100), BLOCK_SIZE, BLOCK_SIZE/2-OBS_SIZE*sinf(count*Pi / 100));
	}
	glEnd();
}
