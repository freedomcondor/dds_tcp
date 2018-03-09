#include<math.h>
#include"function_opengl.h"
#include"tcp_socket.h"

#include <iostream>

#define pi 3.1415926

#define Max_plot 100000

///////////////////  function definations /////////////////
//
//
double buffer_draw2 = 0;
double plot_y_max = 100;
double plot_x_max = 50;
double datalog[Max_plot];
double datalog2[Max_plot];
double datalog3[Max_plot];
double datalog4[Max_plot];


///////////////////  dds  /////////////////////////
TCPSocket *tcpSocket;
int16_t targetLeftSpeed, targetRightSpeed;
int16_t currentLeftSpeed, currentRightSpeed;
int16_t fbLeftSpeed, fbRightSpeed;
int16_t fbLeftError, fbRightError;
float fbLeftErrorInt, fbRightErrorInt;
int16_t fbLeftErrorDev, fbRightErrorDev;
unsigned int uptime = 0;

float currentKp, currentKi, currentKd;
float targetKp, targetKi, targetKd;

///////////////////  functions  ///////////////////////////
int function_exit()
{
	uint8_t pnStopDriveSystemData[] = {0, 0, 0, 0,
										0,0,0,0,
										0,0,0,0,
										0,0,0,0
										};

	tcpSocket->Write((char*)pnStopDriveSystemData,16);
#ifdef windows
	exit(0);
#else
	exit(0);
#endif
}
int function_init(int SystemWeight, int SystemHeight)
{
	//tcpSocket = new TCPSocket("192.168.1.100",8080);
	tcpSocket = new TCPSocket("192.168.1.101",8080);
	//tcpSocket = new TCPSocket("192.168.1.103",8080);
	//tcpSocket = new TCPSocket("192.168.1.104",8080);
	//tcpSocket = new TCPSocket("127.0.0.1",8080);
	if (tcpSocket->Open() != 0)
	{
		printf("Socket open failed\n");
		return -1;
	}

    /*  for no belt
	targetKp = 0.110;
	targetKi = 0.159;
	targetKd = 0.044;
	*/
	
	/*  on the ground
	targetKp = 0.527;
	targetKi = 0.625;
	targetKd = 0.056;
	 */

	/*  on the ground
	targetKp = 0.477;
	targetKi = 0.625;
	targetKd = 0.036;
	*/

	/*  on the ground
	targetKp = 0.420
	targetKi = 0.700;
	targetKd = 0.050;
	*/

	/*
	targetKp = 0.0;
	targetKi = 0.0;
	targetKd = 0.0;
	*/

	return 0;
}

