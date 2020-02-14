#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <stdio.h>
//#include <string.h>
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

#define M_PI 3.14159265

#include "cparser.h"

static void error_callback(int error, const char* description){
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
//	if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) glfwSetWindowShouldClose(window,GL_TRUE);
}

//Global parameters
int width,height;
float aspect;
float apixel;
float px2gl(int px){
	return 2*(float)px/width-1;
}
float a2f(std::string s){
	return atof(s.c_str());
}
float fPOP(std::queue<std::string> q) {
	std::string s = q.front();
	q.pop();
	return a2f(s);
}
const char *cPOP(std::queue<std::string> q) {
	std::string s = q.front();
	q.pop();
	return s.c_str();
}
void drawWav(float lamda,float phase,float g0,float g180,int bin){
	float lamda=, float phase, float g0, float g180, int bin
	fprintf(stderr, "W %f %f %f %f %d\n",  lamda, phase, g0, g180, bin);
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
void drawCircle(float x,float y,float rad,float g){
//	fprintf(stderr,"rad %f\n",apixel);
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
void drawBand(float wid,float g){
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
	if (!glfwInit()) exit(EXIT_FAILURE);
	glfwSetErrorCallback(error_callback);

//	glfwWindowHint(GLFW_ALPHA_BITS, 0);
//	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	int mnumber=0;
//	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	GLFWmonitor **monitors = glfwGetMonitors(&mnumber);
	GLFWmonitor *monitor = monitors[mnumber-1];
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	fprintf(stderr, "monitors %d %d %d\n", mnumber, mode->width, mode->height);
	GLFWwindow* window = mnumber>1?
		glfwCreateWindow(mode->width, mode->height, "OKNGL", monitor, NULL):
		glfwCreateWindow(mode->width * 3 / 4, mode->height * 3 / 4, "OKNGL", NULL, NULL);
	if (!window){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
//	glfwSetKeyCallback(window,key_callback);
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window,&width,&height);
	aspect=(float)height/width;
	apixel=px2gl(1)-px2gl(0);
	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	glOrtho(-1.f,1.f,-aspect,aspect,1.f,-1.f);
//	glLoadIdentity();
//	gluLookAt(0.0,0.0,1.0, 0.0,0.0,0.0, 0.0,1.0,0.0);
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	static GLfloat lightPosition[]={-10.0,10.0,10.0,1.0};
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosition);
	static GLfloat lightAmbient[]={0.4,0.4,0.4,1.0};
	glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmbient);
//	static GLfloat matFrontCol[] = {0.7,1.0,1.0,1.0};
//	glMaterialfv(GL_FRONT,GL_DIFFUSE,matFrontCol);
//Depth Setting
//	glDepthRange(0,255);
//	glDepthFunc(GL_LEQUAL);
//	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);

  /* First call to socket() function */
  WSADATA wsaData;
  int wsares=WSAStartup(MAKEWORD(2, 0), &wsaData);
  if (wsares !=0) {
	  fprintf(stderr,"WSA ERROR %d\n",wsares);
	  exit(1);
  }
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
      perror("ERROR opening socket");
      exit(1);
  }
  /* Initialize socket structure */
  struct sockaddr_in serv_addr;
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  int portno = 8888;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  /* Now bind the host address using bind() call.*/
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(1);
  }
  struct sockaddr_in cli_addr; 
  listen(sockfd,5);
  socklen_t clilen = sizeof(cli_addr);

	double tsch=(float)glfwGetTime();
	static char buffer[20000];
	double gltm=0;
	int seq=0;
	std::string cmd;
CONNECT:
	fprintf(stderr,"Waiting connection\n");
	int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) {
		perror("ERROR on accept");
		goto CONNECT;
 }
//  u_long val = 1;
//  ioctlsocket(newsockfd, FIONBIO, &val);
READ:
	memset(buffer,0,20000);
	int rdcount=recv(newsockfd, buffer, 20000, 0);
	if(rdcount<0){
		fprintf(stderr,"no data");
		goto QUIT;
	}
	cparser(buffer);
	seq=0;
PARSE:
	glfwPollEvents();
	if (glfwWindowShouldClose(window)) goto QUIT;
	if(fifo.empty()) goto READ;
	if(seq==0){
		glClearColor(0.f,0.f,0.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDisable(GL_LIGHTING);
		glLoadIdentity();
		glRotatef(0.f,1.f,0.f,0.f);
	}
	seq++;
	cmd = POP(fifo);
	switch(cmd.c_str()[0]){
	case 'W':
	  drawWav(fPOP(fifo),fPOP(fifo),fPOP(fifo),a2f(POP(fifo)),strcmp(POP(fifo).c_str(),"false"));
	  break;
	case 'C':
	  drawCircle(POP(fifo)),a2f(POP(fifo)),a2f(arg[3]),a2f(arg[4]));
	  break;
	case 'B':
	  drawBand(a2f(arg[1]),a2f(arg[2]));
    break;
	case 'G':
	  goto RESPONSE;
	  break;
	case 'Q':
	  goto QUIT;
      break;
	}
  goto PARSE;
RESPONSE:
	fprintf(stderr,"Glfw swap buffer \n");
	seq=0;
	if((gltm - tsch)>0.1) glfwSwapInterval(3);
	else glfwSwapInterval(1);
	glfwSwapBuffers(window);
	gltm=glfwGetTime();
	sprintf(buffer,"%f\n",(gltm-tsch)*1000);
	rdcount=send(newsockfd, buffer, strlen(buffer), 0);
	fprintf(stderr,"tm=%s\n",buffer);
	tsch=gltm;
	if(rdcount<=0){
		perror("write socket error");
		goto QUIT;
	}
//  glfwPollEvents();
	goto PARSE;
QUIT:
  if(sockfd>0) close(sockfd);
	glfwDestroyWindow(window);
	glfwTerminate();
	WSACleanup();
	exit(EXIT_SUCCESS);
}
