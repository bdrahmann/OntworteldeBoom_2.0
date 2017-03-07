/* Ontwortelde Boom
** versie 2.0
** 
** 20170306
** 
** Uitgangspunt is de Ontwortelde Boom zoals die in juli 2016 in Velp draait.
** Met een regensensor uit de USA: RG-11
** Een drukmeter om het niveau in de waterbak te meten.
** Pomp 1 en Pomp 2 werken itererend aan en uit om de wortels niet te nat te maken.
** Geen SD card meer.
** Bediening op afstand via GPRS.
** Uitlezen op afstand via GPRS
** Geen klok meer nodig
** Geen temperatuur en Humidity voeler nodig
** Geen lichtsensor nodig
**
** Credit: The following example was used as a reference
** Rui Santos: http://randomnerdtutorials.wordpress.com
** aangepast door BDR
** datum: 20151120
** 
**
**
*/
#include <MemoryFree.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Streaming.h>
#include "RunningAverage.h"	// running average

// TODO SMScodes aanpassen
/*
String SMScode08 = "De bak van de ontwortelde boom is leeg. HELP!";
String SMScode10 = "RG-11 sensor droog; Pomp 1 van ontwortelde boom is uitgeschakeld!";
String SMScode11 = "RG-11 sensor droog; Pomp 2 van ontwortelde boom is uitgeschakeld!";
String SMScode12 = "Testbericht Ontwortelde boom";
String SMScode13 = "Alle sensoren van ontwortelde boom zijn stuk!";
*/

#define LOG_R_INTERVAL  600000 // Raindrop interval 10 min
uint32_t LOG_LL_INTERVAL = 0;  // SMS LaagWater interval bij start 0
uint32_t syncTimeLL = 0; // time of last SMS LaagWater

const int Pomp1 = 2;			// pin 2 is aansturen pomp 1
const int Pomp2 = 3;			// Pin 3 is aansturen pomp 2
// const int Simpower = 7;		// voor de sim900 kaart Shield B-v1.1
const int Simpower = 9;			// voor de Geeetech Sim900 kaart
const int RG_sensor = 11;		// regensensor RG-11

// int Pig = Pomp1;				// Starten met PominGebruik = pomp1

// Globale waarde pompregeling
int PompStatus = 0;				// toestand van de Pompstatus
int PompStatusoud = 0;			// de vorige Pompstatus
int handpomp1 = 0;				// geeft aan of pomp1 in handmatige status uit/aan is
int handpomp2 = 0;				// geeft aan of pomp2 in handmatige status uit/aan is
boolean sw_laagwater = true;	// begin met laagwater
boolean laagwateroud;			// vorige meting laagwater
uint32_t laagwater_delay = 0;	// tijdsvertraging in laagwater om dender te voorkomen (10000)
uint32_t looptijdLL = 0;		// loopt tijdens het testen van de laagwatervlotter

// variabelen voor sensorcontrole
boolean Rain_SMS_Gestuurd = false;	// stuur slechts ��n keer een SMS als alle sensoren stuk zijn
boolean Droog = true;			// variable om droog vast te stellen
uint32_t Droogtijd = 0;			// tijd om druppelsensoren nat te laten worden; van buiten instelbaar (20000)
uint32_t DroogtijdLL = 0;		// loopt tijdens het testen van de Droogtijd

// Global variable for SMS yes or no
char SMScode = '0';			//stuur sms bij alarmsituaties, default uit
String telefoonnummer = "";
boolean bericht_gestuurd = false;	// om te voorkomen dat sms testbericht meer dan ��n per minuut gestuurd wordt

void setup() {
	String PS;		// is de PrintString
	
	pinMode(Pomp1, OUTPUT);
	digitalWrite(Pomp1, LOW);		// zet pomp1 uit. De pompen zijn active LOW
	pinMode(Pomp2, OUTPUT);
	digitalWrite(Pomp2, LOW);		// zet pomp2 uit

	pinMode(RG_sensor, INPUT);		// digital Pin to INPUT for the RG-11 sensor
	
	laagwateroud = digitalRead(VlotterLaag);	// lees de beginstand van de vlotter
	
	Serial.begin(9600);			// output via serial monitor
	Serial1.begin(19200);		// connection to GPRS network
	Serial3.begin(19200);		// Default connection rate BT
	Serial.println();
	PS = "In setup is pomp 1 uitgezet. In setup is pomp 2 uitgezet"; Serial.println(PS);
		
	/* EPROM plaats
	0 = SMS code ja/nee = 1/0
	1 - 10 = telefoonnummer
	11 - 14 = Droogtijd in sec
	15 - 18 = Laagwater_delay in sec
	*/
	// Vaste gegevens uit EPROM ophalen
	SMScode = EEPROM.read(0); // SMScode ophalen uit EPROM op plaats 0
	telefoonnummer = "";
	telefoonnummer = LeesEprom(1, 10);
	Serial.print("telefoonnummer uit EPROM =  "); Serial.println(telefoonnummer);
	Droogtijd = LeesEprom(11, 14).toInt()*1000;
	laagwater_delay = LeesEprom(15, 18).toInt()*1000;
	
	ReactieOpy();	// stuur bepaalde berichten opnieuw
	Sendkode29(laagwater_delay, laagwater_delay);	// stuur de status "100" om progressbar uit te zetten
	Sendkode30(Droogtijd, Droogtijd);	// stuur de status 100% om de progressbar uit te zetten
	
}  // einde Setup

void loop() {

	DuoPompRegeling();	// regelt met twee pompen
	LeesRegenSensor();	// de regensensor RG-11
	LaagWater();	// routine om de laagwatervlotter uit te lezen
	TestSignaal();     // Stuurt een teken van leven naar SMS
	ReadBT();		// Lees de BlueTooth input
	SendBT();		// Zend info naar Android toestel	

}  //einde loop

void TestSignaal() {    // Stuurt een teken van leven naar SMS

						// stuur iedere dag om 12:00 uur een Testbericht
	// TODO bedenk een testsignaalroutine
	

}  // einde TestSignaal

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}



String LeesEprom(int b, int e) {
	String result = "";
	int x;
	for (int i = b; i < e + 1; i = i + 1) {
		x = EEPROM.read(i);  // in x staat een ascii cijfer
		result = result + String(x - 48); // en nu staat er een getalstring
	}
	return result;
}