int function_step()
{
	if (uptime < Max_plot)
		uptime++;
	else
		uptime = 0;

	//if ((targetLeftSpeed != currentLeftSpeed) || (targetRightSpeed != currentRightSpeed) ||
	//	(targetKp != currentKp) || (targetKi != currentKi) || (targetKd != currentKd))
	{
		currentLeftSpeed = targetLeftSpeed;
		currentRightSpeed = targetRightSpeed;

		///*
		currentKp = targetKp;
		currentKi = targetKi;
		currentKd = targetKd;
		//*/

		/*
		currentKp = 0.15;
		currentKi = 0.15;
		currentKd = 0.15;
		*/

		//uint8_t temp = reinterpret_cast<uint8_t>(currentLeftSpeed & 255);
		uint8_t pnStopDriveSystemData[] = {
			reinterpret_cast<uint8_t*>(&currentLeftSpeed)[1],
			reinterpret_cast<uint8_t*>(&currentLeftSpeed)[0],
			reinterpret_cast<uint8_t*>(&currentRightSpeed)[1],
			reinterpret_cast<uint8_t*>(&currentRightSpeed)[0],

			///*
			reinterpret_cast<uint8_t*>(&currentKp)[3],
			reinterpret_cast<uint8_t*>(&currentKp)[2],
			reinterpret_cast<uint8_t*>(&currentKp)[1],
			reinterpret_cast<uint8_t*>(&currentKp)[0],

			reinterpret_cast<uint8_t*>(&currentKi)[3],
			reinterpret_cast<uint8_t*>(&currentKi)[2],
			reinterpret_cast<uint8_t*>(&currentKi)[1],
			reinterpret_cast<uint8_t*>(&currentKi)[0],

			reinterpret_cast<uint8_t*>(&currentKd)[3],
			reinterpret_cast<uint8_t*>(&currentKd)[2],
			reinterpret_cast<uint8_t*>(&currentKd)[1],
			reinterpret_cast<uint8_t*>(&currentKd)[0],
			//*/
		};
		printf("before send");
		tcpSocket->Write((char*)pnStopDriveSystemData,16);
		printf("after send");
		/*
		ddsInterface->SendPacket(
			CPacketControlInterface::CPacket::EType::SET_DDS_SPEED,
			pnStopDriveSystemData,
			sizeof(pnStopDriveSystemData));
		*/
	}

	datalog2[uptime] = currentLeftSpeed;
	datalog4[uptime] = currentRightSpeed;

	printf("currentspeed : %d\n",currentLeftSpeed);
	printf("currentpid : %f, %f, %f\n",currentKp,currentKi, currentKd);

	int len;
	char punPacketData[200];
	printf("listening\n");
	tcpSocket->Read(punPacketData, &len);
	printf("feedback get len = %d\n",len);
	//if (len != 20) return -1;
	if (len != 20) return 0;
	//if (len != 4) return 0;
	reinterpret_cast<int16_t&>(fbLeftSpeed) = punPacketData[0]<<8 | punPacketData[1];
	reinterpret_cast<int16_t&>(fbRightSpeed) =punPacketData[2]<<8 | punPacketData[3];

	///*
	reinterpret_cast<int16_t&>(fbLeftError) = punPacketData[4]<<8 | punPacketData[5];
	reinterpret_cast<int16_t&>(fbRightError) =punPacketData[6]<<8 | punPacketData[7];

	reinterpret_cast<int32_t&>(fbLeftErrorInt) = 	
												(punPacketData[8] << 24) | 
												(punPacketData[9] << 16) |
												(punPacketData[10] << 8) |
												(punPacketData[11]);

	reinterpret_cast<int32_t&>(fbRightErrorInt) =	
												punPacketData[12] << 24 | 
												punPacketData[13] << 16 |
												punPacketData[14] << 8  |
												punPacketData[15];

	reinterpret_cast<int16_t&>(fbLeftErrorDev)= punPacketData[16]<<8 | 
												punPacketData[17];
	reinterpret_cast<int16_t&>(fbRightErrorDev)=punPacketData[18]<<8 | 
												punPacketData[19];
	//*/

	///*
	printf("speed: %d\n",fbLeftSpeed);
	printf("speed: %d\n",fbRightSpeed);
	printf("error: %d\n",fbLeftError);
	printf("error: %d\n",fbRightError);
	printf("errorInt: %f\n",fbLeftErrorInt);
	printf("errorInt: %f\n",fbRightErrorInt);
	printf("errorDev: %d\n",fbLeftErrorDev);
	printf("errorDev: %d\n",fbRightErrorDev);
	//*/

	datalog[uptime] = fbLeftSpeed;
	datalog2[uptime] = currentLeftSpeed;
	datalog3[uptime] = fbRightSpeed;
	datalog4[uptime] = currentRightSpeed;

	return 0;
}

int function_draw()
{
	return 0;
}

