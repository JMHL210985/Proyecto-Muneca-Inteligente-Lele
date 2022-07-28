/******************************************************************************
 * reconocimentoDeVoz.ino
 * Reconocimiento y entrenamiento de voz
 * @author  JiapengLi
 * Modificado por Domingo Rosales, Armando Cruz, Juan Manuel Hernandez 
 * Julio 2022
 * este programa nos apoya para el reconocimeinto de comados u ordenes establecidas por voz
 * sobre las instrucciones que se daran 
 * asi como el ajuste de ruidos o sonidos de ambiente
 ******************************************************************************
 * 
 * Todos los comandos son insensibles a mayúsculas y minúsculas. 
 * Velocidad de transmisión en serie predeterminada 115200.
 * 
 * COMANDO        FORMATO                        EJEMPLO                    Comntario 
 * train          train (r0) (r1)...            train 0 2 45               registro de tramas
 * load           load (r0) (r1) ...            load 0 51 2 3              cargar registros
 * clear          clear                         clear                      elimina todos los registros  
 * record         record / record (r0) (r1)...  record / record 0 79       verifica el stado de la trman registrada
 * vr             vr                            vr                         checa el status del registro
 * getsig         getsig (r)                    getsig 0                   obtener la firma del registro
 * sigtrain       sigtrain (r) (sig)            sigtrain 0 ZERO            Entrenar un registro (r) con firma (sig)
 * settings       settings                      settings                   Comprobar la configuración actual del sistema
 ******************************************************************************
 * @section  HISTORY
 */
#include <SoftwareSerial.h>    
#include "VoiceRecognitionV3.h"  // libreria de reconocimiento de voz

/**        
 * configuracion de pines
 * Arduino    modulo de reconocimento de voz
 * 2   ------->     TX
 * 3   ------->     RX
 */
VR myVR(2,3);    // 2:RX 3:TX, // se pueden elegir cualquier otros pines

/***************************************************************************/
/** declaracion de funcione sde imprecion */
void printSeperator();
void printSignature(uint8_t *buf, int len);
void printVR(uint8_t *buf);
void printLoad(uint8_t *buf, uint8_t len);
void printTrain(uint8_t *buf, uint8_t len);
void printCheckRecognizer(uint8_t *buf);
void printUserGroup(uint8_t *buf, int len);
void printCheckRecord(uint8_t *buf, int num);
void printCheckRecordAll(uint8_t *buf, int num);
void printSigTrain(uint8_t *buf, uint8_t len);
void printSystemSettings(uint8_t *buf, int len);
void printHelp(void);

/***************************************************************************/
// Analizis de comandos
#define CMD_BUF_LEN      64+1
#define CMD_NUM     10
typedef int (*cmd_function_t)(int, int);
uint8_t cmd[CMD_BUF_LEN];
uint8_t cmd_cnt;
uint8_t *paraAddr;
int receiveCMD();
int checkCMD(int len);
int checkParaNum(int len);
int findPara(int len, int paraNum, uint8_t **addr);
int compareCMD(uint8_t *para1 , uint8_t *para2, int len);

int cmdTrain(int len, int paraNum);
int cmdLoad(int len, int paraNum);
int cmdTest(int len, int paraNum);
int cmdVR(int len, int paraNum);
int cmdClear(int len, int paraNum);
int cmdRecord(int len, int paraNum);
int cmdSigTrain(int len, int paraNum);
int cmdGetSig(int len, int paraNum);
int cmdSettings(int len, int paraNum);
int cmdHelp(int len, int paraNum);
/** cmdList, cmdLen, cmdFunction revisa si las  funciones tienen correspondencia */
const char cmdList[CMD_NUM][10] = {  // tabla de comandos
  {
    "train"  }
  ,
  {
    "load"  }
  , 
  {
    "clear"  }
  ,
  {
    "vr"  }
  ,
  {
    "record"  }
  ,
  {
    "sigtrain"  }
  ,
  {
    "getsig"  }
  ,
  {
    "Settings"  }
  ,
  {
    "test"  }
  ,
  {
    "help"  }
  ,
};
const char cmdLen[CMD_NUM]= {    // command length
  5,  //  {"train"},
  4,  //  {"load"}, 
  5,  //  {"clear"},
  2,  //  {"vr"},
  6,  //  {"record"},
  8,  //  {"sigtrain"},
  6,  //  {"getsig"},
  8,  //  {"Settings"},
  4,  //  {"test"},
  4,  //  {"help"}
};
cmd_function_t cmdFunction[CMD_NUM]={      // unción de manejo de comando (tabla de puntero de función)
  cmdTrain,
  cmdLoad,
  cmdClear,
  cmdVR,
  cmdRecord,
  cmdSigTrain,
  cmdGetSig,
  cmdSettings,
  cmdTest,
  cmdHelp,
};

