#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *memcpy1(void *desc,const void * src,size_t size)
{
	 if((desc == NULL) && (src == NULL))
	 {
	  return NULL;
	 }
	 unsigned char *desc1 = (unsigned char*)desc;
	 unsigned char *src1 = (unsigned char*)src;
	 while(size-- >0)
	 {
	  *desc1 = *src1;
	  desc1++;
	  src1++;
	 }
	 return desc;
}

int main(int argc, char* argv[])
{
	char dest[10] = {0};
	char *src = "1234";

	memcpy1(dest,src,sizeof(src));
	//*(dest+5) = '/0';
	printf("%s\n",(char *)dest);

	return 0;
}
