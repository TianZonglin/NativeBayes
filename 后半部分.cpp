#include <winsock.h>
#include <iostream>
#include <string>
#include <time.h>
#include <mysql.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

#define H_count 20 //�õ���ѵ����������/����
#define L_count 8 //ѵ����������
#define SUIJI_CNT 1 //��[1]����[0]�����������
#define FLAG_hidden 0//��ʾϸ��[1]��������[0]
 
MYSQL mydata; 
string traindata[H_count][L_count]; //���������
string newdata[1][L_count]; //�������洢
string xindata[100][10]; //ͬ��
string classes[100]; //����ȫ�����
int class_num; //��������
string yes ; 
string no ;


void result_process(int num,char *max_result);
void classes_method(float &x,float &y);
                                             //string traindata[H_count][L_count],string newdata[1][L_count],string yes,string no,
void classes_strategy(); 
int sql_line();
int sql_read();

/**********************************************************************************************************************/


int sql_line()
{
   //��ʼ�����ݿ�
    if (0 == mysql_library_init(0, NULL, NULL)) {
        cout << "mysql_library_init() succeed" << endl;    
	} 
	else {
        cout << "mysql_library_init() failed" << endl;
        return -1;
	}
    
    if (NULL != mysql_init(&mydata)) {//��ʼ�����ݽṹ
        cout << "mysql_init() succeed" << endl;
    } 
	else {
        cout << "mysql_init() failed" << endl;
        return -1;
    }
 
    if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {
		cout << "mysql_options() succeed" << endl;
    } 
	else {
		cout << "mysql_options() failed" << endl;
		return -1;
    }
    
	if(NULL!=mysql_real_connect(&mydata,"localhost","root","","cap",3306,NULL,0)){
		cout << "mysql_real_connect() succeed" << endl;
    }
	else {
        cout << "mysql_real_connect() failed" << endl;
		return -1;
    }
	return 1;
}

/**********************************************************************************************************************/

int sql_read()
{
	string str;   
		
    str = "SELECT * FROM n_e_w;";  /////////////////////////�����ڴ�////////////��ѵ�����ݱ��Զ�����/////////////////
    MYSQL_RES *result1 = NULL;
    if (0 == mysql_query(&mydata, str.c_str())) 
	{
        cout << "mysql_query() select data succeed" << endl;
        cout << "-----------------------------------------------------------" << endl;
        result1 = mysql_store_result(&mydata);  
        int rowcount1 = mysql_num_rows(result1);
        cout << "ѵ�����ݼ�¼���� " << rowcount1 << endl;     
        unsigned int fieldcount1 = mysql_num_fields(result1);  //ȡ�ò���ӡ���ֶε�����
        MYSQL_ROW row1 = NULL;
        row1 = mysql_fetch_row(result1);
		for(int j=0;j<rowcount1;j++){classes[j]=' ';}
		for(j=0;j<rowcount1;j++)
		{
			
			for(int i = 0; i < fieldcount1; i++)
			{
				traindata[j][i] = row1[i];
				cout<<traindata[j][i]<<"\t";
			}
			cout<<endl;
            classes[j] = row1[L_count];
			row1 = mysql_fetch_row(result1);
		}
		//for(j=0;j<rowcount1;j++){cout<<classes[j]<<endl;}
		int flag=1;
		for(int i = 0;i <rowcount1;i++)
		{
			flag=1;
			for(j=0;j<i;j++)
			{
				if(classes[i]==classes[j])flag=0;
			}
			if(flag) class_num++;
		}
cout<<"ѵ�����ݵ��������"<< class_num<<endl;	
    } 

	else 
	{
        cout << "mysql_query() select data failed" << endl;
        mysql_close(&mydata);
        return -1;
    }
	cout << "-----------------------------------------------------------" << endl;

	return 1;
}

/**********************************************************************************************************************/

