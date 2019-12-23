#include <GL/glut.h>
#include<GL/GLAUX.H>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h> 
#include <vector>
#include <stdlib.h>
#include<cstdlib>
#include<ctime>
using namespace std;

//��꽻���йص�
int mousetate = 0; //��굱ǰ��״̬
GLfloat Oldx = 0.0; // ���֮ǰ��λ��
GLfloat Oldy = 0.0;
//��ʵ�ֽǶȴ�С��صĲ�����ֻ��Ҫ�����Ϳ������
float xRotate = 0.0f;
float yRotate = 0.0f;
float tx = 0.0f;
float ty = 0.0f;
float tz = 0.0f;
float scale = 0.2;

vector<double*> vertix;
vector<vector<int>> face;
vector<double*> normal;//�淨����
int vertexNum, faceNum, edgeNum;

GLuint texture[1];    // �洢һ������ 


void readText(string filename) {
	string t = filename.substr(filename.length() - 3, 3);
	if (!t.compare("off")) {//�ж��Ƿ�Ϊoff�ļ�
		string str;
		ifstream file(filename);//���ļ�
		if (!file.is_open())
			cout << "�ļ���ʧ��" << endl;
		else
			file >> str;

		file >> vertexNum;//����
		file >> faceNum;//����
		file >> edgeNum;//����

		for (int i = 0; i < vertexNum; i++)
		{//���������
			double *node = new double[3];
			file >> node[0];
			file >> node[1];
			file >> node[2];
			vertix.push_back(node);//push_back��vector��������Ϊ��vectorβ������һ������
		}
		int Num;
		for (int i = 0; i < faceNum; i++)
		{
			file >> Num;//ÿһ�����Ӧ�ĵ���
			vector<int> point;
			int temp;
			for (int j = 0; j < Num; j++)
			{
				file >> temp;
				point.push_back(temp);
			}
			face.push_back(point);
			//��ÿ����ķ�����
			double u1 = vertix[point[0]][0] - vertix[point[1]][0];
			double u2 = vertix[point[0]][1] - vertix[point[1]][1];
			double u3 = vertix[point[0]][2] - vertix[point[1]][2];
			double v1 = vertix[point[0]][0] - vertix[point[2]][0];
			double v2 = vertix[point[0]][1] - vertix[point[2]][1];
			double v3 = vertix[point[0]][2] - vertix[point[2]][2];
			double c1 = u2*v3 - v2*u3;
			double c2 = u3*v1 - v3*u1;
			double c3 = u1*v2 - v1*u2;
			double c = sqrt(c1*c1 + c2*c2 + c3*c3);
			double *nor = new double[3];
			nor[0] = c1 / c;
			nor[1] = c2 / c;
			nor[2] = c3 / c;
			normal.push_back(nor);
		}
	}
}

AUX_RGBImageRec *LoadBMP(char *Filename)//����bmpͼ�񣬷��ظ�ͼ���ָ��
{
	FILE *File = NULL;
	if (!Filename) {
		return NULL;
	}

	File = fopen(Filename, "r");//���Դ��ļ�
	if (File) {
		fclose(File);
		return auxDIBImageLoad(Filename);//����ָ��
	}
	return NULL;//����ʧ��
}

