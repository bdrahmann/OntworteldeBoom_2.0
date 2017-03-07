/* specificatie van van Android ontvangen kode
nu aangemaakt in Visual Studio2015

a = zet SMS code aan. Return 10 met SMScode
b = set SMS code uit. Return 10 met SMScode
c =
d = geef de raindrop sensor waarde door. Return: 17 + de sensor waarde.
e = 
f = geef de files op de SD card door aan Android. Return 11 met de files.
g = geef de inhoud van de file door aan Android. Return 12 met de inhoud.
h = geef Humidity en temperatuur door. Return 04 met Humidity en 05 met Temperatuur.
i =
j = set de opgestuurde datum en tijd in RTC. Return 3 met datum en 8 met tijd.
k = lees de Arduino settings uit. Meerdere settings in de info. Return
l = geef LUX door. Return 09 met LUX waarde
m = stuur de tijd op. Return 8 met tijd.
n = geef de raindrop droog code door. Return: 23,24 of 25 + de droog code.
o = stuur de datum op. Return 3 met datum.
p =
q = delete de file 'bestand' van SD card. Return 13 met bestandsnaam.
r = geef status VlotterLaag door. Return 2L of 2H
s = lees SDA Array uit over SD card actie. Return 06 en 07 met Schijfino en bestandsnaam
t = lees telefoonnummer uit. Return 14 met telefoonnummer
u = stuur PompStatus. Return: "15" met PompStatus
v = stuur Pompnummer. Return: "16" met Pompnummer.
w = zet pomp 1 of 2 handmatig aan of uit in status 8. Return: "31" met pompnummers en 0/1 voor uit/aan
x =
y = opnieuw zenden bepaalde gegevens bij oa opstart Android app
z = reset Arduino.

Return 26: Arduino send alle vaste gegevens
Return 29: Arduino stuurt de status van de vlotter delay door
Return 30: Arduino stuurt de status van de sensor delay door




*/

boolean VlotterWaarde = false;

void ReadBT() {          // Lees de BlueTooth input. en einde = #

	char inChar = 0;
	String Androidinfo = "";
	char s;
	String inBuffer = "";

	if (Serial3.available()) {
		while (inChar != '#')  // zolang einde info nog niet bereikt is
		{
			inChar = Serial3.read();
			inBuffer = inBuffer + String(inChar);
		}

		s = inBuffer.charAt(0);
		Androidinfo = inBuffer.substring(1, inBuffer.length() - 1);
		Serial.print("Ingelezen kode uit Android = ");
		Serial.print(s);
		Serial.print("   Androidinfo = ");
		Serial.println(Androidinfo);

		if (s == 'a') {  // zet de SMScode aan
			SMScode = '1';
			ReactieOpa(SMScode);	// schrijf EPROM en geef door aan Android
		}

		if (s == 'b') { // zet SMScode uit
			SMScode = '0';
			ReactieOpa(SMScode);	// schrijf EPROM en geef door aan Android
		}

		if (s == 'c') {
			// is niet meer nodig?
		}

		if (s == 'd') {
			ReactieOpd();  // geef de rainDrop door aan de smartphone
		}

		if (s == 'e') {
			ReactieOpe();  // geef de gemiddelde rainDrop door door aan de smartphone
		}

		if (s == 'f') {  // geef de files door aan Android
			ReactieOpf();
		}

		if (s == 'g') {  // geef de inhoud van de gekozen file door
			ReactieOpg(Androidinfo);
		}

		if (s == 'h') {
			ReactieOph();  // geef de humidity en temperatuur door aan de smartphone
		}

		if (s == 'j') {
			ReactieOpj(Androidinfo);	// set de datum en tijd in RTC
			ReactieOpo();				// stuur de nieuwe datum op
		}

		if (s == 'k') {
			ReactieOpk(Androidinfo);  // geef de pref file door aan de smartphone
		}

		if (s == 'l') {
			ReactieOpl();  // geef de lux door door aan de smartphone
		}

		if (s == 'q') {
			ReactieOpq(Androidinfo);  // delete de file op de SD card
		}

		if (s == 'r') {
			ReactieOpr();  // lees de Vlotterstand uit 
		}

		if (s == 's') {
			ReactieOps();  // lees error array SDAction uit
		}

		if (s == 't') {  // lees telefoonnummer uit en sla op
			ReactieOpt(Androidinfo);
		}

		if (s == 'u') {
			ReactieOpu();  // lees PompStatus uit
		}

		if (s == 'w') {		// zet pomp 1 of 2 handmatig aan of uit
			// ReactieOpw(Androidinfo); uitgezet in overleg met Daan
		}

		if (s == 'y') {		// stuur bepaalde gegevens nogmaals
			ReactieOpy();
		}

		if (s == 'z') {		// reset Arduino
							// softwareReset( WDTO_60MS);	// restart in 60 milliseconds. Werkt niet op alle MEGA's
			ReactieOpz();
		}



	}  // einde available
}  // einde ReadBT

