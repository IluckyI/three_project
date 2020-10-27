#include "main.h"


            // #include "./inc/font.h"
            //  #include "./inc/cJSON.h"

#include <netdb.h>
#define DEBUG
typedef struct city_info
{
    char id[20];
    char name[50];

}city_info;

typedef struct time_day
{
    char year[10];
    char month[5];
    char day[5];

}time_day;



typedef struct day_info
{
    char time[50];      //时间
    char week[20];     //星期几
    char temp[100];     //低温~高温
    char wind[20];      //风力
    char fx[30];        //风向
    char weather_info[50];  //天气
    int aqi;
    int color;
    char aqi_status[30];

}info;


int check_cmd( char  *cmd);
bool check_ip(char *cmd);
void *time_show_bmp(void * arg);
bool show_session(char *session);
struct LcdDevice *init_lcd(const char *device);
void chat();
void weather();
bool find_city_id(char *cmd,city_info *c);

void show_day_info(int i,info *day,cJSON *obj);

bool show_2_weather(info *day,int x_bit);
void show_weather_bmp(info *day,int x);
bool show_weather_title();
void *time_show_chat(void * arg);
void handler_exit(int arg);
void shutdown_0();

int get_pic();
void pic_download_show();
void music_get();

enum position
{
    EXIT=0,
    SHOTDOWN,
    WEATHER,
    CHAT,
    PIC,
    MUSIC

};

enum weather_status
{
    SUNNY=1,    //晴
    CLOUDY,     //多云
    SHOWER,     //阵雨
    SNOW_SHOWER,//阵雪
    RAIN,       //雨
    OVERCAST    //阴天

};


pthread_t tid;
int main()
{



    signal(2,handler_exit);
    pthread_create(&tid,NULL,time_show_bmp,NULL);
    pthread_detach(tid);
    while(1)
    {
        char cmd[256]={0};
        show_session("你需要什么帮助么？\n");
        printf("请问你需要什么帮助么?\n");
        scanf("%s",cmd);
        int ret=check_cmd(cmd);

        switch(ret)
        {
            case CHAT:
            {
                pthread_cancel(tid);
                pthread_create(&tid,NULL,time_show_chat,NULL);
                pthread_detach(tid);               
                chat();
                break;
            }
            case WEATHER:
            {
                pthread_cancel(tid);
                pthread_create(&tid,NULL,time_show_chat,NULL);
                pthread_detach(tid);
                weather();
                break;
            }
            case PIC:
            {
                pthread_cancel(tid);              
                pic_download_show();
                break;
            }
            case MUSIC:
            {
                pthread_cancel(tid);
                pthread_create(&tid,NULL,time_show_chat,NULL);
                pthread_detach(tid);
                music_get();

                break;
            }
            case SHOTDOWN: 
            {
                kill(getpid(),2);
                break;
            }




        }
        pthread_cancel(tid);
        pthread_create(&tid,NULL,time_show_bmp,NULL);
        pthread_detach(tid);   
        


    }  

    pause();
    return 0;
}

void chat()
{    
    int i=0;
    loop:
        {
            int tcp_socket=socket(AF_INET, SOCK_STREAM, 0);
            
            if(tcp_socket<0)
            {
                perror("");
                return ;
            }

            struct sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(80); //HTTP 协议默认的端口就是 80 
            server_addr.sin_addr.s_addr = inet_addr("47.107.155.132"); //服务器的IP地址


    //http://api.qingyunke.com/api.php



            int ret = connect(tcp_socket,(struct sockaddr *)&server_addr,sizeof(server_addr));
            if(ret < 0)
            {
                printf("链接失败\n");
                return ;
            }
            else
            {
                printf("链接成功\n");
            }
            while(1)
            {                
                //制定 HTTP 请求协议   
                char  http_head[1024]={0};
                char  cmd[100]={0};
                scanf("%s",cmd);
                if(check_cmd(cmd)==EXIT)
                {
                    show_session("一会儿见哦\n");
                    return ;
                }
                sprintf(http_head,"GET /api.php?key=free&appid=0&msg=%s HTTP/1.1\r\nHost:api.qingyunke.com\r\n\r\n",cmd);

                //发HTTTP 请求协议  
                char revbuf[1024]={0};
                char sessions[256]={0};
                write(tcp_socket,http_head,strlen(http_head));




                read(tcp_socket,revbuf,1024);
                if(strlen(revbuf)==0)
                {
                    printf("服务器连接丢失，正在重新连接\n");
                    show_session("刚才走神了，亲");
                    i++;
                    sleep(5);
                    if(i>=3)
                    {
                        return ;
                    }
                    goto loop;
                }
                
                printf("%s\n",http_head);
                printf("%s\n",revbuf);
                char *p=strstr(revbuf,"\"result\":0,\"content\":\"");
                p=p+strlen("\"result\":0,\"content\":\"");
                char *q=strstr(p,"\"");
                int session_len=(int)(q-p);
                strncpy(sessions,p,session_len);

                show_session(sessions);
                printf("session:%s\n",sessions);

                



            }
        }
}



