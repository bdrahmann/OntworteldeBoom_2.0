/* Ontwortelde Boom
** versie 2.0
**
**
**
** De versie is getest in Velp
**
** Uitgangspunt is de Ontwortelde Boom zoals die in juli 2016 in Velp draait.
** De watersensor moet nog bepaald worden.
**
** Pomp1 werkt geheel buiten de Arduino om en levert itererend water
** Pomp2 wordt ingeschakeld als de "uit" tijd van pomp1 groter dan 12 minuten is en blijft dan aan.
** Bediening op afstand via GPRS mogelijk
** Uitlezen op afstand via GPRS mogelijk
** Pomp2 geeft SMS als hij ingeschakeld wordt
** Pomp2 kan via GPRS in en uitgeschakeld worden.
** Als het water in de bak onder een bepaald niveau komt wordt er een SMS gestuurd.
** De Arduino kan via GPRS gereset worden.
** Het volgen van het programma gebeurd via de seriele poort.
** Spanningsuitval wordt met een UPC opgevangen en bij uitval wordt SMS verstuurd
**
** nog niet gerealiseerd:
** De SMSen kunnen naar meerdere telefoonnummers gestuurd worden.
** Deze telefoonnummers kunnen vis GPRS ge(de)activeerd worden.
**
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


String SMScode01 = "SMS kode aangezet";
String SMScode02 = "SMS kode uitgezet";

String SMScode08 = "De bak van de ontwortelde boom is leeg. HELP!";
String SMScode09 = "Pomp2 is AAN gezet";
String SMScode10 = "Pomp2 is UIT gezet";
String SMScode11 = "Water sensor droog; Pomp 2 van ontwortelde boom is ingeschakeld!";
String SMScode12 = "Testbericht Ontwortelde boom";
String SMScode13 = "";	// later in te vullen naar behoefte
String SMScode14 = "Status pomp2 = AAN";
String SMScode15 = "Status pomp2 = UIT";
String SMScode16 = "Arduino BOOM wordt gereset";
String SMScode17 = "Netspanning weggevallen bij de boom";
String SMScode18 = "Netspanning weer terug bij de boom";
   




uint32_t LOG_LL_INTERVAL = 0;   // SMS LaagWater interval bij start 0
uint32_t syncTimeLL = 0;		// time of last SMS LaagWater

const int VlotterLaag = 8;      // pin 8 is alarmniveau: geeft signaal als HIGH wordt gemeten
//const int Simpower = 9;			// voor de oude Sim900 kaart en de	keystudio kaart
const int Simpower = 7;		// voor de Sim900 in Velp kaart
const int Pomp2 = 10;			// pin 10 voor aansturen pomp2
const int waterSensor = 11;		// watersensor
const int UPSsensor = A0;		// sensor voor UPC stroomuitval

// Globale waarden pompregeling
int PompStatus = 0;				// toestand van de Pompstatus

boolean vlotter = false;			// geeft aan of de vlotter laagwater ziet; true=laag
boolean sw_laagwater = false;	// begin met laagwater
boolean laagwateroud;			// vorige meting laagwater
uint32_t laagwater_delay = 10000;	// tijdsvertraging in laagwater om dender te voorkomen (10000)
uint32_t looptijdLL = 0;		// tijd tijdens het testen van de laagwatervlotter
uint32_t lopende_vlottertijd = 0; // loopt tijdens het testen van de laagwatervlotter

// variabelen voor sensorcontrole
//boolean Rain_SMS_Gestuurd = false;	// stuur slechts ��n keer een SMS als alle sensoren stuk zijn
boolean Droog = true;			// variable om droog vast te stellen
uint32_t droogtijd = 720000;	// tijd dat Pomp1 uit staat < 12 minuten van buiten instelbaar
//uint32_t droogtijd = 7200;		// tijdens testen kleine tijd
uint32_t droogtijdLL = 0;		// tijd tijdens het testen van de Droogtijd
uint32_t lopende_droogtijd = 0;	// loopt tijdens het testen van de Droogtijd

// variabelen voor UPC sensorcontrole
boolean netspanning_weg = false;


// Global variable for SMS yes or no
char SMScode = '1';			//stuur sms bij alarmsituaties, default aan
String telefoonnummer = "";
String telefoonnummer2 = "";
String telefoonnummer3 = "";
boolean bericht_gestuurd = false;	// om te voorkomen dat sms testbericht meer dan ��n per minuut gestuurd wordt
String textOpnieuw = "";			// tekst voor start GPRS module
String textMessage = "";			// input en output voor GPRS
String SMSstatus = "niet verbonden";				// 

void setup() {
	pinMode(Pomp2, OUTPUT);
	digitalWrite(Pomp2, LOW);	// zet pomp2 uit
	pinMode(waterSensor, INPUT);	// digital Pin to INPUT for the water sensor

	laagwateroud = digitalRead(VlotterLaag);	// lees de beginstand van de vlotter

	Serial.begin(9600);			// output via serial monitor
	Serial1.begin(19200);		// connection to GPRS network
	//Serial3.begin(19200);		// Default connection rate BT
	establishContact();  // send a byte to establish contact until receiver responds 

	// start GPRS module en log on
	// Automatically turn on the shield
	PrintProc('k', SMSstatus);
	startModem(textOpnieuw);
	if (textMessage.indexOf("DOWN") >= 0) {	// als modem uit is gezet
		PrintProc('j',"modem is uitgezet. Opnieuw opstarten");
		textOpnieuw = "opnieuw ";
		startModem(textOpnieuw);
	}

	// Give time to your GSM shield log on to network
	PrintProc('j',"GPRS modem logt on...");
	delay(20000);
	PrintProc('j',"GPRS modem ready...");
	SMSstatus = "verbonden met netwerk";
	PrintProc('k',SMSstatus);
	PrintProc('j',"modem wordt in SMS mode gezet");
	Serial1.print("AT+CMGF=1\r");	// AT command to set Serial1 to SMS mode
	delay(100);
	Serial1.print("AT+CNMI=2,2,0,0,0\r");	// Set module to send SMS data to serial out upon receipt 
	delay(100);
	textMessage = Serial1.readString();
	PrintProc('i',textMessage);
	delay(10);

		
	/* EPROM plaats
	0 = SMS code ja/nee = 1/0
	1 - 10 = telefoonnummer
	11 - 14 = Droogtijd in sec
	15 - 18 = Laagwater_delay in sec
	19 - 28 = telefoonnummer2
	29 - 38 = telefoonnummer3
	*/

	
	// Vaste gegevens uit EPROM ophalen
	SMScode = EEPROM.read(0); // SMScode ophalen uit EPROM op plaats 0
	telefoonnummer = "";
	telefoonnummer = LeesEprom(1, 10);
	droogtijd = LeesEprom(11, 14).toInt() * 1000;
	laagwater_delay = LeesEprom(15, 18).toInt() * 1000;
	PrintProc('c', String(laagwater_delay));
	telefoonnummer2 = LeesEprom(19, 28);
	telefoonnummer3 = LeesEprom(29, 38);
	PrintProc('m', String(SMScode));
	PrintProc('n', String(telefoonnummer));
	PrintProc('o', String(telefoonnummer2));
	PrintProc('p', String(telefoonnummer3));
	
}  // einde Setup

