#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<unistd.h>
#include<ctype.h>

int main(){
	
	int dizi[32];
	int a =1;
	int toplam=0;
	int f=0;
	int result=1;
	for(int i  =0 ; i < 32 ; i++){
		scanf("%d",&dizi[i]);
	}	
	
	for(int i=0;i<32;i++){
		printf("%d",dizi[i]);
	}
	
	for(int i  = 0; i < 32 ; i++ ){
		if(i  == 0){
			if(dizi[i]==1){
				a = -1;
			}else a=1;
		}
		if(i<9 && dizi[i] == 1){
			toplam+= pow(2,8-i);
		}else{
			f += pow(2,-i);
		}
		f+=1;
		result = a*toplam*f;
	}
	printf("%d",result);
	
	return 0;
}
