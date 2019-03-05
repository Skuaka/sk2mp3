//sk2mp3_decode.c
//OS: Macintosh
//author: skuaka
//usage: sk2mp3_decode [xxx.mp3]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 124 //name,title,author
#define Ns 20  //suffix

int main(int argc, char *argv[]) {
	
	char name[N]={'\0'};
	char * pt;
	strcat(name, argv[1]);
	pt = strstr(name, ".mp3");
	if(pt != NULL){
		*pt = '\0';
		pt = strstr(name, "_sk_e");
		if(pt != NULL){
			*pt = '\0';
		}
	}
	else {
		perror("Input file must be mp3 file.\n");
		return EXIT_FAILURE;
	}
	
	FILE * p_src = fopen(argv[1], "rb");
	if(!p_src) {
		perror("Input file opening failed.\n");
		return EXIT_FAILURE;
	}
	printf("File %s opened.\n", argv[1]);
	
	int c;
	char buf[10+81*16];
	fread(buf, sizeof(char), 10+81*16, p_src);
	int index = 26+buf[17]+buf[buf[17]+27];
	int ad1 = buf[index]<='Z'?(buf[index]-'A'):(buf[index]-'a'+26);
	int ad2 = buf[index+2]<='Z'?(buf[index+2]-'A'):(buf[index+2]-'a'+26);
	int add = ad1*52+ad2;
//	printf("%c%c add = %d\n",buf[index],buf[index+2],add);
	
	char suffix[Ns]={'\0'};
	suffix[0]='.';
	int len = (buf[index+11]-3)/2;
	for(int i=0; i < len; ++i)
		suffix[1+i] = buf[index+17+2*i];
	
	printf("  detected suffix = %s\n",suffix+1);
	strcat(name,suffix);
	FILE * p_dest;
	p_dest = fopen(name, "r");
	if(p_dest != NULL){
		fclose(p_src);
		fclose(p_dest);
		fprintf(stderr, "Error: file %s exist.", name);
		return EXIT_FAILURE;
	}
	p_dest = fopen(name, "wb");
	if(!p_dest) {
		fclose(p_src);
		fprintf(stderr, "File %s creating failed", name);
		return EXIT_FAILURE;
	}
	printf("File %s created.\nStart converting ...\n", name);
	
	char buf1[1040],buf2[1040];
	char *xpt[2] = {buf1,buf2};
	int xi = 0;
	
	fseek(p_src, 4, SEEK_CUR);
	fread(xpt[xi], sizeof(char), 1040, p_src);
	xi = !xi;
	while(1) {
		fseek(p_src, 4, SEEK_CUR);
		if(fread(xpt[xi], sizeof(char), 1040, p_src) == 1040){
			fwrite(xpt[!xi], sizeof(char), 1040, p_dest);
		}
		else {
			fwrite(xpt[!xi], sizeof(char), add, p_dest);
			break;
		}
		xi = !xi;
	}
	
	puts("Complete!");
	fclose(p_src);
	fclose(p_dest);
	return 0;
}