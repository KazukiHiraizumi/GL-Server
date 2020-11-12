#include <GLFW/glfw3.h>
//#include <GL/glu.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
//#include <unistd.h>
#include <fcntl.h>
//#include <netdb.h>
//#include <netinet/in.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>
#include <queue>
#include <iostream>
#include <thread>
#include <mutex>

#define M_PI 3.14159265

#include "cparser.h"

static void error_callback(int error, const char* description){
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
//	if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) glfwSetWindowShouldClose(window,GL_TRUE);
}

//Global
int width,height;
float aspect;
float apixel;

std::queue<std::string> fifo;
std::queue<std::string> backup;

float px2gl(int px){
	return 2*(float)px/width-1;
}
float a2f(std::string s){
	return atof(s.c_str());
}
float fPOP(std::queue<std::string>& q) {
	std::string s = q.front();
	backup.push(s);
	q.pop();
	return a2f(s);
}
int cPOP(std::queue<std::string>& q) {
	std::string s = q.front();
	backup.push(s);
	q.pop();
	return s.c_str()[0];
}
bool bPOP(std::queue<std::string>& q) {
	std::string s = q.front();
	backup.push(s);
	q.pop();
	return strcmp(s.c_str(), "false");
}
void drawWav(std::queue<std::string>& cont){
	float lamda= fPOP(cont);
	float phase = fPOP(cont);
	float g0 = fPOP(cont);
	float g180 = fPOP(cont);
	int bin = bPOP(cont);
	for(int i=0;i<width;i++){
		float lx=px2gl(i);
		float ip=(1+cos(2*M_PI*(lx/lamda-phase)))/2;
		float grey=ip>0.5? g0:g180;
		if(!bin) grey=g0*ip+g180*(1-ip);
		glBegin(GL_LINES);
		glColor3f(grey,grey,grey);
		glVertex2f(lx,-aspect);
		glVertex2f(lx, aspect);
		glEnd();
	}
}
void drawCircle(std::queue<std::string>& cont){
	float x = fPOP(cont);
	float y = fPOP(cont);
	float rad = fPOP(cont);
	float g = fPOP(cont);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(g,0.f,0.f);
	glVertex2f(x,y);
	for(int i=0;i<=15;i++){
		glVertex2f(x+rad*sin(2*M_PI/15*i),y+rad*cos(2*M_PI/15*i));
	}
	glEnd();
	glPointSize(1);
	glBegin(GL_POINTS);
	glColor3f(g,0.f,0.f);
	glVertex2f(x,y);
	glEnd();
}
void drawBand(std::queue<std::string>& cont){
	float wid = fPOP(cont);
	float g = fPOP(cont);
	float y=wid;
	glBegin(GL_QUADS);
	glColor3f(g,g,g);
	glVertex2f(1,y);
	glVertex2f(-1,y);
	glVertex2f(-1,-y);
	glVertex2f(1,-y);
	glEnd();
}

