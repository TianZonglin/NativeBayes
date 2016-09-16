#include <winsock.h>
#include <time.h>
#include <string>
#include <math.h> 
#include <mysql.h>
#include <stdio.h>
#include <cstring>
#include <malloc.h>        
#include <iostream>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")
using namespace std;


#define DEFINE_COUNT 200 //随机数据的条数
#define FLAG_COUNT 1 //随机生成[1]·或·不变[0]
#define FLAG_HIDDEN 1 //函数内的输出显示[1]·或·隐藏[0]


int abc(char *s,string a); 
int find_next( char *start, int flag ); 
void prt_list( char *name,int n, char *str_list );
void Merge(struct Leizu lei[DEFINE_COUNT],MYSQL mydata_1); 
float value(string a,string b);
float S_Value(string classify[200]);
float Record_Distance(string a,string b,MYSQL mydata_1);   
float Entropy_calculate(string classify[200],struct Entropy entro[DEFINE_COUNT],string array[200]);

string traindata[200][200]; //存储所生成的结果集
string state_start[50]; //存储状态起点
string state_end[50]; //存储状态终点
string caution; //输入关注级别
string ; //输入关注级别

int merge_cnt;
int merge_int;
int T=DEFINE_COUNT; //动态聚类类别数目 
int cnt; //生成状态路径的数目
int n;
int sline; //状态的条数
char array[200][100]; //全局数组，存储状态路径

struct Statelist {
    char * start;//路径的起始点
    char * end;//路径的终止点
};
struct Statelist List[30]; 

struct Leizu   
{   
    string input; //此类组的input值   
    string output; //此类组的output值  
	string before; //此类组的before值
	string after; //此类组的after值
	string state1; //此类组的s1值
	string state2; //此类组的s2值
	string state3; //此类组的s3值
	int classify; //此类组的类号值
    int datenum[DEFINE_COUNT+1]; //此组包含的记录行号   
    int counter; //记录数组元素个数	
};   
struct Leizu lei[DEFINE_COUNT+1];  


struct Entropy   
{   
    string ab_name[DEFINE_COUNT]; //代表此'属性'的值
	string c_lei[DEFINE_COUNT]; //代表此'属性'值的类别
    int o_num[DEFINE_COUNT+1]; //记录此'属性'个数  
	int g_count; //记录此'属性'的值的个数  
};   
struct Entropy entro[DEFINE_COUNT+1]; 



