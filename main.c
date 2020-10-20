//giriş çıkış işlemleri için (printf vs)
#include <stdio.h>
#include <stdlib.h>
//Dizin işlemleri için
#include <dirent.h>
//string operasyonlar için (strlen, split .. vs)
#include <string.h>
//bool kullanımı için
#include <stdbool.h>
//tolower vs gibi işlemler
#include <ctype.h>

//Hata mesajlari icin model
typedef struct {
    char *errorFileName;
    int errorLineNumber;
    char *errorMessageTest;
    int errorCode;
} ErrorMessage;

//Hatalar icin link list modeli
typedef struct ErrorLinkList{
    ErrorMessage data;
    struct ErrorLinkList *next;
} ErrorLinkList;

//Dökümanlarýn içinde bulunan nokta verilerinin saklanacaðý model
typedef struct {
   float  x;
   float  y;
   float  z;
   int r;
   int g;
   int b;
   int lineNumber;
} DocumentDataModel;

//Dökümanlarýn saklanacaðý model
typedef struct {
   char* currentFileName;
   int  version;
   int  pointCount;
   int  realPointCount;
   bool isAscii;
   bool isWithRGB;
   bool isCalculated;
   float biggestX;
   float biggestY;
   float biggestZ;
   float lowestX;
   float lowestY;
   float lowestZ;
   float itemsAvg;
   DocumentDataModel nearestList[2];
   DocumentDataModel farthestList[2];
   DocumentDataModel *itemData;
} Document;

//Proje içinde kullanýlan metotlar
void StartReading(); //Okumayı baslatan ve uygulamanın bulunduğu dizini alan metot
void ReadDocumentsInDirectory(char directoryPath[], char fileExtention[], int appDirectoryFileCount); //dizindeki dosya sayisi genişliğinde struct array olusturan ve dongu baslatan metot
Document ReadDocument(char *filePath, char *fileName); //bir ustteki metot her dondugunde bu metotu cagırır ve ilgili dosyanın header bilgilerini okuyarak hafızaya atar
Document ReadDocumentDataPart(char *filePath, Document docItem, DocumentDataModel finalDataModel[], char *fileName); //header bilgileri okunan ascii dosyanın nokta sayıları eslesiyorsa noktaları okunur ve hafızaya atılır
Document ReadBinaryDocumentDataPart(char *filePath, Document docItem, DocumentDataModel finalDataModel[], char *fileName); //header bilgileri okunan binary dosyanın nokta sayıları eslesiyorsa noktaları okunur ve hafızaya atılır
void CreateUserInterface(); //kullanıcıdan alınan işlem tipini kontrol ederek ilgili metodu cagırır
int GetOperationCodeFromUser(); //kullanıcıdan işlem tipi almak icin gerekli mesajlari basar
void OperationOne(); // 1. ister için gerekli iş kodlarını içerir
void OperationTwo(); // 2. ister için gerekli iş kodlarını içerir
void OperationThree(); // 3. ister için  gerekli iş kodlarını içerir
void OperationFour(); // 4. ister için gerekli iş kodlarını içerir
void OperationFive(); // 5. ister için gerekli iş kodlarını içerir
void CalcDistance(int operationType); //2 ve 5 nolu isterler benzer iş kodlarına sahip olduğu için bu kısımda verilen işlem tipi ile gerekli hesaplamalar yapılır
void PrintCalcDistance(Document doc, int operationType); // 2 ve 5 için gerekli hesaplamalar sonrası ekrana işlem tipine göre sonuc basılır
int GetDatacount(char *filePath); // ascii dosyalardaki nokta sayısını hesaplar
int GetBinaryDataCount(char *filePath, Document doc); // binary dosyalardaki nokta sayısını hesaplar
int GetFileCount(char directoryPath[], char fileExtention[]); // verilen dizindeki nkt uzantılı doyaların sayısını hesaplar
void PushError(char *fileName, int lineNumber, char *message, int code); //hata listesine yeni bir hata basmak için kullanılır
char * ConcateString(char *str1, char *str2, char *str3, char *str4); // verilen stringleri birleştirmek için kullanılır
void CreateOutputFile(); // output.nkt dosyası oluşturur
void SaveOutput(); //oluşturula output.nkt dosyasını kaydeder.

//Proje içerisinde kullanýlacak sabit deðerler
const char *FileExtention = ".nkt";
const char *CheckAscii = "ascii";
const char *OutputFileName = "output.nkt";

//hata mesajlarý
const char *FilePointCountWrong = " dosyasindaki nokta sayisi gecerli degildir.\n";
const char *FileOpenError = "Dosya acilamadi!.";
const char *DirectoryOpenError = "Dizin acilamadi.";
const char *ApplicationDirectoryError = "Uygulama dizini alinamadi!";
const char *NoFileInDirectoryError = "Dizinde hic %s dosyasi bulunamadi!";
const char *LineNoRgbError = " Nokta verisi r g b bilgileri olmadan verilmistir.";
const char *LineRgbError = " Nokta verisi r g b bilgileri ile verilmistir. Dosyada r g b alanlari yoktur.";

//Projede kullanilan enumlar
typedef enum
{
    NoRgb,
    WithRgb,
    PointCountError
} ErrorCodes;

//Global Değişkenler
Document *DocumentsInMemory; //hafızaya alınan dosya ve nokta verilerini tutar
struct ErrorLinkList *HeadError = NULL; //hataların ekleneceği ana node u tutar
int ErrorCount = 0; //dongu için hata mesajlarının sayısını tutar
int FileCount = 0; //dizindeki dosya sayısını tutar
int OutputLineNumber = 0; //output.nkt kaçıncı satırda olduğu bilgisini tutar
FILE *OutputFile; //output.nkt dosyasını okumamak için ismi tutar
char *OutputFullPath; //output.nkt dosyasının hangi dizine yazılacağınız tutar
int lastOp = 0; //kullanıcının son yaptığı işlem tipini tutar
char *DirectoryPath = "C:\\DataFiles";  //bulunulan dizini tutar

int main()
{
    StartReading(); //okumayı baslatan metot
    fclose(OutputFile); //output.nkt dosyası kapatılıyor
    return 0;
}

//mevcut dizini alır output dosyasını oluşturur ve okuma işlemlerini başlatır.
void StartReading()
{
    char applicationPath[256]; //mevcut yolu tutmak için geçiçi değişken
    if (getcwd(applicationPath, sizeof(applicationPath)) != NULL)
    {
       DirectoryPath = malloc(sizeof(applicationPath)); //application path global değişkene atanıyor
       DirectoryPath = applicationPath;
       CreateOutputFile(); //output dosyası oluşturuluyor
       printf("Uygulama Dizini : %s\n", DirectoryPath); //uygulama dizini ekrana basılıyor
       int appDirectoryFileCount = GetFileCount(DirectoryPath, FileExtention); //bulunan dizindeki nkt uzantılı dosyaların sayısı alınıyor
       if(appDirectoryFileCount > 0) // dosya var mı diye kontrol ediliyor
       {
           printf("Dizinde toplam %d adet %s dosyasi bulundu!\n", appDirectoryFileCount, FileExtention); // dosya satısı ekrana basılıyor.

           //Dizinde bulunan dosyaların okunup hafızaya alınması için ilgili metot çağırılıyor
           ReadDocumentsInDirectory(DirectoryPath, FileExtention, appDirectoryFileCount);
       }
       else
       {
            printf(NoFileInDirectoryError, FileExtention); //dizinde dosya yoksa ekrana uyarı veriliyor
       }
    }
    else
    {
       perror(ApplicationDirectoryError); //dizin alınamz ise hata veriliyor
       return 1;
    }
}

