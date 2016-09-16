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


#define DEFINE_COUNT 200 //������ݵ�����
#define FLAG_COUNT 1 //�������[1]���򡤲���[0]
#define FLAG_HIDDEN 1 //�����ڵ������ʾ[1]��������[0]


int abc(char *s,string a); 
int find_next( char *start, int flag ); 
void prt_list( char *name,int n, char *str_list );
void Merge(struct Leizu lei[DEFINE_COUNT],MYSQL mydata_1); 
float value(string a,string b);
float S_Value(string classify[200]);
float Record_Distance(string a,string b,MYSQL mydata_1);   
float Entropy_calculate(string classify[200],struct Entropy entro[DEFINE_COUNT],string array[200]);

string traindata[200][200]; //�洢�����ɵĽ����
string state_start[50]; //�洢״̬���
string state_end[50]; //�洢״̬�յ�
string caution; //�����ע����
string ; //�����ע����

int merge_cnt;
int merge_int;
int T=DEFINE_COUNT; //��̬���������Ŀ 
int cnt; //����״̬·������Ŀ
int n;
int sline; //״̬������
char array[200][100]; //ȫ�����飬�洢״̬·��

struct Statelist {
    char * start;//·������ʼ��
    char * end;//·������ֹ��
};
struct Statelist List[30]; 

struct Leizu   
{   
    string input; //�������inputֵ   
    string output; //�������outputֵ  
	string before; //�������beforeֵ
	string after; //�������afterֵ
	string state1; //�������s1ֵ
	string state2; //�������s2ֵ
	string state3; //�������s3ֵ
	int classify; //����������ֵ
    int datenum[DEFINE_COUNT+1]; //��������ļ�¼�к�   
    int counter; //��¼����Ԫ�ظ���	
};   
struct Leizu lei[DEFINE_COUNT+1];  


struct Entropy   
{   
    string ab_name[DEFINE_COUNT]; //�����'����'��ֵ
	string c_lei[DEFINE_COUNT]; //�����'����'ֵ�����
    int o_num[DEFINE_COUNT+1]; //��¼��'����'����  
	int g_count; //��¼��'����'��ֵ�ĸ���  
};   
struct Entropy entro[DEFINE_COUNT+1]; 



//****************************************************************************************************************************************
//********************************************************************/  ���������  /****************************************************
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
      //����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������
 	 else { cout << "mysql_real_connect() failed" << endl;
 	 return -1;  }

     ///*************************************************************************/  �����������  /***************/
	 if(FLAG_COUNT)
	 {
		  mysql_query(&mydata,"truncate table n_e_w;");

	cout<<"\n\n��ʼ�����������...\n"; 
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
	} else{cout<<"���ݱ��ֲ���..."<<endl;}   
	 ///*****************************************************************************/  ��before������input��ͬ  /*************/
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
	 //**************************************************************************/  ������ɵ����ݼ��浽traindata  /****************/
        MYSQL_RES *result = NULL;
        mysql_query(&mydata,"SELECT * FROM n_e_w");
       
        result = mysql_store_result(&mydata);//ȡ�ò���ӡ����
        int rowcount = mysql_num_rows(result);
      
        unsigned int fieldcount = mysql_num_fields(result);  //ȡ�ò���ӡ���ֶε�����

        MYSQL_ROW row = NULL;
        row = mysql_fetch_row(result);
		
	    
		for(int j=0;j<rowcount;j++)
		{ 
			 for(int i = 0; i < fieldcount-1; i++)
			 {
				traindata[j][i] = row[i];
				//cout<<"�����������ʾ��"<<traindata[j][i] <<"\t";
			 }	    	 
			
             row = mysql_fetch_row(result);
		}	  	
        