void SendBT() {		// send info naar Android toestel

	if (PompStatus != PompStatusoud) {		// Alleen verzenden als er iets gewijzigd is
		ReactieOpu();		// stuur status	 
		PompStatusoud = PompStatus;
	}

	ReactieOph();		// geef hum en temp door
	ReactieOpl();		// geef LUX door
	ReactieOpd();		// geef sensorinfo door
	
	

	VlotterWaarde = digitalRead(VlotterLaag);
	if (VlotterWaarde != laagwateroud) {	// als de waarde veranderd is
		ReactieOpr();						// geef status VlotterLaag door
		laagwateroud = VlotterWaarde;
	}

	ReactieOpm();		// stuur de tijd door

}

void ReactieOpa(char SMSkode) {	// geef SMScode door aan Android
	String tkode = "";
	if (SMSkode == '0') tkode = "nee";
	else tkode = "ja";
	String tydelijk = "10" + String(tkode) + "#";
	Serial3.print(tydelijk);
	EEPROM.write(0, SMScode);	// en schrijf in EPROM
}

void ReactieOpd() { //stuur de rain sensorwaarde door naar de smartphone
	String tydelijk;
	if (Droog) tydelijk = "17droog#";
	else tydelijk = "17nat#";
	Serial3.print(tydelijk);
	delay(20);
	
}    // einde ReactieOpd

void ReactieOpe() { //stuur de gemiddelde rain sensorwaarden door naar de smartphone
	
}    // einde ReactieOpe

void ReactieOpf() {  // geef de files op de SD card door aan Android
					 // tijdens deze actie worden er geen metingen verricht!!!
	File root;
	root = SD.open("/");
	root.rewindDirectory();
	printDirectory(root, 0);
}

void printDirectory(File dir, int numTabs) {
	Serial3.print("11");
	while (true) {
		File entry = dir.openNextFile();
		if (!entry) {
			Serial.println(" no more files");
			Serial3.print("#");
			dir.rewindDirectory();
			break;
		}
		for (uint8_t i = 0; i < numTabs; i++) {
			//Serial.print('\t');
		}
		//Serial.print(entry.name());
		Serial3.print(entry.name());
		if (entry.isDirectory()) {
			//Serial.println("/");
			printDirectory(entry, numTabs + 1);
		}
		else {
			// files have sizes, directories do not
			Serial3.print(" ");
			Serial3.print(entry.size(), DEC);
			Serial3.print(",");
			//Serial.print("\t");
			//Serial.println(entry.size(), DEC);

		}
		entry.close();
	}
}

void ReactieOpg(String bestand) {	// geef de inhoud van de gekozen file door
	int L = bestand.length() + 1;
	char in;
	Serial.print("bestand: ");
	Serial.println(bestand);
	char fileNameCharArray[L];
	bestand.toCharArray(fileNameCharArray, L);
	File dataFile = SD.open(fileNameCharArray, FILE_READ);
	// if the file is available, read it:
	if (dataFile) {
		while (dataFile.available()) {
			Serial3.print("12");
			in = (dataFile.read());
			while (in != '\n') {
				Serial3.write(in);
				in = (dataFile.read());
			}
			Serial3.print("#");
		}
		dataFile.close();
		Serial3.print("12");
		Serial3.print("einde");
		Serial3.print("#");
	}
	// if the file isn't open, pop up an error:
	else {
		Serial.print("error opening ");
		for (int i = 0; i<L; i = i + 1) Serial.print(fileNameCharArray[i]);
		Serial.println();
	}

}