//Dizindeki dosya okuma işlemlerini başlatacak olan metot
void ReadDocumentsInDirectory(char directoryPath[], char fileExtention[], int appDirectoryFileCount)
{
    //verilen dizindeki istenen uzantıdaki dosya sayısı ile yeni bir struct dizi oluşturur.
    Document filesArray[appDirectoryFileCount];

    //Hangi dosyada olduğumuzu anlamak için index
    int fileIndex = 0;

    //istenen dizin içindeki dosyalar tek tek okunacak
    DIR *dir;
    struct dirent *ent;

    //Dizin açılıyor
    if ((dir = opendir (directoryPath)) != NULL)
    {
       //Dosya sayısı kadar döngü
       while ((ent = readdir (dir)) != NULL)
       {
           if(strlen(ent->d_name) > 3)
           {
              //Dosya ismi uzantı kontrolü için yeni bir değişkende saklanıyor.
              char *fileName = (char *)malloc(strlen(ent->d_name)+1);
              strcpy(fileName,ent->d_name);
              char *ext = strrchr(fileName,'.');

              if(ext && strlen(ext) > 1)
              {
                  //Uzantı kontrol ediliyor
                  if(strcmp(ext, FileExtention) == 0 && strcmp(tolower(fileName), OutputFileName) != 0)
                  {
                      //dosyanın tam yolu hesaplanıyor
                      char *fullPath = (char *) malloc(1 + strlen(directoryPath)+ strlen(fileName) );
                      strcpy(fullPath, directoryPath);
                      strcat(fullPath, "//");
                      strcat(fullPath, fileName);

                      //metodun başında oluşturulan arrayin index kontrollü şekilde elemanları atanıyor.
                      filesArray[fileIndex] = ReadDocument(fullPath, fileName);

                      //her bir döküman da index artırılıyor.
                      fileIndex++;
                  }
              }
           }
       }

       //Dosya ile iş bittiğinde dosya kapatılıyor.
       closedir (dir);
    }
    else
    {
       //Dizin açılamaz ise hata veriliyor.
       perror (DirectoryOpenError);
       return EXIT_FAILURE;
    }

    DocumentsInMemory = malloc(filesArray);
    DocumentsInMemory = filesArray;

    FileCount = fileIndex;

    CreateUserInterface();
}

//İstenen dökümanı okuma ve belleğe yazma işini yapar.
Document ReadDocument(char *filePath, char *fileName)
{
   //okuma işlemleri için FILE nesnesi
   FILE *fp;

   //Okuma modunda ilgili dizindeki dosya açılıyor
   fp  = fopen (filePath, "r");

   //Dosya açılamadı ise ekrana hata mesajı yazılıyor.
   if (fp == NULL)
   {
      perror(FileOpenError);
      exit(EXIT_FAILURE);
   }

   //Döküman içindeki nokta datalarının sayısını hesaplamak için kullanılan index
   int count = 0;

   //satır satır  dosya okumak için kullanılan değişken (Satır boyutu 256 karakteri geçmeyeceği düşünülmüştür.)
   char chunk[256];

   //Döküman bilgilerini bellekte tutmak için gerekli tanımlama yapılıyor.
   Document documentItem;
   //bool değerlere default değer ataması yapılıyor.
   documentItem.isAscii = false;
   documentItem.isWithRGB = false;
   documentItem.currentFileName = malloc(fileName);
   documentItem.currentFileName = fileName;
   documentItem.isCalculated = false;
   //Boşluk ile split edilecek datalarda hangi alanda olduğunu bilmek için kullanılan index
   int tempIndex = 0;

   //dosya satır satır okunuyor
   while(fgets(chunk, sizeof(chunk), fp) != NULL)
   {
       //ilk satır yorum satırı olduğu için atlanıyor
       if(count > 0)
       {
           //sscanf ile format belirterek veri almak için array oluşturuyoruz ve kopyalama yapıyoruz.
           char tempArr[strlen(chunk) -1];
           strncpy(tempArr, chunk, strlen(chunk) -1);

           //Verisyon ve nokta sayisi ise calisacak kisim
           if(count == 1 || count == 3)
           {
               //sscanf ile format belirterek okuma yapıyoruz
               char tempStr [20];
               int tempInt;
               sscanf (tempArr,"%s %d",tempStr, &tempInt);

               //versiyon satırında isek versiyonu belleğe atıyoruz
               if(count == 1)
               {
                    documentItem.version = tempInt;
               }
               else if(count == 3) //nokta sayısı satırında isek nokta sayısını bellege atıyoruz
               {
                    documentItem.pointCount = tempInt;
               }
           }
           else if (count == 2) //alanlar satırında kontroller yapılıyor
           {
               //alanlar satırı boşluklarından bölünüyor
               tempIndex = 0;
               char *splitRGB = strtok (tempArr," ");
               while (splitRGB != NULL)
               {
                   //3 ten fazla bölme varsa rgb alanları da vardır
                   if(tempIndex > 3)
                   {
                       //rgb alanların olup olmadığı bir bool değişkende saklanıyor
                        documentItem.isWithRGB = true;
                        break;
                   }

                   //değişken sıfırlanıyor ve index artırılıyor
                   splitRGB = strtok (NULL, " ");
                   tempIndex++;
               }
           }
           else if (count == 4) //döküman içindeki datanın tipi kontrol ediliyor
           {
               //data tipi satırı boşluklarından bölünüyor
               tempIndex = 0;
               char *splitDataType = strtok (tempArr," ");
               while (splitDataType != NULL)
               {
                   //ilk kısım (DATA) atlanıp asıl verinin olduğu bölüm kontrol ediliyor
                   if(tempIndex == 1)
                   {
                       //en üstte tanımlı sabit içindeki ascii değeri ile tolower edilmiş gerçek data karşılaştırılıyor
                       if(strstr(tolower(splitDataType), CheckAscii) != NULL)
                       {
                           // data tipi ascii ise bool bir değişkende saklanıyor
                           documentItem.isAscii = true;
                       }
                   }

                   //değişken sıfırlanıyor ve index artırılıyor.
                   splitDataType = strtok (NULL, " ");
                   tempIndex++;
               }
           }
           else
           {
                //data okuma kısmını başka bir metot yapacağı için header veriler okunduktan sonra döngü durduruluyor.
                break;
           }
       }

       //yeni satıra geçildiği için index artırılıyor
       count++;
   }

   fclose(fp);

   //dosya data tipine göre ilgili metot çağırılıyor ve nokta sayısı hesaplanarak hafızaya atılıyor
   if(documentItem.isAscii)
   {
        documentItem.realPointCount = GetDatacount(filePath); // ascii dosyalar için
   }
   else
   {
       documentItem.realPointCount = GetBinaryDataCount(filePath, documentItem); // binary dosyalar için
   }

   if(documentItem.realPointCount == documentItem.pointCount) // belirtilen nokta sayısı ile gerçek nokta sayısı kıyaslanıyor doğru ise işlemlere devam ediliyor.
   {
       //Döküman içindekiler dataları tutacak array oluşturuluyor. Size kısmı ise nokta sayısı olarak belirleniyor.
       DocumentDataModel myModel[documentItem.pointCount];

       if(documentItem.isAscii) // data tipine göre okuma metodu çağırılıyor
       {
            //Ascii Döküman içinde nokta datasının okunması için ilgili metot çağırılarak geriye dönen değer document nesnesinin dataitem propertisine atanıyor.
            documentItem = ReadDocumentDataPart(filePath, documentItem, myModel, fileName);
       }
       else
       {
           //Binary Döküman içinde nokta datasının okunması için ilgili metot çağırılarak geriye dönen değer document nesnesinin dataitem propertisine atanıyor.
           documentItem = ReadBinaryDocumentDataPart(filePath, documentItem, myModel, fileName);
       }
   }
   else
   {
       PushError(fileName, 0, FilePointCountWrong, PointCountError); //nokta sayıları farklı ise hata link listine hata basılıyor
       ErrorCount++;
   }

   //içeriği doldurulmuş document döndürülüyor.
   return documentItem;
}

