# Robot-switch
C语言实现的远程指令控制yanshee机器人

远程界面客户端和机器人通过TCP 网络连接到服务器

客户端发送动作指令到服务器上，服务器将指令转发到机器人中

yanshee机器人在收到指令后执行对应动作
## 项目结构：  
```bash
│  .gitignore  
│  server-flowchat.pdf        // 服务程序流程图 
│  README.md  
└─robot-switch-fifo           // 使用队列的服务程序
│ 
└─robot-switch				  // 不使用队列的服务程序
│               
└─yanshee-client-c			  // yanshee机器人C语言调用API的Demo

```

## 操作说明：
- 到各个目录下执行make 生成对应的服务程序

