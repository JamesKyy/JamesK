#include <ESP8266WiFi.h>        //  ESP8266WiFi库
#include <ESP8266WiFiMulti.h>   //  ESP8266WiFiMulti库
#include <ESP8266WebServer.h>   //  ESP8266WebServer库

ESP8266WiFiMulti wifiMulti;     // 建立ESP8266WiFiMulti对象,用于连接到多个WiFi网络
 
ESP8266WebServer esp8266_server(80);// 建立ESP8266WebServer对象，对象名称为esp8266_server
                                    // 括号中的数字是网路服务器响应http请求的端口号
                                    // 网络服务器标准http端口号为80，因此这里使用80为端口号

// 定义两个HTML字符串，分别对应开灯和关灯的网页界面
String open = "<!DOCTYPE html> <html lang= \"ch \">  <head>     <meta charset= \"UTF-8 \">     <meta http-equiv= \"X-UA-Compatible \" content= \"IE=edge \">     <meta name= \"viewport \" content= \"width=device-width, initial-scale=1.0 \">     <title>WIFI_lamp</title>     <style>         .outbox {             height: 700px;             position: relative;                 }          .midbox {             height: 200px;             width: 200px;             top: 50%;             left: 50%;             margin: -150px 0 0 -150px;             position: absolute;             line-height: 100px;             justify-content: center;             align-items: Center;         }          .a {             text-decoration: none;         }          .button1 {             width: 250px;             height: 250px;             font-size: 30px;    border-radius: 50%;       border: 5px solid black;      }          .space {             width: 100px;         }     </style> </head>  <body>     <div class= \" outbox \">         <div class= \"midbox \">             <a href='./LED?light_button=1' class='a'>                 <button type='button1' class='button1'>开灯</button>             </a>          </div>     </div> </body>  </html>  ";
String close = "<!DOCTYPE html> <html lang= \"ch \">  <head>     <meta charset= \"UTF-8 \">     <meta http-equiv= \"X-UA-Compatible \" content= \"IE=edge \">     <meta name= \"viewport \" content= \"width=device-width, initial-scale=1.0 \">     <title>WIFI_lamp</title>     <style>         .outbox {             height: 700px;             position: relative;                 }          .midbox {             height: 200px;             width: 200px;             top: 50%;             left: 50%;             margin: -150px 0 0 -150px;             position: absolute;             line-height: 100px;             justify-content: center;             align-items: Center;         }          .a {             text-decoration: none;         }          .button1 {             width: 250px;             height: 250px;             font-size: 30px;    border-radius: 50%;       border: 5px solid black;       }          .space {             width: 100px;         }     </style> </head>  <body>     <div class= \" outbox \">         <div class= \"midbox \">             <a href='./LED?light_button=0' class='a'>                 <button type='button1' class='button1'>关灯</button>             </a>          </div>     </div> </body>  </html>  ";


void setup(void){
  // 启动串口通讯，波特率设置为115200
  Serial.begin(115200);
  // 设置D1引脚为输出模式
  pinMode(05, OUTPUT);           
  // 默认状态下，LED灯关闭
  digitalWrite(05, LOW);        
  
  // 通过addAp函数存储  addAp(WiFi名称,WiFi密码)
  wifiMulti.addAP("wrz", "123456789");  
   
  int i = 0;                                 
  while (wifiMulti.run() != WL_CONNECTED) {  // 此处的wifiMulti.run()是重点。通过wifiMulti.run()，NodeMCU将会在当前
    delay(1000);                             // 环境中搜索addAP函数所存储的WiFi。如果搜到多个存储的WiFi那么NodeMCU
    Serial.print(i++); Serial.print(' ');    // 将会连接信号最强的那一个WiFi信号。
  }                                          // 一旦连接WiFI成功，wifiMulti.run()将会返回“WL_CONNECTED”。这也是
                                             // 此处while循环判断是否跳出循环的条件。
 
  // WiFi连接成功后将通过串口监视器输出连接成功信息 
  Serial.println('\n');                     // WiFi连接成功后
  Serial.print("Connected to ");            // NodeMCU将通过串口监视器输出。
  Serial.println(WiFi.SSID());              // 连接的WiFI名称
  Serial.print("IP address:\t");            // 以及
  Serial.println(WiFi.localIP());           // NodeMCU的IP地址
  
//--------"启动网络服务功能"程序部分开始-------- 
  esp8266_server.begin();    // 开始监听HTTP请求，等待客户端的连接               
  esp8266_server.on("/", handleRoot);     //有HTTP GET请求访问服务器的根路径/时，将调用handleRoot函数进行处理
  esp8266_server.on("/LED", HTTP_GET, handleLED);    //针对/LED路径上的HTTP GET请求。当这个路径被请求时，会调用handleLED函数  
  esp8266_server.onNotFound(handleNotFound);   //设置404页面处理器。如果客户端请求的页面在服务器上没有找到（即所有其他路由均不匹配时），将调用handleNotFound函数，返回一个简单的404错误信息页面

//--------"启动网络服务功能"程序部分结束--------
  Serial.println("HTTP esp8266_server started");//  告知用户ESP8266网络服务功能已经启动
}

void loop(void){
  esp8266_server.handleClient();     // 处理http服务器访问
}
                                                                          
void handleRoot() {   //处理网站根目录“/”的访问请求 
  esp8266_server.send(200, "text/html", open);   // NodeMCU将调用此函数，发送HTTP响应，状态码200表示成功，内容类型为HTML，内容为openh字符串。
}

// 设置处理404情况的函数'handleNotFound'
void handleNotFound(){                                        // 当浏览器请求的网络资源无法在服务器找到时，
  esp8266_server.send(404, "text/plain", "404: Not found");   // NodeMCU将调用此函数，发送404响应
}

// 处理"/LED"的GET请求，根据查询参数light_button的值控制LED并返回相应的HTML页面
void handleLED() {
  
  if (esp8266_server.arg("light_button") == "1"){
    digitalWrite(05, HIGH); // D1，打开LED       
    esp8266_server.send(200, "text/html", close);
  } // 2端口
  else if (esp8266_server.arg("light_button") == "0"){
    digitalWrite(05, LOW); // D1，关闭LED
    esp8266_server.send(200, "text/html", open);
  }
}