//İstenen Binary dökümanın nokta verilerini okuyarak verilen Document nesnesi içine atar
Document ReadBinaryDocumentDataPart(char *filePath, Document docItem, DocumentDataModel finalDataModel[], char *fileName)
{
   //okuma işlemleri için FILE nesnesi
   FILE *fp;

   //Okuma modunda ilgili dizindeki dosya açılıyor
   fp  = fopen (filePath, "r");

   //Dosya açılamadı ise ekrana hata mesajı yazılıyor.
   if (fp == NULL)
   {
      perror(FileOpenError);
      exit(EXIT_FAILURE);
   }

   //Döküman içindeki nokta datalarının sayısını hesaplamak için kullanılan index
   int count = 0;

   //satır satır  dosya okumak için kullanılan değişken (Satır boyutu 256 karakteri geçmeyeceği düşünülmüştür.)
   char chunk[256];

   //x y z r g b kısmını boşluk ile split ediyoruz bu index bize hangi alanda olduğumuzu bildirecek.
   int tempIndex = 0;

   //Döküman modeli içindeki dataItem arrayine eklenecek dataların indexleri
   int dataItemIndex = 0;

   //geçici olarak float verileri tutacak değişken
   float tempValue = 0.00;
   int lineIndex = 0; //x y z sırasını tutmak için değişken
   while(fread(&tempValue, sizeof(float), 1, fp) != NULL) //f read ile okuma yapılıyor
    {
       if(tempIndex > 17) // header kısmı atlanıyor
       {
           if(lineIndex == 0) //X
           {
               finalDataModel[dataItemIndex].x = tempValue;
               //lowest hesaplanırken eğer ilk satır ise atama yapılıyor değilse değer korunuyor.
               docItem.lowestX = (dataItemIndex == 0) ?  tempValue : docItem.lowestX;
               docItem.biggestX = (dataItemIndex == 0) ?  tempValue : docItem.biggestX;

               if(tempValue > docItem.biggestX)
               {
                    docItem.biggestX = tempValue;
               }
               if(tempValue < docItem.lowestX)
               {
                    docItem.lowestX = tempValue;
               }

               lineIndex++;
           }
           else if(lineIndex == 1) //Y
           {
                finalDataModel[dataItemIndex].y = tempValue;

                docItem.lowestY = (dataItemIndex == 0) ? tempValue : docItem.lowestY;
                docItem.biggestY = (dataItemIndex == 0) ? tempValue : docItem.biggestY;

                if(tempValue > docItem.biggestY)
                {
                    docItem.biggestY = tempValue;
                }
                if(tempValue < docItem.lowestY)
                {
                    docItem.lowestY = tempValue;
                }

               lineIndex++;
           }
           else if(lineIndex == 2) //Z
           {
               finalDataModel[dataItemIndex].z = tempValue;

               docItem.lowestZ = (dataItemIndex == 0) ? tempValue : docItem.lowestZ;
               docItem.biggestZ = (dataItemIndex == 0) ? tempValue : docItem.biggestZ;

               if(tempValue > docItem.biggestZ)
               {
                    docItem.biggestZ = tempValue;
               }
               if(tempValue < docItem.lowestZ)
               {
                    docItem.lowestZ = tempValue;
               }

               if(docItem.isWithRGB)
               {
                   lineIndex++;
               }
               else
               {
                   finalDataModel[dataItemIndex].r = 0;
                   finalDataModel[dataItemIndex].g = 0;
                   finalDataModel[dataItemIndex].b = 0;
                   lineIndex = 0;
                   dataItemIndex++;
               }
           }

           //eğer dökümanda RGB datası var ise aşağıdaki atamalar da aynı şekilde yapılıyor
           if(docItem.isWithRGB)
           {
                if(lineIndex == 3) // R ise
                {
                        finalDataModel[dataItemIndex].r = (int)tempValue;
                        lineIndex++;
                }
                else if(lineIndex == 4) // G ise
                {
                    finalDataModel[dataItemIndex].g = (int)tempValue;
                    lineIndex++;
                }
                else if(lineIndex == 5) //B ise
                {
                    finalDataModel[dataItemIndex].b = (int)tempValue;
                    lineIndex=0;
                    dataItemIndex++;
                }
           }
       }
       tempIndex++;
    }

    //dosya okuma işlemi bittiğinde kapatılıyor.
   fclose(fp);

   //döküman modeli içindeki item datalara aktarmak yapmak üzere veriler kopyalanıyor (memory de ayrı alanlar açılması için her seferinde malloc ile yer ayrılıyor.)
   DocumentDataModel *finalDataPointer = malloc(finalDataModel);

   for(int i = 0; i < docItem.pointCount; i++)
   {
        finalDataPointer[i].x = finalDataModel[i].x;
        finalDataPointer[i].y = finalDataModel[i].y;
        finalDataPointer[i].z = finalDataModel[i].z;
        //eğer dosya rgb kısmını içermiyorsa o alanlara atama yapılmıyor.
        if(docItem.isWithRGB)
        {
            finalDataPointer[i].r = finalDataModel[i].r;
            finalDataPointer[i].g = finalDataModel[i].g;
            finalDataPointer[i].b = finalDataModel[i].b;
        }
        finalDataPointer[i].lineNumber = i+1;
   }

   //döküman modelinin içindeki dataitem'a array ataması yapılıyor.
   docItem.itemData = finalDataPointer;

   //içi dolu şekilde döküman modeli geri döndürülüyor.
   return docItem;
}

