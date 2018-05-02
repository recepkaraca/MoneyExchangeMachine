#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"


volatile unsigned long bekle;

void kur()
{
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	bekle = SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R = 0xff;
	GPIO_PORTE_DEN_R = 0xff;
	GPIO_PORTF_DIR_R = 0xff;
	GPIO_PORTF_DEN_R = 0xff;
}

#define data_F GPIO_PORTF_DATA_R
#define LCD_RS_PORT(X)     data_F = ((data_F & ~(1<<2)) | (X<<2))
#define LCD_EN_PORT(X)     data_F = ((data_F & ~(1<<3)) | (X<<3))
#define LCD_RESETLE do{LCD_EN_PORT(1);LCD_EN_PORT(0);}while(0)
#define veripinleri GPIO_PORTE_DATA_R
#define satir1 komut(0x80)
#define satir2 komut(0xc0)


void isle(unsigned char islenecekHarf)
{
	LCD_RS_PORT(1);
    SysCtlDelay(400);
    veripinleri = islenecekHarf >>4;
    LCD_RESETLE;
    veripinleri = islenecekHarf;
    LCD_RESETLE;
}

void komut(unsigned char gelenKomut)
{
	LCD_RS_PORT(0);
    SysCtlDelay(400);
    veripinleri = gelenKomut >>4;
    LCD_RESETLE;
    veripinleri = gelenKomut;
    LCD_RESETLE;
}

void bit8_komut(unsigned char gelenKomut)
{
	LCD_RS_PORT(0);
    SysCtlDelay(15000);
    veripinleri = (gelenKomut);
    LCD_RESETLE;
}
void temizle()
{
	komut(0x01);
    SysCtlDelay(30000);
}

void lcd_init()
{
    bit8_komut(0x30);
    bit8_komut(0x30);
    bit8_komut(0x30);
    bit8_komut(0x20);
    komut(0x28);
    komut(0x0c);
    temizle();
    komut(0x6);
}

void yaz(char *cumle)
{
    while(*cumle) isle(*cumle++);
}

void init_port_B(){
	volatile unsigned long bekle2;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
	bekle2=SYSCTL_RCGC2_R;
	GPIO_PORTB_DIR_R = 0x00;
	GPIO_PORTB_AFSEL_R = ~0x00;
	GPIO_PORTB_DEN_R = 0xFF;
}

char * parayi_cevir(int para,int paraVirgul){
	static char paraStringTam[5];
	int i;
	for(i=0;i<5;i++){
		paraStringTam[i]="\0";
	}
	char paraStringVirgul[2];
	itoa(para,paraStringTam,10);
	itoa(paraVirgul,paraStringVirgul,10);

	if(para<10){
		paraStringTam[1] = paraStringTam[0];
		paraStringTam[0] = '0';
		paraStringTam[2] = '\0';
	}
	if(paraVirgul>=10){
		strcat(paraStringTam,",");
		strcat(paraStringTam,paraStringVirgul);
	}else if(paraVirgul<10){
		strcat(paraStringTam,",0");
		strcat(paraStringTam,paraStringVirgul);
	}
	return paraStringTam;
}

int bekle_resetle(int islemReset){
	SysCtlDelay(8000000);
	int buton3;
	buton3 = GPIO_PORTB_DATA_R & 0b00100;
	if(buton3 == 0){
		islemReset++;

	}
	SysCtlDelay(8000000);
	buton3 = GPIO_PORTB_DATA_R & 0b00100;
	if(buton3 == 0){
		islemReset++;
	}
	SysCtlDelay(8000000);
	buton3 = GPIO_PORTB_DATA_R & 0b00100;
	if(buton3 == 0){
		islemReset++;
	}
	return islemReset;
}