/***************************************************************************/
/** datos temporales */
uint8_t buf[255];
uint8_t records[7]; // save record

void setup(void)
{
  myVR.begin(9600);

  /** initialize */
  Serial.begin(115200);
  Serial.println(F("Elechouse modulo d reconocimeinto de voz V3 \"trama\" ejemplo."));

  printSeperator();
  Serial.println(F("Usage:"));
  printSeperator();
  printHelp();
  printSeperator();
  cmd_cnt = 0;
}

void loop(void)
{
  int len, paraNum, paraLen, i;

  /** recibe comando por puerto seial */
  len = receiveCMD();
  if(len>0){
    /** comprobar si el comando recibido es válido */
    if(!checkCMD(len)){

      /** verificar el número de parámetro del comando recibido  */
      paraNum = checkParaNum(len);

      /** mostrar el comando recibido  */
      Serial.write(cmd, len);

      /** encontrar el primer parámetro */
      paraLen = findPara(len, 1, &paraAddr);

      /** comparar el comando recibido con el comando en la lista */
      for(i=0; i<CMD_NUM; i++){
        /** comparar la longitud del comando */
        if(paraLen == cmdLen[i]){
          /** comparar el contenido del comando */
          if( compareCMD(paraAddr, (uint8_t *)cmdList[i], paraLen) == 0 ){
             /** función de comando de llamada */
            if( cmdFunction[i](len, paraNum) != 0){
              printSeperator();
              Serial.println(F("Command Format Error!"));
              printSeperator();
            }
            break;
          }
        }
      }

/** el comando no es compatible*/
      if(i == CMD_NUM){ 
        printSeperator();
        Serial.println(F("Unkonwn command"));
        printSeperator();
      }
    }
    else{
   /** el comando recibido no es válido */
      printSeperator();
      Serial.println(F("Command format error"));
      printSeperator();
    }
  }

  /** intenta recibir el resultado del reconocimiento */
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    
/** voz reconocida, imprimir resultado */
    printVR(buf);
  }
}
/**
 * @brief recibe comando de Serial.
 * @param NINGUNO.
 * Longitud del comando @retval, si no recibe ningún comando, devuelve -1.
 */
int receiveCMD()
{
  int ret;
  int len;
  unsigned long start_millis;
  start_millis = millis();
  while(1){
    ret = Serial.read();
    if(ret>0){
      start_millis = millis();
      cmd[cmd_cnt] = ret;
      if(cmd[cmd_cnt] == '\n'){
        len = cmd_cnt+1;
        cmd_cnt = 0;
        return len;
      }
      cmd_cnt++;
      if(cmd_cnt == CMD_BUF_LEN){
        cmd_cnt = 0;
        return -1;
      }
    }

    if(millis() - start_millis > 100){
      cmd_cnt = 0;
      return -1;
    }
  }
}

/**
 * @brief compara dos comandos, no distingue entre mayúsculas y minúsculas.
 * @param para1 --> búfer de comando 1
 * para2 --> búfer de comandos 2
 * len --> longitud del búfer
 * @retval 0 --> igual
 * -1 --> desigual
 */