//****************************************************************************************************************************************
//********************************************************************/  主函数入口  /****************************************************
int main()
{
     MYSQL mydata;
     if (0 == mysql_library_init(0, NULL, NULL)) {cout << "mysql_library_init() succeed" << endl;} 
	 else { cout << "mysql_library_init() failed" << endl;
     return -1;}

	 if (NULL != mysql_init(&mydata)) { cout << "mysql_init() succeed" << endl;}
	 else { cout << "mysql_init() failed" << endl;
     return -1;}

     if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {cout << "mysql_options() succeed" << endl; } 
	 else {cout << "mysql_options() failed" << endl;
	 return -1;  }
   
	 if(NULL!=mysql_real_connect(&mydata,"localhost","root","","cap",3306,NULL,0)){cout << "mysql_real_connect() succeed" << endl;}
      //这里的地址，用户名，密码，端口可以根据自己本地的情况更改
 	 else { cout << "mysql_real_connect() failed" << endl;
 	 return -1;  }

     ///*************************************************************************/  随机生成数据  /***************/
	 if(FLAG_COUNT)
	 {
		  mysql_query(&mydata,"truncate table n_e_w;");

	cout<<"\n\n开始随机生成数据...\n"; 
		 string sqlstr;   
		 sqlstr += "insert into n_e_w "; 
		 sqlstr += "set input=(SELECT c_name FROM concept ORDER BY RAND() LIMIT 1),";
		 sqlstr += "output=(SELECT c_name FROM concept ORDER BY RAND() LIMIT 1),";
		 sqlstr += "beforer=(SELECT c_name FROM concept ORDER BY RAND() LIMIT 1),";
		 sqlstr += "afterr=(SELECT c_name FROM concept ORDER BY RAND() LIMIT 1),";
		 sqlstr += "s_one=(SELECT s FROM s_table ORDER BY RAND() LIMIT 1),";
		 sqlstr += "s_two=(SELECT s FROM s_table ORDER BY RAND() LIMIT 1),";
		 sqlstr += "s_three=(SELECT s FROM s_table ORDER BY RAND() LIMIT 1);";
		 //printf("*********************************************************\n");
		 for(int t = 0; t < DEFINE_COUNT ; t++)mysql_query(&mydata,sqlstr.c_str());
	} else{cout<<"数据保持不变..."<<endl;}   
	 ///*****************************************************************************/  令before数据与input相同  /*************/
	 MYSQL_RES *r = NULL;
	 mysql_query(&mydata,"SELECT input FROM n_e_w");
	 r = mysql_store_result(&mydata);
	 MYSQL_ROW input = NULL;
     input = mysql_fetch_row(r);
	 char a[3];
	 
	 for(int t = 1; t < 1+DEFINE_COUNT ; t++)
	 {
		 sprintf(a, "%d", t);
		 string str = "update n_e_w set beforer ='";
				str +=input[0];
				str +="'";
				str +=" where Id =";
				str +=a;
				mysql_query(&mydata,str.c_str());  
				input = mysql_fetch_row(r);
	 }
	 mysql_free_result(r);
	 //**************************************************************************/  获得生成的数据集存到traindata  /****************/
        MYSQL_RES *result = NULL;
        mysql_query(&mydata,"SELECT * FROM n_e_w");
       
        result = mysql_store_result(&mydata);//取得并打印行数
        int rowcount = mysql_num_rows(result);
      
        unsigned int fieldcount = mysql_num_fields(result);  //取得并打印各字段的名称

        MYSQL_ROW row = NULL;
        row = mysql_fetch_row(result);
		
	    
		for(int j=0;j<rowcount;j++)
		{ 
			 for(int i = 0; i < fieldcount-1; i++)
			 {
				traindata[j][i] = row[i];
				//cout<<"整个结果集显示："<<traindata[j][i] <<"\t";
			 }	    	 
			
             row = mysql_fetch_row(result);
		}	  	
        
cout<<"\n数据已存储...\n"; 
        //for(int ii=0;ii<DEFINE_COUNT;ii++) cout<<traindata[ii][1]<<endl;
       mysql_free_result(result) ;
       //***************************************************************************************/   查S状态表  存贮路径  /************/


        MYSQL mydata_2;
	    mysql_init(&mydata_2);
		if(NULL==mysql_real_connect(&mydata_2,"localhost","root","","cap",3306,NULL,0))
		{
			cout<<"\nerror: mydata_2 connect failed!\n\n";
			return 0;
		}
	  	string jieguo;

		MYSQL_RES *result_2 = NULL;  
	    MYSQL_ROW row_2 = NULL;
		
		MYSQL_RES *result_1 = NULL;  
	    MYSQL_ROW row_1 = NULL;

        mysql_query(&mydata_2,"select started from s_relation");
		result_2 = mysql_store_result(&mydata_2); 
		mysql_query(&mydata_2,"select ended from s_relation");
		result_1 = mysql_store_result(&mydata_2);
		row_2 = mysql_fetch_row(result_2);  
		row_1 = mysql_fetch_row(result_1); 

		int rowcount_2 = mysql_num_rows(result_2);
		for(int i = 0;i < rowcount_2; i++) //获得起始点，终止点，分别存数组
		{
			state_start[i] = row_2[0];
			state_end[i] = row_1[0];
			//cout<<"起始状态-> 终止状态"<<state_start[i] <<" -> "<<state_end[i]<<endl;    	   
			row_2 = mysql_fetch_row(result_2);
			row_1 = mysql_fetch_row(result_1);
		}	  	
        int q=0,fg=1;
		string aaa[30];
cout<<"\n状态起始点已获取...\n"; 
		sline = rowcount_2;
	    for(i=0;i<rowcount_2;i++)//查重后获取 无重复的起始点->放入aaa[]
		{
			fg=1;
   		    for(int k=0;k<i;k++){
				if(state_start[i]==state_start[k]) fg=0;
			}
			if(fg){
				aaa[q++]=state_start[i];
			}
		}
		//显示无重复起点for(i=0;i<q;i++)cout<<"-> "<<aaa[i]<<endl;

		
		for(j=0;j<rowcount_2;j++)
		{
			List[j].start=(char *)state_start[j].c_str();
			List[j].end=(char *)state_end[j].c_str();
		}
		char *name[50];
cout<<"\n开始查询状态路径...\n"; 
		for ( i=0;i<q;i++ )name[i]=(char *)aaa[i].c_str();
		for ( i=0;i<q;i++ ) //循环嵌套查路径，调用prt_list()函数
		{
			char buf[100];
			strcpy( buf, name[i] );
			prt_list( name[i],0, buf );
		}
		//输出数组for( i=0;i<cnt;i++ )cout<<array[i]<<endl;

cout<<"\n已获取路径...\n"; 
	mysql_free_result(result_1); 
	mysql_free_result(result_2); 
   //*************************************************************************************************/  层次聚类分析出口  /*************/


     MYSQL mydata_1;
 	 mysql_init(&mydata_1);
	 if(NULL==mysql_real_connect(&mydata_1,"localhost","root","","cap",3306,NULL,0))
	 {
		 cout<<"\nerror: mydata_1 connect failed!\n\n";
		 return 0;
	 }

     ////********************************************************************************************/  
     int p,x; 
     cout<<"\n-------------------------------\n";   
     cout<<"层次聚类开始....\n";   
     cout<<"-------------------------------\n";   
     cout<<"\n请输入你要分成的类别数:    ";  cin>>n;
	 cout<<"\n分类区分度字符串对照：\n[inter]->intersection  [sub]->subClassOf  \n[super]->superClassOf  [equ]->equivalentOf";  
	 cout<<"\n请输入简写：   ";  cin>>caution;
     merge_cnt = 0;
	 merge_int = 0;
     clock_t begin, end;   
 	 double  cost;
	 begin = clock(); //计时函数开始
     int k1;   
     for(k1=1;k1<=DEFINE_COUNT;k1++)//聚类类组的初始化   
     {   
        lei[k1].input = traindata[k1-1][1];   
        lei[k1].output = traindata[k1-1][2];
	    lei[k1].before = traindata[k1-1][3];
	    lei[k1].after = traindata[k1-1][5];
		lei[k1].state1 = traindata[k1-1][4];
		lei[k1].state2 = traindata[k1-1][6];
		lei[k1].state3 = traindata[k1-1][7];
        lei[k1].datenum[1] = k1 ;   
        lei[k1].counter = 1;     
     }   
     if(n==1)   
     {  
		 cout<<endl<<"第1个类即全部("<<DEFINE_COUNT<<"条)："<<endl;   
         for(int u=1;u<=DEFINE_COUNT;u++)   
         cout<<u<<"\t";   
     }   
     else   
	 {   
         for(x=1;x<=(DEFINE_COUNT-n);x++)   
		 {  
			++merge_int;
            Merge(lei,mydata_1);   
            T=T-1;
		   	
		 }   

         char x[3],y[3];
         
         for(i=1;i<=n;i++) //类组元素输出     
		 {   
			 printf("\n第%d个类包含的记录(%d条)：\n",i,lei[i].counter);   
   
			 for(j=1;j<=DEFINE_COUNT;j++)   
			 {   
				p=lei[i].datenum[j];   
			    if(p) //将类别写回数据库的表中  
				{   
					sprintf(x, "%d", i);
					sprintf(y, "%d", p);
					string str2 = "update n_e_w set classify = ";
						   str2 += x;
						   str2 += " where id = ";
						   str2 += y;
						   str2 += ";";
					mysql_query(&mydata_1,str2.c_str());
					lei[i].classify = i;
					printf(" %d   ",p); 
					
				}   
			 }   
		 }   
	 }


	 end = clock();
     cost = (double)(end - begin) / CLOCKS_PER_SEC;
     printf("\n************************ Time: %lf seconds  ********************\n\n", cost);


    //****************************************************************/ 查出分类信息 到classify[] /***********/

	 string classify[200];
	 MYSQL_RES *result_4 = NULL;  
     MYSQL_ROW row_4 = NULL; 
 
	 mysql_query(&mydata_2,"select classify from n_e_w"); 

	 result_4 = mysql_store_result(&mydata_2);
	 row_4 = mysql_fetch_row(result_4); 
 
	 for(i = 0;i < mysql_num_rows(result_4); i++)
	 { 
		    classify[i] = row_4[0];
			//cout<<"分类具体类别："<<classify[i] <<"\t";    	 
			//printf("*****\n");  
			row_4 = mysql_fetch_row(result_4);
	 }	  
  ///******************************************************************/  信息增益出口  /***************/   
cout<<"\n开始计算信息增益...\n"; 
	
     string vv[200];
	 string str_list[]={"","Input","Output","Before","S_one","After","S_two","S_three"};
	 for(int zt = 1;zt <= 7; zt++)
	 {
		 for(i = 0;i < DEFINE_COUNT; i++) {vv[i] = traindata[i][zt] ;} 
		 cout<<"属性"<<str_list[zt]<<"的信息增益："<<Entropy_calculate(classify,entro,vv)<<endl;
		 memset(entro,0,sizeof(Entropy)*DEFINE_COUNT);
	 }

     mysql_free_result(result_4);

cout<<"\n训练数据阶段到此完成...\n\n"; 
	 return 0;
}


