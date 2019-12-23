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

//鼠标交互有关的
int mousetate = 0; //鼠标当前的状态
GLfloat Oldx = 0.0; // 点击之前的位置
GLfloat Oldy = 0.0;
//与实现角度大小相关的参数，只需要两个就可以完成
float xRotate = 0.0f;
float yRotate = 0.0f;
float tx = 0.0f;
float ty = 0.0f;
float tz = 0.0f;
float scale = 0.2;

vector<double*> vertix;
vector<vector<int>> face;
vector<double*> normal;//面法向量
int vertexNum, faceNum, edgeNum;

GLuint texture[1];    // 存储一个纹理 


void readText(string filename) {
	string t = filename.substr(filename.length() - 3, 3);
	if (!t.compare("off")) {//判断是否为off文件
		string str;
		ifstream file(filename);//读文件
		if (!file.is_open())
			cout << "文件打开失败" << endl;
		else
			file >> str;

		file >> vertexNum;//点数
		file >> faceNum;//面数
		file >> edgeNum;//边数

		for (int i = 0; i < vertexNum; i++)
		{//读入点坐标
			double *node = new double[3];
			file >> node[0];
			file >> node[1];
			file >> node[2];
			vertix.push_back(node);//push_back在vector类中作用为在vector尾部加入一个数据
		}
		int Num;
		for (int i = 0; i < faceNum; i++)
		{
			file >> Num;//每一个面对应的点数
			vector<int> point;
			int temp;
			for (int j = 0; j < Num; j++)
			{
				file >> temp;
				point.push_back(temp);
			}
			face.push_back(point);
			//求每个面的法向量
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

AUX_RGBImageRec *LoadBMP(char *Filename)//加载bmp图像，返回该图像的指针
{
	FILE *File = NULL;
	if (!Filename) {
		return NULL;
	}

	File = fopen(Filename, "r");//尝试打开文件
	if (File) {
		fclose(File);
		return auxDIBImageLoad(Filename);//返回指针
	}
	return NULL;//载入失败
}

int LoadGLTextures()  //根据加载的位图创建纹理
{
	int Status = FALSE;  //指示纹理创建是否成功的标志
	AUX_RGBImageRec *TextureImage[1]; //创建一个纹理图像数组
	memset(TextureImage, 0, sizeof(void *) * 1); //初始化纹理图像数组，为其分配内存

	if (TextureImage[0] = LoadBMP("star.bmp"))
	{
		Status = TRUE;
		glGenTextures(1, &texture[0]); //为第0个位图创建纹理
		glBindTexture(GL_TEXTURE_2D, texture[0]);//将生成的纹理的名称绑定到对应的目标纹理上

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);  //生成纹理
	}

	if (TextureImage[0]) {// 释放位图数组占用的内存空间
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
	glRotatef(xRotate, 1.0f, 0.0f, 0.0f); // 让物体旋转的函数 第一个参数是角度大小，后面的参数是旋转的法向量
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	glTranslatef(tx, ty, tz);//平移
	glScalef(scale, scale, scale); // 缩放 参数x,y,z分别为模型在x,y,z轴方向的缩放比

	glBindTexture(GL_TEXTURE_2D, texture[0]);//选择纹理

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


// 窗口改变回调函数
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

	glEnable(GL_LIGHT0);//打开光源0
	glEnable(GL_LIGHTING);//打开光照

	//深度测试
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	if (!LoadGLTextures()) {//调用纹理载入子例程
		return FALSE;
	}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);//启用纹理映射
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

// 鼠标交互
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

// 鼠标运动时
void onMouseMove(int x, int y) {
	if (mousetate) {
		//x对应y是因为对应的是法向量
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
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // GLUT_Double 表示使用双缓存而非单缓存
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 500);
	glutCreateWindow("三维图形");
	readText("helix2.off");
	//readText("snub_cube.off");
	init();
	glutMouseFunc(myMouse);//鼠标按钮回调函数
	glutMotionFunc(onMouseMove); // 鼠标移动回调函数
	glutSpecialFunc(&mySpecial);
	glutReshapeFunc(&myReshape);//窗口改变回调函数
	glutDisplayFunc(&myDisplay);//显示回调函数

	glutMainLoop();//进入事件处理循环
	return 0;
}