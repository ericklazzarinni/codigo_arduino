#include <manipular.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

/*
 * sd2 do esp e o pino rx
 * sd3 do esp e o pino tx
 * 
 * 8 pino rx do arduino conecta ao tx do esp(pino 4)
 * 9 pino tx do arduino conecta ao rx do esp(pino 2) com divisor de tensao
 * 
 * 
 */
SoftwareSerial mySerial(8, 9); // RX, TX //conexao serial virtual
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

int pulso_simulado = 5; 
int pulso_serra = 3;
//String tam_tubo = "4";
int contador_tubos_atual = 0;

//variaveis para entradas de tamanho de tubo


int barras[3] = {5,6,7}; //5 -> 4 metros, 6 -> 2 mettros, 7 -> 6 metros



ManipularArquivos gerenciar(10);
  File myFile;
  File arquivoBase;

void registro_producao()
{
  //escrever no arquivo base
  boolean aux = true;
  int tentativas  = 0;
  String texto = pegar_hora();
  String tam_tubo  = retorna_barra();
  
  texto = texto + "," + tam_tubo + ",";
  
  Serial.println("Pulso recebido");
  /*
     while(aux && tentativas < 10){
        if(gerenciar.escrever(arquivoBase, "/tubos/bd/arqbase.txt", "20,03,2020,20,00,00,"))
        { 
            Serial.println("Escrito");
           // contador_tubos_atual++;
         //   atualiza_lcd(contador_tubos_atual);
            aux = false;
            
        }
        else
        {
        Serial.println("Erro ao escrever");
        aux = true;
        tentativas++;
      
        }
        }
        */
 while(aux && tentativas < 10){
  arquivoBase = SD.open( "/tubos/bd/arqbase.txt", FILE_WRITE);
   if(arquivoBase)
   {
    arquivoBase.println(texto);
    arquivoBase.close();
    Serial.println("Escrito");
    aux = false;
    contador_tubos_atual++;
    atualiza_lcd(contador_tubos_atual);
    break;
   }
   else
    {
      arquivoBase.close();
      Serial.println("Não escrito");
      aux = true;
      tentativas++;

   }
 }
    
 }
      
  
String retorna_barra()
{
  int valores[3];
  for(int i = 0; i <3; i++)
  {
    valores[i] = digitalRead(barras[i]);
  }
  if(barras[0]) // pino 5, barra de 4 metros
   return "4";
  else if(barras[1])
   return "2"; //pino 6, barra de 2 metros
  else if(barras[2])
   return "6";  //pino 7, barra de 6 metros
   else
    return "-1";
  
}

String pegar_hora()
{
  String baffer;
  int aux = 0;
  // tratamento de hora
  mySerial.println("p");
  delay(500);
 while(aux < 10)
 { 
  char buffered = mySerial.read();
  //Serial.println(buffered);
  if(buffered == '*')
  {
  while(buffered != '&')
  {
    if(buffered == '*' || buffered == '&')
    {
     //nao fazer nada 
    }
    else{
    baffer = baffer + buffered;
    }
        buffered = mySerial.read();

  }
  }
  else
  {
    // char buffered = mySerial.read();
     aux++;
  }
 }
  
  
  return baffer;


  
}

void mostra_lcd(int pos1,int pos2, String texto)
{//parametros de entrada:
  /*
   * pos1, coluna do lcd
   * pos2, linha do lcd
   * texto, mensagem a ser exibida
   */
 lcd.setCursor(pos1, pos2);
 lcd.print(texto);

}

void atualiza_lcd(int contador)
{

  String contador_convertido = String(contador);
  Serial.println(contador);
  mostra_lcd(13, 1, contador_convertido);
}

void setup() {
  // put your setup code here, to run once:


  //confgiura das entradas da barra

  for(int i = 0;i < 3;i++)
  {
      pinMode(barras[i], INPUT);

  }
  
  pinMode(pulso_serra, INPUT);
  pinMode(pulso_simulado, OUTPUT);
  digitalWrite(pulso_simulado, HIGH);
  
  

//attachInterrupt(digitalPinToInterrupt(pulso_serra), registro_producao, RISING);

  Serial.begin(9600); //0 e 1 //inicio (begin) da conexao serial fisica
  mySerial.begin(9600); //inicio(begin) da conexao serial virtual
  //
  gerenciar.testar();

//  lcd.init();
  lcd.begin(16,2);
  lcd.setBacklight(HIGH);
  
  mostra_lcd(0, 0, "Tubo:");
  String tam_tubo = retorna_barra();
  mostra_lcd(7, 0, tam_tubo);
  mostra_lcd(9, 0, "metros");
  mostra_lcd(0,1, "Quantidade: ");
  String contador_convertido = String(contador_tubos_atual);

  mostra_lcd(13, 1, contador_convertido);

}

void loop() {
if(digitalRead(pulso_serra) == HIGH)
 {
  while(digitalRead(pulso_serra) != LOW)
  {
    
  }
  registro_producao();
 }
 if(Serial.available())
 {
     char comando = Serial.read();
    switch(comando)
    {
      case 'a':
      {//testar cartao
         if(gerenciar.testar())
   Serial.println("Cartão Conectado");
  else
   Serial.println("Cartão Desconectado");

      }break;

       case 'b':
      {//listar diretorios
        File root = SD.open("/");
         gerenciar.listarPastas(root, 0);
         root.close();
        
      }break;


      case 'c':
      {//criar arquivo
        if(gerenciar.criarArquivo("/tubos/bd/arqbase.txt"))
         {
           Serial.println("Arquivo criado");
        }
       else
       {
                  Serial.println("Arquivo nao criado");

        
       }
        
      }break;


       case 'd':
      {//ler arquivo
        gerenciar.abrirLer(arquivoBase, "/tubos/bd/arqbase.txt");
      
        
      }break;


      case 'e':
      {//criar diretorio
        if(gerenciar.criarDiretorio("/tubos/bd/"))
        Serial.println("Diretorio Criado");
        else
          Serial.println("Erro ao criar diretorio");
        
      }break;

      case 'f':
      {//escrever em um arquivo
        if(gerenciar.escrever(arquivoBase, "/tubos/bd/arqbase.txt", " "))
        Serial.println("Escrito");
        else
        Serial.println("Erro ao escrever");
      }break;

      case 'g':
      {

registro_producao();
}break;

case 'i':
{
//pulso simulado
Serial.println("Simulando pulso...");
digitalWrite(pulso_simulado, LOW);
digitalWrite(pulso_simulado, HIGH);
  
}break;

case 'j':
{
//pulso simulado
Serial.println("Simulando pulso...");
digitalWrite(pulso_simulado, LOW);
  
}break;

case 'p':
 {//pedir hora ao esp
   //mySerial.println("p");
   String hora = pegar_hora();
   Serial.println(hora);
 }break;
/*
  case 'l':
  { 
 Serial.println("Enviando mensagem para o esp");
 mySerial.println("Goodnight moon!"); //envia atraves da conexao ser virtual uma saudacao

   
    
  }break;
      */
 }//fechar o switch
 }//fecha o while


        
}//fecha o lopp


  