int LoadGLTextures()  //���ݼ��ص�λͼ��������
{
	int Status = FALSE;  //ָʾ�������Ƿ�ɹ��ı�־
	AUX_RGBImageRec *TextureImage[1]; //����һ������ͼ������
	memset(TextureImage, 0, sizeof(void *) * 1); //��ʼ������ͼ�����飬Ϊ������ڴ�

	if (TextureImage[0] = LoadBMP("star.bmp"))
	{
		Status = TRUE;
		glGenTextures(1, &texture[0]); //Ϊ��0��λͼ��������
		glBindTexture(GL_TEXTURE_2D, texture[0]);//�����ɵ���������ư󶨵���Ӧ��Ŀ��������

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);  //��������
	}

	if (TextureImage[0]) {// �ͷ�λͼ����ռ�õ��ڴ�ռ�
		if (TextureImage[0]->data) {
			free(TextureImage[0]->data);
		}
		free(TextureImage[0]);
	}
	return Status;
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//glPushMatrix();
	glRotatef(xRotate, 1.0f, 0.0f, 0.0f); // ��������ת�ĺ��� ��һ�������ǽǶȴ�С������Ĳ�������ת�ķ�����
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	glTranslatef(tx, ty, tz);//ƽ��
	glScalef(scale, scale, scale); // ���� ����x,y,z�ֱ�Ϊģ����x,y,z�᷽������ű�

	glBindTexture(GL_TEXTURE_2D, texture[0]);//ѡ������

	for (int i = 0; i <faceNum; i++) {
		glBegin(GL_POLYGON);
		glNormal3f(normal[i][0], normal[i][1], normal[i][2]);
		if (face[i].size() == 4) {
			glTexCoord2f(0.0, 0.0);
			glVertex3f(vertix[face[i][0]][0], vertix[face[i][0]][1], vertix[face[i][0]][2]);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(vertix[face[i][1]][0], vertix[face[i][1]][1], vertix[face[i][1]][2]);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(vertix[face[i][2]][0], vertix[face[i][2]][1], vertix[face[i][2]][2]);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(vertix[face[i][3]][0], vertix[face[i][3]][1], vertix[face[i][3]][2]);
		}
		else if (face[i].size() == 3) {
			glTexCoord2f(0.0, 0.0);
			glVertex3f(vertix[face[i][0]][0], vertix[face[i][0]][1], vertix[face[i][0]][2]);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(vertix[face[i][1]][0], vertix[face[i][1]][1], vertix[face[i][1]][2]);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(vertix[face[i][2]][0], vertix[face[i][2]][1], vertix[face[i][2]][2]);
		}
		else {
			cout << "error!" << endl;
		}
		glEnd();
	}

	glutSwapBuffers();
	glFlush();
}


// ���ڸı�ص�����
void myReshape(GLint w, GLint h)
{
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h)
		glOrtho(-static_cast<GLdouble>(w) / h, static_cast<GLdouble>(w) / h, -1.0, 1.0, -100.0, 100.0);
	else
		glOrtho(-1.0, 1.0, -static_cast<GLdouble>(h) / w, static_cast<GLdouble>(h) / w, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHT0);//�򿪹�Դ0
	glEnable(GL_LIGHTING);//�򿪹���

	//��Ȳ���
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	if (!LoadGLTextures()) {//������������������
		return FALSE;
	}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);//��������ӳ��
	return TRUE;

}

void mySpecial(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		tx += 0.02;
		break;
	case GLUT_KEY_LEFT:
		tx -= 0.02;
		break;
	case GLUT_KEY_UP:
		ty += 0.02;
		break;
	case GLUT_KEY_DOWN:
		ty -= 0.02;
		break;
	case GLUT_KEY_PAGE_UP:
		tz += 0.02;
		break;
	case GLUT_KEY_PAGE_DOWN:
		tz -= 0.02;
		break;
	case GLUT_KEY_F1:
		scale += 0.1;
		break;
	case GLUT_KEY_F2:
		scale -= 0.1;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

// ��꽻��
void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mousetate = 1;
		Oldx = x;
		Oldy = y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		mousetate = 0;
	glutPostRedisplay();
}

// ����˶�ʱ
void onMouseMove(int x, int y) {
	if (mousetate) {
		//x��Ӧy����Ϊ��Ӧ���Ƿ�����
		yRotate += x - Oldx;
		glutPostRedisplay();
		Oldx = x;
		xRotate += y - Oldy;
		glutPostRedisplay();
		Oldy = y;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // GLUT_Double ��ʾʹ��˫������ǵ�����
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 500);
	glutCreateWindow("��άͼ��");
	readText("helix2.off");
	//readText("snub_cube.off");
	init();
	glutMouseFunc(myMouse);//��갴ť�ص�����
	glutMotionFunc(onMouseMove); // ����ƶ��ص�����
	glutSpecialFunc(&mySpecial);
	glutReshapeFunc(&myReshape);//���ڸı�ص�����
	glutDisplayFunc(&myDisplay);//��ʾ�ص�����

	glutMainLoop();//�����¼�����ѭ��
	return 0;
}