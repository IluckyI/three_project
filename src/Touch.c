#include "Touch.h"







int init_Ts()
{
    int fd_ts=open(EVENT0,O_RDONLY);
    if(fd_ts==-1)
    {
        perror("error:");
        return -1;
    }
    return fd_ts;
}

bool ts_display(int fd_ts,int *x,int *y)
{
    
    struct input_event event;
    int ret;
    int i=1;
    int flag=0;
    while(1)
    {
        ret =read(fd_ts,&event,sizeof(struct input_event));
        switch(event.type)
        {
            //case EV_SYN:printf("--------SYN------\n");break;
            case EV_ABS:
            switch(event.code)
            {
                case ABS_X:*x=(event.value*800)/1024;flag++;break;
                case ABS_Y:*y=(event.value*480)/600;flag++;break;
            }
            case EV_KEY:
            {
                if(event.code == BTN_TOUCH)  //Y轴的坐标点 
                {
                    if(event.value == 1)
                    {
                        //printf("手指按下   x=%d,y=%d\n",x,y);
                    }
                    
                    if(event.value == 0)
                    {
                        printf("(%d,%d)\n",*x,*y);
                        return true;
                    
                    }
                }
            }
        // if(flag==2)
        // {
        //     printf("(%d,%d)\n",*x,*y);
        //     break;            
        //     flag=0;
        // }

        }
    }
    return true;
}


void Ts_exit(int fd)
{
    close(fd);
}