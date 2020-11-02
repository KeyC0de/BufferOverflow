#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> // for time_t, size_t 
#include <sys/stat.h>
#include <sys/syscall.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_LOG_ENTRY_SIZE 4096

void help(void) {
    fprintf(stderr, "Usage: ./securelog <YYYY-mm-ddTHH:MM:SS>_<message>\n");
}

// returns length of a fixed-size string, excluding \0 
size_t strnlen(const char *s, size_t maxlen) {
    size_t z;
    for (z=0; z < maxlen && s[z]; z++); // - secure function
    return z;
}

// secure function - no vulnerabilities detected
int check_log_format(char *msg) {
    struct tm logtime;
    // strptime     *converts* input msg string into values stored in logtime tm struct, 
    //              using the format displayed
    char *p = strptime(msg, "%Y-%m-%dT%H:%M:%S", &logtime);

    // if p == NULL error occured, otherwise returns pointer to character following last 
    // character parsed
    if (!p) {
        fprintf(stderr, "Could not match ISO8601 date format\n");
        return 1;
    }

    if (*p != '_') {
        fprintf(stderr, "Could not find delimiter '_'\n");
        return 1;
    }

    return 0;
}

int append_to_file(const char *path, char *data) {
	// returns fd of file specified. The file is opened for writing + appending data to
    int fd = open(path, O_APPEND | O_WRONLY);
    if (fd < 0) {
    	// perror prints to stderr the contents of the null-terminated string pointed to by path
        perror(path);
        return 1;
    }
    size_t size = strlen(data);
    // if not all data were written to secure.log 
    if (write(fd, data, size) != size) {
        perror("write"); // write error
        return 2;
    }
    // add a new line character too
    if (write(fd, "\n", 1) != 1) {
        perror("write");
        return 3;
    }
    return 0;
}

int log_message(char *msg) {
    char buf[MAX_LOG_ENTRY_SIZE];

    // time(NULL)   returns time in seconds since the Epoch (00:00:00 UTC, January 1, 1970)
    time_t now_epoch = time(NULL);
    // localtime()  converts time in seconds since now_epoch into a broken down time / series of 
    //				values fitting for the tm struct (ie. into calendar like time)
    struct tm now = *localtime(&now_epoch);
    // strftime     *creates* string given tm struct into the format specified and stores it into 
    //				buf string of max size 32 chars
    // returns 0    if the resulting string (/0 inclusive) exceeds 32 and buf contents are 
    //				undefined 
    // returns # of bytes read (excluding /0) if resulting string (/0 inclusive) <= 32
    size_t t = strftime(buf, 32, "%Y-%m-%dT%H:%M:%S_", &now);

    size_t u;
    // sprintf		rightmost argument is the input
    //				leftmost argument buffer gets the output
    //              middle argument is the format of the output string / buffer
    // returns      on success = number of bytes in output string (excluding /0)
    //              no error occurs for sprintf
    u = sprintf(buf + t, "%05d_", getuid());

    // strcpy       copies right string argument into address indicated by left string argument
    // 1. bypass security here : i have to intercept this return address where everything has 
    //		been written into buffer.
    // 2. gain root access + cover tracks
    // 3. unlock secure.log
    // 4. enjoy
    strcpy(buf + u + t, msg);

    return append_to_file("./secure.log", buf);
}

// BNm2HQ5KzuSSDhaXFvkCPj83
int main(int argc, char **argv) {

    if (argc <= 1) {
        help();
        exit(1);
    }

    char *msg = argv[1];

    // returns 1 if input string is not properly formatted as described in help()
    if (check_log_format(msg)) {
        fprintf(stderr, "Wrong log format\n");
        help();
        exit(2);
    }

    if (log_message(msg)) {
        fprintf(stderr, "Could not log message\n");
        exit(3);
    }

    return 0;
}


