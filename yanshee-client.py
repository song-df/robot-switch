#!/usr/bin/python
# -*-coding: UTF-8 -*-

import time
import socket

# Api库导入
import RobotApi
HOST = '124.70.148.79'
PORT = 18733 
BUFSIZ = 1024
SERVADDR = (HOST, PORT)

def connectRobot():
	'''
	连接机器人
	:return: None
	'''
	RobotApi.ubtRobotInitialize()
	ret=RobotApi.ubtRobotConnect("SDK","1","127.0.0.1")
	if(0!=ret):
		RobotApi.ubtVoiceTTS(0,"无法连接Yanshee")
		exit(1)

def disConnectRobot():
	'''
	断开机器人的连接,只需要执行一次
	:return: None
	'''
	RobotApi.ubtRobotDisconnect("SDK","1","127.0.0.1")
	RobotApi.ubtRobotDeinitialize()

def connectECS():
    c = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    c.connect(SERVADDR)
    while True:
        print("waiting...")
        data = c.recv(BUFSIZ)#block recv
	print("received " + data)
	if not data:
            break
        c.send("start")
	RobotApi.ubtStartRobotAction(data,1)
	c.send("done")
	if data == b'':
	    break;

if __name__ == '__main__':
	connectRobot()


	import sys
	reload(sys) 
	sys.setdefaultencoding('utf-8')
	
	connectECS()

	disConnectRobot()


