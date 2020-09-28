#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct token
{
    int16_t offsetLenght;
    char c;
}; // 2 byte

struct tokenBastir
{
    int offset;
    int length;
    char c;
};

const int OffsetBits = 12;
const int LenghtBits = 4;
const int OffsetMaxUzunluk = 4096;
const int LenghtMaxUzunluk = 16;

char dosyaYolu[100];

char offsetLenghtOlustur(int numberOne, int numberTwo);
char* fileRead(FILE *dosya,int *size);
struct token *lz77encode(int *tokenSayisi, int limit, char *content);
void dosyaYazdir(struct token *encoded, int *tokenSayisi);
void dosyaBoyutHesapla(int *tokenSayisi,int *metninBoyutu);

int main()
{
    int metninBoyutu = 0,tokenSayisi = 0;
    strcpy(dosyaYolu,"Metin.txt");
    char *content;
    struct token *encoded;

    FILE *dosya;

    if(dosya = fopen(dosyaYolu,"rb"))
    {
        content = fileRead(dosya,&metninBoyutu);
        fclose(dosya);
    }
    else
    {
        printf("Dosya Acilamadi");
        exit(1);
    }
    encoded = lz77encode(&tokenSayisi,metninBoyutu,content);
    dosyaYazdir(encoded,&tokenSayisi);
    dosyaBoyutHesapla(&tokenSayisi,&metninBoyutu);
    char c;
    scanf("%c",&c);
    return 0;
}

struct token* lz77encode(int *tokenSayisi, int limit, char *content)
{
    printf("---------- LZ77 ALGORITMASI ----------\n\n\n");
    int _tokenSayisi=0;
    int kapasite = 10;

    struct token *encoded = malloc(kapasite *sizeof(struct token));
    struct tokenBastir *bastir = malloc(kapasite *sizeof(struct tokenBastir));

    char *lookhead, *search;

    for(lookhead = content; lookhead < content + limit; lookhead++)
    {

        if(lookhead-content>4096)
        {
            search = lookhead - OffsetMaxUzunluk;
        }
        else
        {
            search = content;
        }

        int enuzuneslesme=0;
        int eslesme=0;
        char *eslesenkelime = lookhead;
        int denetim=0;

        if(search<lookhead)
        {
            while(search!=lookhead)
            {
                if(*search==*lookhead)
                {
                    denetim = 1;

                    int eslesme = eslestir(lookhead,search,LenghtMaxUzunluk);
                    if(eslesme > enuzuneslesme)
                    {
                        enuzuneslesme = eslesme;
                        eslesenkelime = search;
                    }
                }
                search++;
            }
        }

        if(lookhead+enuzuneslesme>=content+limit)
        {
            enuzuneslesme = content + limit - lookhead - 1;
        }

        struct token t;
        t.offsetLenght = offsetLenghtOlustur(lookhead-eslesenkelime,enuzuneslesme);
        lookhead+=enuzuneslesme;
        t.c=*lookhead;

        struct tokenBastir b;
        b.c=*lookhead;
        b.length=enuzuneslesme;
        b.offset=lookhead-eslesenkelime;

        if(_tokenSayisi==kapasite-1)
        {
            kapasite+=3;
            encoded = realloc(encoded,kapasite * sizeof(struct token));
            bastir = realloc(bastir,kapasite * sizeof(struct tokenBastir));
        }

        encoded[_tokenSayisi] = t;
        bastir[_tokenSayisi] = b;
        _tokenSayisi++;
    }

    if(_tokenSayisi!=0)
    {
        *tokenSayisi = _tokenSayisi;
    }

    printf("Olusturulan Tokenler... \n\n");
    int i;
    for(i=0; i<_tokenSayisi; i++)
    {
        printf("[%d,%d,%c]\n",bastir[i].offset,bastir[i].length,bastir[i].c);
    }
    printf("\n");
    printf("Olusturulan Tokenler Dosyaya Isleniyor...\n");
    return encoded;
}

char offsetLenghtOlustur(int offset,int lenght)
{
    char c = (offset<<LenghtBits)| lenght;
    return c;
}


int eslestir (char *iter1, char* iter2, int LenghtMaxUzunluk)
{
    int len = 0;

    while(*iter1==*iter2)
    {
        if(len<LenghtMaxUzunluk)
        {
            len++;
            iter1++;
            iter2++;
        }
        else
        {
            break;
        }
    }
    return len;
}

char* fileRead(FILE *dosya,int *size)
{
    char *content;
    fseek(dosya,0,SEEK_END);
    *size = ftell(dosya);
    fseek(dosya,0,SEEK_SET);
    content = malloc(*size * (sizeof(char)));
    fread(content,1,*size,dosya);
    return content;
}

void dosyaYazdir(struct token *encoded, int *tokenSayisi){

    FILE *dosya;

    if(dosya = fopen("Ciktilz77.txt","w+")){
        int i=0;
        for(i=0;i<*tokenSayisi;i++){
            fprintf(dosya,"%c%c",encoded[i].offsetLenght,encoded[i].c);
        }
        printf("Ciktilz77.txt dosyasina bakabilirsiniz. \n\n");
        fclose(dosya);
    }
    else{

        printf("Ciktilz77.txt dosyasi olusturulamadi !!");
        exit(1);
    }
}

void dosyaBoyutHesapla(int *tokenSayisi,int *metninBoyutu){

    int i=0;
    int normalMetinToplam = 0;
    for(i=0;i<*metninBoyutu;i++)
        normalMetinToplam += 8;

    int j=0;
    int sikistirilmisMetinToplam = 0;
    for(j=0;j<*tokenSayisi;j++)
        sikistirilmisMetinToplam += 24;

    printf("Normal Metnin Boyutu --------> %d bit\n",normalMetinToplam);
    printf("Sikistirilmis Metnin Boyutu --------> %d bit\n",sikistirilmisMetinToplam);
}