int main()
{
	init_port_B();
	kur();
	int buton1,buton2,buton3,buton4,buton5;
	int sayi=55;
	char buffer[16];
	char *paraStringTam;
	int para=0;
	int paraVirgul=0;
	short flag1=0,flag2=0,flag3=0,flag4=0,flag5=0,sayac,sayacButon,sayacButonIslem,sayacIslem;


	while(1){
		buton1 = GPIO_PORTB_DATA_R & 0b10000;
		buton2 = GPIO_PORTB_DATA_R & 0b01000;
		buton3 = GPIO_PORTB_DATA_R & 0b00100;
		buton4 = GPIO_PORTB_DATA_R & 0b00010;
		buton5 = GPIO_PORTB_DATA_R & 0b00001;


        itoa(buton1,buffer,2);
        if(sayac<50 || (para==0 && paraVirgul==0)){
			if(buton1 == 0){
				para+= 10;
				flag1++;
				sayac=0;
				if(flag1 == 10){
					para-= 100;
					flag1 = 0;
				}
			}else if(buton2 == 0){
				para+= 1;
				flag2++;
				sayac=0;
				if(flag2 == 10){
					para-= 10;
					flag2 = 0;
				}
			}else if(buton3 == 0){
				flag3=1;
				sayac=0;
			}else if(buton4 == 0 && flag3 == 1){
				paraVirgul+= 10;
				flag4++;
				sayac=0;
				if(flag4 == 10){
					paraVirgul-= 100;
					flag4 = 0;
				}
			}else if(buton5 == 0 && flag3 == 1){
				paraVirgul+= 1;
				flag5++;
				sayac=0;
				if(flag5 == 10){
					paraVirgul-= 10;
					flag5 = 0;
				}
			}
        }

        if(buton3 == 0){
        	sayacButon++;

        }else{
        	sayacButon=0;
        }

        if(sayacButon>30){
        	sayac=0;
        }


        paraStringTam=parayi_cevir(para,paraVirgul);

        lcd_init();


        temizle();

        if(sayac>50 && (para!=0 || paraVirgul!=0)){
        	satir1;
        	yaz("           ");
        	yaz(paraStringTam);
        	satir2;
        	yaz("Girilen Tutar");

        	sayacIslem++;
        	if(sayacIslem>30){
        		temizle();
        		int paraAdet=0;
        		short islemReset=0;
        		char yirmilikStr[2],onlukStr[2],beslikStr[2],birlikStr[2],
				yarimlikStr[2],ceyreklikStr[2],metelikStr[2],delikStr[2],kurusStr[2];
        		while(para>=20){
        			para-= 20;
        			paraAdet++;
        		}
        		if(paraAdet!=0 && islemReset<5){
        			temizle();
        			satir1;
        			paraStringTam=parayi_cevir(para,paraVirgul);
        			yaz("           ");
        			yaz(paraStringTam);
					itoa(paraAdet,yirmilikStr,10);
					satir2;
					yaz(yirmilikStr);
					yaz(" - Yirmilik");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
        		}

        		while(para>=10){
					para-= 10;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,onlukStr,10);
					satir2;
					yaz(onlukStr);
					yaz(" - Onluk");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);

				}

				while(para>=5){
					para-= 5;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,beslikStr,10);
					satir2;
					yaz(beslikStr);
					yaz(" - Beslik");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
				}

				while(para>=1){
					para-= 1;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,birlikStr,10);
					satir2;
					yaz(birlikStr);
					yaz(" - Birlik");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
				}

				while(paraVirgul>=50){
					paraVirgul-= 50;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,yarimlikStr,10);
					satir2;
					yaz(yarimlikStr);
					yaz(" - Yarimlik");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
				}

				while(paraVirgul>=25){
					paraVirgul-= 25;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,ceyreklikStr,10);
					satir2;
					yaz(ceyreklikStr);
					yaz(" - Ceyreklik");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
				}

				while(paraVirgul>=10){
					paraVirgul-= 10;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,metelikStr,10);
					satir2;
					yaz(metelikStr);
					yaz(" - Metelik");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
				}

				while(paraVirgul>=5){
					paraVirgul-= 5;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,delikStr,10);
					satir2;
					yaz(delikStr);
					yaz(" - Delik");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
				}

				while(paraVirgul>=1){
					paraVirgul-= 1;
					paraAdet++;
				}
				if(paraAdet!=0 && islemReset<5){
					temizle();
					satir1;
					paraStringTam=parayi_cevir(para,paraVirgul);
					yaz("           ");
					yaz(paraStringTam);
					itoa(paraAdet,kurusStr,10);
					satir2;
					yaz(kurusStr);
					yaz(" - Kurusluk");
					paraAdet=0;

					islemReset=bekle_resetle(islemReset);
				}

				sayac=0;
				para=0;
				paraVirgul=0;
				SysCtlReset();
        	}
        }else{
        	satir1;
        	yaz(paraStringTam);
        	satir2;
        	yaz("Para Girisi");
        	sayacIslem=0;
        }
        SysCtlDelay(800000);
        sayac++;
        LCD_RS_PORT(0);
	}
}