//询问gif表情
void *time_show_bmp(void * arg)
{
    lcd_open();
    char name[100];
    int i=1;
    while(1)
    {
        bzero(name,100);
        sprintf(name,"/mora/network_project/pic/3-%d.jpg",i);
        lcd_draw_jpg(0,0,name,0);
        if(i==50)
        {
            i=1;
        }
        i++;
    }

    lcd_close();

}

//闲聊gif表情
void *time_show_chat(void * arg)
{
    lcd_open();
    char name[100];
    int i=1;
    while(1)
    {
        bzero(name,100);
        sprintf(name,"/mora/network_project/pic/chat/1-%d.jpg",i);
        lcd_draw_jpg(0,0,name,0);
        if(i==50)
        {
            i=1;
        }
        i++;
    }

    lcd_close();

}



void handler_exit(int arg)
{
    pthread_cancel(tid);
    sleep(1);
    shutdown_0();
    exit(0);

}

void shutdown_0()
{
    lcd_open();
    char name[100];
    int i=1;
    while(1)
    {
        bzero(name,100);
        sprintf(name,"/mora/network_project/pic/shutdown/1-%d.jpg",i);
        lcd_draw_jpg(0,0,name,0);
        if(i==56)
        {
            return ;
        }
        i++;
    }

    lcd_close();

}



bool show_session(char *session)
{
	struct LcdDevice* lcd = init_lcd("/dev/fb0");
			
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f,40);
	
	
	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(800,100,4,getColor(0,49,31,14)); //也可使用createBitmapWithInit函数，改变画板颜色
	//bitmap *bm = createBitmap(288, 100, 4);
	
	
	//将字体写到点阵图上
	fontPrint(f,bm,0,0,session,getColor(0,255,255,0),0);
	
	
	
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp,0,380,bm);


	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
	
}

struct LcdDevice *init_lcd(const char *device)
{
	//申请空间
	struct LcdDevice* lcd = malloc(sizeof(struct LcdDevice));
	if(lcd == NULL)
	{
		return NULL;
	} 

	//1打开设备
	lcd->fd = open(device, O_RDWR);
	if(lcd->fd < 0)
	{
		perror("open lcd fail");
		free(lcd);
		return NULL;
	}
	
	//映射
	lcd->mp = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd->fd,0);

	return lcd;
}


int check_cmd( char  *cmd)
{
    if(strstr(cmd,"天气"))
    {
        return WEATHER;
    }
    if(strstr(cmd,"聊天"))
    {
        return CHAT;
    }
    if(strstr(cmd,"退出"))
    {
        return EXIT;
    }
    if(strstr(cmd,"关机"))
    {
        return SHOTDOWN;
    }
    if(strstr(cmd,"图片下载"))
    {
        return PIC;
    }
    if(strstr(cmd,"音乐"))
    {
        return MUSIC;
    }
    return 10;

}


