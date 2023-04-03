#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "cJSON.h"
#include "network.h"

domain[] = {
{},
{},
{},
};

int parse_json(struct network *np)
{
	int i = 0;

	FILE *fp;
	if ((fp = fopen("network.json", "rb")) == NULL) {
		exit(0);
	}
	fseek(fp, 0, SEEK_END);
	int fileLen = ftell(fp);
	char *json_string = (char *)malloc(sizeof(char) * fileLen);
	fseek(fp, 0, SEEK_SET);
	fread(json_string, fileLen, sizeof(char), fp);
	fclose(fp);
	for (int i = 0; i < fileLen; ++i) {
		printf("%c", json_string[i]);
	}
	printf("\n");
	if ((fp = fopen("example.txt", "wb")) == NULL) {
		exit(0);
	}
	rewind(fp);
	fwrite(json_string, fileLen, sizeof(char), fp);
	fclose(fp);

	cJSON *cjson = cJSON_Parse(json_string);	//将JSON字符串转换成JSON结构体
	if (cjson == NULL)	//判断转换是否成功
	{
		printf("cjson error...\r\n");
	} else {
		printf("%s\n", cJSON_Print(cjson));	//打包成功调用cJSON_Print打印输出
	}
	free(json_string);

	printf
	    ("/*********************以下就是提取的数据**********************/\n");
	cJSON *net_domain = cJSON_GetObjectItem(cjson, "net_domain");	//解析数组
	char *domain = cJSON_GetObjectItem(net_domain, "domain")->valuestring;
	printf("%s\n", domain);
	cJSON *net_type = cJSON_GetObjectItem(cjson, "net_type");	//解析数组
	char *type = cJSON_GetObjectItem(net_type, "type")->valuestring;
	printf("%s\n", type);
	cJSON *net_protocol = cJSON_GetObjectItem(cjson, "net_protocol");	//解析数组
	char *protocol =
	    cJSON_GetObjectItem(net_protocol, "protocol")->valuestring;
	printf("%s\n", protocol);
	char *recv_func = cJSON_GetObjectItem(cjson, "recv_func")->valuestring;	//解析字符串
	printf("%s\n", recv_func);
	char *send_func = cJSON_GetObjectItem(cjson, "send_func")->valuestring;	//解析字符串
	printf("%s\n", send_func);



	/*************************/
	if (!strcmp(domain, "AF_UNIX")) {
		np->net_domain.domain = AF_UNIX;
		printf("np->net_domain.domain = %d\n", np->net_domain.domain);
	} else if (!strcmp(domain, "AF_LOCAL")) {
		np->net_domain.domain = AF_LOCAL;
		printf("np->net_domain.domain = %d\n", np->net_domain.domain);
	} else {
		printf("Error: Not support domain.\n");
		return -1;
	}


	if (!strcmp(type, "SOCK_STREAM")) {
		np->net_type.type = SOCK_STREAM;
	} else if (!strcmp(type, "SOCK_DGRAM")) {
		np->net_type.type = SOCK_DGRAM;
	} else {
		printf("Error: Not support type.\n");
		return -1;
	}
	printf("np->net_type.type = %d\n", np->net_type.type);
	if (!strcmp(protocol, "0"))
		np->net_protocol.protocol = 0;

	if (!strcmp(recv_func, "RECVMSG"))
		np->recv_func = RECVMSG;

	if (!strcmp(send_func, "SENDMSG"))
		np->send_func = SENDMSG;

	/* Must clear at the end */
	cJSON_Delete(cjson);	//清除结构体 
	return 0;
}