//************************************************************************************************************************************
                                                        // 功能函数 //
//************************************************************************************************************************************

//*************************************************************/  返回记录"间距",服务于Merge函数  /************/
float Record_Distance(string a,string b,MYSQL mydata_1)
{	
if(FLAG_HIDDEN)cout<<"record_inside num: "<<++merge_cnt<<"   "<<merge_int<<endl;
	 double k=0; 
     string  aaa;
	 string  sql;
        
	 MYSQL_RES *result_1 = NULL;  
	 MYSQL_ROW row_1 = NULL;  
        		
	 
	 sql+="select ifnull(r_name,'0') as r_name  ";
	 sql+="from concepterelation  ";
	 sql+="where cname_1='";
	 sql+=a;
	 sql+="' and cname_2='";
	 sql+=b;
	 sql+="'"; 		    
     mysql_query(&mydata_1, sql.c_str());

		
	 result_1 = mysql_store_result(&mydata_1); 
     row_1 = mysql_fetch_row(result_1);

	 if(!mysql_num_rows(result_1)){aaa="fail";}
	 else aaa=row_1[0]; //查得关系替代词，对应赋值  
	 if(caution=="inter")
	 {
		 if(aaa=="equivalentOf")  k=-1.0;
		 if(aaa=="superClassOf")  k=-0.8;
		 if(aaa=="subClassOf")    k=-0.6;
		 if(aaa=="intersection")  k=-0.4;
		 if(aaa=="fail")          k=-0; 
	 }
	 if(caution=="sub")
	 {
		 if(aaa=="equivalentOf")  k=-1.0;
		 if(aaa=="superClassOf")  k=-0.8;
		 if(aaa=="subClassOf")    k=-0.6;
		 if(aaa=="intersection")  k=-0;
		 if(aaa=="fail")          k=-0; 
	 }
	 if(caution=="super")
	 {
		 if(aaa=="equivalentOf")  k=-1.0;
		 if(aaa=="superClassOf")  k=-0.8;
		 if(aaa=="subClassOf")    k=-0;
		 if(aaa=="intersection")  k=-0;
		 if(aaa=="fail")          k=-0; 
	 }
	 if(caution=="equ")
	 {cout<<"equ";
		 if(aaa=="equivalentOf")  k=-1.0;
		 if(aaa=="superClassOf")  k=-0.01;
		 if(aaa=="subClassOf")    k=-0.01;
		 if(aaa=="intersection")  k=-0.01;
		 if(aaa=="fail")          k=-0.01; 
	 }
    mysql_free_result(result_1);
	return k;
}	 