int compareCMD(uint8_t *para1 , uint8_t *para2, int len)
{
  int i;
  uint8_t res;
  for(i=0; i<len; i++){
    res = para2[i] - para1[i];
    if(res != 0 && res != 0x20){
      res = para1[i] - para2[i];
      if(res != 0 && res != 0x20){
        return -1;
      }
    }
  }
  return 0;
}


/**
 * @brief Comprueba el formato del comando.
 * @param len --> longitud del comando
 * @retval 0 --> el comando es válido
 * -1 --> el comando no es válido
 */
int checkCMD(int len)
{
  int i;
  for(i=0; i<len; i++){
    if(cmd[i] > 0x1F && cmd[i] < 0x7F){

    }
    else if(cmd[i] == '\t' || cmd[i] == ' ' || cmd[i] == '\r' || cmd[i] == '\n'){

    }
    else{
      return -1;
    }
  }
  return 0;
}

/**
 * @brief Comprobar el número de parámetros en el comando
 * @param len --> longitud del comando
 * @retval número de parámetros
 */
int checkParaNum(int len)
{
  int cnt=0, i;
  for(i=0; i<len; ){
    if(cmd[i]!='\t' && cmd[i]!=' ' && cmd[i] != '\r' && cmd[i] != '\n'){
      cnt++;
      while(cmd[i] != '\t' && cmd[i] != ' ' && cmd[i] != '\r' && cmd[i] != '\n'){
        i++;
      }
    }
    i++;
  }
  return cnt;
}
/**
 * @brief Encuentra el parámetro especificado.
 * @param len --> longitud del comando
 * paraIndex --> índice de parámetros
 * dirección --> valor de retorno. posición del parámetro
 * @retval longitud del parámetro especificado
 */
int findPara(int len, int paraIndex, uint8_t **addr)
{
  int cnt=0, i, paraLen;
  uint8_t dt;
  for(i=0; i<len; ){
    dt = cmd[i];
    if(dt!='\t' && dt!=' '){
      cnt++;
      if(paraIndex == cnt){
        *addr = cmd+i;
        paraLen = 0;
        while(cmd[i] != '\t' && cmd[i] != ' ' && cmd[i] != '\r' && cmd[i] != '\n'){
          i++;
          paraLen++;
        }
        return paraLen;
      }
      else{
        while(cmd[i] != '\t' && cmd[i] != ' ' && cmd[i] != '\r' && cmd[i] != '\n'){
          i++;
        }
      }
    }
    else{
      i++;
    }
  }
  return -1;
}