void weather()
{
    
    int tcp_socket=socket(AF_INET, SOCK_STREAM, 0);
    
    if(tcp_socket<0)
    {
        perror("");
        return ;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); //HTTP 协议默认的端口就是 80 
    server_addr.sin_addr.s_addr = inet_addr("1.81.5.190"); //服务器的IP地址


    //http://api.qingyunke.com/api.php



    int ret = connect(tcp_socket,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if(ret < 0)
    {
        printf("链接失败\n");
        return ;
    }
    else
    {
        printf("链接成功\n");
    }
    while(1)
    {                
        //制定 HTTP 请求协议   
        char  http_head[1024]={0};
        char  cmd[100]={0};
        printf("请输入城市名称：\n");
        show_session("请告诉我是哪个城市哦");
        scanf("%s",cmd);
        
        if(check_cmd(cmd)==EXIT)
        {
            show_session("正在退出天气模式哦，亲");
            sleep(5);
            close(tcp_socket);
            return ;
        }
        //处理城市名称转代码数据，待会写
        
        char id[50]={0};
        city_info c;
        int ret=find_city_id(cmd,&c);
        if(ret==false)
        {
            show_session("出现错误了，亲");
            sleep(5);
            close(tcp_socket);
            return;
        }
        printf("city_id:%s\n",c.id);
        int i=0;

                                                //城市id
        sprintf(http_head,"GET /api/weather/city/%s HTTP/1.1\r\nHost:t.weather.itboy.net\r\n\r\n",c.id);
        restart:
        {
            //发HTTTP 请求协议  
            char head[10240*5]={0};
            write(tcp_socket,http_head,strlen(http_head));

            // char  head[1024*10] ={0};
            int file_size = 0,size_one = 0;
            
            //读取回应内容
            size_one = read(tcp_socket,head,sizeof(head));
        
            //偏移到数据区
            char *head_tail = strstr(head,"{\"message\":");
            //取得数据
            cJSON * obj = cJSON_Parse(head_tail);
            if(obj==NULL)
            {
                show_session("数据获取失败，尝试重新获取中...\n");
                sleep(1);
                i++;
                if(i>3)
                {
                    show_session("天气获取失败！\n");
                    return ;
                }
                goto restart;
                
            }
            else
            {
                printf("正确的json数据");
                show_session("跳转中......");
            }
            
            

            info today[6];
            for(int i=0;i<6;i++)
            {
                show_day_info(i,&today[i],obj);
            }
            
            for(int i=0;i<6;i++)
            {
                printf("tadoy:%s,%s,%s,%s,%s,%s\n",today[i].time,today[i].week,today[i].temp,today[i].weather_info,today[i].wind,today[i].fx);
            }
            
            pthread_cancel(tid);
            usleep(200);
            
            
                   

            
            //显示天气框图

            lcd_draw_jpg(0,0,"./weather.jpg",0);
            show_weather_title();
            usleep(100);
            int x=0;
            for(i=0;i<6;i++)
            {
                show_weather_bmp(&today[i],x);
                show_2_weather(&today[i],x);
                x+=133;
                                
            }
            

            //sleep(10);
                                    ///监控键盘，按任意键退出天气显示，或者等待达到   显示时间   20s
            fd_set set;
            FD_ZERO(&set);
            FD_SET(0,&set);
            struct timeval time;
            time.tv_sec=20;
            time.tv_usec=0;
            char cmdd[200];
            ret=select(1,&set,NULL,NULL,&time);
            if(ret>0)
            {
                if(FD_ISSET(0,&set))
                {
                    scanf("%s",cmdd);
                    printf("正在退出。。。\n");
                    return ;

                }
            }
            if(ret==0)
            {
                printf("到达最大显示时间\n");
                return ;
            }
            if(ret<0)
            {
                return ;
            }



            return ;
            

        }


    }


}

void music_get()
{

    int tcp_socket=socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_socket<0)
    {
        perror("");
        return ;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); //HTTP 协议默认的端口就是 80 
    server_addr.sin_addr.s_addr = inet_addr("43.248.190.76"); //服务器的IP地址


    //http://api.qingyunke.com/api.php



    int ret = connect(tcp_socket,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if(ret < 0)
    {
        printf("链接失败\n");
        return ;
    }
    else
    {
        printf("链接成功\n");
    }

    //制定 HTTP 请求协议   
    char  http_head[1024]={0};
    char  cmd[100]={0};
    sprintf(http_head,"GET /api/rand.music?sort=热歌榜&format=json HTTP/1.1\r\nHost:api.uomg.com\r\n\r\n",cmd);
    printf("cmd:%s\n",http_head);
    int i=0;


        //发HTTTP 请求协议  
        char head[10240*100]={0};
        write(tcp_socket,http_head,strlen(http_head));

        // char  head[1024*10] ={0};
        
        //读取回应内容
        int size_one = read(tcp_socket,head,sizeof(head));
        if(strlen(head)==0)
        {
            show_session("断开连接");
            sleep(2);
            return ;
        }
        char * p=strstr(head,"{");
        char *q=strstr(head,"}}");
        int len=(int)(q-p)+strlen("}}");

        char tmp[1024]={0};
        strncpy(tmp,p,len);
        printf("tmp:%s\n",tmp);
        cJSON * obj = cJSON_Parse(tmp);
        if(obj==NULL)
        {
            show_session("数据获取失败，尝试重新获取中...\n");
            sleep(1);
            return ;
            
        }
        else
        {
            printf("正确的json数据\n");
            show_session("跳转中......");
        }
        //获取歌曲信息
        cJSON *data=cJSON_GetObjectItem(obj,"data");
        cJSON *name=cJSON_GetObjectItem(data,"name");
        cJSON *urll=cJSON_GetObjectItem(data,"url");
        cJSON *picurl=cJSON_GetObjectItem(data,"picurl");
        cJSON *artistname=cJSON_GetObjectItem(data,"artistsname");



        char host[100]={0};
        char url[256]={0};
        char *tmp2=strstr(urll->valuestring,"/song");
        //printf("%s,\n",tmp2);
        char *tmp3=strstr(urll->valuestring,"music");
        strncpy(host,tmp3,(int)(tmp2-tmp3));
        printf("host:%s\n",host);
        strncpy(url,tmp2,strlen(tmp2));
        printf("url:%s\n",url);


        //---------------------------------第二次请求
    
        struct hostent *Host;
        Host=gethostbyname(host);

        int tcp_socket1 = socket(AF_INET, SOCK_STREAM, 0);
        if(tcp_socket1<0)
        {
            perror("");
            return ;
        }


        struct sockaddr_in service_addr_1;
        service_addr_1.sin_port = htons(80);
        service_addr_1.sin_family = AF_INET;
        service_addr_1.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)Host->h_addr_list[0])); //服务器的IP地址

        ret = connect(tcp_socket1, (struct sockaddr *)&service_addr_1,sizeof(service_addr_1));
        if (ret == -1 )
        {
            printf("connect lost!!!\n");
            return ;
        }else
        {
            printf("connect success!!!\n");
        }
        
   //制定 HTTP 请求协议   
        char  http_head2[1024]={0};
        sprintf(http_head2,"GET %s HTTP/1.1\r\nHost:%s\r\n\r\n",url,host);
        printf("cmd:%s\n",http_head2);
        int i2=0;
        
    restart2:
    {
        //发HTTTP 请求协议  
        char head2[10240]={0};
        write(tcp_socket1,http_head2,strlen(http_head2));

        // char  head[1024*10] ={0};
        
        //读取回应内容
        size_one = read(tcp_socket1,head2,sizeof(head2));
        if(strlen(head)==0)
        {
            show_session("断开连接");
            sleep(2);
            return ;
        }
        printf("head2:%s\n",head2);



        char *p2_2=strstr(head2,"Location: ");
        p2_2=p2_2+strlen("Location: http://" );
        char *p3_2=strstr(p2_2,"/");
        char host_2[1000]={0};
        int host_2_len=(int)(p3_2-p2_2);
        strncpy(host_2,p2_2,host_2_len);
        char *p4_2 =strstr(p2_2,"\r\n");
        char url_2[256]={0};
        int url_2_len=(int)(p4_2-p3_2);
        strncpy(url_2,p3_2,url_2_len);

        printf("host:%s\turl:%s\n",host_2,url_2);

        char cmd_2[1024]={0};
        


        //--------------------------------------第三次请求
        sprintf(cmd_2,"GET %s HTTP/1.1\r\nHost:%s\r\n\r\n",url_2,host_2);
        printf("%s\n",cmd);


        
        struct hostent *Host_3;
        Host_3=gethostbyname(host_2);

        int tcp_socket3 = socket(AF_INET, SOCK_STREAM, 0);
        if(tcp_socket3<0)
        {
            perror("");
            return ;
        }
            

        struct sockaddr_in service_addr_3;
        service_addr_3.sin_port = htons(80);
        service_addr_3.sin_family = AF_INET;
        service_addr_3.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)Host_3->h_addr_list[0])); //服务器的IP地址

        int ret = connect(tcp_socket3, (struct sockaddr *)&service_addr_3,sizeof(service_addr_3));
        if (ret == -1 )
        {
            printf("connect lost!!!\n");
            return ;
        }else
        {
            printf("connect success!!!\n");
        }
        char buf_3[10240];
        write(tcp_socket3,cmd_2,strlen(cmd_2));

        int size1=read(tcp_socket3,buf_3,sizeof(buf_3));
        printf("%s\n",buf_3);

        

        //处理信息
        int file_size=0;
        //下载的大小 
        int load_size=0;
        
        char *p_3 =  strstr(buf_3,"Content-Length");  //查找关键字
        
        sscanf(p_3,"Content-Length:%d\r\n",&file_size);
        
        printf("file_size=%d\n",file_size);
        
        
        //求出头数据的长度 
        p_3 = strstr(buf_3,"\r\n\r\n");   //回文末尾  
        p_3 = p_3+4; 
        
        int head_len = (int)(p_3 - buf_3);
        printf("head_len = %d\n",head_len);
        int fd=open("./music/1.mp3",O_CREAT|O_TRUNC|O_RDWR);
        
        //写入去头的一次数据 
        int len  =size1-head_len; 
        write(fd,p,len);
        load_size =+  len;
        
        show_session("正在下载");
        while(1)
        {
        //回收HTTP 服务器的消息
            char  buf[40960]={0};
            int size=read(tcp_socket3,buf,4096);
            load_size += size;
            printf("file_size=%d load_size=%d\n",file_size,load_size);
            if(file_size == load_size)
            {
                printf("文件下载完毕\n");
                write(fd,buf,size);
                
                break;
            }
            //把数据写入到本地文件中 
            write(fd,buf,size);
        
            //printf("buf=%s\n",buf);
        }
        
        
        
        close(fd); 
        close(tcp_socket);
        close(tcp_socket3);
        close(tcp_socket1);
        char cmd_3[256]={0};
        sprintf(cmd_3,"%s   %s",name->valuestring,artistname->valuestring);

        show_session(cmd_3);
        system("chmod 777 ./music/1.mp3");
        system("mplayer -slave -quiet ./music/1.mp3 &");
                                ///监控键盘，按任意键退出，或者等待达到   显示时间   20s
        fd_set set;
        FD_ZERO(&set);
        FD_SET(0,&set);
        struct timeval time;
        time.tv_sec=60;
        time.tv_usec=0;
        char cmdd[200];
        ret=select(1,&set,NULL,NULL,&time);
        if(ret>0)
        {
            if(FD_ISSET(0,&set))
            {
                scanf("%s",cmdd);
                system("killall -9 mplayer");
                printf("正在退出。。。\n");
                return ;

            }
        }
        if(ret==0)
        {
            printf("到达最大显示时间\n");
            system("killall -9 mplayer");
            return ;
        }
        if(ret<0)
        {
            return ;
        }



        return ;
    }
}




