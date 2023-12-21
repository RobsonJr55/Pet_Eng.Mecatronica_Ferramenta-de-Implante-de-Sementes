// Pinos da Ponte H:
int stepper[4] = {8, 9, 10, 11};

// Definições e parâmetros do motor:
#define tbs 1         // time between step (ms)
#define spr 200       // steps per revolution (passos/volta)
#define ss 2          // screw step (mm/volta)
#define TAM_SEM 3     // tamanho da semente (mm)

float position = 0;

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
  while(1)
  {
    Serial.println("\nInsira o deslocamento desejado para zerar a posição do êmbolo:");
    Serial.print("(-165mm < D < 165mm) ou '0' para confirmar a calibração:");
    //Aguarda comando:
    while(1)
    {
      if (Serial.available() > 0)
      {
        command = Serial.readString();
        break;
      }
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

void setup()
{
  for(int i=0; i<4; i++)
  {
    pinMode(stepper[i], OUTPUT);
  }
  Serial.begin(115200);

  callsys();

  Serial.end();
}

void loop() 
{
  Serial.begin(115200);
  Serial.println("\n\n----------------------------------------------------------------\n");
  Serial.println("* Recarga de sementes *");
  Serial.print("\nNúmero de sementes: ");
  while(1)
  {
    if (Serial.available() > 0)
    {
      command = Serial.readString();
      break;
    }
  }
  Serial.println(command);
  Serial.flush();
  int semtot = command.toInt();
  Serial.println("Recuando êmbolo...\n");
  move(-semtot*TAM_SEM);
  Serial.println("\n----------------------------------------------------------------");
  Serial.println("\n\n----------------------------------------------------------------");
  Serial.println("* Implante de sementes *");
  for(int i=0; i<semtot; i++)
  {
    Serial.print("\nSementes disponíveis: ");
    Serial.print(semtot-i);
    Serial.print("/");
    Serial.println(semtot);
    Serial.println("\n* Informe um número de sementes para implante *");
    while(1)
    {
      if (Serial.available() > 0)
      {
        command = Serial.readString();
        break;
      }
    }
    Serial.flush();
    int nsem = command.toInt();
    if((nsem > 0) && (nsem <= (semtot-i)))
    {
      move(nsem*TAM_SEM);
      i += (nsem-1);
    }else
    {
      Serial.print("\n! Não há ");
      Serial.print(nsem);
      Serial.println(" sementes disponíveis !");
      i--;
    }    
  }
  Serial.println("\n----------------------------------------------------------------");
  Serial.println("\n\n! Sementes esgotadas !");
  Serial.end();
  delay(2000);
}