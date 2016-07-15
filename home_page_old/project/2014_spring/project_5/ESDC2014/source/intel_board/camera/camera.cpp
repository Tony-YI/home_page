
#include "camera.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>


#include <stdio.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <time.h> 
#include <netdb.h>
#include <errno.h> 
#include <signal.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/time.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 




#define DEBUG_MODE 0

using namespace std;

int my_itoa(int val, char* buf);



Camera::Camera()
{
    path_temp = glo_PATH_TEMP;
    path_capture = PATH_CAPTURE;
    count_temp_photo=0;
    count_capture_photo=0;
  
}

Camera::~Camera()
{
     cout<<"camera instance destroyed"<<endl;
}

void Camera::setip(string ip_address)
{
  if (mode==PHONE)
  {
     ip=ip_address;
  }
  else
  {
    printf("!!!Error mode!!! mode:%d should be PHONE\n",mode);
  }
   
}

/*
string Camera::photo()
{
     if(DEBUG_MODE) cout<<"Function:photo()"<<endl;

        int temp_count = count_temp_photo;
          char buffer[20];   
        count_temp_photo++;
        my_itoa(temp_count,buffer);

        string temp(buffer);

        cout << temp<<endl;

     if(DEBUG_MODE) cout<<"Create command"<<endl;

        string path_temp_function = path_temp+temp+".jpg";

             if(DEBUG_MODE) cout<<"set path finished"<<endl;

       string str_command="wget  -O "+path_temp_function+" http://"+ip+"/photo.jpg";

     if(DEBUG_MODE) cout<<"convert command"<<endl;

      const char* command=str_command.c_str();

      if(DEBUG_MODE) cout<<"wget command"<<" "<<command<<endl;

        if ((fp = popen(command, "r")) == NULL) {// fp = popen("wget  -O ~/a.jpg http://192.168.43.1:8080/photoaf.jpg", "r"))
                perror("popen failed");
                // return -1;
            }
          
            if (pclose(fp) == -1) {
                perror("pclose failed");
                // return -2;
            }

        return path_temp_function;
}
*/

string Camera::photo_af()
{
     if(DEBUG_MODE) cout<<"Function:photoaf()"<<endl;

        int temp_count = count_temp_photo;
          char buffer[20];   
        count_temp_photo++;
        my_itoa(temp_count,buffer);

        string temp(buffer);

        cout << temp<<endl;

     if(DEBUG_MODE) cout<<"Create command"<<endl;

        string path_temp_function = path_temp+temp+".jpg";

             if(DEBUG_MODE) cout<<"set path finished"<<endl;

   if(mode==PHONE)

     {  string str_command="wget  -O "+path_temp_function+" http://"+ip+"/photoaf.jpg";
     
          if(DEBUG_MODE) cout<<"convert command"<<endl;
     
           const char* command=str_command.c_str();
     
           if(DEBUG_MODE) cout<<"wget command"<<" "<<command<<endl;
     
            My_popen(command);
     
             return path_temp_function;}

    else if  (mode==CANON)


        { 
         gphoto_cmd="gphoto2 --capture-image-and-download --no-keep --filename "+path_temp_function; //640*480
                  My_popen(gphoto_cmd);
        
                return path_temp_function;}




    
    
}

string Camera::photo_frame()
{
   
        if(DEBUG_MODE) cout<<"Function:photo_JPEG()"<<endl;

        int temp_count = count_temp_photo;
          char buffer[20];   
        count_temp_photo++;
        my_itoa(temp_count,buffer);

        string temp(buffer);

        cout << temp<<endl;

     if(DEBUG_MODE) cout<<"Create command"<<endl;

        string path_temp_function = path_temp+temp+".jpg";

             if(DEBUG_MODE) cout<<"set path finished"<<endl;

     if(mode==PHONE)
{
       string str_command="wget  -O "+path_temp_function+" http://"+ip+"/shot.jpg";

     if(DEBUG_MODE) cout<<"convert command"<<endl;

      const char* command=str_command.c_str();

      if(DEBUG_MODE) cout<<"wget command"<<" "<<command<<endl;
      My_popen(command);

        return path_temp_function;}

     else if(mode==CANON)


         { gphoto_cmd="gphoto2 --capture-preview --no-keep --filename "+path_temp_function; //640*480
                   My_popen(gphoto_cmd);
         
                 return path_temp_function;}
    
}

