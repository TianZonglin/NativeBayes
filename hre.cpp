#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
//#include <afxsock.h>
#include "mysql.h"
int main(int avgc,char *argv[]){
    MYSQL *conn_ptr;
    conn_ptr = mysql_init(NULL);
    if(!conn_ptr){
        fprintf(stderr,"mysql init failed/n");
        return EXIT_FAILURE;
    }
    conn_ptr = mysql_real_connect(conn_ptr,"172.20.97.228","root","qwertyuiop","mysql",3306,NULL,0);
    if(conn_ptr){
        printf("Connection success/n");
    }else{
        printf("Connection failed/n");
    }
    mysql_close(conn_ptr);
    return EXIT_SUCCESS;
}