cout<<"\n�����Ѵ洢...\n"; 
        //for(int ii=0;ii<DEFINE_COUNT;ii++) cout<<traindata[ii][1]<<endl;
       mysql_free_result(result) ;
       //***************************************************************************************/   ��S״̬��  ����·��  /************/


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
		for(int i = 0;i < rowcount_2; i++) //�����ʼ�㣬��ֹ�㣬�ֱ������
		{
			state_start[i] = row_2[0];
			state_end[i] = row_1[0];
			//cout<<"��ʼ״̬-> ��ֹ״̬"<<state_start[i] <<" -> "<<state_end[i]<<endl;    	   
			row_2 = mysql_fetch_row(result_2);
			row_1 = mysql_fetch_row(result_1);
		}	  	
        int q=0,fg=1;
		string aaa[30];
cout<<"\n״̬��ʼ���ѻ�ȡ...\n"; 
		sline = rowcount_2;
	    for(i=0;i<rowcount_2;i++)//���غ��ȡ ���ظ�����ʼ��->����aaa[]
		{
			fg=1;
   		    for(int k=0;k<i;k++){
				if(state_start[i]==state_start[k]) fg=0;
			}
			if(fg){
				aaa[q++]=state_start[i];
			}
		}
		//��ʾ���ظ����for(i=0;i<q;i++)cout<<"-> "<<aaa[i]<<endl;

		
		for(j=0;j<rowcount_2;j++)
		{
			List[j].start=(char *)state_start[j].c_str();
			List[j].end=(char *)state_end[j].c_str();
		}
		char *name[50];
cout<<"\n��ʼ��ѯ״̬·��...\n"; 
		for ( i=0;i<q;i++ )name[i]=(char *)aaa[i].c_str();
		for ( i=0;i<q;i++ ) //ѭ��Ƕ�ײ�·��������prt_list()����
		{
			char buf[100];
			strcpy( buf, name[i] );
			prt_list( name[i],0, buf );
		}
		//�������for( i=0;i<cnt;i++ )cout<<array[i]<<endl;