void ReactieOph() { //stuur de humidity en temperatuur door naar de smartphone
	String tydelijk = "04" + Hum + "#";
	if (isnan(h)) {
		Serial3.print(F("04Failed to read from DHT sensor!#"));
		delay(20);
	}
	else {
		Serial3.print(tydelijk);
		delay(20);
	}

	tydelijk = "05" + TempC + "#";
	if (isnan(t)) {
		Serial3.print(F("05Failed to read from DHT sensor!#"));
		delay(20);
	}
	else {
		Serial3.print(tydelijk);
		delay(20);
	}
}    // einde ReactieOph

void ReactieOpj(String DatumTijd) {// set de datum en tijd in RTC
								   // DatumTijd splitsen in afzonderlijke delen. Ze zijn gescheiden door":".
	int jaar, maand, dag, uur, min, sec;
	int l = DatumTijd.length();			// lengte van de string
	int b = 0;	// beginpositie in string
	int e = DatumTijd.indexOf(":");		// eindpositie
	jaar = (DatumTijd.substring(b, e)).toInt();
	b = e + 1;
	e = DatumTijd.indexOf(":", b);
	maand = (DatumTijd.substring(b, e)).toInt();
	b = e + 1;
	e = DatumTijd.indexOf(":", b);
	dag = (DatumTijd.substring(b, e)).toInt();
	b = e + 1;
	e = DatumTijd.indexOf(":", b);
	uur = (DatumTijd.substring(b, e)).toInt();
	b = e + 1;
	e = DatumTijd.indexOf(":", b);
	min = (DatumTijd.substring(b, e)).toInt();
	b = e + 1;
	e = DatumTijd.indexOf(":", b);
	sec = (DatumTijd.substring(b)).toInt();
	RTC.adjust(DateTime(jaar, maand, dag, uur, min, sec));
}

void ReactieOpk(String prefinfo) {	// lees de Arduino settings uit. Meerdere settings in info. En sla op in EPROM
	int b = 0;	// beginpositie in string
	int e = prefinfo.length();	// eindpositie in string

								// opsplitsen prefinfo in afzonderlijke velden. Ze zijn gescheiden door een "$"
								// alle velden zijn indien nodig, voorzien van voorloopnullen
	
	// droogtijd
	e = prefinfo.indexOf("$");
	String strDroogtijdsec = prefinfo.substring(b, e);
	uint32_t Droogtijdsec = strDroogtijdsec.toInt();
	SchrijfEprom(11, 14, strDroogtijdsec);
		
	// laagwaterdelay
	b = e + 1;	// het nieuwe begin
	e = prefinfo.indexOf("$", b);
	String strLaagwaterdelaysec = prefinfo.substring(b, e);
	uint32_t laagwater_delaysec = strLaagwaterdelaysec.toInt();
	SchrijfEprom(15, 18, strLaagwaterdelaysec);
		
	Droogtijd = Droogtijdsec * 1000;				// tijden in milliseconden omzetten
	laagwater_delay = laagwater_delaysec * 1000;	// tijden in milliseconden omzetten
	Sendkode26();	// en stuur naar Android
}

void SchrijfEprom(int b, int e, String info) {		// schrijf de info in de EPROM
	for (int i = b; i < e + 1; i = i + 1) {
		EEPROM.write(i, info.charAt(i - b));
		delay(20);	// tijdsvertraging nodig
	}
}

void ReactieOpl() { //stuur de lux door naar de smartphone
					// TODO hier later de error code aan toevoegen
	String tydelijk;
	tydelijk = "09" + Light + "#";
	Serial3.print(tydelijk);
	delay(20);
}    // einde ReactieOpl

