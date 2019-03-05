//sk2mp3_encode.c
//OS: Macintosh
//author: skuaka
//usage: sk2mp3_encode [file] [title] [author]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 124 //name,title,author
#define Ns 32  //suffix

void utf_8_to_utf_16_le(char * dest,const char * src){
	int d=0,s=0;
	while(src[s] != '\0'){
		if( (unsigned char)src[s] > 0xdf){
			dest[d]   = ( (src[s+2]&(char)0x3f) )|( (src[s+1]&(char)0x03)<<6 );
			dest[d+1] = ( (src[s+1]>>2)&(char)0x0f )|( src[s]<<4 );
			s += 3;
		}
		else{
			dest[d]=src[s];
			++s;
		}
		d += 2;
	}
	dest[d]='\0';
	dest[d+1]='\0';
}

int main(int argc, char *argv[]) {

	char name[N]={'\0'};
	char suffix[Ns]={'\0'};

	FILE * p_src = fopen(argv[1], "rb");
	if(!p_src) {
		fprintf(stderr, "Error: file %s opening failed.\n", argv[1]);
		return EXIT_FAILURE;
	}
	printf("File %s opened.\n",argv[1]);
	
	strcat(name, argv[1]);
	char * s = strchr(name, '.');//find the first name suffix
	if(s){
		*s = '\0';
		utf_8_to_utf_16_le(suffix, s+1);
	}
	else {
		suffix[0]='~';
	}
	strcat(name, "_sk_e.mp3");
	FILE * p_dest;
	p_dest = fopen(name, "r");
	if(p_dest != NULL){
		fclose(p_src);
		fclose(p_dest);
		fprintf(stderr, "Error: file %s exist.\n", name);
		return EXIT_FAILURE;
	}
	p_dest= fopen(name , "wb");
	if(!p_dest) {
		fclose(p_src);
		fprintf(stderr, "Error: file %s opening failed.\n", name);
		return EXIT_FAILURE;
	}
	printf("File %s created.\n",name);
	puts("Start covering (3 steps in total)");
	puts("Step 1 ...");
	char buf[81*16+10]="";
	
	char id3_h[10]   ={0x49,0x44,0x33,0x03,0x00,0x00,0x00,0x00,0x0a,0x10};
	char title_h[13] ={0x54,0x49,0x54,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xfe};
	char author_h[13]={0x54,0x50,0x45,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xfe};
	char album_h[13] ={0x54,0x41,0x4c,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xfe};
	
	char title[N];
	char author[N-6];
	utf_8_to_utf_16_le(title, argv[2]);
	utf_8_to_utf_16_le(author, argv[3]);
	
	int tit = 0;
	int aut = 0;
	int suf = 0;
	while(title[tit]!='\0' || title[tit+1]!='\0')
			tit+=2;
	while(author[aut]!='\0' || author[aut+1]!='\0')
			aut+=2;
	while(suffix[suf]!='\0' || suffix[suf+1]!='\0')
			suf+=2;
	title_h[7] = tit+3;
	author_h[7]= aut+6+3;
	album_h[7] = suf+3;
	
	memmove(buf,            id3_h,      10);//ID3 header +10
	memmove(buf+10,         title_h,    13);//title header +13
	memmove(buf+23,         title,     tit);//title +tit
	memmove(buf+23+tit,     author_h,   13);//author header +13
	memmove(buf+36+tit,     author,  aut+6);//author +aut+6
	memmove(buf+42+tit+aut, album_h,    13);//album header +13
	memmove(buf+55+tit+aut, suffix,    suf);//album +suf
	
	fwrite(buf, sizeof(char), 10+81*16, p_dest);
	
	puts("Step 2 ...");
	puts("(This process may take a long time, please wait)");
	char b4[4] ={0xff,0xfb,0xe0,0x64};
	int size,size2;
	while((size = fread(buf, sizeof(char), 1040, p_src)) && (size != 0)){
		fwrite(b4,  sizeof(char),    4, p_dest);
		fwrite(buf, sizeof(char), 1040, p_dest);
		size2=size;
	}//size2 ranges [1,1040]
	
//	printf("\tsize = %d\n", size2);
	fclose(p_src);
	puts("Step 3 ...");
	
	char addition[6]={'\0'};
	addition[0] = '~';
	int add1 = size2 / 52;
	int add2 = size2 % 52;
	addition[2] = (add1<26)?(add1+'A'):(add1-26+'a');
	addition[4] = (add2<26)?(add2+'A'):(add2-26+'a');
	
	fseek(p_dest, 36+tit+aut, SEEK_SET);
	fwrite(addition, sizeof(char), 6, p_dest);
	fclose(p_dest);
	puts("Completed!");
	return 0;
}