int cmdHelp(int len, int paraNum)
{
  if(paraNum != 1){
    return -1;
  }
  printSeperator();
  printHelp();
  printSeperator();
  return 0;
}
/**
 * @brief Maneja el comando "entrenar"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdTrain(int len, int paraNum)
{
  int i, ret;
  if(paraNum < 2 || paraNum > 8 ){
    return -1;
  }

  for(i=2; i<=paraNum; i++){
    findPara(len, i, &paraAddr);
    records[i-2] = atoi((char *)paraAddr);
    if(records[i-2] == 0 && *paraAddr != '0'){
      return -1;
    }
  }
  printSeperator();
  ret = myVR.train(records, paraNum-1, buf);
  //  ret = myVR.train(records, paraNum-1);
  if(ret >= 0){
    printTrain(buf, ret);
  }
  else if(ret == -1){
    Serial.println(F("Train failed."));
  }
  else if(ret == -2){
    Serial.println(F("Train Timeout."));
  }
  printSeperator();
  return 0;
}

/**
 * @brief Maneja el comando "cargar"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdLoad(int len, int paraNum)
{
  int i, ret;
  if(paraNum < 2 || paraNum > 8 ){
    return -1;
  }

  for(i=2; i<=paraNum; i++){
    findPara(len, i, &paraAddr);
    records[i-2] = atoi((char *)paraAddr);
    if(records[i-2] == 0 && *paraAddr != '0'){
      return -1;
    }
  }
  //  myVR.writehex(records, paraNum-1);
  ret = myVR.load(records, paraNum-1, buf);
  printSeperator();
  if(ret >= 0){
    printLoad(buf, ret);
  }
  else{
    Serial.println(F("Load failed or timeout."));
  }
  printSeperator();
  return 0;
}

/**
 * @brief Maneja el comando "borrar"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdClear(int len, int paraNum)
{
  if(paraNum != 1){
    return -1;
  }
  if(myVR.clear() == 0){
    printSeperator();
    Serial.println(F("Recognizer cleared."));
    printSeperator();
  }
  else{
    printSeperator();
    Serial.println(F("Clear recognizer failed or timeout."));
    printSeperator();
  }
  return 0;
}

/**
 * @brief Maneja el comando "vr"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdVR(int len, int paraNum)
{
  int ret;
  if(paraNum != 1){
    return -1;
  }
  ret = myVR.checkRecognizer(buf);
  if(ret<=0){
    printSeperator();
    Serial.println(F("Check recognizer failed or timeout."));
    printSeperator();
    return 0;
  }
  printSeperator();
  printCheckRecognizer(buf);
  printSeperator();
  return 0;
}
/**
 * @brief Maneja el comando "grabar"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdRecord(int len, int paraNum)
{
  int ret;
  if(paraNum == 1){
    ret = myVR.checkRecord(buf);
    printSeperator();
    if(ret>=0){
      printCheckRecordAll(buf, ret);
    }
    else{
      Serial.println(F("Check record failed or timeout."));
    }
    printSeperator();
  }
  else if(paraNum < 9){
    for(int i=2; i<=paraNum; i++){
      findPara(len, i, &paraAddr);
      records[i-2] = atoi((char *)paraAddr);
      if(records[i-2] == 0 && *paraAddr != '0'){
        return -1;
      }      
    }

    ret = myVR.checkRecord(buf, records, paraNum-1);    // auto clean duplicate records
    printSeperator();
    if(ret>=0){
      printCheckRecord(buf, ret);
    }
    else{
      Serial.println(F("Check record failed or timeout."));
    }
    printSeperator();
  }
  else{
    return -1;
  }
  return 0;
}

/**
 * @brief Maneja el comando "sigtrain"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdSigTrain(int len, int paraNum)
{
  int ret, sig_len;
  uint8_t *lastAddr;
  if(paraNum < 2){
    return -1;
  }

  findPara(len, 2, &paraAddr);
  records[0] = atoi((char *)paraAddr);
  if(records[0] == 0 && *paraAddr != '0'){
    return -1;
  }

  findPara(len, 3, &paraAddr);
  sig_len = findPara(len, paraNum, &lastAddr);
  sig_len +=( (unsigned int)lastAddr - (unsigned int)paraAddr );

  printSeperator();
  ret = myVR.trainWithSignature(records[0], paraAddr, sig_len, buf);
  //  ret = myVR.trainWithSignature(records, paraNum-1);
  if(ret >= 0){
    printSigTrain(buf, ret);
  }
  else{
    Serial.println(F("Train with signature failed or timeout."));
  }
  printSeperator();

  return 0;
}

/**
 * @brief Maneja el comando "getsig"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdGetSig(int len, int paraNum)
{
  int ret;
  if(paraNum != 2){
    return -1;
  }

  findPara(len, 2, &paraAddr);
  records[0] = atoi((char *)paraAddr);
  if(records[0] == 0 && *paraAddr != '0'){
    return -1;
  }

  ret = myVR.checkSignature(records[0], buf);

  printSeperator();
  if(ret == 0){
    Serial.println(F("Signature isn't set."));
  }
  else if(ret > 0){
    Serial.print(F("Signature:"));
    printSignature(buf, ret);
    Serial.println();
  }
  else{
    Serial.println(F("Get sig error or timeout."));
  }
  printSeperator();

  return 0;
}

/**
 * @brief Maneja el comando "prueba"
 * @param len --> longitud del comando
 * paraNum --> número de parámetros
 * @retval 0 --> éxito
 * -1 --> Error de formato de comando
 */
int cmdTest(int len, int paraNum)
{
  printSeperator();
  Serial.println(F("TEST is not supported."));
  printSeperator();
  return 0;
}