//İstenen Ascii dökümanın nokta verilerini okuyarak verilen Document nesnesi içine atar
Document ReadDocumentDataPart(char *filePath, Document docItem, DocumentDataModel finalDataModel[], char *fileName)
{
   //okuma işlemleri için FILE nesnesi
   FILE *fp;

   //Okuma modunda ilgili dizindeki dosya açılıyor
   fp  = fopen (filePath, "r");

   //Dosya açılamadı ise ekrana hata mesajı yazılıyor.
   if (fp == NULL)
   {
      perror(FileOpenError);
      exit(EXIT_FAILURE);
   }

   //Döküman içindeki nokta datalarının sayısını hesaplamak için kullanılan index
   int count = 0;

   //satır satır  dosya okumak için kullanılan değişken (Satır boyutu 256 karakteri geçmeyeceği düşünülmüştür.)
   char chunk[256];

   //x y z r g b kısmını boşluk ile split ediyoruz bu index bize hangi alanda olduğumuzu bildirecek.
   int tempIndex = 0;

   //Döküman modeli içindeki dataItem arrayine eklenecek dataların indexleri
   int dataItemIndex = 0;

   //dosyayı satır satır dönmek için döngü
   //fgets chunk size kadar okuma yapar ancak size öncesi satır sonu varsa işlemi durdurur dolayısı ile satır satır data elimizde olur
   while(fgets(chunk, sizeof(chunk), fp) != NULL)
   {
        //fgets ile alınan satır verisi döngü kurmak için başka bir array e aktarlıyor
        char tempArr[strlen(chunk) -1];
        strncpy(tempArr, chunk, strlen(chunk) -1);

        //ilk 4 satır header bilgisi barındırdığı için 4. satırdan itibaren datalar okunuyor
        if(count > 4)
        {
            //her bir satır döngüsünde bu index sıfırlanmalıdır.
            tempIndex = 0;

            //satırdaki veriler boşluklarından parçalanıyor
            char *splitData = strtok (tempArr," ");

            //parçalanan veri döngüye giriyor.
            while (splitData != NULL)
            {
                //her bir parça yeni bir char arraye atanıyor. (son karakter sorun çıkardığı için - 1 ile yeni dizi oluşturulup atanıyor)
                char myChar[strlen(splitData) -1];
                strncpy(myChar, splitData, strlen(splitData));

                //eğer ifadeler 0 a eşit ise float convert problemi olduğundan kontrol yapılıyor.
                bool isZero = strcmp(splitData, "0") == 0;

                //index numarası üzerinden hangi alanda olunduğu kontrol edilerek atama yapılıyor değer 0 mı diye kontrollü atama yapılıyor.
                if(tempIndex == 0) // X ise
                {
                    //float değer int 0 gelirse sorun çıkardığı için kontrollü atama yapılıyor
                    finalDataModel[dataItemIndex].x = isZero ? 0.00 : (float)atof(myChar);

                    //lowest hesaplanırken eğer ilk satır ise atama yapılıyor değilse değer korunuyor.
                    docItem.lowestX = (dataItemIndex == 0) ?  finalDataModel[dataItemIndex].x : docItem.lowestX;
                    docItem.biggestX = (dataItemIndex == 0) ?  finalDataModel[dataItemIndex].x : docItem.biggestX;

                    if(finalDataModel[dataItemIndex].x > docItem.biggestX)
                    {
                        docItem.biggestX = finalDataModel[dataItemIndex].x;
                    }
                    if(finalDataModel[dataItemIndex].x < docItem.lowestX)
                    {
                        docItem.lowestX = finalDataModel[dataItemIndex].x;
                    }
                }
                else if(tempIndex == 1) // Y ise
                {
                    finalDataModel[dataItemIndex].y = isZero ? 0.00 : (float)atof(myChar);

                    docItem.lowestY = (dataItemIndex == 0) ? finalDataModel[dataItemIndex].y : docItem.lowestY;
                    docItem.biggestY = (dataItemIndex == 0) ? finalDataModel[dataItemIndex].y : docItem.biggestY;

                    if(finalDataModel[dataItemIndex].y > docItem.biggestY)
                    {
                        docItem.biggestY = finalDataModel[dataItemIndex].y;
                    }
                    if(finalDataModel[dataItemIndex].y < docItem.lowestY)
                    {
                        docItem.lowestY = finalDataModel[dataItemIndex].y;
                    }
                }
                else if(tempIndex == 2) // Z ise
                {
                    finalDataModel[dataItemIndex].z = isZero ? 0.00 : (float)atof(myChar);

                    docItem.lowestZ = (dataItemIndex == 0) ? finalDataModel[dataItemIndex].z : docItem.lowestZ;
                    docItem.biggestZ = (dataItemIndex == 0) ? finalDataModel[dataItemIndex].z : docItem.biggestZ;

                    if(finalDataModel[dataItemIndex].z > docItem.biggestZ)
                    {
                        docItem.biggestZ = finalDataModel[dataItemIndex].z;
                    }
                    if(finalDataModel[dataItemIndex].z < docItem.lowestZ)
                    {
                        docItem.lowestZ = finalDataModel[dataItemIndex].z;

                    }
                }

                //eğer dökümanda RGB datası var ise aşağıdaki atamalar da aynı şekilde yapılıyor
                if(docItem.isWithRGB)
                {
                    if(tempIndex == 3){finalDataModel[dataItemIndex].r = (int)atof(splitData);} // R ise
                    else if(tempIndex == 4){finalDataModel[dataItemIndex].g = (int)atof(splitData);} // G ise
                    else if(tempIndex == 5){finalDataModel[dataItemIndex].b = (int)atof(splitData);} //B ise
                }

                //bir sonraki alana geçerken değişken sıfırlanıyor ve index artırılıyor
                splitData = strtok (NULL, " ");
                tempIndex++;
            }

            //Dosya rgb alanları ile verildi ise ancak ilgili satır r g b bilgilerini içermiyorsa hatalar link listine eklenir.
            if(docItem.isWithRGB && tempIndex < 6)
            {
                //önce ilgili satır verisinde rgb verileri 0 a eşitlenir.
                finalDataModel[dataItemIndex].r = 0;
                finalDataModel[dataItemIndex].g = 0;
                finalDataModel[dataItemIndex].b = 0;

                //yeni bir hata nodu oluşturulur ve link liste push yöntemi ile eklenir.
                PushError(fileName, dataItemIndex + 1, LineNoRgbError, NoRgb);

                //hata mesajları indexi 1 artırılır.
                ErrorCount++;
            }
            else if(!docItem.isWithRGB && tempIndex > 3) //Dosya rgb alanlar olmadan verildi ise ancak ilgili satır rb alanlarını içeriyorsa hatalar link listine eklenir.
            {
                finalDataModel[dataItemIndex].r = 0;
                finalDataModel[dataItemIndex].g = 0;
                finalDataModel[dataItemIndex].b = 0;

                //yeni bir hata nodu oluşturulur ve link liste push yöntemi ile eklenir.
                PushError(fileName, dataItemIndex + 1, LineRgbError, WithRgb);
            }

            //bir sonraki sonraki data satırına geçerken index artırılıyor
            dataItemIndex++;
        }
        //bir sonraki döküman satırına geçerken index artırılıyor.
        count++;
    }

    //dosya okuma işlemi bittiğinde kapatılıyor.
   fclose(fp);

   //döküman modeli içindeki item datalara aktarmak yapmak üzere veriler kopyalanıyor (memory de ayrı alanlar açılması için her seferinde malloc ile yer ayrılıyor.)
   DocumentDataModel *finalDataPointer = malloc(finalDataModel);

   for(int i = 0; i < docItem.pointCount; i++)
   {
        finalDataPointer[i].x = finalDataModel[i].x;
        finalDataPointer[i].y = finalDataModel[i].y;
        finalDataPointer[i].z = finalDataModel[i].z;
        //eğer dosya rgb kısmını içermiyorsa o alanlara atama yapılmıyor.
        if(docItem.isWithRGB)
        {
            finalDataPointer[i].r = finalDataModel[i].r;
            finalDataPointer[i].g = finalDataModel[i].g;
            finalDataPointer[i].b = finalDataModel[i].b;
        }
        finalDataPointer[i].lineNumber = i+1;
   }

   //döküman modelinin içindeki dataitem'a array ataması yapılıyor.
   docItem.itemData = finalDataPointer;

   //içi dolu şekilde döküman modeli geri döndürülüyor.
   return docItem;
}