void startModem(String opnieuw) {
	PrintProc('j',"GPRS modem wordt " + opnieuw + "gestart...");
	digitalWrite(Simpower, HIGH);
	delay(1000);
	digitalWrite(Simpower, LOW);
	delay(5000);
	PrintProc('j',"GPRS modem is " + opnieuw + "gestart...");
	textMessage = Serial1.readString();
	PrintProc('i',textMessage);
	delay(10);
}

void establishContact() {	// om contact te maken met Processing
	uint32_t looptijd = millis();
	while (((Serial.available() <= 0) & (millis() - looptijd) <= 10000)) {
				Serial.print("A#");   // send a capital A
				delay(300);
	}
}

void loop() {
	
	LeesUPS();			// lees de UPS uit
	LeesSMS();			// kijk of er sms'jes gestuurd zijn
	LeesLaagwater();	// routine om de laagwatervlotter uit te lezen
	LeesWatersensor();	// kijk of er water gemeten wordt
	RegelPomp2();		// regel pomp2
	

	// TODO is routine Testsignaal nog nodig?
	TestSignaal();     // Stuurt een teken van leven naar SMS

}  //einde loop

void TestSignaal() {    // Stuurt een teken van leven naar SMS

						// stuur iedere dag om 12:00 uur een Testbericht
	// TODO bedenk een testsignaalroutine


}  // einde TestSignaal

int freeRam() {		// om te controleren hoeveel RAM er over is. Kan later weg
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

void SchrijfEprom(int b, int e, String info) {		// schrijf de info in de EPROM
	for (int i = b; i < e + 1; i = i + 1) {
		EEPROM.write(i, info.charAt(i - b));
		delay(20);	// tijdsvertraging nodig
	}
}

void LeesUPS() {		// UPS uitlezen om te kijken of stroom is uitgevallen
	
	// read the input on analog pin UPCsensor:
	int sensorValue = analogRead(UPSsensor);
	
	//float voltage = sensorValue * (10.0 / 1023.0); // oorsptonkelijke statement
	long voltage = map(sensorValue, 0, 100, 0, 5);		//  het nieuwe statement. Gemaakt om het kleine verschil tussen lamp aan en lamp uit van de UPS kenbaar te maken.
	PrintProc('q', String(voltage));
	
	if (voltage < 1)			// lamp is uit; 
		if (netspanning_weg) {	// dan even kijken of het het piepje was
			delay(1000);					// even wachten
			sensorValue = analogRead(UPSsensor);  // nog een keer lezen
			voltage = map(sensorValue, 0, 100, 0, 5);  // mappen
			if (voltage < 1) {	// lampje is nog steeds uit, dus netspanning is weer terug
				netspanning_weg = false;
				StuurBericht("18"); // zend SMS
			}
			else   // het lampje brandt weer, dus de netspanning is nog steeds weg
				;	// doe niks
		}
		else   // netspanning dus aanwezig
			;	// doe niks
	else  {  // lamp is aan = netspanning is weg
		if (!netspanning_weg) {	// nog niet gemeld
			netspanning_weg = true;			// zet de switch
			StuurBericht ("17"); // stuur SMS
		}


	}
	if (netspanning_weg) PrintProc('r', "1");
	else PrintProc('r', "0");
	
}

void PrintProc(char k, String record) {	// routine om naar Processing te printen
	/*
	char geef kode aan, en bepaalt plaats waar String wordt weergegeven
	*/
	Serial.print (k);
	Serial.print (record);
	Serial.print("#");
}