void ReactieOpm() {		// stuur tijd
	now = RTC.now();  // fetch the time
	String tydelijk = "08" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "#";
	Serial3.print(tydelijk);
	delay(20);

}




void ReactieOpo() {		// stuur datum
	now = RTC.now();  // fetch the time
	String tydelijk = "03" + String(now.day()) + ":" + String(now.month()) + ":" + String(now.year()) + "#";
	Serial3.print(tydelijk);
	delay(20);
}

void ReactieOpq(String bestand) {  // delete de file 'bestand' van SD card
	int L = bestand.length() + 1;
	char in;
	char fileNameCharArray[L];
	bestand.toCharArray(fileNameCharArray, L);
	if (bestand.equals(filename)) {
		Serial.print("File: ");
		Serial.print(bestand);
		Serial.println(" is in gebruik.");
		Serial3.print("13");
		Serial3.print(bestand);
		Serial3.print(" is in gebruik.#");
	}
	else
		if (SD.exists(fileNameCharArray)) {
			SD.remove(fileNameCharArray);
			Serial.print("File: ");
			Serial.print(bestand);
			Serial.println(" deleted.");
			Serial3.print("13");
			Serial3.print(bestand);
			Serial3.print(" deleted#");
		}
		else {
			Serial.print("File: ");
			Serial.print(bestand);
			Serial.println(" bestaat niet.");
			Serial3.print("13");
			Serial3.print(bestand);
			Serial3.print(" bestaat niet.#");
		}
}

void ReactieOpr() {		// geeft Vlotterstand door
	VlotterWaarde = digitalRead(VlotterLaag);
	if (VlotterWaarde == HIGH) {
		Serial3.print("02L#");
		delay(20);
	}
	else {
		Serial3.print("02H#");
		delay(20);
	}

} // einde ReactieOpr

void ReactieOps() {  // lees error array SDAction uit
	String tydelijk = filename;
	if (SDActionS) Serial3.print(F("06card initialized#"));
	else Serial3.print(F("06Card failed, or not present#"));
	delay(20);
	if (SDActionF) {
		tydelijk = "07" + tydelijk + "#";
		Serial3.print(tydelijk);
	}
	else {
		Serial3.print(F("07couldnt create file#"));
	}
	delay(20);
}  // einde reactieOps

void ReactieOpt(String telnummer) {  // lees telefoonnummer uit en sla op
	telefoonnummer = telnummer;		// sla telefoonnummer op
	String tydelijk = "14" + telnummer + "#";
	Serial3.print(tydelijk);
	delay(20);
	for (int i = 1; i < 11; i = i + 1) {
		EEPROM.write(i, telnummer.charAt(i - 1));
		delay(20);	// wegschrijven duurt 3,3 msec
		Serial.println("EEPROM.write is aangeroepen voor telefoonnummer wegschrijven");
	}
}

void ReactieOpu() {		// stuur PompStatus. Return: "15" met PompStatus
	if (PompStatus == 4) ReactieOpv();	// stuur dan eerst het Pompnummer op

	String tydelijk;
	tydelijk = "15" + String(PompStatus) + "#";
	Serial3.print(tydelijk);
	delay(20);

	//Serial.print("Pompstatus verzonden = ");
	//Serial.println(PompStatus);
}

void ReactieOpv() {		// stuur Pompnummer. Return: "16" met Pompnummer.
	String tydelijk;
	tydelijk = "16" + String(Pig - 8) + "#";
	Serial3.print(tydelijk);
	delay(20);

}