/*  
    函数名：
            bool find_city_id(char *cmd,city *c)
    参数：
            char cmd : 城市名称
            city *c:   城市结构体（方便读取的，可以只用城市名称对比，返回id值）
    返回值：
            成功        返回true
            失败        返回false
    功能：  
        从文件中把数据读出来，并比对城市名称，然后将对应的id存入结构体中，以便使用

    注意：文件需要UTF-8编码格式的，直接的txt文件并不能直接使用


*/
bool find_city_id(char *cmd,city_info *c)
{

    FILE *fp=fopen("./txt/city.txt","r");//打开文件
    if(fp==NULL)
    {
        printf("文件打开错误！\n");

        sleep(5);

        return false;
    }
    while(1)
    {
        bzero(c->name,50);
        bzero(c->id,20);
        fscanf(fp,"%s %s\n",c->id,c->name);  //格式化读取文件内容并存到对应的结构体变量中
        if(strstr(cmd,c->name))         //比较是否为要查询的城市
        {  
            fclose(fp);
            return true;
        }

        if(feof(fp))        //读取到末尾失败返回
        {
            fclose(fp); 
            return false;
        }

    }


}




void show_day_info(int i,info *day,cJSON *obj)
{
    bzero(day,sizeof(info));
    cJSON * value = cJSON_GetObjectItem(obj,"data");
    cJSON * date_arry  = cJSON_GetObjectItem(value,"forecast");
    int size = cJSON_GetArraySize(date_arry);

    cJSON *day_cjson =cJSON_GetArrayItem(date_arry,i);
    cJSON *high= cJSON_GetObjectItem(day_cjson,"high");
    cJSON *low= cJSON_GetObjectItem(day_cjson,"low");
    cJSON *fx= cJSON_GetObjectItem(day_cjson,"fx");
    cJSON *fl= cJSON_GetObjectItem(day_cjson,"fl");
    cJSON *type= cJSON_GetObjectItem(day_cjson,"type");
    cJSON *aqi= cJSON_GetObjectItem(day_cjson,"aqi");

    char temp_low[50]={0};
    char temp_high[50]={0};

    char *p=strtok(low->valuestring," ");
    p=strtok(NULL," ");
    strcpy(temp_low,p);

    p=strtok(high->valuestring," ");
    p=strtok(NULL," ");
    strcpy(temp_high,p);



    sprintf(day->temp," %s ~ %s",temp_low,temp_high);
    sprintf(day->fx,"  %s",fx->valuestring);
    sprintf(day->wind,"  %s",fl->valuestring);
    sprintf(day->weather_info,"    %s",type->valuestring);


    cJSON *times= cJSON_GetObjectItem(day_cjson,"ymd");
    cJSON *week= cJSON_GetObjectItem(day_cjson,"week");
    day->aqi=aqi->valueint;
    if(day->aqi<50)
    {
        strcpy(day->aqi_status,"  优");
        day->color=getColor(0,128,255,0);
        //                 b,g,r,a
    }
    else if(day->aqi<100)
    {
        strcpy(day->aqi_status,"  良");
        day->color=getColor(0,128,255,0);
    }
    else if(day->aqi<150)
    {
        strcpy(day->aqi_status,"  轻度污染");
        day->color=getColor(0,128,255,255);
    }
    else if(day->aqi<200)
    {
        strcpy(day->aqi_status,"  中度污染");
        day->color=getColor(0,128,128,255);
    }
    else if(day->aqi<2500)
    {
        strcpy(day->aqi_status,"  重度污染");
        day->color=getColor(0,64,128,255);
    }
    else
    {
        strcpy(day->aqi_status,"  严重污染");
        day->color=getColor(0,0,0,255);
    }
    
    

    sprintf(day->week,"  %s",week->valuestring);

    char time[100]={0};
    sprintf(time,"%s",times->valuestring);

    char *p1=strtok(time,"-");
    time_day d_i;
    sprintf(d_i.year,"%s",p1);
    p1=strtok(NULL,"-");
    sprintf(d_i.month,"%s",p1);
    p1=strtok(NULL,"-");
    sprintf(d_i.day,"%s",p1);


    sprintf(day->time," %s月%s日",d_i.month,d_i.day);


}


