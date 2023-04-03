#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "cJSON.h"
 
static void create_network()
{

	cJSON *network = cJSON_CreateObject();				//创建一个对象

	cJSON *net_domain = cJSON_CreateObject();				//创建一个对象
	cJSON_AddStringToObject(net_domain,"domain", "AF_UNIX");	    	//添加整型数字 

	cJSON *net_type = cJSON_CreateObject();				//创建一个对象
	cJSON_AddStringToObject(net_type,"type", "SOCK_STREAM");	    	//添加整型数字 

	cJSON *net_protocol = cJSON_CreateObject();				//创建一个对象
	cJSON_AddStringToObject(net_protocol,"protocol", "0");	    	//添加整型数字 

	cJSON_AddItemToObject(network,"net_domain",net_domain);	//添加数组到对象中
	cJSON_AddItemToObject(network,"net_type",net_type);	//添加数组到对象中
	cJSON_AddItemToObject(network,"net_protocol",net_protocol);	//添加数组到对象中
	cJSON_AddStringToObject(network,"recv_func", "RECVMSG");	    	//添加整型数字 
	cJSON_AddStringToObject(network,"send_func", "SENDMSG");	    	//添加整型数字 





	char *json_data = cJSON_Print(network);	//JSON数据结构转换为JSON字符串
	printf("%s\n",json_data);//输出字符串
	cJSON_Delete(network);//清除结构体


}

int main(void)
{
	double  grade[4]={66.51,118.52,61.53,128.54};
	int		time[4]={123,456,789,150};

	create_network();
	
	return 0;
}
 
