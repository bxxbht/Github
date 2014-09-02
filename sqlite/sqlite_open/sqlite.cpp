#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>

int main(void){

sqlite3 *db = NULL;

int rc;

rc = sqlite3_open("/root/test.db",&db);

if(rc){
	sqlite3_close(db);
	printf("open db failed\n");
	exit(1);
}
else{
	printf("open db sucessfully\n");
}
printf("sqlite\n");
return 0;
}
