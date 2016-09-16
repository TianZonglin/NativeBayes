#include <winsock.h>
#include <iostream>
#include <string>
#include <time.h>
#include <mysql.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

#define H_count 20 //得到的训练数据条数/行数
#define L_count 8 //训练数据列数
#define SUIJI_CNT 1 //是[1]·否[0]随机生成样例
#define FLAG_hidden 0//显示细节[1]·或·隐藏[0]
 
MYSQL mydata; 
string traindata[H_count][L_count]; //结果集存贮
string newdata[1][L_count]; //新样例存储
string xindata[100][10]; //同上
string classes[100]; //分类全部类别
int class_num; //分类类数
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
   //初始化数据库
    if (0 == mysql_library_init(0, NULL, NULL)) {
        cout << "mysql_library_init() succeed" << endl;    
	} 
	else {
        cout << "mysql_library_init() failed" << endl;
        return -1;
	}
    
    if (NULL != mysql_init(&mydata)) {//初始化数据结构
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
		
    str = "SELECT * FROM n_e_w;";  /////////////////////////换表在此////////////此训练数据表自动长度/////////////////
    MYSQL_RES *result1 = NULL;
    if (0 == mysql_query(&mydata, str.c_str())) 
	{
        cout << "mysql_query() select data succeed" << endl;
        cout << "-----------------------------------------------------------" << endl;
        result1 = mysql_store_result(&mydata);  
        int rowcount1 = mysql_num_rows(result1);
        cout << "训练数据记录数： " << rowcount1 << endl;     
        unsigned int fieldcount1 = mysql_num_fields(result1);  //取得并打印各字段的名称
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
cout<<"训练数据的类别数："<< class_num<<endl;	
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
    char a[20],A[100]={0},B[100]={0},max_result[20]={0}; //A[]是拆分成的正类别，B[]为负类别，max_result[]存每次二元分类的胜出类别

if(FLAG_hidden)cout<<"类别数量已获取..."<<class_num<<endl;

	n = class_num;

if(FLAG_hidden)cout<<"类别名称已获取...\n"<<endl;

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
		classes_method(x,y);                 //调用classes_method()计算概率
 
if(FLAG_hidden)cout<<"二元："<<yes<<"与"<<no<<"  后验概率为 [ "<<x<<" ] [ "<<y<<" ]";	

		if(x>y) {
			max_result[i]=A[i];if(FLAG_hidden)cout<<" 胜出->"<<A[i]<<endl;}    //把二元分类结果的胜出方 给max_result[]
		else {
			max_result[i]=B[i];if(FLAG_hidden)cout<<" 胜出->"<<B[i]<<endl;}
	}
if(FLAG_hidden)cout<<"\n概率已计算完毕...\n";
    result_process(num,max_result);              //调用 result_process() 用投票法找出最终类
}

/**********************************************************************************************************************/

void result_process(int num,char *max_result) 
{
	int i,j,end_class,max=0;

	//for(i=0;i<num;i++) cout<<max_result[i]<<endl;
if(FLAG_hidden)cout<<"\n投票结果："<<endl;         
	for(i=0;i<class_num;i++)         //投票计数,确定票数最多的类别
	{
		int numb=0;
		for(j=0;j<num;j++) {if(max_result[j]==('0'+i+1)) numb++;}
if(FLAG_hidden)cout<<"第"<<i+1<<"类有 "<<numb<<"票"<<endl;
		if(numb > max) {max = numb; end_class = i+1;}
	}

if(FLAG_hidden)cout<<"++++++++++++++++++++++"<<endl;                              //程序出口_输出所属类别
    cout<<"本样例所属类别为:[ "<<end_class<<" ]"<<endl;               
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
if(FLAG_hidden)cout<<"此类中yes是"<<yes<<" "<<"no是"<<no<<endl;   //检测拆分的二元分类  
if(FLAG_hidden)cout<<"Pyes-> "<<Pyes<<" Pno-> "<<Pno<<endl;
	float PxIyes=1;
	float PxIno=1;   
    for(int k=1;k<=L_count;k++)
	{
		cnt_1=0;cnt_2=0;
		for(i=0;i<H_count;i++)
		{
			if(classes[i]==yes)
			{      //在yes的前提下，各属性在所给值下的概率
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

if(FLAG_hidden)cout<<"在yes的前提下，概率"<<PxIyes<<endl;//在yes的前提下，各属性在所给值下的概率                                                           
if(FLAG_hidden)cout<<"在no的前提下，概率"<<PxIno<<endl;                                                                                                    	
	x=Pyes*PxIyes;  
	y=Pno*PxIno;     
    //可得后验概率P(yes|x)=(px|yes)*p(yes)    
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

int main()
{
    int count;
	if(!sql_line()){return 0;}      //调用sql_line() 完成连接数据库                                                           
	if(!sql_read()){return 0;}  	//调用sql_read() 存储 通过聚类算法得到的训练数据     

    //**************************************************************************

if(SUIJI_CNT){
	cout<<"输入生成样例条数："; cin>>count;   //用户输入 生成样例的 条数
	cout<<"生成样例如下所示："<<endl;
	cout<<endl;                      
	
	mysql_query(&mydata,"truncate table zzz;");  //每次清空样例生成表zzz便于实验

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

    for(int t = 0; t < count ; t++)mysql_query(&mydata,sqlstr.c_str()); //随机生成cout条样例
}else{cout<<"样例已固定为自定义...\n";}

	MYSQL_RES *result = NULL; //查询取值操作，取出随机生成的样例(存到了zzz表中)
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
			cout<<xindata[j][i] <<"\t";  //生成样例->xindata[]
		}	    	 
	    cout<<endl;
        row = mysql_fetch_row(result);
	}	  	
        
     clock_t begin, end;   
 	 double  cost;
	 begin = clock(); //计时函数开始

	cout<<"\n样例已获取,开始逐个计算...\n\n"<<endl;
	for(int i = 0;i < rowcount;i++) //对样例，循环，分类
	{
		for(int k=0;k<L_count;k++){	newdata[0][k]=xindata[i][k];  }  //生成样例->xindata[]->newdata[]	
		classes_strategy();   //调用classes_strategy()进行分类
	}
	end = clock();
    cost = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\n************************ Time: %lf seconds  ********************\n\n", cost);
	return 0;

}

/******************************************************************************************************/