//*************************************************************/  进行聚类，每次合并  /************/
void Merge(struct Leizu lei[DEFINE_COUNT],MYSQL mydata_1)   
{      
if(FLAG_HIDDEN)cout<<"\n-------------------开始聚类[function]\n";
    float **m;   
    m = (float**)malloc((T+1)*sizeof(float*));   
    for(int ii = 0; ii < T+1; ii++ )   
    {   
        m[ii] = (float*)malloc((T+1)*sizeof(float));   
    }   
   
    int i,j,i1,j1,st1,st2,p1,p2,p4,p3; //st1，st2为要合并的类别   
    double a1,a2,a3,a4,a5,a6,a7,c=0,min;   
    //建立距离距阵  
    for(i=1;i<=T;i++)   
    {   
        for(j=i;j<=T;j++)   
        {  

            a1 = Record_Distance(lei[i].input , lei[j].input,mydata_1); 
            a2 = Record_Distance(lei[i].output , lei[j].output,mydata_1); 
			a3 = Record_Distance(lei[i].before , lei[j].before,mydata_1); 
			a4 = Record_Distance(lei[i].after , lei[j].after,mydata_1);
			a5 = value(lei[i].state1 , lei[j].state1); 
			a6 = value(lei[i].state2 , lei[j].state2);
			a7 = value(lei[i].state3 , lei[j].state3);
            //cout<<"前后状态及关系值：<<lei[i].state1<<" "<<lei[j].state1<<"-> "<<a5<<endl;
			//cout<<"前后状态及关系值：<<lei[i].state2<<" "<<lei[j].state2<<"-> "<<a6<<endl;
			//cout<<"前后状态及关系值：<<lei[i].state3<<" "<<lei[j].state3<<"-> "<<a7<<endl;
            c = (-0.5)*float(a1+a2+a3+a4+a5+a6+a7);   
            m[j][i] = c;
            m[i][j] = c;
        }   
     }   
	 
     min=m[1][2];/////////////////////////////【动态的阀值，如果定死可能会有影响】  
	 
     //查找最小距离类组   
     for(i1=1;i1<=T;i1++)   
     {   
         for(j1=i1+1;j1<=T;j1++)   
         {   
            if(m[i1][j1]<=min)   
            {   
               min=m[i1][j1];   
               st1=i1;   
               st2=j1;  
            }
			else continue;
         }   
	 }   
     //cout<<"合并的类组号："<<st1<<" "<<st2<<endl;
    
	 //合并最小距离的类组 
     if(st1>=st2)   
     {   
         p1=st1;   
         st2=st1;   
         st1=p1;   
     }   
     p3=lei[st1].counter;   
     lei[st1].input=lei[st2].input; 
     lei[st1].output=lei[st2].output; 
	 lei[st1].before=lei[st2].before;
	 lei[st1].after=lei[st2].after;
     lei[st1].state1=lei[st2].state1;
	 lei[st1].state2=lei[st2].state2;
	 lei[st1].state3=lei[st2].state3;
	  
if(FLAG_HIDDEN)cout<<"-------------------数目累加[function]\n";   
     for(p2=1;p2<=lei[st2].counter;p2++)   
     {   
         p3++;   
         lei[st1].datenum[p3]=lei[st2].datenum[p2];   
     }   
   
   
     lei[st1].counter=lei[st1].counter+lei[st2].counter;
	 
if(FLAG_HIDDEN)cout<<"-------------------对象调整[function]\n";

     for(p4=st2;p4<T;p4++) //类组对象调整，后面留出空位   
     {   
		 //cout<<"类组对象调整："<<p4<<" "<<p4+1<<" "<<T<<endl;
         lei[p4]=lei[p4+1]; 
		 
     } 

	for(i=0;i<T+1;i++){ free(m[i]);}
    free(m); 
if(FLAG_HIDDEN)cout<<"-------------------聚类完成[function]\n";      
}   
	





