#include<stdio.h>             // KEND�M YAPT�M ENES �ELEB�
#include<math.h>
int x = 1;
int n;
int toplam = 0;
int mod=0;
int deciminal_to_binary(int);
void main(){
	printf("bu program rekursif fonksiyon kullanarak deciminal sayilari binary formatina donu�turur\nlutfen bir sayi giriniz\n");
	scanf("%d",&n);
	printf("%d",deciminal_to_binary(n));
}
int deciminal_to_binary(int n){
	if(n!=0){
		mod = n%2;
		toplam += mod*x;
		x= x*10;
		return deciminal_to_binary(n/2);
	}
	return toplam;
}