//未完
bool show_2_weather(info *day,int x_bit)
{
	struct LcdDevice* lcd = init_lcd("/dev/fb0");
			
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f,30);
	
	
	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(133,240,4,getColor(0,104,54,1)); //也可使用createBitmapWithInit函数，改变画板颜色
	//bitmap *bm = createBitmap(288, 100, 4);
	
	
	//将字体写到点阵图上
	fontPrint(f,bm,0,0,day->weather_info,getColor(0,255,255,0),0);
	fontPrint(f,bm,0,35,day->time,getColor(0,255,255,0),0);
	fontPrint(f,bm,0,70,day->week,getColor(0,255,255,0),0);
	fontPrint(f,bm,0,105,day->temp,getColor(0,255,255,0),0);
    fontPrint(f,bm,0,140,day->fx,getColor(0,255,255,0),0);
    fontPrint(f,bm,0,175,day->wind,getColor(0,255,255,0),0);
    fontPrint(f,bm,0,210,day->aqi_status,day->color,0);
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp,x_bit,240,bm);


	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
	





}



void show_weather_bmp(info *day,int x)
{
    lcd_open();
    char name[100];
    int i=1;
    if(strstr(day->weather_info,"晴"))
    {
        sprintf(name,"./weather/%d.jpg",SUNNY);
    }
    else if(strstr(day->weather_info,"多云"))
    {
        sprintf(name,"./weather/%d.jpg",CLOUDY);
    }
    else if(strstr(day->weather_info,"阵雨"))
    {
        sprintf(name,"./weather/%d.jpg",SHOWER);
    }
    else if(strstr(day->weather_info,"阵雪"))
    {
        sprintf(name,"./weather/%d.jpg",SNOW_SHOWER);
    }
    else if(strstr(day->weather_info,"雨"))
    {
        sprintf(name,"./weather/%d.jpg",RAIN);
    }
    else if(strstr(day->weather_info,"阴"))
    {
        sprintf(name,"./weather/%d.jpg",OVERCAST);
    }

    lcd_draw_jpg(x,120,name,0);
    usleep(100);
    lcd_close();

}