int cmdSettings(int len, int paraNum)
{
  int ret;
  if(paraNum != 1){
    return -1;
  }
  ret = myVR.checkSystemSettings(buf);
  if( ret > 0){
    printSeperator();
    printSystemSettings(buf, ret);
    printSeperator();
  }
  else{
    printSeperator();
    Serial.println(F("Check system settings error or timeout"));
    printSeperator();
  }
  return 0;
}

/**************************************************** ***************************/
/**
 * @brief Imprimir firma, si el carácter es invisible,
 * imprimir valor hexadecimal en su lugar.
 * @param buf --> longitud del comando
 * len --> número de parámetros
 */
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print(F("["));
      Serial.print(buf[i], HEX);
      Serial.print(F("]"));
    }
  }
}
/**
 * @brief Imprimir firma, si el carácter es invisible,
 * imprimir valor hexadecimal en su lugar.
 * @param buf --> Valor de retorno del módulo VR cuando se reconoce la voz.
 * buf[0] --> Modo de grupo (FF: Ninguno Grupo, 0x8n: Usuario, 0x0n: Sistema
 * buf[1] --> número de registro que se reconoce.
 * buf[2] --> Valor del índice (posición) del reconocedor del registro reconocido.
 * buf[3] --> Longitud de la firma
 * buf[4]~buf[n] --> Firma
 */
void printVR(uint8_t *buf)
{
  Serial.println(F("VR Index\tGroup\tRecordNum\tSignature"));

  Serial.print(buf[2], DEC);
  Serial.print(F("\t\t"));

  if(buf[0] == 0xFF){
    Serial.print(F("NONE"));
  }
  else if(buf[0]&0x80){
    Serial.print(F("UG "));
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print(F("SG "));
    Serial.print(buf[0], DEC);
  }
  Serial.print(F("\t"));

  Serial.print(buf[1], DEC);
  Serial.print(F("\t\t"));
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print(F("NONE"));
  }
  Serial.println(F("\r\n"));
}
/**
 * Separador de impresión @brief. Imprimir 80 '-'.
 */
void printSeperator()
{
  for(int i=0; i<80; i++){
    Serial.write('-');
  }
  Serial.println();
}

/**
 * @brief Imprime el estado del reconocedor.
 * @param buf --> Valor de retorno del módulo VR cuando se reconoce la voz.
 * buf[0] --> Número de registros de voz válidos en el reconocedor
 * buf[i+1] --> Número de registro.(0xFF: No cargado (modo no grupal), o no configurado (modo grupal)) (i= 0, 1, ... 6)
 * buf[8] --> Número de todos los registros de voz en el reconocedor
 * buf[9] --> Indicar posición de registros válidos.
 * buf[10] --> Modo de grupo indica (FF: Ninguno Grupo, 0x8n: Usuario, 0x0n: Sistema)
 */
void printCheckRecognizer(uint8_t *buf)
{
  Serial.print(F("All voice records in recognizer: "));
  Serial.println(buf[8], DEC);
  Serial.print(F("Valid voice records in recognizer: "));
  Serial.println(buf[0], DEC);
  if(buf[10] == 0xFF){
    Serial.println(F("VR is not in group mode."));
  }
  else if(buf[10]&0x80){
    Serial.print(F("VR is in user group mode:"));
    Serial.println(buf[10]&0x7F, DEC);
  }
  else{
    Serial.print(F("VR is in system group mode:"));
    Serial.println(buf[10], DEC);
  }
  Serial.println(F("VR Index\tRecord\t\tComment"));
  for(int i=0; i<7; i++){
    Serial.print(i, DEC);
    Serial.print(F("\t\t"));
    if(buf[i+1] == 0xFF){
      if(buf[10] == 0xFF){
        Serial.print(F("Unloaded\tNONE"));
      }
      else{
        Serial.print(F("Not Set\t\tNONE"));
      }
    }
    else{
      Serial.print(buf[i+1], DEC);
      Serial.print(F("\t\t"));
      if(buf[9]&(1<<i)){
        Serial.print(F("Valid"));
      }
      else{
        Serial.print(F("Untrained"));
      }
    }

    Serial.println();
  } 
}