cout<<"\n�ѻ�ȡ·��...\n"; 
	mysql_free_result(result_1); 
	mysql_free_result(result_2); 
   //*************************************************************************************************/  ��ξ����������  /*************/


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
     cout<<"��ξ��࿪ʼ....\n";   
     cout<<"-------------------------------\n";   
     cout<<"\n��������Ҫ�ֳɵ������:    ";  cin>>n;
	 cout<<"\n�������ֶ��ַ������գ�\n[inter]->intersection  [sub]->subClassOf  \n[super]->superClassOf  [equ]->equivalentOf";  
	 cout<<"\n�������д��   ";  cin>>caution;
     merge_cnt = 0;
	 merge_int = 0;
     clock_t begin, end;   
 	 double  cost;
	 begin = clock(); //��ʱ������ʼ
     int k1;   
     for(k1=1;k1<=DEFINE_COUNT;k1++)//��������ĳ�ʼ��   
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
		 cout<<endl<<"��1���༴ȫ��("<<DEFINE_COUNT<<"��)��"<<endl;   
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
         
         for(i=1;i<=n;i++) //����Ԫ�����     
		 {   
			 printf("\n��%d��������ļ�¼(%d��)��\n",i,lei[i].counter);   
   
			 for(j=1;j<=DEFINE_COUNT;j++)   
			 {   
				p=lei[i].datenum[j];   
			    if(p) //�����д�����ݿ�ı���  
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


    //****************************************************************/ ���������Ϣ ��classify[] /***********/

	 string classify[200];
	 MYSQL_RES *result_4 = NULL;  
     MYSQL_ROW row_4 = NULL; 
 
	 mysql_query(&mydata_2,"select classify from n_e_w"); 

	 result_4 = mysql_store_result(&mydata_2);
	 row_4 = mysql_fetch_row(result_4); 
 
	 for(i = 0;i < mysql_num_rows(result_4); i++)
	 { 
		    classify[i] = row_4[0];
			//cout<<"����������"<<classify[i] <<"\t";    	 
			//printf("*****\n");  
			row_4 = mysql_fetch_row(result_4);
	 }	  
  ///******************************************************************/  ��Ϣ�������  /***************/   
cout<<"\n��ʼ������Ϣ����...\n"; 
	
     string vv[200];
	 string str_list[]={"","Input","Output","Before","S_one","After","S_two","S_three"};
	 for(int zt = 1;zt <= 7; zt++)
	 {
		 for(i = 0;i < DEFINE_COUNT; i++) {vv[i] = traindata[i][zt] ;} 
		 cout<<"����"<<str_list[zt]<<"����Ϣ���棺"<<Entropy_calculate(classify,entro,vv)<<endl;
		 memset(entro,0,sizeof(Entropy)*DEFINE_COUNT);
	 }

     mysql_free_result(result_4);

cout<<"\nѵ�����ݽ׶ε������...\n\n"; 
	 return 0;
}


//************************************************************************************************************************************
                                                        // ���ܺ��� //
//************************************************************************************************************************************

//*************************************************************/  ���ؼ�¼"���",������Merge����  /************/
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
	 else aaa=row_1[0]; //��ù�ϵ����ʣ���Ӧ��ֵ  
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

//*************************************************************/  ���о��࣬ÿ�κϲ�  /************/
void Merge(struct Leizu lei[DEFINE_COUNT],MYSQL mydata_1)   
{      
if(FLAG_HIDDEN)cout<<"\n-------------------��ʼ����[function]\n";
    float **m;   
    m = (float**)malloc((T+1)*sizeof(float*));   
    for(int ii = 0; ii < T+1; ii++ )   
    {   
        m[ii] = (float*)malloc((T+1)*sizeof(float));   
    }   
   
    int i,j,i1,j1,st1,st2,p1,p2,p4,p3; //st1��st2ΪҪ�ϲ������   
    double a1,a2,a3,a4,a5,a6,a7,c=0,min;   
    //�����������  
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
            //cout<<"ǰ��״̬����ϵֵ��<<lei[i].state1<<" "<<lei[j].state1<<"-> "<<a5<<endl;
			//cout<<"ǰ��״̬����ϵֵ��<<lei[i].state2<<" "<<lei[j].state2<<"-> "<<a6<<endl;
			//cout<<"ǰ��״̬����ϵֵ��<<lei[i].state3<<" "<<lei[j].state3<<"-> "<<a7<<endl;
            c = (-0.5)*float(a1+a2+a3+a4+a5+a6+a7);   
            m[j][i] = c;
            m[i][j] = c;
        }   
     }   
	 
     min=m[1][2];/////////////////////////////����̬�ķ�ֵ������������ܻ���Ӱ�졿  
	 
     //������С��������   
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
     //cout<<"�ϲ�������ţ�"<<st1<<" "<<st2<<endl;
    
	 //�ϲ���С��������� 
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
	  
if(FLAG_HIDDEN)cout<<"-------------------��Ŀ�ۼ�[function]\n";   
     for(p2=1;p2<=lei[st2].counter;p2++)   
     {   
         p3++;   
         lei[st1].datenum[p3]=lei[st2].datenum[p2];   
     }   
   
   
     lei[st1].counter=lei[st1].counter+lei[st2].counter;
	 
if(FLAG_HIDDEN)cout<<"-------------------�������[function]\n";

     for(p4=st2;p4<T;p4++) //����������������������λ   
     {   
		 //cout<<"������������"<<p4<<" "<<p4+1<<" "<<T<<endl;
         lei[p4]=lei[p4+1]; 
		 
     } 

	for(i=0;i<T+1;i++){ free(m[i]);}
    free(m); 
if(FLAG_HIDDEN)cout<<"-------------------�������[function]\n";      
}   
	





//*********************************************************************/ S״̬·�������ؼ���ֵ /****************/
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
            //cout <<"������ʼ������"<<i<<endl;
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
    if ( n >= 4 )  //�������4���Խ���ظ���Խ��
    {
        //cout<<"�����·����"<<str_list<<endl;
        strcpy( array[cnt++], str_list ); 
        return ;
    }
    int flag=0;
    for( flag=0;;flag++) //����Ӧ�ĺ�̽�㣬��ǰ����������趨Ϊ����
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
        prt_list( List[i].end,n+1, buf ); //������
    }
}


