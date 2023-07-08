
#include "fis_header.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include <LiquidCrystal_I2C.h> // Debe descargar la Libreria que controla el I2C
#include<Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); //0x20 o 0x27
int pinVentilador = 3;    // Pin del Arduino para controlar el ventilador
int pinCalentador = 4;   // Pin del Arduino para controlar el calentador
// Pin del Arduino donde está conectado el sensor DS18B20
const int pinSensorTemperatura = 2;
// Crea una instancia de la clase OneWire para comunicarse con el sensor
OneWire oneWire(pinSensorTemperatura);
// Crea una instancia de la clase DallasTemperature para controlar el sensor
DallasTemperature sensors(&oneWire);
float derivada = 0;

float x = 0;
float y = 0;
float xy = 0;
String texto = "";
// Number of inputs to the fuzzy inference system
const int fis_gcI = 2;
// Number of outputs to the fuzzy inference system
const int fis_gcO = 1;
// Number of rules to the fuzzy inference system
const int fis_gcR = 25;

FIS_TYPE g_fisInput[fis_gcI];
FIS_TYPE g_fisOutput[fis_gcO];

// Setup routine runs once when you press reset:
void setup()
{
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  lcd.init();
  lcd.backlight();          // Activar luz de fondo
  lcd.clear();              // Borrar LCD

  // Inicializa la comunicación con el sensor
  sensors.begin();
  // Inicia la comunicación serie
  Serial.begin(9600);
  // initialize the Analog pins for input.
  // Pin mode for Input: Error
  //pinMode(0 , INPUT);
  // Pin mode for Input: VariableE
  //pinMode(1 , INPUT);
  // initialize the Analog pins for output.
  // Pin mode for Output: voltaje
  //pinMode(2 , OUTPUT);

}

// Loop routine runs over and over again forever:
void loop()
{
  // Realiza una lectura de temperatura
  sensors.requestTemperatures();
  // Obtiene la temperatura en grados Celsius
  float temperatura = sensors.getTempCByIndex(0);
  int x = int(error(temperatura));
  int y = int(derivadaError(temperatura));
  //
  // Read Input: Error
  g_fisInput[0] =  x;
  // Read Input: VariableE
  g_fisInput[1] =  y;

  g_fisOutput[0] = 0;

  fis_evaluate();
  int xy = g_fisOutput[0];
  lcd.setCursor(0, 0);      // coordenadas LCD (x,y)
  lcd.print("Temp: ");   // Mensaje de inicio
  lcd.print(temperatura);
  lcd.print(" C");
  lcd.setCursor(0, 1);      // coordenadas LCD (x,y)
  lcd.print(texto);  // Mensaje de inicio

  // Set output vlaue: voltaje
  //analogWrite(2 , g_fisOutput[0]);
  //====================================
  controlarDispositivos(xy, temperatura);
  // Imprime la temperatura en el monitor serial
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  Serial.print("Voltaje:");
  Serial.println(xy);
  Serial.print("de x:");
  Serial.println(x);
  Serial.print("de y:");
  Serial.println(y);
  // Espera 1 segundo antes de realizar la siguiente lectura
  delay(2000);
}

void controlarDispositivos(float valorControl, float temp) {

  int valorSalida; // Variable para almacenar el valor de la señal de salida

  if (valorControl >= 0 && temp < 25) {
    // Accionar el calentador
    valorSalida = map(valorControl, 512, 1023, valorControl, 0);
    // Utilizar el valorSalida para controlar el calentador
    analogWrite(pinCalentador, valorSalida);
    digitalWrite(pinVentilador, LOW);  // Apagar el ventilador
    if (temp >= 20 && temp <= 24) {
      texto = "Obs Temp";
      digitalWrite(7, LOW); // Enciende el LED rojo
      digitalWrite(6, HIGH); // Enciende el LED amarillo
      digitalWrite(5, LOW); // Enciende el LED verde
    } else if (temp < 20) {
      texto = "Temp baja";
      digitalWrite(7, HIGH); // Enciende el LED rojo
      digitalWrite(6, LOW); // Enciende el LED amarillo
      digitalWrite(5, LOW); // Enciende el LED verde
    } else if (temp >= 24 && temp <= 26) {
      texto = "Temp opti";
      digitalWrite(7, LOW); // Enciende el LED rojo
      digitalWrite(6, LOW); // Enciende el LED amarillo
      digitalWrite(5, HIGH); // Enciende el LED verde
    }
  }
  else if (valorControl >= 0 && temp > 25) {

    // Accionar el ventilador
    valorSalida = map(valorControl, 0, 512, valorControl, 0);
    // Utilizar el valorSalida para controlar el ventilador
    analogWrite(pinVentilador, valorSalida);
    digitalWrite(pinCalentador, LOW);  // Apagar el calentador
    if (temp >= 26 && temp <= 30) {
      texto = "obs Temp";
      digitalWrite(7, LOW); // Enciende el LED rojo
      digitalWrite(6, HIGH); // Enciende el LED amarillo
      digitalWrite(5, LOW); // Enciende el LED verde
    } else if (temp > 30) {
      texto = "Temp alta";
      digitalWrite(7, HIGH); // Enciende el LED rojo
      digitalWrite(6, LOW); // Enciende el LED amarillo
      digitalWrite(5, LOW); // Enciende el LED verde
    } else if (temp >= 24 && temp <= 26) {
      texto = "Temp opti";
      digitalWrite(7, LOW); // Enciende el LED rojo
      digitalWrite(6, LOW); // Enciende el LED amarillo
      digitalWrite(5, HIGH); // Enciende el LED verde
    }

  }
}


