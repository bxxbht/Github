#include "stdio.h"
#include "stdlib.h"
#include "sqlite3.h"


char dataname[50];
char tablename[50];


int displaycb(void *para,int n_column,char **column_value,char **column_name)
{
        int i = 0;
        printf("Total column is %d\n",n_column);
        for(i = 0;i<n_column;i++)
        {
                printf("字段名: %s---->字段值:%s\n",column_name[i],column_value[i]);

        }
        printf("==========================\n");
        return 0;
}
int inquire_Usecb(sqlite3 *db)
{
        char sql[50];
        char *zErrMsg;
	sprintf(sql,"select * from %s",tablename);
        if(SQLITE_OK != sqlite3_exec(db,sql,displaycb,NULL,&zErrMsg))
        {
                printf("operate failed: %s\n",zErrMsg);
        }
        return 0;
}


int inquire_nocb(sqlite3 *db)
{

        int nrow = 0,ncolumn = 0;
        char **azResult=0;
        int i = 0;
        char sql[50];
        char *zErrMsg;
	sprintf(sql,"select * from %s",tablename);
        if(SQLITE_OK != sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg))
        {
                printf("operate failed: %s\n",zErrMsg);
        }

        printf("row:%d column = %d\n",nrow,ncolumn);
        printf("The result of querying is :\n");
        for ( i = 0;i < ( nrow + 1) * ncolumn; i++)
                printf("azResult[%d] = %s\n",i,azResult[i]);
        sqlite3_free_table(azResult);
        return 0;
}

int main(int argc,char *argv[])
{
	if(argc < 3)
	{
		printf("no database or table!!!\n");
		return 1;
	}


	sprintf(dataname,argv[1]);
	sprintf(tablename,argv[2]);

	printf("database :%s table:%s\n",dataname,tablename);
	sqlite3 *db;
	int rc;
        //打开指定的数据库文件
        rc = sqlite3_open(dataname,&db);
        if(rc)
        {
                fprintf(stderr,"can't open database: %s",sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(1);
        }
        else
                printf("You have opened a sqlite3  %s database successfully!\n",dataname);
	
	printf("==============================================\n");
	printf("inquire nocb\n");
	inquire_nocb(db);
	printf("\n\n\n==============================================\n");
	printf("inqure usecb\n");
	inquire_Usecb(db);
	return 0;
}
