#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "glm.h"
#include "Obj.h"
extern GLuint win, lose,start,end;
void loadOBJ() {
	char s1[]="winer.obj";
	char s2[]="loser.obj";
	char s3[]="start.obj";
	char s4[]="end.obj";
	win = drawOBJ(s1);
	lose = drawOBJ(s2);
	start = drawOBJ(s3);
	end = drawOBJ(s4);
}
GLuint drawOBJ(char * filename) {
	GLMmodel *glm_model;
	GLuint list;
	glm_model = glmReadOBJ(filename);
	glmUnitize(glm_model);
	glmScale(glm_model, 6);
	glmFacetNormals(glm_model);
	glmVertexNormals(glm_model, 90);
	list = glmList(glm_model, GLM_SMOOTH | GLM_MATERIAL);

	glmDelete(glm_model);
	return list;
}