/**
 * @brief Imprime el estado del tren del registro.
 * @param buf --> Comprobar el valor de retorno del comando de registro
 * buf[0] --> Número de registros comprobados
 * buf[2i+1] --> Número de registro.
 * buf[2i+2] --> Registrar estado del tren. (00: no entrenado, 01: entrenado, FF: valor de registro fuera de rango)
 * (i = 0 ~ buf[0]-1 )
 * num --> Número de registros entrenados
 */
void printCheckRecord(uint8_t *buf, int num)
{
  Serial.print(F("Check "));
  Serial.print(buf[0], DEC);
  Serial.println(F(" records."));

  Serial.print(num, DEC);
  if(num>1){
    Serial.println(F(" records trained."));
  }
  else{
    Serial.println(F(" record trained."));
  }

  for(int i=0; i<buf[0]*2; i += 2){
    Serial.print(buf[i+1], DEC);
    Serial.print(F("\t-->\t"));
    switch(buf[i+2]){
    case 0x01:
      Serial.print(F("Trained"));
      break;
    case 0x00:
      Serial.print(F("Untrained"));
      break;
    case 0xFF:
      Serial.print(F("Record value out of range"));
      break;
    default:
      Serial.print(F("Unknown Stauts"));
      break;
    }
    Serial.println();
  }
}

/**
 * @brief Imprime el estado del tren del registro.
 * @param buf --> Comprobar el valor de retorno del comando de registro
 * buf[0] --> Número de registros comprobados
 * buf[2i+1] --> Número de registro.
 * buf[2i+2] --> Registrar estado del tren. (00: no entrenado, 01: entrenado, FF: valor de registro fuera de rango)
 * (i = 0 ~ buf[0]-1 )
 * num --> Número de registros entrenados
 */
void printCheckRecordAll(uint8_t *buf, int num)
{
  Serial.print(F("Check 255"));
  Serial.println(F(" records."));

  Serial.print(num, DEC);
  if(num>1){
    Serial.println(F(" records trained."));
  }
  else{
    Serial.println(F(" record trained."));
  }
  myVR.writehex(buf, 255);
  for(int i=0; i<255; i++){
    if(buf[i] == 0xF0){
      continue;
    }
    Serial.print(i, DEC);
    Serial.print(F("\t-->\t"));
    switch(buf[i]){
    case 0x01:
      Serial.print(F("Trained"));
      break;
    case 0x00:
      Serial.print(F("Untrained"));
      break;
    case 0xFF:
      Serial.print(F("Record value out of range"));
      break;
    default:
      Serial.print(F("Unknown Stauts"));
      break;
    }
    Serial.println();
  }
}

/**
 * @brief Imprimir comprobar el resultado del grupo de usuarios.
 * @param buf --> Comprobar el valor de retorno del comando de registro
 * buf[8i] --> número de grupo.
 * buf[8i+1] --> posición de grupo 0 estado.
 * buf[8i+2] --> estado de la posición 1 del grupo.
 * ... ...
 * buf[8i+6] --> estado de la posición 5 del grupo.
 * buf[8i+7] --> estado de la posición del grupo 6.
 * (i = 0 ~ largo)
 * len --> número de grupos marcados
 */
void printUserGroup(uint8_t *buf, int len)
{
  int i, j;
  Serial.println(F("Check User Group:"));
  for(i=0; i<len; i++){
    Serial.print(F("Group:"));
    Serial.println(buf[8*i]);
    for(j=0; j<7; j++){
      if(buf[8*i+1+j] == 0xFF){
        Serial.print(F("NONE\t"));
      }
      else{
        Serial.print(buf[8*i+1+j], DEC);
        Serial.print(F("\t"));
      }
    }
    Serial.println();
  }
}

 /**
 * @brief Imprime el valor de retorno del comando "cargar".
 * @param buf --> valor de retorno del comando "cargar"
 * buf[0] --> número de registros que se cargan con éxito.
 * buf[2i+1] --> número de registro
 * buf[2i+2] --> registrar estado de carga.
 * 00 --> Cargado
 * FC --> Registro ya en reconocedor
 * FD --> Reconocedor lleno
 * FE --> Registro no entrenado
 * FF --> Valor fuera de rango"
 * (i = 0 ~ (len-1)/2 )
 * len --> longitud de buf
 */