void ReactieOpw(String tydelijk) {		// zet pomp 1 of 2 handmatig aan of uit
	String PS;
	PompStatus = 8; //  naar status 8
	Sendkode30(Droogtijd, Droogtijd);	// stuur de status 100% om de Sensor progressbar uit te zetten, mocht die nog aan staan

	if (tydelijk.substring(0, 1).equals("0")) {
		digitalWrite(Pomp1, LOW);	//  zet pomp 1 uit
		PS = "In ReactieOpw is pomp 1 uitgezet. "; Serial.println(PS);
		handpomp1 = 0;
	}
	else {
		digitalWrite(Pomp1, HIGH);	// zet pomp 1 aan
		PS = "In ReactieOpw is pomp 1 aangezet. "; Serial.println(PS);
		handpomp1 = 1;
	}
	if (tydelijk.substring(1).equals("0")) {
		digitalWrite(Pomp2, LOW);	//  zet pomp 2 uit
		PS = "In ReactieOpw is pomp 2 uitgezet. "; Serial.println(PS);
		handpomp2 = 0;
	}
	else {
		digitalWrite(Pomp2, HIGH);	// zet pomp 2 aan
		PS = "In ReactieOpw is pomp 2 aangezet. "; Serial.println(PS);
		handpomp2 = 1;
	}

	Sendkode31();	// stuur de info terug
}

void ReactieOpy() {		// stuur bepaalde berichten opnieuw

	ReactieOps();		// schijfinfo
	ReactieOpa(SMScode);
	ReactieOpt(telefoonnummer);
	ReactieOpo();	// stuur de datum op
	ReactieOpu();	// pompstatus
	ReactieOpv();	// pompnummer
	ReactieOpr();	// stuur vlotterstatus op
	Sendkode26();	// stuur de vaste gegevens
	Sendkode31();	// stuur de handbedieningsstatus

}

void ReactieOpz() {		// restart Arduino door initialisatie gegevens
	String PS;
	Pig = Pomp1;
	PompStatus = 0;				// toestand van de Pompstatus
	PompStatusoud = 0;			// de vorige Pompstatus
	sw_laagwater = true;	// begin met laagwater
	digitalWrite(Pomp1, LOW);		// zet pomp1 uit. De pompen zijn active LOW
	PS = "In ReactieOpz is pomp 1 uitgezet. "; Serial.println(PS);
	handpomp1 = 0;
	digitalWrite(Pomp2, LOW);		// zet pomp2 uit
	PS = "In ReactieOpz is pomp 2 uitgezet. "; Serial.println(PS);
	handpomp2 = 0;
	laagwateroud = digitalRead(VlotterLaag);	// lees de beginstand van de vlotter
	WriteSDcard1("00");
	logfile.println(F("0,0,Reset arduino"));
	logfile.flush();
	ReactieOpy();	// stuur bepaalde berichten opnieuw
	Sendkode29(laagwater_delay, laagwater_delay);	// stuur de status "100" om progressbar uit te zetten
	Sendkode30(Droogtijd, Droogtijd);	// stuur de status 100% om de progressbar uit te zetten
	Sendkode31();	// zet de handschakelaars in de juiste stand
}

void Sendkode26() {	// alle vaste gegevens naar Android sturen
	String tydelijk = "26" + String(Droogtijd / 1000) + "$"  + String(laagwater_delay / 1000) + "#";
	Serial3.print(tydelijk);
	delay(20);
}

void Sendkode29(uint32_t status, uint32_t max) { // stuur de delaystatus op van laagwater
											//Serial.print("status en max = ");
											//Serial.print(status);
											//Serial.println(max);
	String s = String(status / 1000);
	String m = String(max / 1000);
	Serial3.print("29" + s + "$" + m + "#");
	delay(20);
}

void Sendkode30(uint32_t status, uint32_t max) { // stuur de delaystatus op van droogtijd
	Serial.print("droogtijdstatus en max = ");
	Serial.print(status);
	Serial.println(max);
	String s = String(status / 1000);
	String m = String(max / 1000);
	Serial3.print("30" + s + "$" + m + "#");
	delay(20);
}

void Sendkode31() {
	// Serial.print("handpomp1 = ");
	// Serial.println(handpomp1);
	// Serial.print("handpomp2 = ");
	// Serial.println(handpomp2);
	Serial3.print("31" + String(handpomp1) + String(handpomp2) + "#");
	delay(20);
}