bool show_weather_title()
{
	struct LcdDevice* lcd = init_lcd("/dev/fb0");
			
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f,80);
	
	
	//创建一个画板（点阵图）
	bitmap *bm = createBitmapWithInit(800,120,4,getColor(0,104,54,1)); //也可使用createBitmapWithInit函数，改变画板颜色
	//bitmap *bm = createBitmap(288, 100, 4);
	
	
	//将字体写到点阵图上
	fontPrint(f,bm,0,0,"  今天及未来五天的天气情况",getColor(0,255,255,0),0);
	
	
	
	//把字体框输出到LCD屏幕上
	show_font_to_lcd(lcd->mp,0,0,bm);


	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
	
}






int get_pic()
{
    struct hostent *main_host;
    for(int i=0;i<=3;i++)
    {
        if(i==3)
        {
            show_session("出现错误了亲");
            sleep(2);
            return 0;
        }
        //https://v1.alapi.cn/api/acg
        main_host=gethostbyname("www.dmoe.cc");
        if(main_host==NULL)
        {
            continue;
        }
        break;
    }
	int tcp_socket1 = socket(AF_INET, SOCK_STREAM, 0);


    struct sockaddr_in service_addr_1;
    service_addr_1.sin_port = htons(80);
    service_addr_1.sin_family = AF_INET;
    service_addr_1.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)main_host->h_addr_list[0])); //服务器的IP地址

    int ret = connect(tcp_socket1, (struct sockaddr *)&service_addr_1,sizeof(service_addr_1));
        if (ret == -1 )
        {
            printf("connect lost!!!\n");
            return -1;
        }else
        {
            printf("connect success!!!\n");
        }

        

        char *http_head_1 = "GET /random.php HTTP/1.1\r\nHOST:www.dmoe.cc\r\n\r\n";

        write(tcp_socket1,http_head_1,strlen(http_head_1));

        char  head_1[2048*100] ={0};
        read(tcp_socket1,head_1,sizeof(head_1));
        printf("%s\n",head_1);

		char *p = strstr(head_1,"Location: ");
        char *p1 = strstr(head_1,".jpg");
        int txt_len = (int)(p1-p);
        char content[1024] = {0};
        strncpy(content,p,txt_len);
        
        char *ch ="/";
        char *p3 = strtok(content,ch);
        char new_http_head[1024] = {0};
        char yuming[50] ={0};
        char URL[100] ={0};
        
        p3 = strtok(NULL,ch);
        
        strcpy(yuming,p3);
        
        p3 = strtok(NULL,ch);
        
        sprintf(URL,"/%s",p3);
        p3 = strtok(NULL,ch);
        
        sprintf(URL,"%s/%s.jpg",URL,p3);
        

        sprintf(new_http_head,"GET %s HTTP/1.1\r\nHOST:%s\r\n\r\n",URL,yuming);
        struct hostent *host=gethostbyname(yuming);


        printf("%s\n",new_http_head);


        printf("%s\n",host->h_addr_list[0]);