//*********************************************************************/ S状态路径，返回计算值 /****************/
int abc(char *s,string a)
{
	char *str = new char[strlen(s)+1];
    memcpy(str, s, strlen(s));
    str[strlen(s)] = 0;
    char *sPtr = strtok(str, "s");
    int i=0;
    while (sPtr != NULL) {
        ++i;
        if ( strcmp(sPtr,a.substr(1).c_str()) == 0) {
            //cout <<"距离起始点间隔："<<i<<endl;
			 delete []str; return i; 
        }
		
        sPtr = strtok(NULL, "s");
    }

    delete []str;   
	return 0; 
	
}

int find_next( char *start, int flag )
{
    int i,k=-1;
    for( i=0;i<sline;i++ )
    {
        if ( !strcmp(List[i].start,start ) )
        {
            k++;
            if ( flag==k )
                return i;
        }
    }
    return -1;
}


void prt_list( char *name,int n, char *str_list )
{
    if ( n >= 4 )  //这里如果4变的越大，重复性越多
    {
        //cout<<"输出整路径："<<str_list<<endl;
        strcpy( array[cnt++], str_list ); 
        return ;
    }
    int flag=0;
    for( flag=0;;flag++) //找相应的后继结点，当前根据情况，设定为两个
    {
        int i=find_next( name,flag );
        char buf[100];
        if( i<0 )
        {
			 if ( flag==0 )
               strcpy( array[cnt++], str_list );
            break;
        }
        sprintf( buf, "%s%s", str_list,List[i].end  );
        prt_list( List[i].end,n+1, buf ); //输出后继
    }
}