int function_draw2()
{
	int drawWindow = 100;

	int drawStart = 0;
	if (uptime < plot_x_max)
		drawStart = 0;
	else
		drawStart = uptime - drawWindow;

	for (int i = drawStart; i <= uptime; i++)
	{
		if (uptime < plot_x_max)
		{
		glBegin(GL_LINES);
			glVertex3f( (i-1)/plot_x_max,	datalog[i-1]/plot_y_max,0.0f);
			glVertex3f(  i/plot_x_max,		datalog[i] / plot_y_max,0.0f);

			glVertex3f( (i-1)/plot_x_max,	datalog2[i-1]/plot_y_max,0.0f);
			glVertex3f(  i/plot_x_max,		datalog2[i] / plot_y_max,0.0f);

			glVertex3f( (i-1)/plot_x_max + 1,	datalog3[i-1]/plot_y_max,0.0f);
			glVertex3f(  i/plot_x_max + 1,		datalog3[i] / plot_y_max,0.0f);

			glVertex3f( (i-1)/plot_x_max + 1,	datalog4[i-1]/plot_y_max,0.0f);
			glVertex3f(  i/plot_x_max + 1,		datalog4[i] / plot_y_max,0.0f);
		glEnd();
		}
		else
		{
		glBegin(GL_LINES);
			glVertex3f( 1.0*(i-1-drawStart)/drawWindow,	datalog[i-1]/plot_y_max,0.0f);
			glVertex3f( 1.0*(i - drawStart)/drawWindow,	datalog[i] / plot_y_max,0.0f);

			glVertex3f( 1.0*(i-1-drawStart)/drawWindow,	datalog2[i-1]/plot_y_max,0.0f);
			glVertex3f( 1.0*(i - drawStart)/drawWindow,	datalog2[i] / plot_y_max,0.0f);

			glVertex3f( 1.0*(i-1-drawStart)/drawWindow + 1,	datalog3[i-1]/plot_y_max,0.0f);
			glVertex3f( 1.0*(i - drawStart)/drawWindow + 1,	datalog3[i] / plot_y_max,0.0f);

			glVertex3f( 1.0*(i-1-drawStart)/drawWindow + 1,	datalog4[i-1]/plot_y_max,0.0f);
			glVertex3f( 1.0*(i - drawStart)/drawWindow + 1,	datalog4[i] / plot_y_max,0.0f);
		glEnd();
		}
	}
		
	return 0;
}


////////////////////////////// drawings  /////////////////////////
int drawSphere(double x, double y, double z, double r)
{
	glTranslatef(x,y,z);
	glutSolidSphere(r,10,10);
	glTranslatef(-x,-y,-z);

	return 0;
}

int drawCube(double x, double y, double z, double half)
{
	glTranslatef(x,y,z);
	glutSolidCube(half);
	glTranslatef(-x,-y,-z);

	return 0;
}

int drawCylinder(	double base, double top, double height,
				double lx,	double ly, double lz,
				double ex,	double ey, double ez
			)
{
	double xaxis,yaxis,zaxis,angleaxis;
	double xbase,ybase,zbase;
	double e;
	int rotateflag = 1;
	GLUquadricObj *quadratic;
	double error = 0.001;

	quadratic = gluNewQuadric();

	//printf("l: %lf %lf %lf\n",lx,ly,lz);
	//printf("e: %lf %lf %lf\n",ex,ey,ez);

	if (((ex-0)*(ex-0)<error) && ((ey-0)*(ey-0)<error) && ((ez-1)*(ez-1)<error))
		rotateflag = 0;

	if (rotateflag == 1)
	{
		e = sqrt(ex * ex + ey * ey + ez * ez);
		if (e == 0) return -1;

		xbase = 0; ybase = 0; zbase = 1;
		xaxis = ybase * ez - zbase * ey;
		yaxis = zbase * ex - xbase * ez;
		zaxis = xbase * ey - ybase * ex;
		//angleaxis = acos((xbase*ex+ybase*ey+zbase*ez)/e) + pi;
		angleaxis = acos((xbase*ex+ybase*ey+zbase*ez)/e);

		//printf("%lf %lf %lf %lf\n",angleaxis,xaxis,yaxis,zaxis);

	}

	glTranslatef(lx,ly,lz);

	if (rotateflag == 1)
		glRotatef(angleaxis*180/pi,xaxis,yaxis,zaxis);

	gluCylinder(quadratic,base,top,height,32,32);//»­Ô²Öù	base top height
	if (rotateflag == 1)
		glRotatef(-angleaxis*180/pi,xaxis,yaxis,zaxis);

	glTranslatef(-lx,-ly,-lz);
	return 0;
}