//girilen işlem tipine göre ilgili metotları çağırır
void CreateUserInterface()
{
    int operationCode = GetOperationCodeFromUser();

    switch(operationCode)
    {
        case 1:
            OperationOne();
            break;
        case 2:
            OperationTwo();
            break;
        case 3:
            OperationThree();
            break;
        case 4:
            OperationFour();
            break;
        case 5:
            OperationFive();
            break;
        default:
            printf("Gecerli bir operasyon girilmedi!");
            break;
    }
}

//kullanıcıya işlem listesini gösterir ve bir işlem tipi girmesini ister
int GetOperationCodeFromUser()
{
    printf("\nIslem listesi : ");
    printf("\n( 1-) Dosya Kontrolu");
    printf(" | ( 2-) En Yakin/En Uzak Noktalar");
    printf(" | ( 3-) Kup");
    printf(" | ( 4-) Kure");
    printf(" | ( 5-) Nokta Uzakliklari\n");
    int operationCode = 0;
    // sadece 1,2,3,4,5 rakamlarını kabul etmek için gerekli kontroller yapılıyor
    do
    {
        printf("Lutfen gecerli bir islem tipi giriniz : ");
        scanf("%d", &operationCode);
        while(getchar() != '\n');
    }
    while(operationCode > 5 || operationCode < 1);

    return operationCode;
}

// hata mesajları link listinde kayıt var ise ekrana basılıyor
void OperationOne()
{
    if(lastOp == 0)
    {
        lastOp = 1;
        fprintf (OutputFile, "\nISLEM 1\n",OutputLineNumber);
        printf("\nISLEM 1\n");
        ErrorLinkList *current_node = HeadError;
        if(HeadError != NULL)
        {
            OutputLineNumber++;
            while (current_node != NULL)
            {
                ErrorMessage messageToPrint = current_node->data;
                if(messageToPrint.errorCode == NoRgb)
                {
                    char *finalString = ConcateString("(", messageToPrint.errorFileName, ") ", messageToPrint.errorLineNumber);
                    finalString = ConcateString(finalString, ". ", LineNoRgbError, " ");
                    fprintf (OutputFile, finalString ,OutputLineNumber);

                    printf("(%s) - %d. %s\n", messageToPrint.errorFileName, messageToPrint.errorLineNumber, LineNoRgbError);
                    OutputLineNumber++;
                }
                else if(messageToPrint.errorCode == WithRgb)
                {
                    char *finalString = ConcateString("(", messageToPrint.errorFileName, ") ", messageToPrint.errorLineNumber);
                    finalString = ConcateString(finalString, ". ", LineRgbError, " ");
                    fprintf (OutputFile, finalString ,OutputLineNumber);

                    printf("(%s) - %d. %s\n", messageToPrint.errorFileName, messageToPrint.errorLineNumber, LineRgbError);
                    OutputLineNumber++;
                }
                else
                {
                    char *finalString = ConcateString(messageToPrint.errorFileName, " ", messageToPrint.errorMessageTest, " ");
                    fprintf (OutputFile, finalString ,OutputLineNumber);

                    printf("%s %s", messageToPrint.errorFileName, messageToPrint.errorMessageTest);
                    OutputLineNumber++;
                }
                current_node = current_node->next;
            }
        }
        else
        {
            fprintf (OutputFile, "Tum dosyalar uyumludur.\n",OutputLineNumber);
            printf("Tum dosyalar uyumludur.\n");
            OutputLineNumber++;
        }
        SaveOutput();
    }
    else
    {
        printf("\nLutfen siradaki islemi giriniz (Siradaki islem : %d)\n", lastOp + 1);
    }
    CreateUserInterface();  //işlem bitiminde yeniden işlem tipi isteniyor
}

// dosyalar içindeki en yakın ve en uzak noktalar hesaplanıyor
void OperationTwo()
{
    if(lastOp == 1)
    {
        lastOp = 2;
        if(sizeof(DocumentsInMemory) > 0)
        {
            CalcDistance(2); // 2 ve 5 aynı mantığa sahip olduğu için işlem kodu ile ayrılarak aynı iş kodu çalıştırılıyor
        }
        else
        {
            printf("Okunmus dosya bulunmuyor.");
        }
    }
    else
    {
        printf("\nLutfen siradaki islemi giriniz (Siradaki islem : %d)\n", lastOp + 1);
    }
    CreateUserInterface(); //işlem bitiminde yeniden işlem tipi isteniyor
}