void classes_strategy()
{
	int n,num=0;
	float aa=0,bb=0,cc=0,x=0,y=0;
    char a[20],A[100]={0},B[100]={0},max_result[20]={0}; //A[]�ǲ�ֳɵ������B[]Ϊ�����max_result[]��ÿ�ζ�Ԫ�����ʤ�����

if(FLAG_hidden)cout<<"��������ѻ�ȡ..."<<class_num<<endl;

	n = class_num;

if(FLAG_hidden)cout<<"��������ѻ�ȡ...\n"<<endl;

	for(int i=0;i<n;i++)a[i]='0'+(i+1);

	for(i=0;i<n;i++){
		for(int j=i;j<n;j++){
			if(a[i]!=a[j]){
				A[num]=a[i];
		    	B[num]=a[j];
				num++;
			}
		}
	}

	for(i=0;i<num;i++)
	{  
if(FLAG_hidden)cout<<A[i]<<" "<<B[i]<<endl;
		yes=A[i];
		no=B[i];
		classes_method(x,y);                 //����classes_method()�������
 
if(FLAG_hidden)cout<<"��Ԫ��"<<yes<<"��"<<no<<"  �������Ϊ [ "<<x<<" ] [ "<<y<<" ]";	

		if(x>y) {
			max_result[i]=A[i];if(FLAG_hidden)cout<<" ʤ��->"<<A[i]<<endl;}    //�Ѷ�Ԫ��������ʤ���� ��max_result[]
		else {
			max_result[i]=B[i];if(FLAG_hidden)cout<<" ʤ��->"<<B[i]<<endl;}
	}
if(FLAG_hidden)cout<<"\n�����Ѽ������...\n";
    result_process(num,max_result);              //���� result_process() ��ͶƱ���ҳ�������
}

/**********************************************************************************************************************/

void result_process(int num,char *max_result) 
{
	int i,j,end_class,max=0;

	//for(i=0;i<num;i++) cout<<max_result[i]<<endl;
if(FLAG_hidden)cout<<"\nͶƱ�����"<<endl;         
	for(i=0;i<class_num;i++)         //ͶƱ����,ȷ��Ʊ���������
	{
		int numb=0;
		for(j=0;j<num;j++) {if(max_result[j]==('0'+i+1)) numb++;}
if(FLAG_hidden)cout<<"��"<<i+1<<"���� "<<numb<<"Ʊ"<<endl;
		if(numb > max) {max = numb; end_class = i+1;}
	}

if(FLAG_hidden)cout<<"++++++++++++++++++++++"<<endl;                              //�������_����������
    cout<<"�������������Ϊ:[ "<<end_class<<" ]"<<endl;               
if(FLAG_hidden)cout<<"++++++++++++++++++++++++++++++++++"<<endl<<endl;
}

/**********************************************************************************************************************/

