
#include <linux/uinput.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>


char *serial_port = "/dev/ttyACM0";


struct __attribute__((packed)) serial_message{
        int16_t x_axis;
        int16_t y_axis;
        uint16_t button_pushed;
};



int setup_serial(int fd, struct termios *ter){
    
    usleep(3500000);
    tcgetattr(fd, ter);
    
    /* Set baud to 9600 */
    cfsetispeed(ter, B9600);
    cfsetospeed(ter, B9600);
    
    /* Set options */
    /*Input modes */
    // ter.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);


     ter->c_cflag &= ~PARENB;
     ter->c_cflag &= ~CSTOPB;
     ter->c_cflag &= ~CSIZE;
     ter->c_cflag |= CS8;
     /* no hardware flow control */
     ter->c_cflag &= ~CRTSCTS;
     /* enable receiver, ignore status lines */
     ter->c_cflag |= CREAD | CLOCAL;
     /* disable input/output flow control, disable restart chars */
     ter->c_iflag &= ~(IXON | IXOFF | IXANY);
     /* disable canonical input, disable echo,
     disable visually erase chars,
     disable terminal-generated signals */
     ter->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
     /* disable output processing */
     ter->c_oflag &= ~OPOST;

    
    ter->c_cc[VMIN] = sizeof(struct serial_message);
    ter->c_cc[VTIME] = 0;

    /* Write back the new attributes of the serial port*/
    tcsetattr(fd, TCSANOW, ter);

    tcflush(fd, TCIFLUSH);
    usleep(1000*1000);
/*    
    char buf[256];
    int n = read(fd, buf, 128);

    printf("Read %d bytes \n", n);
    printf("Message: %s\n", buf);
*/  
    return EXIT_SUCCESS;

}
 

void emit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

//    gettimeofday(&ie.time, NULL);
//    if (write(fd, &ie, sizeof(ie)) < 0) {
//        perror("Error writing event");
//    }

   write(fd, &ie, sizeof(ie));
}


int main(void){
    /* Open arduino serial port*/
    int fd = open(serial_port, O_RDWR | O_NOCTTY);
    struct termios ter;
    setup_serial(fd, &ter);

    /* Read struct serial_message from Arduino */
    struct serial_message msg;
    int i = 100;
   
   
     /* Setup uinput */ 
    struct uinput_setup utup;
    int fd_uinput = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd_uinput < 0){
        perror("Opening uinput");    
    }
    printf("fd uinput %d \n", fd_uinput);
    sleep(1);
    
    /* Second parameter tells function what to do, and the next parameters will be according to that */
    int io;
    io = ioctl(fd_uinput, UI_SET_EVBIT, EV_KEY);
     if(io < 0){
        perror("io");
       } 
    
    io = ioctl(fd_uinput, UI_SET_KEYBIT, BTN_LEFT);
      if(io < 0){
        perror("io");
       } 
    
   io = ioctl(fd_uinput, UI_SET_EVBIT, EV_REL);
      if(io < 0){
        perror("io");
       } 
   io = ioctl(fd_uinput, UI_SET_RELBIT, REL_X);
      if(io < 0){
        perror("io");
       } 
   io = ioctl(fd_uinput, UI_SET_RELBIT, REL_Y);
      if(io < 0){
        perror("io");
       } 



   memset(&utup, 0, sizeof(utup));
   utup.id.bustype = BUS_USB;
   utup.id.vendor = 0x1234; /* sample vendor */
   utup.id.product = 0x5678; /* sample product */
   strcpy(utup.name, "Example device");

    

    /* Set up device */
   io = ioctl(fd_uinput, UI_DEV_SETUP, &utup);
      if(io < 0){
        perror("io");
       } 
   io = ioctl(fd_uinput, UI_DEV_CREATE);  
      if(io < 0){
        perror("io");
       }   
   
    /* First simulate press down, then tell kernel to act upon that (second line is SYNCHROMIZING)*/ 
 
    
//     while(1){
//        int n = read(fd, &msg, sizeof(struct serial_message));
//        printf("x_axis: %d y_axis: %d pushed: %d \n", msg.x_axis, msg.y_axis, msg.button_pushed);
//          if(msg.button_pushed == 0){
//            printf("Button pushed \n");
//            emit(fd_uinput, EV_KEY, KEY_SPACE, 1);
//            emit(fd_uinput, EV_SYN, SYN_REPORT, 0);
//            /* Now simpulate key to go up */
//            usleep(100000);
//            emit(fd_uinput, EV_KEY, KEY_SPACE, 0);
//            emit(fd_uinput, EV_SYN, SYN_REPORT, 0);
//            sleep(0.1);
//        }   
//    }
    printf("Setting up ...\n");
    sleep(1);
   while (i) {
      int n = read(fd, &msg, sizeof(struct serial_message));
       
//      if(msg.x_axis > 600){
//        emit(fd_uinput, EV_REL, REL_X, msg.x_axis);  
//       }

       // emit(fd_uinput, EV_SYN, SYN_REPORT, 0);
    printf("x axis: %d y_axis: %d button: %d\n ",  msg.x_axis, msg.y_axis, msg.button_pushed);
      emit(fd_uinput, EV_REL, REL_X, msg.x_axis);
      emit(fd_uinput, EV_REL, REL_Y, msg.y_axis);

      /* Button value is 0 if pressed so we set left button to be flippen value; */
      
      emit(fd_uinput, EV_KEY, BTN_LEFT, (msg.button_pushed ^ 1));    
    
      emit(fd_uinput, EV_SYN, SYN_REPORT, 0);
      usleep(15000);
//      printf("in while\n");
//      i--;
    }


//    for(int y = 0; y < 250; y++){
//      emit(fd_uinput, EV_REL, REL_X, 5);
//      emit(fd_uinput, EV_REL, REL_Y, 5);
//      emit(fd_uinput, EV_SYN, SYN_REPORT, 0);
//      usleep(100000);
//    }

     printf("test finished\n");

   
    /* Kill device */
    ioctl(fd_uinput, UI_DEV_DESTROY);
    close(fd_uinput);
    
    
    return 0;

    
    
    

}