//okuma esnasında hesaplanan en buyuk ve en kucuk koordinat değerleri ile hesaplama yaparak küp koordinatları ekrana basılıyor.
void OperationThree()
{
    if(lastOp == 2)
    {
        lastOp = 3;
        if(sizeof(DocumentsInMemory) > 0)
        {
            fprintf (OutputFile, "\nISLEM 3\n" ,OutputLineNumber);
            OutputLineNumber++;
            printf("\nISLEM 3\n");
            for(int j = 0; j < FileCount; j++)
            {
                if(DocumentsInMemory[j].realPointCount == DocumentsInMemory[j].pointCount)
                {
                    float difX = DocumentsInMemory[j].biggestX - DocumentsInMemory[j].lowestX;
                    float difY = DocumentsInMemory[j].biggestY - DocumentsInMemory[j].lowestY;
                    float difZ = DocumentsInMemory[j].biggestZ - DocumentsInMemory[j].lowestZ;

                    float biggestDiff = difZ;
                    if(difX > difY && difX > difZ){biggestDiff = difX;}
                    else if(difY > difX && difY > difZ){biggestDiff = difY;}

                    printf("(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, DocumentsInMemory[j].lowestY, DocumentsInMemory[j].lowestZ);
                    printf("(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, DocumentsInMemory[j].lowestY, (DocumentsInMemory[j].lowestZ + biggestDiff));
                    printf("(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, (DocumentsInMemory[j].lowestY + biggestDiff), DocumentsInMemory[j].lowestZ);
                    printf("(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, (DocumentsInMemory[j].lowestY + biggestDiff), (DocumentsInMemory[j].lowestZ + biggestDiff));
                    printf("(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), DocumentsInMemory[j].lowestY, DocumentsInMemory[j].lowestZ);
                    printf("(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), DocumentsInMemory[j].lowestY, (DocumentsInMemory[j].lowestZ + biggestDiff));
                    printf("(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), (DocumentsInMemory[j].lowestY + biggestDiff), DocumentsInMemory[j].lowestZ);
                    printf("(%s) %f %f %f 255 255 255\n\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), (DocumentsInMemory[j].lowestY + biggestDiff), (DocumentsInMemory[j].lowestZ + biggestDiff));

                    char buf[256];
                    sprintf(buf,"(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, DocumentsInMemory[j].lowestY, DocumentsInMemory[j].lowestZ);
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    sprintf(buf,"(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, DocumentsInMemory[j].lowestY, (DocumentsInMemory[j].lowestZ + biggestDiff));
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    sprintf(buf,"(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, (DocumentsInMemory[j].lowestY + biggestDiff), DocumentsInMemory[j].lowestZ);
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    sprintf(buf,"(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, DocumentsInMemory[j].lowestX, (DocumentsInMemory[j].lowestY + biggestDiff), (DocumentsInMemory[j].lowestZ + biggestDiff));
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    sprintf(buf,"(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), DocumentsInMemory[j].lowestY, DocumentsInMemory[j].lowestZ);
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    sprintf(buf,"(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), DocumentsInMemory[j].lowestY, (DocumentsInMemory[j].lowestZ + biggestDiff));
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    sprintf(buf,"(%s) %f %f %f 255 255 255\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), (DocumentsInMemory[j].lowestY + biggestDiff), DocumentsInMemory[j].lowestZ);
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    sprintf(buf,"(%s) %f %f %f 255 255 255\n\n", DocumentsInMemory[j].currentFileName, (DocumentsInMemory[j].lowestX + biggestDiff), (DocumentsInMemory[j].lowestY + biggestDiff), (DocumentsInMemory[j].lowestZ + biggestDiff));
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;
                }
            }
        }
        else
        {
            printf("Okunmus dosya bulunmuyor.");
        }
    }
    else
    {
        printf("\nLutfen siradaki islemi giriniz (Siradaki islem : %d)\n", lastOp + 1);
    }
    CreateUserInterface();  //işlem bitiminde yeniden işlem tipi isteniyor
}

//kullanıcıdan küre koordinatları ve yarı çapı isteniyor. noktalar arası uzaklık formulu ile her bir nokta kontrol ediliyor ve fark yarı çaptan küçük ise ekrana basılıyor.
void OperationFour()
{
    if(lastOp == 3)
    {
        lastOp = 4;
        if(sizeof(DocumentsInMemory) > 0)
        {
            float sphereX, sphereY, sphereZ, sphereR;
            fprintf (OutputFile, "\nISLEM 4\n" ,OutputLineNumber);
            OutputLineNumber++;
            printf("\nISLEM 4\n");
            printf("Lutfen kurenin x degerini giriniz : ");
            scanf("%f", &sphereX);
            printf("Lutfen kurenin y degerini giriniz : ");
            scanf("%f", &sphereY);
            printf("Lutfen kurenin z degerini giriniz : ");
            scanf("%f", &sphereZ);
            printf("Lutfen kurenin yaricapini giriniz : ");
            scanf("%f", &sphereR);
            while(getchar() != '\n');

            char buf[256];
            sprintf(buf,"cx : (%f)\ncy : (%f)\ncz : (%f)\ncr : (%f)\n",
                     sphereX, sphereY, sphereZ, sphereR);
            fprintf (OutputFile, buf ,OutputLineNumber);
            OutputLineNumber++;
            printf("Girdiginiz X : %f - Y : %f - Z : %f - R : %f", sphereX, sphereY, sphereZ, sphereR);


            for(int j = 0; j < FileCount; j++)
            {
                if(DocumentsInMemory[j].realPointCount == DocumentsInMemory[j].pointCount)
                {
                    char fields[12] = "x y z";
                    if(DocumentsInMemory[j].isWithRGB){strcpy(fields,"x y z r g b");}
                    char dataType[7] = "ascii";
                    if(!DocumentsInMemory[j].isAscii){strcpy(dataType, "binary");}

                    printf("\nDOSYA : %s\nALANLAR : %s\nNOKTALAR : %d\nDATA : %s\n",
                    DocumentsInMemory[j].currentFileName, fields, DocumentsInMemory[j].pointCount, dataType);

                    sprintf(buf,"DOSYA : %s\nALANLAR : %s\nNOKTALAR : %d\nDATA : %s\n",
                    DocumentsInMemory[j].currentFileName, fields, DocumentsInMemory[j].pointCount, dataType);
                    fprintf (OutputFile, buf ,OutputLineNumber);
                    OutputLineNumber++;

                    for(int i = 0; i < DocumentsInMemory[j].pointCount; i++) //nokta sayısı kadar dön
                    {
                        float sonuc= sqrt((pow(sphereX-DocumentsInMemory[j].itemData[i].x,2)) +
                                          (pow(sphereY-DocumentsInMemory[j].itemData[i].y,2)) +
                                          (pow(sphereZ-DocumentsInMemory[j].itemData[i].z,2)));

                        if(sonuc < sphereR)
                        {
                            if(DocumentsInMemory[j].isWithRGB)
                            {
                                sprintf(buf,"%d. Satir %f %f %f %d %d %d\n",
                                       DocumentsInMemory[j].itemData[i].lineNumber, DocumentsInMemory[j].itemData[i].x,
                                       DocumentsInMemory[j].itemData[i].y, DocumentsInMemory[j].itemData[i].z,
                                       DocumentsInMemory[j].itemData[i].r, DocumentsInMemory[j].itemData[i].g,
                                       DocumentsInMemory[j].itemData[i].b);
                                fprintf (OutputFile, buf ,OutputLineNumber);
                                OutputLineNumber++;

                                printf("%d. Satir %f %f %f %d %d %d\n",
                                       DocumentsInMemory[j].itemData[i].lineNumber, DocumentsInMemory[j].itemData[i].x,
                                       DocumentsInMemory[j].itemData[i].y, DocumentsInMemory[j].itemData[i].z,
                                       DocumentsInMemory[j].itemData[i].r, DocumentsInMemory[j].itemData[i].g,
                                       DocumentsInMemory[j].itemData[i].b);
                            }
                            else
                            {
                                sprintf(buf,"%d. Satir %f %f %f\n",
                                       DocumentsInMemory[j].itemData[i].lineNumber, DocumentsInMemory[j].itemData[i].x,
                                       DocumentsInMemory[j].itemData[i].y, DocumentsInMemory[j].itemData[i].z);
                                fprintf (OutputFile, buf ,OutputLineNumber);
                                OutputLineNumber++;

                                printf("%d. Satir %f %f %f\n",
                                       DocumentsInMemory[j].itemData[i].lineNumber, DocumentsInMemory[j].itemData[i].x,
                                       DocumentsInMemory[j].itemData[i].y, DocumentsInMemory[j].itemData[i].z);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            printf("Okunmus dosya bulunmuyor.");
        }
    }
    else
    {
        printf("\nLutfen siradaki islemi giriniz (Siradaki islem : %d)\n", lastOp + 1);
    }
    CreateUserInterface();  //işlem bitiminde yeniden işlem tipi isteniyor
}

//dosya içindeki noktaların birbirlerine olan uzaklıklarının ortalaması hesaplanıyor
void OperationFive()
{
    if(lastOp == 4)
    {
        lastOp = 5;
        if(sizeof(DocumentsInMemory) > 0)
        {
            CalcDistance(5); // 2 ve 5 aynı mantığa sahip olduğu için işlem kodu ile ayrılarak aynı iş kodu çalıştırılıyor
        }
        else
        {
            printf("Okunmus dosya bulunmuyor.");
        }
        fclose(OutputFile);
        getchar();
    }
    else
    {
        printf("\nLutfen siradaki islemi giriniz (Siradaki islem : %d)\n", lastOp + 1);
    }
    if(lastOp != 5){CreateUserInterface();} //5. işlem bitiminde program sonlandırılıyor.
}

// 2 ve 5 nolu isterler için gerekli iş kodlarını barındıran metot.
void CalcDistance(int operationType)
{
    int avgIndex = 0;
    char buf[7];
    sprintf(buf,"%d", operationType);
    char *finalString = ConcateString("\nISLEM ", buf, "\n",  "");
    fprintf (OutputFile, finalString ,OutputLineNumber);
    printf("\nISLEM %d\n", operationType);
    for(int j = 0; j < FileCount; j++)
    {
        if(DocumentsInMemory[j].realPointCount == DocumentsInMemory[j].pointCount)
        {
            if(DocumentsInMemory[j].isCalculated == false)
            {
                avgIndex = 0;
                DocumentsInMemory[j].itemsAvg = 0.00;
                float tempMinCalculation = 0;
                float tempMaxCalculation = 0;

                for(int i = 0; i < DocumentsInMemory[j].pointCount; i++) //nokta sayısı kadar dön
                {
                    for(int k = i+1; k < DocumentsInMemory[j].pointCount; k++)// fazladan dönmemek için her seferinde başlangıcı i+1 kadar yap zaten ondan onceki noktayı karşılaştırmıştın
                    {
                        if(i != k)
                        {
                            if(i == 0 && k == 1)
                            {
                                float calculationResult = sqrt((pow(DocumentsInMemory[j].itemData[i].x-DocumentsInMemory[j].itemData[k].x,2)) +
                                                               (pow(DocumentsInMemory[j].itemData[i].y-DocumentsInMemory[j].itemData[k].y,2)) +
                                                               (pow(DocumentsInMemory[j].itemData[i].z-DocumentsInMemory[j].itemData[k].z,2)));

                                DocumentsInMemory[j].itemsAvg = calculationResult;

                                tempMinCalculation = calculationResult;
                                tempMaxCalculation = calculationResult;

                                DocumentsInMemory[j].nearestList[0] = DocumentsInMemory[j].itemData[i];
                                DocumentsInMemory[j].nearestList[1] = DocumentsInMemory[j].itemData[k];

                                DocumentsInMemory[j].farthestList[0] = DocumentsInMemory[j].itemData[k];
                                DocumentsInMemory[j].farthestList[1] = DocumentsInMemory[j].itemData[i];

                                avgIndex++;
                            }
                            else
                            {
                                float calculationResult = sqrt((pow(DocumentsInMemory[j].itemData[i].x-DocumentsInMemory[j].itemData[k].x,2)) +
                                                               (pow(DocumentsInMemory[j].itemData[i].y-DocumentsInMemory[j].itemData[k].y,2)) +
                                                               (pow(DocumentsInMemory[j].itemData[i].z-DocumentsInMemory[j].itemData[k].z,2)));

                                DocumentsInMemory[j].itemsAvg += calculationResult;

                                if(calculationResult < tempMinCalculation) //hesaplama gecici en küçük değerden küçük mü
                                {
                                    DocumentsInMemory[j].nearestList[0] = DocumentsInMemory[j].itemData[i];
                                    DocumentsInMemory[j].nearestList[1] = DocumentsInMemory[j].itemData[k];
                                    tempMinCalculation = calculationResult;
                                }
                                if(calculationResult > tempMaxCalculation) //hesaplama gecici en büyük değerden büyük mü
                                {
                                    DocumentsInMemory[j].farthestList[0] = DocumentsInMemory[j].itemData[k];
                                    DocumentsInMemory[j].farthestList[1] = DocumentsInMemory[j].itemData[i];
                                    tempMaxCalculation = tempMaxCalculation;
                                }

                                avgIndex++;
                            }
                        }
                    }
                }

                DocumentsInMemory[j].itemsAvg = DocumentsInMemory[j].itemsAvg / avgIndex;
                DocumentsInMemory[j].isCalculated = true;

                PrintCalcDistance(DocumentsInMemory[j], operationType);
            }
            else
            {
                PrintCalcDistance(DocumentsInMemory[j], operationType);
            }
        }
    }
}

// 2 ve 5 nolu isterler için hesaplamaları ekrana basan metot
void PrintCalcDistance(Document doc, int operationType)
{
    if(operationType == 2)
    {
        char buf[256];
        sprintf(buf,"(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.nearestList[0].lineNumber,
            doc.nearestList[0].x, doc.nearestList[0].y,
            doc.nearestList[0].z, doc.nearestList[0].r,
            doc.nearestList[0].g, doc.nearestList[0].b);
        fprintf (OutputFile, buf ,OutputLineNumber);
        OutputLineNumber++;

        printf("(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.nearestList[0].lineNumber,
            doc.nearestList[0].x, doc.nearestList[0].y,
            doc.nearestList[0].z, doc.nearestList[0].r,
            doc.nearestList[0].g, doc.nearestList[0].b);

        sprintf(buf,"(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.nearestList[1].lineNumber,
            doc.nearestList[1].x, doc.nearestList[1].y,
            doc.nearestList[1].z, doc.nearestList[1].r,
            doc.nearestList[1].g, doc.nearestList[1].b);
        fprintf (OutputFile, buf ,OutputLineNumber);
        OutputLineNumber++;

        printf("(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.nearestList[1].lineNumber,
            doc.nearestList[1].x, doc.nearestList[1].y,
            doc.nearestList[1].z, doc.nearestList[1].r,
            doc.nearestList[1].g, doc.nearestList[1].b);

        sprintf(buf,"(%s) En Uzak Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.farthestList[0].lineNumber,
            doc.farthestList[0].x, doc.farthestList[0].y,
            doc.farthestList[0].z, doc.farthestList[0].r,
            doc.farthestList[0].g, doc.farthestList[0].b);
        fprintf (OutputFile, buf ,OutputLineNumber);
        OutputLineNumber++;

        printf("(%s) En Uzak Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.farthestList[0].lineNumber,
            doc.farthestList[0].x, doc.farthestList[0].y,
            doc.farthestList[0].z, doc.farthestList[0].r,
            doc.farthestList[0].g, doc.farthestList[0].b);

        sprintf(buf,"(%s) En Uzak Nokta (%d. Satir):%f %f %f %d %d %d\n\n",
            doc.currentFileName, doc.farthestList[1].lineNumber,
            doc.farthestList[1].x, doc.farthestList[1].y,
            doc.farthestList[1].z, doc.farthestList[1].r,
            doc.farthestList[1].g, doc.farthestList[1].b);
        fprintf (OutputFile, buf ,OutputLineNumber);
        OutputLineNumber++;

        printf("(%s) En Uzak Nokta (%d. Satir):%f %f %f %d %d %d\n\n",
            doc.currentFileName,   doc.farthestList[1].lineNumber,
            doc.farthestList[1].x, doc.farthestList[1].y,
            doc.farthestList[1].z, doc.farthestList[1].r,
            doc.farthestList[1].g, doc.farthestList[1].b);
    }
    else if(operationType == 5)
    {
        char buf[7];
        sprintf(buf,"%f", doc.itemsAvg);
        char *finalString = ConcateString("(", doc.currentFileName, ") Noktalar arasi ortalama uzaklik : \n",  buf);
        fprintf (OutputFile, finalString ,OutputLineNumber);
        printf("(%s) Noktalar arasi ortalama uzaklik : %f\n", doc.currentFileName, doc.itemsAvg);
    }
    SaveOutput();
}

//istenen ascii dökümanın içindeki satır sayısını getirir
int GetDatacount(char *filePath)
{
   //Dosyadan okuma için file nesnesi
   FILE *fp;

   //Okuma modunda istenen dosya açılıyor
   fp  = fopen (filePath, "r");

   //Dosya açılamaz ise hata veriliyor.
   if (fp == NULL)
   {
      perror(FileOpenError);
      exit(EXIT_FAILURE);
   }

   //satır sayısını bulmak için index
   int count = 0;
   int dataCount = 0;

   //Satır satır döngü çin char array
   char chunk[256];

   //satır satır döngüye giriliyor
   while(fgets(chunk, sizeof(chunk), fp) != NULL)
   {
       if(count > 4)
       {
           dataCount++;
       }
       //her bir satırda index artırılıyor
       count++;
   }

   //iş bitince dosya kapatılıyor
   fclose(fp);

   //hesaplanan satir sayisi geri döndürülüyor.
   return dataCount;
}

//istenen binary dökümanın içindeki satır sayısını getirir
int GetBinaryDataCount(char *filePath, Document doc)
{
    FILE *file = fopen(filePath, "rb");
    float i = 0;
    int lineIndex=0;
    int tempIndex = 0;
    int rowCount = 0;
    while(fread(&i, sizeof(float), 1, file) != NULL)
    {
       if(tempIndex > 17)
       {
           if(lineIndex<2)
           {
               lineIndex++;
           }
           else if(lineIndex == 2)
           {
               if(doc.isWithRGB)
               {
                   lineIndex++;
               }
               else
               {
                   lineIndex = 0;
                   rowCount++;
               }
           }
           else if(lineIndex == 5)
           {
               lineIndex = 0;
               rowCount++;
           }
           else
           {
                lineIndex++;
           }
       }
       tempIndex++;
    }
    return rowCount;
}

//Verilen dizindeki istenen uzantılı dosyaların sayısını veren motot
int GetFileCount(char directoryPath[], char fileExtention[])
{
    //dosya sayısı hesaplamak için index
    int fileCount = 0;

    //dizin işlemleri için gerekli tanımlama yapılıyor.
    DIR *dir;
    struct dirent *ent;

    //dizin aciliyor
    if ((dir = opendir (directoryPath)) != NULL)
    {
       //dizin içindeki dosya sayısı kadar döngü başlıyor
       while ((ent = readdir (dir)) != NULL)
       {
           if(strlen(ent->d_name) > 3)
           {
              //her dönüşte dosya adı için raden farklı bir yer ayrılıyor
              char *fileName = (char *)malloc(strlen(ent->d_name)+1);
              //dosya adı yeni değğişkene kopyalanıyor.
              strcpy(fileName,ent->d_name);

              //uzantıyı almak için dosya adı . lar üzerinden parçalanıyor.
              char *ext = strrchr(fileName,'.');
              if(ext && strlen(ext) > 1)
              {
                  //uzantı en üstte tanımlanan değişkenle eşleşiyor mu diye kontrol ediliyor.
                  if(strcmp(ext, FileExtention) == 0 && strcmp(tolower(fileName), OutputFileName) != 0)
                  {
                      //eşleşen dosyalar için index 1 artırılıyor.
                      fileCount++;
                  }
              }
           }
       }

       //iş bitince dosya kapatılıor.
       closedir (dir);
    }
    else
    {
      //dizin açılamaz ise hata veriliyor.
      perror (DirectoryOpenError);
      return EXIT_FAILURE;
    }

    //hesaplanan dosya sayısı geri döndürülüyor.
    return fileCount;
}

//hata link listine yeni hatalar eklemek için kullanılan metot
void PushError(char *fileName, int lineNumber, char *message, int code)
{
    ErrorMessage currentError;
    currentError.errorFileName = fileName;
    currentError.errorLineNumber = lineNumber;
    currentError.errorMessageTest = message;
    currentError.errorCode = code;

    struct ErrorLinkList *node = (struct ErrorLinkList*) malloc(sizeof(struct ErrorLinkList));
    node->data = currentError;
    node->next = HeadError;
    HeadError = node;
}

//output.nkt dosyasını oluşturmak için kullanılan metot
void CreateOutputFile()
{
    FILE * fp;
    OutputFullPath = malloc(strlen(DirectoryPath) + strlen(OutputFileName) + 2);
    strcpy(OutputFullPath, DirectoryPath);
    strcat(OutputFullPath, "\\");
    strcat(OutputFullPath, OutputFileName);

    fp = fopen (OutputFullPath,"w"); //tek bir output yazılacağı için her program başlangıcınde içi dolu ise w ile içeriği temizleniyor.
    OutputFile = fp;
}

//oluşturulan output.nkt dosyasına yazılanları kaydeden metot.
void SaveOutput()
{
    fclose(OutputFile);
    OutputFile = fopen (OutputFullPath,"a"); //her bir kayıt işlemi sonrası dosya ekleme yani a modunda açılıyor
}

 //veirlen stringleri birlestiren helper metot
char * ConcateString(char *str1, char *str2, char *str3, char *str4)
{
    char *result = malloc(strlen(str1) + strlen(str2) + strlen(str3) + strlen(str4) + 10);
    strcpy(result, str1);
    strcat(result, str2);
    strcat(result, str3);
    strcat(result, str4);
    return result;
}