float value(string a,string b)
{
//cout<<"-------------------计算状态关系值[function]\n"; 
	if(a==b)return 1;
	else
	{
		//cout<<"生成路径的条数："<<cnt<<endl;
		int mm[20],p=0;
		for(int i=0;i<cnt;i++)
		{
			int k = abc(array[i],a);
			if(k==1) 
			{
				int n = abc(array[i],b);
				if(n) mm[p++] = n;			
			}
			else continue;
		}
		sort(&mm[0],&mm[p]);
		//for(i=0;i<p;i++)cout<<mm[i]<<endl<<endl;
		if(p)return (float)1/mm[0];
		else return 0;
	}
}





//************************************************************************************/  计算节点的熵，含计算属性的熵  /************/
float Entropy_calculate(string classify[200],struct Entropy entro[DEFINE_COUNT],string array[200])
{
	int p = 2,b=0;
	for(int k1 = 0; k1 < DEFINE_COUNT; k1++)
	{
		entro[k1].ab_name[1] = array[k1];  
		entro[k1].o_num[1] = k1+1;
		entro[k1].c_lei[1] = classify[k1];
		entro[k1].g_count = 1;
	}
	for(int i = 0; i < DEFINE_COUNT; i++) //循环每条记录找同类
	{
		for(int j = i+1; j < DEFINE_COUNT; j++)
		{
			if(entro[i].ab_name[1] == entro[j].ab_name[1])//合并记录 获得属性
			{
				entro[i].ab_name[p] = entro[j].ab_name[1];
				entro[i].o_num[p] = entro[j].o_num[1];
				entro[i].c_lei[p] = entro[j].c_lei[1];
				entro[i].g_count += entro[j].g_count; 
				for(int k = j; k < DEFINE_COUNT; k++)//腾出空间
				{
					entro[k].ab_name[1] = entro[k+1].ab_name[1];
					entro[k].o_num[1] = entro[k+1].o_num[1];
					entro[k].c_lei[1] = entro[k+1].c_lei[1];
					entro[k].g_count = entro[k+1].g_count;
				}
				p = p + 1;
			}
		}
	    p = 2;
	}

    for(i = 0;i < DEFINE_COUNT;i++){if(entro[i].g_count)b++;}//得到属性数目
	//cout<<"可划分属性的个数："<<b<<endl;
   
	int flag = 1,count = 0;
	

	float all_value = 0;
	/////////////////////////////////////////////计算·属性的·熵
if(FLAG_HIDDEN)cout<<"-------------------得到属性的熵[function]\n"; 
    for(i = 0 ; i < b ; i ++)
	{ 
		float adder = 0;
		int size = entro[i].g_count;
		if(size == 1){adder = 0;}
		else
		{
			for(int j = 1;j <= size; j++)
			{				
				count=0;flag=1;
				for(int k = 1;k < j; k++){
				        //cout<<entro[i].c_lei[j]<<"  查重  "<<entro[i].c_lei[k]<<endl;
						if(j!=0 && entro[i].c_lei[j]==entro[i].c_lei[k]) flag = 0;					 
				}
				//cout<<"查重标志位："<<flag<<endl;
                if(flag){ 
					for(int p = j;p <= size; p++){
						//cout<<entro[i].c_lei[p]<<"  计数  "<<entro[i].c_lei[j]<<endl;
						if(entro[i].c_lei[p]==entro[i].c_lei[j]) count++;
					}
				
					if(count==0) adder = 0;
					else{
						//cout<<"属性中个数："<<size<<endl;
						//cout<<"本次的同类："<<count<<endl;
						//cout<<"分式中间式："<<(float)count/size<<endl;
						//cout<<"对数分式值："<<(log((float)count/size)/log(2))<<endl;
						adder +=  (float)count/size *(log((float)count/size)/log(2));
						//cout<<"每次的结果："<<adder<<endl;
						 		
					}
				}
			}
		}
if(FLAG_HIDDEN)cout<<"-------------------得到节点的熵[function]\n"; 
		float m = (float)entro[i].g_count/DEFINE_COUNT;
		//cout<<"此属性概率："<<m<<endl;
		all_value += float(m*adder);
		//cout<<"项累加得熵："<<all_value<<endl;
	}
    //cout<<"最终类的熵："<<S_Value(classify)<<endl;

if(FLAG_HIDDEN)cout<<"-------------------返回节点的信息增益[function]\n";  
return S_Value(classify)+all_value;//得到信息增益
}