void printLoad(uint8_t *buf, uint8_t len)
{
  if(len == 0){
    Serial.println(F("Load Successfully."));
    return;
  }
  else{
    Serial.print(F("Load success: "));
    Serial.println(buf[0], DEC);
  }
  for(int i=0; i<len-1; i += 2){
    Serial.print(F("Record "));
    Serial.print(buf[i+1], DEC);
    Serial.print(F("\t"));
    switch(buf[i+2]){
    case 0:
      Serial.println(F("Loaded"));
      break;
    case 0xFC:
      Serial.println(F("Record already in recognizer"));
      break;
    case 0xFD:
      Serial.println(F("Recognizer full"));
      break;
    case 0xFE:
      Serial.println(F("Record untrained"));
      break;
    case 0xFF:
      Serial.println(F("Value out of range"));
      break;
    default:
      Serial.println(F("Unknown status"));
      break;
    }
  }
}



 /**
 * @brief Imprime el valor de retorno del comando "entrenar".
 * @param buf --> valor de retorno del comando "entrenar"
 * buf[0] --> número de registros que se entrenaron con éxito.
 * buf[2i+1] --> número de registro
 * buf[2i+2] --> registrar el estado del tren.
 * 00 --> Entrenado
 * FE --> Tiempo de espera del tren
 * FF --> Valor fuera de rango"
 * (i = 0 ~ len-1)
 * len --> longitud de buf
 */
void printTrain(uint8_t *buf, uint8_t len)
{
  if(len == 0){
    Serial.println(F("Train Finish."));
    return;
  }
  else{
    Serial.print(F("Train success: "));
    Serial.println(buf[0], DEC);
  }
  for(int i=0; i<len-1; i += 2){
    Serial.print(F("Record "));
    Serial.print(buf[i+1], DEC);
    Serial.print(F("\t"));
    switch(buf[i+2]){
    case 0:
      Serial.println(F("Trained"));
      break;
    case 0xFE:
      Serial.println(F("Train Time Out"));
      break;
    case 0xFF:
      Serial.println(F("Value out of range"));
      break;
    default:
      Serial.print(F("Unknown status "));
      Serial.println(buf[i+2], HEX);
      break;
    }
  }
}

 /**
 * @brief Imprime el valor de retorno del comando "sigtrain".
 * @param buf --> valor de retorno del comando "sigtrain"
 * buf[0] --> número de registros que se entrenaron con éxito.
 * buf[1] --> número de registro
 * buf[2] --> registrar el estado del tren.
 * 00 --> Entrenado
 * F0 --> Entrenado, firma truncada
 * FE --> Tiempo de espera del tren
 * FF --> Valor fuera de rango"
 * buf[3] ~ buf[len-1] --> Firma.
 * len --> longitud de buf
 */
void printSigTrain(uint8_t *buf, uint8_t len)
{
  if(len == 0){
    Serial.println(F("Train With Signature Finish."));
    return;
  }
  else{
    Serial.print(F("Success: "));
    Serial.println(buf[0], DEC);
  }
  Serial.print(F("Grabado "));
  Serial.print(buf[1], DEC);
  Serial.print(F("\t"));
  switch(buf[2]){
  case 0:
    Serial.println(F("Trained"));
    break;
  case 0xF0:
    Serial.println(F("Trained, signature truncate"));
    break;
  case 0xFE:
    Serial.println(F("Trama  Time Out"));
    break;
  case 0xFF:
    Serial.println(F("Valor fuera de rango"));
    break;
  default:
    Serial.print(F("Estado desconocido "));
    Serial.println(buf[2], HEX);
    break;
  }
  Serial.print(F("SIG: "));
  Serial.write(buf+3, len-3);
  Serial.println();
}