/*
string Camera::take_photo_af()
{
          if(DEBUG_MODE) cout<<"Function:take_photo_af()"<<endl;

       int temp_count = count_capture_photo;
          char buffer[20];   
        count_capture_photo++;
        my_itoa(temp_count,buffer);

        if(DEBUG_MODE) cout<<"After my_itoa()"<<endl;
        string temp(buffer);



        string path_capture = path_capture+temp+".jpg";
     if(DEBUG_MODE) cout<<"Create command"<<endl;

        string str_command="wget  -O "+path_capture+"http://"+ip+"/photoaf.jpg";
      const char* command=str_command.c_str();

   if(DEBUG_MODE) cout<<"wget command"<<endl;
       My_popen(command);
        return path_capture;
}
*/


int Camera::test_connection()
{
   if (mode==PHONE)
  { 
    int sd=socket(AF_INET,SOCK_STREAM,0); 
    struct sockaddr_in server_addr; 
    memset(&server_addr,0,sizeof(server_addr)); 
    server_addr.sin_family=AF_INET; 
    string ip=IP;
    string port=PORT;
    server_addr.sin_addr.s_addr=inet_addr(ip.c_str()); 
    server_addr.sin_port=htons(atoi(port.c_str())); 
    if(connect(sd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){ 
        printf("connection error: %s (Errno:%d)\n",strerror(errno),errno); 
        close(sd);
       return -1;
    } 
    printf("connect success \n"); 


    char str1[4096];
    memset(str1, 0, 4096); 
    int ret;
    sprintf(str1,"GET / HTTP/1.0\r\nHost:%s\r\nContent-Type=text/plain\r\nConnection:Close\r\n\r\n",ip.c_str());   
    ret = send(sd,(void *)str1,strlen(str1),0); 
    if (ret < 0) 
    { 
        printf("send error %d£¬Error message'%s'\n",errno, strerror(errno)); 
         close(sd);
        return -1;
       
    }
    else
    { 
        printf("send success ,total send %d \n", ret); 
    } 
    
         close(sd);
        return 1;
     
  }
  else
  {
    printf("!!!Error mode!!! mode:%d should be PHONE\n",mode);
  }

    
}


void Camera::zoom(float scaler){
// (scaler>1 in) (scaler< 1 out)

//conver scaler to index
  if(mode==PHONE)
    {
      int index=0;
      if(scaler-1<0.1) index=0;  
    else if(scaler-1.02<0.1) index=1;  
    else if(scaler-1.04<0.1) index=2;  
    else if(scaler-1.09<0.1) index=3;
    else if(scaler-1.11<0.1) index=4;  
    else if(scaler-1.13<0.1) index=5;
    else if(scaler-1.19<0.1) index=6;  
                    else if(scaler-1.21<0.1) index=7;  
                   else if(scaler-1.24<0.1) index=8;  
                   else if(scaler-1.31<0.1) index=9;  
                   else if(scaler-1.34<0.1) index=10;  
                   else if(scaler-1.38<0.1) index=11;  
                   else if(scaler-1.46<0.1) index=12;  
                    else if(scaler-1.5<0.1) index=13;  
                   else if(scaler-1.55<0.1) index=14;  
                   else if(scaler-1.59<0.1) index=15;  
                   else if(scaler-1.65<0.1) index=16;  
                   else if(scaler-1.7<0.1) index=17;  
                   else if(scaler-1.82<0.1) index=18;  
                    else if(scaler-1.89<0.1) index=19;  
                   else if(scaler-2<0.1) index=20;  
                   else if(scaler-2.13<0.1) index=21;  
                   else if(scaler-2.22<0.1) index=22;  
                   else if(scaler-2.32<0.1) index=23;  
                   else if(scaler-2.43<0.1) index=24;  
                       else if(scaler-2.55<0.1) index=25;  
                   else if(scaler-2.83<0.1) index=26;  
                   else if(scaler-3<0.1) index=27;  
                   else if(scaler-3.19<0.1) index=28;  
                   else if(scaler-3.64<0.1) index=29;  
                   else if(scaler-4<0.1) index=30;  
                   else {cout<< "Error:scaler error"<<endl;
                           exit(0);}
              
              
              
              if(DEBUG_MODE) cout<<"Create command"<<endl;
              
                  char buffer[20];   
                  count_capture_photo++;
                  my_itoa(index,buffer);
              
                  string temp(buffer);
              
                  string str_command="wget http://"+ip+"/ptz?zoom="+temp;
                const char* command=str_command.c_str();
              
              if(DEBUG_MODE) cout<<"wget command"<<" " << command<<endl;
                My_popen(command);
              
    }

  else if(mode==CANON)
  {

        int canon_scaler=(int)scaler;
        if (canon_scaler<0||canon_scaler>19)
     { printf("!!!!Error choice: %d  is out of range\n", canon_scaler); exit(0);}

        char buffer[10];   
        my_itoa(canon_scaler,buffer);
        string temp(buffer);


          /*Label: Zoom                                                                    
          Type: MENU
          Current: 0
          Choice: 0 0
          Choice: 1 1
          Choice: 2 2
          Choice: 3 3
          Choice: 4 4
          Choice: 5 5
          Choice: 6 6
          Choice: 7 7
          Choice: 8 8
          Choice: 9 9
          Choice: 10 10
          Choice: 11 11
          Choice: 12 12
          Choice: 13 13
          Choice: 14 14
          Choice: 15 15
          Choice: 16 16
          Choice: 17 17
          Choice: 18 18
          Choice: 19 19*/
    gphoto_cmd="gphoto2 --set-config d02a="+temp;
  }
}



void Camera::save_photo_af()
    {
      if(mode==PHONE)

    { 
      if(DEBUG_MODE) cout<<"Create command"<<endl;
    
            string str_command="wget http://"+ip+"/photoaf_save_only.jpg";
          const char* command=str_command.c_str();
    
          if(DEBUG_MODE) cout<<"wget command"<<" " << command<<endl;
    
            My_popen(command);

          }
        

      else if(mode==CANON) 
     {
      if(DEBUG_MODE) cout<<"Create command"<<endl;
     
           /*Set to high resolution*/
           gphoto_cmd="gphoto2 --set-config d008=0"; //5M
            My_popen(gphoto_cmd);
     
            /*capture and keep*/
            gphoto_cmd="gphoto2 --no-keep --capture-image-and-download"; 
            My_popen(gphoto_cmd);
      }
        else
        {
          printf("!!!Error mode!!!!\n");
                exit(0);
              }


    


    }



/*void Camera::save_photo()
    {


     if(DEBUG_MODE) cout<<"Create command"<<endl;

        string str_command="wget http://"+ip+"/photo_save_only.jpg";
      const char* command=str_command.c_str();

   if(DEBUG_MODE) cout<<"wget command"<<" " << command<<endl;

       My_popen(command);




    }*/


    void Camera::flash_open()
    {

          if(mode==PHONE)
          { 
            if(DEBUG_MODE) cout<<"Create command"<<endl;
          
                  string str_command="wget http://"+ip+"/enabletorch";
                const char* command=str_command.c_str();
          
             if(DEBUG_MODE) cout<<"wget command"<<" " << command<<endl;
          
                 My_popen(command);
          }

          else if(mode==CANON)
          {
              gphoto_cmd="gphoto2 --set-config flashmode=3";
              My_popen(gphoto_cmd);
          }
       


    }

    void Camera::flash_close()
    {

      if(mode==PHONE) 
      {
             if(DEBUG_MODE) cout<<"Create command"<<endl;

                string str_command="wget http://"+ip+"/disabletorch";
              const char* command=str_command.c_str();

           if(DEBUG_MODE) cout<<"wget command"<<" " << command<<endl;

                 My_popen(command);
       }
      else if(mode==CANON)
      {
         gphoto_cmd="gphoto2 --set-config flashmode=4";
          My_popen(gphoto_cmd);
      }
    }

    void Camera::set_qingxin()
    {
     if(mode==CANON)
      {
         gphoto_cmd="gphoto2 --set-config flashmode=4";
          My_popen(gphoto_cmd);
      }
    }

    void Camera::set_fugu()
    {
       if(mode==CANON)
      {
         gphoto_cmd="gphoto2 --set-config flashmode=4";
          My_popen(gphoto_cmd);
      }
    }

 //    void Camera::af_open()
 //    {


 // if(DEBUG_MODE) cout<<"Create command"<<endl;

 //        string str_command="wget http://"+ip+"/focus";
 //      const char* command=str_command.c_str();

 //   if(DEBUG_MODE) cout<<"wget command"<<" " << command<<endl;

 //       My_popen(command);

 //    }


   //  void Camera::af_close()
   //  {

   //  if(DEBUG_MODE) cout<<"Create command"<<endl;

   //    string str_command="wget http://"+ip+"/nofocus";
   //  const char* command=str_command.c_str();

   // if(DEBUG_MODE) cout<<"wget command"<<" " << command<<endl;
   //  My_popen(command);
   //  }






int my_itoa(int val, char* buf)
{
        // if(DEBUG_MODE) cout<<"Function:my_itoa()"<<endl;

        const unsigned int radix = 10;
        char* p;
        unsigned int a; //every digit
        int len;
        char* b; //start of the digit char
        char temp;
        unsigned int u;
        p = buf;
        if (val < 0)
        {
        *p++ = '-';
        val = 0 - val;
        }
        u = (unsigned int)val;
        b = p;
        do
        {
        a = u % radix;
        u /= radix;
        *p++ = a + '0';
        } while (u > 0);
        len = (int)(p - buf);
        *p-- = 0;
        //swap
        do
        {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;
        } while (b < p);
        // if(DEBUG_MODE) cout<<"finished my_itoa()"<<endl;
        return len;
}



 void Camera::set_mode(int mode)
{
        this->mode=mode;

        if(mode==CANON)
        {

            // INIT CANON SETTINGS
           init_canon();


        } 
        else if(mode==PHONE)
        {

           // INIT Phone SETTINGS
            init_phone();
        }
 }


void Camera::init_canon()
{


            /*0.turn on camera*/
            gphoto_cmd="gphoto2 --capture-image";
            My_popen(gphoto_cmd);

            /*1.set imagesize=5M*/
            gphoto_cmd="gphoto2 --set-config d008=2"; //2M
            My_popen(gphoto_cmd);
            

            /*2.set zoom=normal*/
            gphoto_cmd="gphoto2 --set-config d02a=0";
            My_popen(gphoto_cmd);

             /*3.set light=off*/
            flash_close();

            /*4.set iso=auto*/
            gphoto_cmd="gphoto2 --set-config iso=0";
            My_popen(gphoto_cmd);


            /*5.set imagequality=normal*/
            gphoto_cmd="gphoto2 --set-config imagequality=1";
            My_popen(gphoto_cmd);
            
            /*6.set imagesize=large*/
            gphoto_cmd="gphoto2 --set-config imagesize=2";
            My_popen(gphoto_cmd);


}


void Camera::init_phone()
{
    ip=IP_PORT;
}



void Camera::My_popen(std::string cmd)
{
     const char* command=cmd.c_str();
         // if ((fp = popen(command, "r")) == NULL) 

    if ((fp = popen(command, "w")) == NULL) 
    {
          perror("popen failed");
                // return -1;
    }
          
    if (pclose(fp) == -1) 
    {
          perror("pclose failed");
           // return -2;
    }
}





void setchildsignal()
{
  signal(SIGINT,SIG_DFL); 
  signal(SIGTERM,SIG_DFL);  
  signal(SIGQUIT,SIG_DFL);  
  signal(SIGTSTP,SIG_DFL);  
  signal(SIGSTOP,SIG_DFL);  
  signal(SIGKILL,SIG_DFL);      
}


void signal_hander(int status)  
{ //wait for child to ternimate
  int child_status;  
  wait(&child_status);    
  printf("child exited.\n");  
}  

void Camera::start_video()

{


      if((pid = fork())<0)
      {
        printf("--------------------------------\n");
        printf("fork error \n");
        printf("--------------------------------\n");

      }
      
      
     else if( pid != 0 )
    {
    //parent
      signal(SIGCHLD,signal_hander);  

    }
      
     else
     {
      //child
      
      setchildsignal();


      /*capture video*/
      gphoto_cmd="gphoto2 --capture-movie";
      My_popen(gphoto_cmd);
      printf("----------------------------------------\n");
      printf("Error !!!!!!!!!!!!!!!!!!\n");
      printf("should be capturing movie!!!!!!!\n");
      printf("----------------------------------------\n");
      exit(0);
      }//childprocess
}

void Camera::close_video()
{
  printf("--------------------------------------\n");
  printf("ready to kill child\n");
  kill(pid,SIGINT);
  printf("finished killing child\n");
  printf("--------------------------------------\n");

}


 