//***********************************************************************/  计算'类别'的熵，服务于Entropy_calculate /******************/
float S_Value(string classify[200])
{
	int num=0,flag;
	for(int i = 0;i < DEFINE_COUNT; i++)
	{
		flag=1;
		for(int j = 0;j < i; j++)
		{
			if(classify[i] == classify[j])flag=0;
		}
		if(flag)num++;

	}
	//cout<<"分类数目："<<num<<endl;

    float s_value=0;
	
    for(i = 1;i <= num ; i++)
	{
		char b[3];
		int count=0;
		sprintf(b, "%d", i);
		for(int j = 0;j <= DEFINE_COUNT;j++)
		{
			if(b == classify[j])count++;
		}
		//cout<<"最终中项："<<(float)count/DEFINE_COUNT<<endl;
	    //cout<<"对数式值："<<log((float)count/DEFINE_COUNT)/log(2)<<endl;
		//cout<<"中间乘积："<<((float)count/DEFINE_COUNT)*(log((float)count/DEFINE_COUNT)/log(2))<<endl;
		s_value += ((float)count/DEFINE_COUNT)*(log((float)count/DEFINE_COUNT)/log(2));
		
	}
if(FLAG_HIDDEN)cout<<"-------------------得到类别的熵[function]\n"; 
    s_value = (-1)*s_value;
	return s_value;

}


//*************************************************************/   END   /***************************************************************//