int main(void){
/* First call to socket() function */
	std::mutex mutex;
	int cplock = 0;
	int portno = 8888;
	int sockfd;
	int newsockfd;
	long seq = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	WSADATA wsaData;
	int wsares = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (wsares !=0) {
		fprintf(stderr,"WSA ERROR %d\n",wsares);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		perror("ERROR opening socket");
		exit(1);
	}
/* Initialize socket structure */
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
/* Now bind the host address using bind() call.*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

CONNECT:
	fprintf(stderr,"Waiting connection\n");
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) {
		perror("ERROR on accept");
		goto CONNECT;
	}
//  u_long val = 1;
//  ioctlsocket(newsockfd, FIONBIO, &val);
	seq = 0;
THREAD:
	std::thread wsth([&] {
		static char buffer[20000];
		int len=0;
		memset(buffer, 0, 20000);
	READ:
		int rdcount;
		if (seq < 0) goto CLOSE;
		rdcount = recv(newsockfd, buffer+len, 20000, 0);
		if (rdcount < 0) {
			fprintf(stderr, "no data");
			goto CLOSE;
		}
		len += rdcount;
		if (len < 3) goto READ;
		if (strstr(buffer + len-2, "\n") == NULL) goto READ;
		cplock = 1;
		cparser(buffer);
		memset(buffer, 0, 20000);
		len = cplock = 0;
		goto READ;
	CLOSE:
		if (newsockfd > 0) closesocket(newsockfd);
		WSACleanup();
	});

	char buffer[100];
	int cmd, wrcount;
	double tsch = (float)glfwGetTime();
	double gltm = 0;
	int glsleep = 0,glswap=1;
	GLFWmonitor** monitors;
	GLFWmonitor* monitor;
	int mnumber;
	const GLFWvidmode* mode;
	GLFWwindow* window = NULL;

	if (!glfwInit()) goto CLOSE;
	glfwSetErrorCallback(error_callback);
	monitors = glfwGetMonitors(&mnumber);
	monitor = monitors[mnumber - 1];
	//monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);
	fprintf(stderr, "GLmonitors %d %d %d\n", mnumber, mode->width, mode->height);
//	glfwWindowHint(GLFW_FLOATING, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
	window = mnumber > 1 ?
		glfwCreateWindow(mode->width, mode->height, "OKNGL", monitor, NULL) :
		glfwCreateWindow(mode->width * 3 / 4, mode->height * 3 / 4, "OKNGL", NULL, NULL);
	if (!window) {
		glfwTerminate();
		goto CLOSE;
	}
//	glfwSetKeyCallback(window,key_callback);
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &width, &height);
	fprintf(stderr, "Frame buffer %d %d\n", width,height);
	aspect = (float)height*2 / width;
	apixel = px2gl(1) - px2gl(0);
	glViewport(0, 0, width, height);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(-1.f, 1.f, -aspect, aspect, 1.f, -1.f);
//	glLoadIdentity();
//	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

//	static GLfloat lightPosition[] = { -10.0,10.0,10.0,1.0 };
//	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//	static GLfloat lightAmbient[] = { 0.4,0.4,0.4,1.0 };
//	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
//	static GLfloat matFrontCol[] = {0.7,1.0,1.0,1.0};
//	glMaterialfv(GL_FRONT,GL_DIFFUSE,matFrontCol);
//	Depth Setting
//	glDepthRange(0,255);
//	//glDepthFunc(GL_LEQUAL);
//	//glEnable(GL_DEPTH_TEST);
//	glDisable(GL_DEPTH_TEST);
//	glMatrixMode(GL_MODELVIEW);
	glfwSwapInterval(1);
NEXT:
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glLoadIdentity();
	glRotatef(0.f, 1.f, 0.f, 0.f);
	if (!cplock) {
		std::queue<std::string> cp;
		if (!cparser_queue.empty()) {
			std::lock_guard<std::mutex> lock(mutex);
			cp.swap(cparser_queue);
		}
		while (!cp.empty()) {
			fifo.push(cp.front());
			cp.pop();
		}
	}
	if (!fifo.empty()) std::queue<std::string>().swap(backup);
	else fifo.swap(backup);
/*	{
		std::queue<std::string> cp(fifo);
		std::cout << "fifo ";
		while (!cp.empty()) {
			std::cout << cp.front() << " ";
			cp.pop();
		}
		std::cout << "\n";
	}*/
PARSE:
	if (fifo.empty()) goto RESPONSE;
	cmd = cPOP(fifo);
	switch (cmd) {
	case 'W':
		drawWav(fifo);
		break;
	case 'C':
		drawCircle(fifo);
		break;
	case 'B':
		drawBand(fifo);
		break;
	case '*':
		goto RESPONSE;
		break;
	case 'Q':
		goto CLOSE;
		break;
	}
	goto PARSE;
RESPONSE:
//	fprintf(stderr, "Swap\n");
//	for(glswap=1;glsleep>11;glswap++)
//		glsleep=floor((gltm+0.016666*glswap-glfwGetTime())*1000)-3;
	Sleep(16);
	glfwSwapBuffers(window);
	gltm = glfwGetTime();
	sprintf(buffer, "%f\n", (gltm - tsch) * 1000);
	wrcount = send(newsockfd, buffer, strlen(buffer), 0);
//	fprintf(stderr, "tm=%s\n", buffer);
	glfwPollEvents();
	if (glfwWindowShouldClose(window)) goto CLOSE;
	tsch = gltm;
	if (wrcount <= 0) {
		perror("write socket error");
		goto CLOSE;
	}
	seq++;
	goto NEXT;
CLOSE:
	glfwDestroyWindow(window);
	glfwTerminate();
	seq = -1;
JOIN:
	wsth.join();
	exit(EXIT_SUCCESS);
}
