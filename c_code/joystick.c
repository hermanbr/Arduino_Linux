#include <linux/uinput.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>



char *serial_port = "/dev/ttyACM0";


struct __attribute__((packed)) serial_message{
        uint16_t x_axis;
        uint16_t y_axis;
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
 



int main(void){
    /* Open arduino serial port*/
    int fd = open(serial_port, O_RDWR | O_NOCTTY);
    struct termios ter;
    setup_serial(fd, &ter);

    /* Read struct serial_message from Arduino */
    struct serial_message msg;
    
    while(1){
        int n = read(fd, &msg, sizeof(struct serial_message));
        printf("x_axis: %d y_axis: %d pushed: %d \n", msg.x_axis, msg.y_axis, msg.button_pushed);
    }
    


    return 0;

}


