#define PIN_RELEU 7
int a;

void setup() {
  pinMode(A0,INPUT); // pinul de la senzorul de apa il setez ca input
                    // fiind cel care transmite semnalul de tensiune la Arduino
  Serial.begin(9600);

  pinMode(PIN_RELEU,OUTPUT); //pinul de la releu , IN , il setez ca output 
                            //deoarece acesta primeste semnalul de la Arduino
     
  digitalWrite(PIN_RELEU ,HIGH); 
}

void loop() {
  a = analogRead(A0); // variabila care retine valoarea convertita a semnalului de tensiune transmis de catre senzor
                     // poate lua valori din intervalul 0 - 1023
  if(a>500)
  {
    digitalWrite(PIN_RELEU,HIGH); // daca a este mai mare de 500 sa se opreasca pompa de apa
  }
  
  if(a<450)
  {
    digitalWrite(PIN_RELEU,LOW); // daca a este mai mic decat 450 sa se porneasca pompa de apa
  }

 Serial.println(a);
 delay(1000);
}