/**
 * @brief Imprime el valor de retorno del comando "configuración".
 * @param buf --> valor de retorno del comando "configuración"
 * buf[0] --> número de registros que se entrenaron con éxito.
 * buf[1] --> número de registro
 * buf[2] --> registrar el estado del tren.
 * 00 --> Entrenado
 * F0 --> Entrenado, firma truncada
 * FE --> Tiempo de espera del tren
 * FF --> Valor fuera de rango"
 * buf[3] ~ buf[len-1] --> Firma.
 * len --> longitud de buf
 **/

const unsigned int io_pw_tab[16]={
  10,  15,  20,  25,  30,  35,  40,  45, 
  50,  75,  100, 200, 300, 400, 500, 1000
};

void printSystemSettings(uint8_t *buf, int len)
{

  switch(buf[0]){
  case 0:
  case 3:
    Serial.println(F("Baud rate: 9600"));
    break;
  case 1:
    Serial.println(F("Baud rate: 2400"));
    break;
  case 2:
    Serial.println(F("Baud rate: 4800"));
    break;
  case 4:
    Serial.println(F("Baud rate: 19200"));
    break;
  case 5:
    Serial.println(F("Baud rate: 38400"));
    break;
  default:
    Serial.println(F("Baud rate: UNKONOWN"));
    break;
  }

  switch(buf[1]){
  case 0:
  case 0xFF:
    Serial.println(F("Salida IO Mode: Pulse"));
    break;
  case 1:
    Serial.println(F("Salida IO Mode: Toggle"));
    break;
  case 2:
    Serial.println(F("Modo IO de salida: limpiar (cuando sea reconocido) "));
    break;
  case 3:
    Serial.println(F("Modo IO de salida: Set (cuando se reconoce)"));
    break;
  default:
    Serial.println(F("salida IO Mode: UNKONOWN"));
    break;
  }

  if(buf[2] > 15){
    Serial.println(F("ancho de pulso: DESCONICIDO"));
  }
  else{
    Serial.print(F("Ancho de pulos: "));
    Serial.print(io_pw_tab[buf[2]], DEC);
    Serial.println(F("ms"));
  }

  if(buf[3] == 0 || buf[3] == 0xFF){
    Serial.println(F("Carga automática: des habilitada"));
  }
  else{
    Serial.println(F("Carga automática: habilitada"));
  }

  switch(buf[4]){
  case 0:
  case 0xFF:
    Serial.println(F("Control de grupo por IO externo: deshabilitado"));
    break;
  case 1:
    Serial.println(F("Group control by external IO: system group selected"));
    break;
  case 2:
    Serial.println(F("Control de grupo por E/S externa: grupo de usuarios seleccionado"));
    break;
  default:
    Serial.println(F("Control de grupo por IO externo: ESTADO DESCONOCIDO"));
    break;
  }
}

void printHelp(void)
{
  Serial.println(F("COMANDO        FORMATO                        EJEMPLO                    COMENTARIO"));
  printSeperator();
  //  Serial.println(F("--------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("train          train (r0) (r1)...            train 0 2 45               cadena registro"));
  Serial.println(F("load           load (r0) (r1) ...            load 0 51 2 3              leer records"));
  Serial.println(F("clear          clear                         clear                      remueve registros  Recognizer"));
  Serial.println(F("record         record / record (r0) (r1)...  record / record 0 79       comprobar el status de la cadena recibidad"));
  Serial.println(F("vr             vr                            vr                         Comprobar el estado del reconocedor"));
  Serial.println(F("getsig         getsig (r)                    getsig 0                   Obtener la firma del registro (r)"));
  Serial.println(F("sigtrain       sigtrain (r) (sig)            sigtrain 0 ZERO            Entrenar un registro (r) con firma (sig)"));
  Serial.println(F("settings       settings                      settings                   Comprobar la configuración actual del sistema "));
  Serial.println(F("help           help                          help                       imprime el mensaje"));
}
