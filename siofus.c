#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>


int main(int argc, char *argv[])
{
    // see attached udev rule ;)
    char *portName = "/dev/siofus";

    //  if key "a" is pressed send +5V to PIN4 (DTR)
    char *key1 = "a";
    //  if key "s" is pressed send -5V to PIN4 (DTR)
    char *key2 = "s";
    //  if key "d" is pressed send +5V to PIN7 (RTS)
    char *key3 = "d";
    //  if key "f" is pressed send -5V to PIN7 (RTS)
    char *key4 = "f";
	
    int mainDelay = 2000;
    int waitAfterInput = 90000;    
    
    int opt;
    
    while ((opt=getopt(argc, argv, "p:d:D:"))>0) {
        switch (opt) {
            case '?':
                printf("Bad argument");
            break;
            case 'p':
                portName="\0";
                portName=malloc(strlen(optarg));
                strcpy(portName,optarg);
            break;
            case 'd':
                mainDelay=atoi(optarg);
            break;
            case 'D':
                waitAfterInput=atoi(optarg);
            break;
            default:
            break;
        }

    }

    // Get parameters
    /* Setup serial parameters */
    struct termios term = {
        0,
        0,
        B1200 | CS8 | CREAD | CLOCAL,
        0,
        N_TTY,
        { 0x03, 0x1c, 0x08, 0x15, 0x04, 0x00, 0x00, 0x00, 0x11,
            0x13, 0x1a, 0x00, 0x12, 0x0f, 0x17, 0x00, 0x00 },
    };

    // set unbuffered stdout
    setvbuf(stdout, NULL, _IONBF, 0);

    int keyPressed, fd, errno;

    if ((fd = open( portName, O_RDWR | O_NDELAY | O_NONBLOCK)) < 0) {
        printf("Unable to open %s: %s\n", portName, strerror(errno));
        exit(1);
    }

    int bitsRead=0;
    int returnVal;

    //printf("READY!\n");
    fflush(0); 

    int cur1 = 0, cur2 = 0, cur3 = 0, cur4 = 0;
    int prev1 = 0, prev2 = 0, prev3 = 0, prev4 = 0;
    int status = 0;

    struct termios oldt, newt;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    ioctl( fd, TCSETSW, &term );

    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Main Loop
    while(1) {

        /*****************
         **    INPUT    **
         *****************/
        // READ INPUT from:
        //      PIN1 (DCD)
        //      PIN6 (DSR)
        //      PIN8 (CTS)
        //      PIN9 (RNG)
        //
        // Use USB Pin1 (5V) to send signal to each INPUT PIN.

        bitsRead=0;
        returnVal = ioctl(fd, TIOCMGET, &bitsRead);
        if (returnVal == -1) {
                printf("Invalid read.");
                exit(2);
        }


        // PIN 1 (DCD)
        cur1 = (bitsRead & TIOCM_CAR)/TIOCM_CAR;
        if (cur1 != prev1) {
            if (cur1 == 0) {
                //INPUT0 == open
                printf("#O0#\n");
            } else {
                //INPUT0 == close
                printf("#C0#\n");
            }
            usleep(waitAfterInput);
        }

        // PIN 6 (DSR)
        cur2 = (bitsRead & TIOCM_DSR)/TIOCM_DSR;
        if (cur2 != prev2) {
            if (cur2 == 0) {
                printf("#O1#\n");
            } else {
                printf("#C1#\n");
            }
            usleep(waitAfterInput);
        }

        // PIN 8 (CTS)
        cur3 = (bitsRead & TIOCM_CTS)/TIOCM_CTS;
        if (cur3 != prev3) {
            if (cur3 == 0) {
                printf("#O2#\n");
            } else {
                printf("#C2#\n");
            }
            usleep(waitAfterInput);
        }

        // PIN 9 (RNG)
        cur4 = (bitsRead & TIOCM_RNG)/TIOCM_RNG;
        if (cur4 != prev4) {
            if (cur4 == 0) {
                printf("#O3#\n");
            } else {
                printf("#C3#\n");
            }
            usleep(waitAfterInput);
        }

        prev1 = cur1;
        prev2 = cur2;
        prev3 = cur3;
        prev4 = cur4;


        /******************
         **    OUTPUT    **
         ******************/

        // WRITE OUTPUT based on key presses
        //  if key "a" is pressed send +5V to PIN4 (DTR)
        //  if key "s" is pressed send -5V to PIN4 (DTR)
        //  if key "d" is pressed send +5V to PIN7 (RTS)
        //  if key "f" is pressed send -5V to PIN7 (RTS)
        //  ~0.05A

        // NONBLOCK READ
        keyPressed = getchar();

        // On by default!
        status |= TIOCM_DTR;
        status |= TIOCM_RTS;

        if (keyPressed != EOF) { 
            
            if (keyPressed == 'a') {
                printf("OUTPUT#ON O1\n");
                
            }
            if (keyPressed == 's') {
                status &= ~TIOCM_DTR;
                printf("OUTPUT#OFF O1\n");
            }

            if (keyPressed == 'd') {
                printf("OUTPUT#ON O2\n");
            }
            if (keyPressed == 'f') {
                printf("OUTPUT#OFF O2\n");
                status &= ~TIOCM_RTS;	
            }
        }

        ioctl(fd, TIOCMSET, &status);
        usleep(mainDelay);

    } // Main Loop

    exit(0);

}