#ifdef DEBUG	
   //1.创建TCP socket  
       
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

  // 2.设置服务器的IP地址并链接    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); //HTTP 协议默认的端口就是 80 
    //server_addr.sin_addr.s_addr = inet_addr("183.60.138.230"); //服务器的IP地址
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)host->h_addr_list[0])); //服务器的IP地址
											//图片的地址 

  // 3.链接 
  int ret1 = connect(tcp_socket,(struct sockaddr *)&server_addr,sizeof(server_addr));
	  if(ret1 < 0)
	  {
		printf("链接失败\n");
		return 0;
	  }
	  else
	  {
		  printf("链接成功\n");
	  }
			
	//制定 HTTP 请求协议   重点！！！ 
	/* char  *http_head = "GET /large/0072Vf1pgy1foxkiswmqaj31kw0w0kh8.jpg HTTP/1.1\r\nHost:tva1.sinaimg.cn\r\n\r\n"; */
														

	//发HTTTP 请求协议  
	write(tcp_socket,new_http_head,strlen(new_http_head));
	
	
	//新建一个文件  


       int fd=open("./download/1.jpeg",O_RDWR|O_CREAT|O_TRUNC,0777);
			if(fd < 0)
			{
				perror("");
				return -1;
			}
	
	//读取头数据  
	char  head[2048]={0}; 
	int size1 = read(tcp_socket,head,2048);  
		printf("size1 = %d\n",size1);
	//取出文件的大小 
	int file_size=0;
	//下载的大小 
	int load_size=0;
	
	char *p10 =  strstr(head,"Content-Length");  //查找关键字
	
	sscanf(p10,"Content-Length:%d\r\n",&file_size);
	
	printf("file_size=%d\n",file_size);
	
	
	//求出头数据的长度 
	p10 = strstr(head,"\r\n\r\n");   //回文末尾  
	p10 = p10+4; 
	
  int head_len = (int)(p10 - head);
      printf("head_len = %d\n",head_len);
	
	//写入去头的一次数据 
	  int len  =size1-head_len; 
	  write(fd,p10,len);
	  load_size =+  len;
	
	
	while(1)
	{
	//回收HTTP 服务器的消息
	char  buf[4096]={0};
	int size=read(tcp_socket,buf,4096);
		load_size += size;
		printf("file_size=%d load_size=%d\n",file_size,load_size);
		if(file_size == load_size)
		{
			printf("文件下载完毕\n");
			write(fd,buf,size);
			
			break;
		}
	     //把数据写入到本地文件中 
		   write(fd,buf,size);
	
		//printf("buf=%s\n",buf);
	}
