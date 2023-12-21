// Pinos da Ponte H:
int stepper[4] = {8, 9, 10, 11};

// Definições e parâmetros do motor:
#define tbs 1         // time between step (ms)
#define spr 200       // steps per revolution (passos/volta)
#define ss 2          // screw step (mm/volta)
#define TAM_SEM 3     // tamanho da semente (mm)
#define PW 100        // largura do pulso (ms)

#define RECEIVE 3
#define SEND 4

float position = 0;
unsigned long t0;
bool skipcal=true;

int fss[4][4] = {{1, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 1}, {1, 0, 0, 1}}; //Foward Step Sequence

void move(float dist)
{ 
  int nsteps = int(abs((float)((float)dist/(float)ss)*spr)); //número de passos necessários para andar a distância desejada

  if(dist>0)
  {
    for(int i=0; i<nsteps; i++)
    {
      digitalWrite(stepper[0], fss[i%4][0]);
      digitalWrite(stepper[1], fss[i%4][1]);
      digitalWrite(stepper[2], fss[i%4][2]);
      digitalWrite(stepper[3], fss[i%4][3]);
      delay(tbs);
      position += ((float)ss/(float)spr);
      Serial.print("Posição atual: ");
      Serial.print(position);
      Serial.println(" mm");
    }
  }
  if(dist<0)
  {
    for(int i=(nsteps-1); i>=0; i--)
    {
      digitalWrite(stepper[0], fss[i%4][0]);
      digitalWrite(stepper[1], fss[i%4][1]);
      digitalWrite(stepper[2], fss[i%4][2]);
      digitalWrite(stepper[3], fss[i%4][3]);
      delay(tbs);
      position -= ((float)ss/(float)spr);
      Serial.print("Posição atual: ");
      Serial.print(position);
      Serial.println(" mm");
    }
  }
  digitalWrite(stepper[0], LOW);
  digitalWrite(stepper[1], LOW);
  digitalWrite(stepper[2], LOW);
  digitalWrite(stepper[3], LOW);
}

String command = "";

void callsys()
{
  Serial.println("\n\n----------------------------------------------------------------\n");
  Serial.println("* Calibração da ferramenta *");
  t0 = millis();
  while(1)
  {
    Serial.println("\nInsira o deslocamento desejado para zerar a posição do êmbolo:");
    Serial.print("(-165mm < D < 165mm) ou '0' para confirmar a calibração:");
    //Aguarda comando:
    while(1)
    {
      if (Serial.available() > 0)
      {
        skipcal = false;
        command = Serial.readString();
        break;
      }
      if(skipcal&&((millis()-t0)>5000))
      {
        break;
      }
    }
    if(skipcal&&((millis()-t0)>5000))
    {
      Serial.println("\n\nCALLSYS TIMED OUT!");
      break;
    }
    Serial.println(command);
    float desloc = command.toFloat();
    if(desloc==0)
    {
      Serial.println("Ferramenta Calibrada com sucesso!");
      break;
    }
    move(desloc);
  }
  position = 0;
  Serial.println("\n----------------------------------------------------------------");
}

int waitPulses()
{
  unsigned long tant=millis();
  int cont=0;
  while(1)
  {
    if(digitalRead(RECEIVE))
    {
      cont++;
      while(digitalRead(RECEIVE))
      {
        tant = millis();
      }
      do
      {
        if(digitalRead(RECEIVE))
        {
          cont++;
          while(digitalRead(RECEIVE))
          {
            tant = millis();
          }
        }
      }while((millis()-tant)<(2*PW));
      break;
    }
  }
  return cont;
}

void feedback()
{
  digitalWrite(SEND, HIGH);
  delay(PW);
  digitalWrite(SEND, LOW);
  delay(PW);
}

void setup()
{
  for(int i=0; i<4; i++)
  {
    pinMode(stepper[i], OUTPUT);
  }

  pinMode(RECEIVE, INPUT);
  pinMode(SEND, OUTPUT);

  Serial.begin(115200);

  callsys();

  //Serial.end();
}

void loop() 
{
  //Aguarda comando da controladora:
  int command = waitPulses();

  //Serial.println(command);

  //Escolhe e executa o comando:
  switch(command)
  {
    case 1:
      //Implanta a semente:
      move(TAM_SEM);
    break;
    case 2:
      //Abastece a semente:
      move(-TAM_SEM);
    break;
  }

  delay(PW);

  //Retorna que a tarefa foi concluída à controladora:
  feedback();
}