float value(string a,string b)
{
//cout<<"-------------------����״̬��ϵֵ[function]\n"; 
	if(a==b)return 1;
	else
	{
		//cout<<"����·����������"<<cnt<<endl;
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





//************************************************************************************/  ����ڵ���أ����������Ե���  /************/
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
	for(int i = 0; i < DEFINE_COUNT; i++) //ѭ��ÿ����¼��ͬ��
	{
		for(int j = i+1; j < DEFINE_COUNT; j++)
		{
			if(entro[i].ab_name[1] == entro[j].ab_name[1])//�ϲ���¼ �������
			{
				entro[i].ab_name[p] = entro[j].ab_name[1];
				entro[i].o_num[p] = entro[j].o_num[1];
				entro[i].c_lei[p] = entro[j].c_lei[1];
				entro[i].g_count += entro[j].g_count; 
				for(int k = j; k < DEFINE_COUNT; k++)//�ڳ��ռ�
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

    for(i = 0;i < DEFINE_COUNT;i++){if(entro[i].g_count)b++;}//�õ�������Ŀ
	//cout<<"�ɻ������Եĸ�����"<<b<<endl;
   
	int flag = 1,count = 0;
	

	float all_value = 0;
	/////////////////////////////////////////////���㡤���Եġ���
if(FLAG_HIDDEN)cout<<"-------------------�õ����Ե���[function]\n"; 
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
				        //cout<<entro[i].c_lei[j]<<"  ����  "<<entro[i].c_lei[k]<<endl;
						if(j!=0 && entro[i].c_lei[j]==entro[i].c_lei[k]) flag = 0;					 
				}
				//cout<<"���ر�־λ��"<<flag<<endl;
                if(flag){ 
					for(int p = j;p <= size; p++){
						//cout<<entro[i].c_lei[p]<<"  ����  "<<entro[i].c_lei[j]<<endl;
						if(entro[i].c_lei[p]==entro[i].c_lei[j]) count++;
					}
				
					if(count==0) adder = 0;
					else{
						//cout<<"�����и�����"<<size<<endl;
						//cout<<"���ε�ͬ�ࣺ"<<count<<endl;
						//cout<<"��ʽ�м�ʽ��"<<(float)count/size<<endl;
						//cout<<"������ʽֵ��"<<(log((float)count/size)/log(2))<<endl;
						adder +=  (float)count/size *(log((float)count/size)/log(2));
						//cout<<"ÿ�εĽ����"<<adder<<endl;
						 		
					}
				}
			}
		}
if(FLAG_HIDDEN)cout<<"-------------------�õ��ڵ����[function]\n"; 
		float m = (float)entro[i].g_count/DEFINE_COUNT;
		//cout<<"�����Ը��ʣ�"<<m<<endl;
		all_value += float(m*adder);
		//cout<<"���ۼӵ��أ�"<<all_value<<endl;
	}
    //cout<<"��������أ�"<<S_Value(classify)<<endl;

if(FLAG_HIDDEN)cout<<"-------------------���ؽڵ����Ϣ����[function]\n";  
return S_Value(classify)+all_value;//�õ���Ϣ����
}


//***********************************************************************/  ����'���'���أ�������Entropy_calculate /******************/
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
	//cout<<"������Ŀ��"<<num<<endl;

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
		//cout<<"�������"<<(float)count/DEFINE_COUNT<<endl;
	    //cout<<"����ʽֵ��"<<log((float)count/DEFINE_COUNT)/log(2)<<endl;
		//cout<<"�м�˻���"<<((float)count/DEFINE_COUNT)*(log((float)count/DEFINE_COUNT)/log(2))<<endl;
		s_value += ((float)count/DEFINE_COUNT)*(log((float)count/DEFINE_COUNT)/log(2));
		
	}
if(FLAG_HIDDEN)cout<<"-------------------�õ�������[function]\n"; 
    s_value = (-1)*s_value;
	return s_value;

}


//*************************************************************/   END   /***************************************************************//