float error(float temperatura) {
  float ref = 25;
  float erroractual = abs(ref - temperatura);
  return erroractual;
}

float derivadaError(float tempActual) {
  derivada = abs( error(tempActual) - derivada);
  return derivada;
}

//***********************************************************************
// Support functions for Fuzzy Inference System
//***********************************************************************
// Triangular Member Function
FIS_TYPE fis_trimf(FIS_TYPE x, FIS_TYPE* p)
{
  FIS_TYPE a = p[0], b = p[1], c = p[2];
  FIS_TYPE t1 = (x - a) / (b - a);
  FIS_TYPE t2 = (c - x) / (c - b);
  if ((a == b) && (b == c)) return (FIS_TYPE) (x == a);
  if (a == b) return (FIS_TYPE) (t2 * (b <= x) * (x <= c));
  if (b == c) return (FIS_TYPE) (t1 * (a <= x) * (x <= b));
  t1 = min(t1, t2);
  return (FIS_TYPE) max(t1, 0);
}

FIS_TYPE fis_min(FIS_TYPE a, FIS_TYPE b)
{
  return min(a, b);
}

FIS_TYPE fis_max(FIS_TYPE a, FIS_TYPE b)
{
  return max(a, b);
}

FIS_TYPE fis_array_operation(FIS_TYPE *array, int size, _FIS_ARR_OP pfnOp)
{
  int i;
  FIS_TYPE ret = 0;

  if (size == 0) return ret;
  if (size == 1) return array[0];

  ret = array[0];
  for (i = 1; i < size; i++)
  {
    ret = (*pfnOp)(ret, array[i]);
  }

  return ret;
}


//***********************************************************************
// Data for Fuzzy Inference System
//***********************************************************************
// Pointers to the implementations of member functions
_FIS_MF fis_gMF[] =
{
  fis_trimf
};

// Count of member function for each Input
int fis_gIMFCount[] = { 5, 5 };

// Count of member function for each Output
int fis_gOMFCount[] = { 5 };

// Coefficients for the Input Member Functions
FIS_TYPE fis_gMFI0Coeff1[] = { 0, 127, 255 };
FIS_TYPE fis_gMFI0Coeff2[] = { 192, 319, 446 };
FIS_TYPE fis_gMFI0Coeff3[] = { 384, 511, 638 };
FIS_TYPE fis_gMFI0Coeff4[] = { 576, 703, 830 };
FIS_TYPE fis_gMFI0Coeff5[] = { 769, 897, 1023 };
FIS_TYPE* fis_gMFI0Coeff[] = { fis_gMFI0Coeff1, fis_gMFI0Coeff2, fis_gMFI0Coeff3, fis_gMFI0Coeff4, fis_gMFI0Coeff5 };
FIS_TYPE fis_gMFI1Coeff1[] = { 0, 127, 254 };
FIS_TYPE fis_gMFI1Coeff2[] = { 384, 511, 638 };
FIS_TYPE fis_gMFI1Coeff3[] = { 769, 897, 1023 };
FIS_TYPE fis_gMFI1Coeff4[] = { 192, 319, 446 };
FIS_TYPE fis_gMFI1Coeff5[] = { 576, 703, 830 };
FIS_TYPE* fis_gMFI1Coeff[] = { fis_gMFI1Coeff1, fis_gMFI1Coeff2, fis_gMFI1Coeff3, fis_gMFI1Coeff4, fis_gMFI1Coeff5 };
FIS_TYPE** fis_gMFICoeff[] = { fis_gMFI0Coeff, fis_gMFI1Coeff };

// Coefficients for the Output Member Functions
FIS_TYPE fis_gMFO0Coeff1[] = { 0, 127, 254 };
FIS_TYPE fis_gMFO0Coeff2[] = { 384, 511, 638 };
FIS_TYPE fis_gMFO0Coeff3[] = { 769, 897, 1023 };
FIS_TYPE fis_gMFO0Coeff4[] = { 192, 319, 446 };
FIS_TYPE fis_gMFO0Coeff5[] = { 576, 703, 830 };
FIS_TYPE* fis_gMFO0Coeff[] = { fis_gMFO0Coeff1, fis_gMFO0Coeff2, fis_gMFO0Coeff3, fis_gMFO0Coeff4, fis_gMFO0Coeff5 };
FIS_TYPE** fis_gMFOCoeff[] = { fis_gMFO0Coeff };