void classes_method(float &x,float &y)//string traindatag 
{
    float countyes=0,cnt_1=0,countno=0,cnt_2=0;

	for( int i=0;i<H_count;i++)
	{
		if(classes[i]==yes) {countyes++;}
		else if(classes[i]==no) {countno++;}
	}

	float Pyes=countyes/H_count;   
	float Pno=countno/H_count;  
if(FLAG_hidden)cout<<"������yes��"<<yes<<" "<<"no��"<<no<<endl;   //����ֵĶ�Ԫ����  
if(FLAG_hidden)cout<<"Pyes-> "<<Pyes<<" Pno-> "<<Pno<<endl;
	float PxIyes=1;
	float PxIno=1;   
    for(int k=1;k<=L_count;k++)
	{
		cnt_1=0;cnt_2=0;
		for(i=0;i<H_count;i++)
		{
			if(classes[i]==yes)
			{      //��yes��ǰ���£�������������ֵ�µĸ���
				if(!(traindata[i][k].compare(newdata[0][k])))  cnt_1++;  
			} 			
	 		if(classes[i]==no)
			{  
				if(!(traindata[i][k].compare(newdata[0][k])))  cnt_2++;  
			}
		}
if(FLAG_hidden)cout<<cnt_1<<"  "<<cnt_2<<endl;
		if(cnt_1) PxIyes *= cnt_1/countyes;
		if(cnt_2) PxIno *= cnt_2/countno;
	}

if(FLAG_hidden)cout<<"��yes��ǰ���£�����"<<PxIyes<<endl;//��yes��ǰ���£�������������ֵ�µĸ���                                                           
if(FLAG_hidden)cout<<"��no��ǰ���£�����"<<PxIno<<endl;                                                                                                    	
	x=Pyes*PxIyes;  
	y=Pno*PxIno;     
    //�ɵú������P(yes|x)=(px|yes)*p(yes)    
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

int main()
{
    int count;
	if(!sql_line()){return 0;}      //����sql_line() ����������ݿ�                                                           
	if(!sql_read()){return 0;}  	//����sql_read() �洢 ͨ�������㷨�õ���ѵ������     

    //**************************************************************************

if(SUIJI_CNT){
	cout<<"������������������"; cin>>count;   //�û����� ���������� ����
	cout<<"��������������ʾ��"<<endl;
	cout<<endl;                      
	
	mysql_query(&mydata,"truncate table zzz;");  //ÿ������������ɱ�zzz����ʵ��

    string sqlstr;   
    sqlstr += "insert into zzz "; 
	sqlstr += "set ";
	sqlstr += "input=(SELECT input FROM n_e_w ORDER BY RAND() LIMIT 1),";
    sqlstr += "output=(SELECT output FROM n_e_w ORDER BY RAND() LIMIT 1),";
    sqlstr += "beforer=(SELECT beforer FROM n_e_w ORDER BY RAND() LIMIT 1),";
    sqlstr += "s_one=(SELECT s_one FROM n_e_w ORDER BY RAND() LIMIT 1),";
	sqlstr += "afterr=(SELECT afterr FROM n_e_w ORDER BY RAND() LIMIT 1),";
	sqlstr += "s_two=(SELECT s_two FROM n_e_w ORDER BY RAND() LIMIT 1),";
	sqlstr += "s_three=(SELECT s_three FROM n_e_w ORDER BY RAND() LIMIT 1)";

    for(int t = 0; t < count ; t++)mysql_query(&mydata,sqlstr.c_str()); //�������cout������
}else{cout<<"�����ѹ̶�Ϊ�Զ���...\n";}

	MYSQL_RES *result = NULL; //��ѯȡֵ������ȡ��������ɵ�����(�浽��zzz����)
    mysql_query(&mydata,"SELECT * FROM zzz");      
    result = mysql_store_result(&mydata);
    int rowcount = mysql_num_rows(result);
    unsigned int fieldcount = mysql_num_fields(result); 
    MYSQL_ROW row = NULL;
    row = mysql_fetch_row(result);
			 
	for(int j=0;j<rowcount;j++)
	{ 
		for(int i = 0; i < fieldcount; i++)
		{
			xindata[j][i] = row[i];
			cout<<xindata[j][i] <<"\t";  //��������->xindata[]
		}	    	 
	    cout<<endl;
        row = mysql_fetch_row(result);
	}	  	
        
     clock_t begin, end;   
 	 double  cost;
	 begin = clock(); //��ʱ������ʼ

	cout<<"\n�����ѻ�ȡ,��ʼ�������...\n\n"<<endl;
	for(int i = 0;i < rowcount;i++) //��������ѭ��������
	{
		for(int k=0;k<L_count;k++){	newdata[0][k]=xindata[i][k];  }  //��������->xindata[]->newdata[]	
		classes_strategy();   //����classes_strategy()���з���
	}
	end = clock();
    cost = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\n************************ Time: %lf seconds  ********************\n\n", cost);
	return 0;

}

/******************************************************************************************************/