#endif	
	
	/* close(fd); 
	close(tcp_socket); */
	
}

void pic_download_show()
{
    
    get_pic();


    lcd_open();
    lcd_draw_jpg(0,0,"./download/1.jpeg",1);
    lcd_close();


    fd_set set;
    FD_ZERO(&set);
    FD_SET(0,&set);
    struct timeval time;
    time.tv_sec=20;
    time.tv_usec=0;
    char cmdd[200];
    int ret=select(1,&set,NULL,NULL,&time);
    if(ret>0)
    {
        if(FD_ISSET(0,&set))
        {
            scanf("%s",cmdd);
            printf("正在退出。。。\n");
            return ;

        }
    }
    if(ret==0)
    {
        printf("到达最大显示时间\n");
        return ;
    }
    if(ret<0)
    {
        return ;
    }

}

bool check_ip(char *cmd)
{
    char ip[20]={0};
    if(strlen(cmd)>16)
    {
        return false;
    }
    strcpy(ip,cmd);
    for(int i=0;i<strlen(ip);i++)
    {
        if(ip[i]>'9'||ip[i]<'0')
        {
            if(ip[i]!='.')
                return false;
        }
    }

    char *p=strtok(ip,".");
    if(atoi(p)>255||atoi(p)<0)
    {
        return false;
    }
    while(1)
    {
        p=strtok(NULL,".");
        if(p==NULL)
        {
            break;
        }
        if(atoi(p)>255||atoi(p)<0)
        {
            return false;
        }
    }
    return true;

}