// Input membership function set
int fis_gMFI0[] = { 0, 0, 0, 0, 0 };
int fis_gMFI1[] = { 0, 0, 0, 0, 0 };
int* fis_gMFI[] = { fis_gMFI0, fis_gMFI1};

// Output membership function set
int fis_gMFO0[] = { 0, 0, 0, 0, 0 };
int* fis_gMFO[] = { fis_gMFO0};

// Rule Weights
FIS_TYPE fis_gRWeight[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

// Rule Type
int fis_gRType[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

// Rule Inputs
int fis_gRI0[] = { 1, 1 };
int fis_gRI1[] = { 1, 4 };
int fis_gRI2[] = { 1, 2 };
int fis_gRI3[] = { 1, 5 };
int fis_gRI4[] = { 1, 3 };
int fis_gRI5[] = { 2, 1 };
int fis_gRI6[] = { 2, 4 };
int fis_gRI7[] = { 2, 2 };
int fis_gRI8[] = { 2, 5 };
int fis_gRI9[] = { 2, 3 };
int fis_gRI10[] = { 3, 1 };
int fis_gRI11[] = { 3, 4 };
int fis_gRI12[] = { 3, 2 };
int fis_gRI13[] = { 3, 5 };
int fis_gRI14[] = { 3, 3 };
int fis_gRI15[] = { 4, 1 };
int fis_gRI16[] = { 4, 4 };
int fis_gRI17[] = { 4, 2 };
int fis_gRI18[] = { 4, 3 };
int fis_gRI19[] = { 4, 5 };
int fis_gRI20[] = { 5, 1 };
int fis_gRI21[] = { 5, 4 };
int fis_gRI22[] = { 5, 2 };
int fis_gRI23[] = { 5, 5 };
int fis_gRI24[] = { 5, 3 };
int* fis_gRI[] = { fis_gRI0, fis_gRI1, fis_gRI2, fis_gRI3, fis_gRI4, fis_gRI5, fis_gRI6, fis_gRI7, fis_gRI8, fis_gRI9, fis_gRI10, fis_gRI11, fis_gRI12, fis_gRI13, fis_gRI14, fis_gRI15, fis_gRI16, fis_gRI17, fis_gRI18, fis_gRI19, fis_gRI20, fis_gRI21, fis_gRI22, fis_gRI23, fis_gRI24 };

// Rule Outputs
int fis_gRO0[] = { 1 };
int fis_gRO1[] = { 1 };
int fis_gRO2[] = { 1 };
int fis_gRO3[] = { 3 };
int fis_gRO4[] = { 3 };
int fis_gRO5[] = { 1 };
int fis_gRO6[] = { 1 };
int fis_gRO7[] = { 1 };
int fis_gRO8[] = { 5 };
int fis_gRO9[] = { 5 };
int fis_gRO10[] = { 1 };
int fis_gRO11[] = { 1 };
int fis_gRO12[] = { 1 };
int fis_gRO13[] = { 2 };
int fis_gRO14[] = { 2 };
int fis_gRO15[] = { 1 };
int fis_gRO16[] = { 1 };
int fis_gRO17[] = { 1 };
int fis_gRO18[] = { 5 };
int fis_gRO19[] = { 5 };
int fis_gRO20[] = { 1 };
int fis_gRO21[] = { 1 };
int fis_gRO22[] = { 1 };
int fis_gRO23[] = { 3 };
int fis_gRO24[] = { 3 };
int* fis_gRO[] = { fis_gRO0, fis_gRO1, fis_gRO2, fis_gRO3, fis_gRO4, fis_gRO5, fis_gRO6, fis_gRO7, fis_gRO8, fis_gRO9, fis_gRO10, fis_gRO11, fis_gRO12, fis_gRO13, fis_gRO14, fis_gRO15, fis_gRO16, fis_gRO17, fis_gRO18, fis_gRO19, fis_gRO20, fis_gRO21, fis_gRO22, fis_gRO23, fis_gRO24 };

// Input range Min
FIS_TYPE fis_gIMin[] = { 0, 0 };

// Input range Max
FIS_TYPE fis_gIMax[] = { 1023, 1023 };

// Output range Min
FIS_TYPE fis_gOMin[] = { 0 };

// Output range Max
FIS_TYPE fis_gOMax[] = { 1023 };

//***********************************************************************
// Data dependent support functions for Fuzzy Inference System
//***********************************************************************
FIS_TYPE fis_MF_out(FIS_TYPE** fuzzyRuleSet, FIS_TYPE x, int o)
{
  FIS_TYPE mfOut;
  int r;

  for (r = 0; r < fis_gcR; ++r)
  {
    int index = fis_gRO[r][o];
    if (index > 0)
    {
      index = index - 1;
      mfOut = (fis_gMF[fis_gMFO[o][index]])(x, fis_gMFOCoeff[o][index]);
    }
    else if (index < 0)
    {
      index = -index - 1;
      mfOut = 1 - (fis_gMF[fis_gMFO[o][index]])(x, fis_gMFOCoeff[o][index]);
    }
    else
    {
      mfOut = 0;
    }

    fuzzyRuleSet[0][r] = fis_min(mfOut, fuzzyRuleSet[1][r]);
  }
  return fis_array_operation(fuzzyRuleSet[0], fis_gcR, fis_max);
}

FIS_TYPE fis_defuzz_mom(FIS_TYPE** fuzzyRuleSet, int o)
{
  FIS_TYPE step = (fis_gOMax[o] - fis_gOMin[o]) / (FIS_RESOLUSION - 1);
  FIS_TYPE max = 0, dist, value, sum;
  int count;
  int i;

  for (i = 0; i < FIS_RESOLUSION; ++i)
  {
    dist = fis_gOMin[o] + (step * i);
    value = fis_MF_out(fuzzyRuleSet, dist, o);
    max = max(max, value);
  }

  sum = 0;
  count = 0;
  for (i = 0; i < FIS_RESOLUSION; i++)
  {
    dist = fis_gOMin[o] + (step * i);
    value = fis_MF_out(fuzzyRuleSet, dist, o);
    if (max == value)
    {
      ++count;
      sum += i;
    }
  }

  return (fis_gOMin[o] + ((step * sum) / count));
}

//***********************************************************************
// Fuzzy Inference System
//***********************************************************************
void fis_evaluate()
{
  FIS_TYPE fuzzyInput0[] = { 0, 0, 0, 0, 0 };
  FIS_TYPE fuzzyInput1[] = { 0, 0, 0, 0, 0 };
  FIS_TYPE* fuzzyInput[fis_gcI] = { fuzzyInput0, fuzzyInput1, };
  FIS_TYPE fuzzyOutput0[] = { 0, 0, 0, 0, 0 };
  FIS_TYPE* fuzzyOutput[fis_gcO] = { fuzzyOutput0, };
  FIS_TYPE fuzzyRules[fis_gcR] = { 0 };
  FIS_TYPE fuzzyFires[fis_gcR] = { 0 };
  FIS_TYPE* fuzzyRuleSet[] = { fuzzyRules, fuzzyFires };
  FIS_TYPE sW = 0;

  // Transforming input to fuzzy Input
  int i, j, r, o;
  for (i = 0; i < fis_gcI; ++i)
  {
    for (j = 0; j < fis_gIMFCount[i]; ++j)
    {
      fuzzyInput[i][j] =
        (fis_gMF[fis_gMFI[i][j]])(g_fisInput[i], fis_gMFICoeff[i][j]);
    }
  }

  int index = 0;
  for (r = 0; r < fis_gcR; ++r)
  {
    if (fis_gRType[r] == 1)
    {
      fuzzyFires[r] = FIS_MAX;
      for (i = 0; i < fis_gcI; ++i)
      {
        index = fis_gRI[r][i];
        if (index > 0)
          fuzzyFires[r] = fis_min(fuzzyFires[r], fuzzyInput[i][index - 1]);
        else if (index < 0)
          fuzzyFires[r] = fis_min(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
        else
          fuzzyFires[r] = fis_min(fuzzyFires[r], 1);
      }
    }
    else
    {
      fuzzyFires[r] = FIS_MIN;
      for (i = 0; i < fis_gcI; ++i)
      {
        index = fis_gRI[r][i];
        if (index > 0)
          fuzzyFires[r] = fis_max(fuzzyFires[r], fuzzyInput[i][index - 1]);
        else if (index < 0)
          fuzzyFires[r] = fis_max(fuzzyFires[r], 1 - fuzzyInput[i][-index - 1]);
        else
          fuzzyFires[r] = fis_max(fuzzyFires[r], 0);
      }
    }

    fuzzyFires[r] = fis_gRWeight[r] * fuzzyFires[r];
    sW += fuzzyFires[r];
  }

  if (sW == 0)
  {
    for (o = 0; o < fis_gcO; ++o)
    {
      g_fisOutput[o] = ((fis_gOMax[o] + fis_gOMin[o]) / 2);
    }
  }
  else
  {
    for (o = 0; o < fis_gcO; ++o)
    {
      g_fisOutput[o] = fis_defuzz_mom(fuzzyRuleSet, o);
    }
  }
}
