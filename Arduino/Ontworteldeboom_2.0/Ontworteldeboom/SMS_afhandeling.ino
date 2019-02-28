/*	Afhandeling SMS
** Versie 2.0

/*	Afhandeling SMS en schrijven naar SD card


*/
void StuurBericht(String kode) {

	if (SMScode == '1') sendSMS(kode);      // stuur SMS bericht
	
}  // einde StuurBericht

void sendSMS(String kode) {

		String SMSstatus = "";
		SMSstatus = "Arduino belt";
		PrintProc('j', SMSstatus);

		String slash = "\"";
		String beginString = "AT + CMGS = ";
		String totaalString = beginString + slash + telefoonnummer + slash;

		Serial1.print("AT+CMGF=1\r"); // AT command to send SMS message
		delay(100);
		Serial1.println(totaalString);
		delay(100);
		
		if (kode == "01") {		// SMSkode aan
			Serial1.println(SMScode01);
			PrintProc('m', String(SMScode));
			goto gedaan;
		}
		if (kode == "02") {		// SMSkode uit
			Serial1.println(SMScode02);
			PrintProc('m', String(SMScode));
			goto gedaan;
		}
		if (kode == "08") {		// lege bak
			Serial1.println(SMScode08);
			PrintProc('j', SMScode08);
			goto gedaan;
		}
		if (kode == "09") {		// Pomp2 handmatig AAN gezet
			Serial1.println(SMScode09);
			PrintProc('j', SMScode09);
			goto gedaan;
		}
		if (kode == "10") {		// Pomp2 handmatig UIT gezet
			Serial1.println(SMScode10);
			PrintProc('j', SMScode10);
			goto gedaan;
		}
		if (kode == "11") {		// Pomp2 ingeschakeld
			Serial1.println(SMScode11);
			PrintProc('j', SMScode11);
			goto gedaan;
		}
		if (kode == "12") {		// Testbericht
			Serial1.println(SMScode12);
			PrintProc('j', SMScode12);
			goto gedaan;
		}
		if (kode == "13") {		// variabel bericht
			Serial1.println(SMScode13);
			PrintProc('j', SMScode13);
			goto gedaan;
		}
		if (kode == "14") {		// Status pomp2 is aan
			Serial1.println(SMScode14);
			PrintProc('j', SMScode14);
			goto gedaan;
		}
		if (kode == "15") {		// Status pomp2 is uit
			Serial1.println(SMScode15);
			PrintProc('j', SMScode15);
			goto gedaan;
		}
		if (kode == "16") {		// Arduino wordt gereset
			Serial1.println(SMScode16);
			PrintProc('j', SMScode16);
			goto gedaan;
		}
		if (kode == "17") {		// Netstroom weggevallen
			Serial1.println(SMScode17);
			PrintProc('j', SMScode17);
			goto gedaan;
		}
		if (kode == "18") {		// Netstroom weer terug
			Serial1.println(SMScode18);
			PrintProc('j', SMScode18);
			
		}
		gedaan:
		delay(100);
		Serial1.println((char)26);  // End AT command with a ^Z, ASCII code 26
		delay(100);
		Serial1.println();
		delay(5000);            // give module time to send SMS
			
}

void LeesSMS() {	// kijk of er SMS'jes gestuurd zijn
	String SMSstatus = "";
	String ingelezentekst = "";
	
	if (Serial1.available() > 0) { // er is een bericht binnengekomen
		ingelezentekst = Serial1.readString();
		PrintProc('i',ingelezentekst);
		delay(10);

		if (ingelezentekst.indexOf("Sms aan") >= 0) {		// SMS kode aanzetten
			String message = "SMS wordt AAN gezet";
			SMScode = '1';
			EEPROM.write(0, SMScode);	// en schrijf in EPROM
			sendSMS("01");
			goto klaar;
		}

		if (ingelezentekst.indexOf("Sms uit") >= 0) {		// SMS kode uitzetten
			String message = "SMS wordt UIT gezet";
			SMScode = '0';
			EEPROM.write(0, SMScode);	// en schrijf in EPROM
			sendSMS("02");
			goto klaar;
		}

		if (ingelezentekst.indexOf("Droog") >= 0) {		// Droogtijd sensor zetten
			int n = ingelezentekst.indexOf("Droog");
			String message = ingelezentekst.substring(n + 6, n + 10); // lees de droogtijd in
			int droogtijdsec = message.toInt();	// controle op nummeriek
			if (droogtijdsec == 0) {	// conversie is fout gegaan
				SMScode13 = "Foutieve invoer";
				sendSMS("13");
				goto klaar;
			}
			SchrijfEprom(11, 14, message);
			droogtijd = droogtijdsec * 1000;	// droogtijd aangepast aan miliiseconden
			PrintProc('f', String(droogtijd));
			SMScode13 = "droogtijd is nu " + message + "seconden";
			sendSMS("13");
			goto klaar;
		}

		if (ingelezentekst.indexOf("Vlotter") >= 0) {		// vlotter delay zetten
			int n = ingelezentekst.indexOf("Vlotter");
			String message = ingelezentekst.substring(n + 8, n + 12); // lees de vlottertijd in
			int vlottersec = message.toInt();	// controle op nummeriek
			if (vlottersec == 0) {	// conversie is fout gegaan
				SMScode13 = "Foutieve invoer";
				sendSMS("13");
				goto klaar;
			}
			SchrijfEprom(15, 18, message);
			laagwater_delay = vlottersec * 1000;	// laagwater delay aangepast aan miliiseconden
			PrintProc('c', String(laagwater_delay));
			SMScode13 = "laagwater delay is nu " + message + "seconden";
			sendSMS("13");
			goto klaar;
		}

		if (ingelezentekst.indexOf("T1=") >= 0) {		// telefoonnummer 1 zetten
			int n = ingelezentekst.indexOf("T1=");
			String message = ingelezentekst.substring(n + 3, n + 13); // lees telefoonnummer in
			int telnr = message.toInt();	// controle op nummeriek
			if (telnr == 0) {	// conversie is fout gegaan
				SMScode13 = "Foutieve invoer";
				sendSMS("13");
				goto klaar;
			}
			SchrijfEprom(1, 10, message);
			telefoonnummer = message;
			PrintProc('n', message);
			SMScode13 = "telefoonnummer 1 is nu " + message;
			sendSMS("13");
			goto klaar;
		}

		if (ingelezentekst.indexOf("T2=") >= 0) {		// telefoonnummer 2 zetten
			int n = ingelezentekst.indexOf("T2=");
			String message = ingelezentekst.substring(n + 3, n + 13); // lees telefoonnummer in
			int telnr = message.toInt();	// controle op nummeriek
			if (telnr == 0) {	// conversie is fout gegaan
				SMScode13 = "Foutieve invoer";
				sendSMS("13");
				goto klaar;
			}
			SchrijfEprom(19, 28, message);
			telefoonnummer2 = message;
			PrintProc('o', message);
			SMScode13 = "telefoonnummer 2 is nu " + message;
			sendSMS("13");
			goto klaar;
		}

		if (ingelezentekst.indexOf("T3=") >= 0) {		// telefoonnummer 3 zetten
			int n = ingelezentekst.indexOf("T3=");
			String message = ingelezentekst.substring(n + 3, n + 13); // lees telefoonnummer in
			int telnr = message.toInt();	// controle op nummeriek
			if (telnr == 0) {	// conversie is fout gegaan
				SMScode13 = "Foutieve invoer";
				sendSMS("13");
				goto klaar;
			}
			SchrijfEprom(29, 38, message);
			telefoonnummer3 = message;
			PrintProc('p', message);
			SMScode13 = "telefoonnummer 3 is nu " + message;
			sendSMS("13");
			goto klaar;
		}

		if (ingelezentekst.indexOf("?t") >= 0) {		// telefoonnummer gevraagd
			int tn = 0;
			String message = "Telefoonnummer ";
			// zoek volgorde tel nr
			if (ingelezentekst.indexOf("?t1") >= 0) {
				tn = 1;	//  nummer 1 gevraagd
				message = message + String(tn) + " " + String(telefoonnummer);
			}
			else if (ingelezentekst.indexOf("?t2") >= 0) {
				tn = 2;	//  nummer 2 gevraagd
				message = message + String(tn) + " " + String(telefoonnummer2);
			}
			else if (ingelezentekst.indexOf("?t3") >= 0) {
				tn = 3;	//  nummer 3 gevraagd
				message = message + String(tn) + " " + String(telefoonnummer3);
			}
			else message = "fout in aanvraag telefoonnummer";
			SMScode13 = message;
			sendSMS("13");
			goto klaar;
		}

		if (ingelezentekst.indexOf("Statusp2") >= 0) {		// vraag de status van pomp2 uit
			String StatusP2 = "";
			String SMSKode = "";
			int status = digitalRead(Pomp2);
			if (status == HIGH) {
				StatusP2 = "aan";
				SMSKode = "14";
			}
			else {
				StatusP2 = "uit";
				SMSKode = "15";
			}
			sendSMS(SMSKode);
			goto klaar;
		}

		if (ingelezentekst.indexOf("P2 aan") >= 0) {		// pomp2 aanzetten
			String message = "Pomp2 wordt AAN gezet";
			sendSMS("09");
			PompStatus = 4;				// toestand van de Pompstatus
			goto klaar;
		}

		if (ingelezentekst.indexOf("P2 uit") >= 0) {		// pomp2 uitzetten
			String message = "Pomp2 wordt UIT gezet";
			sendSMS("10");
			PompStatus = 5;				// toestand van de Pompstatus
			goto klaar;
		}
		
		if (ingelezentekst.indexOf("Reset") >= 0) {		// reset de Arduino
			String message = "Arduino wordt gereset";
			sendSMS("16");
			PompStatus = 0;				// toestand van de Pompstatus
			sw_laagwater = false;	// begin met laagwater
			vlotter = false;
			digitalWrite(Pomp2, LOW);		// zet pomp2 uit
			laagwateroud = digitalRead(VlotterLaag);	// lees de beginstand van de vlotter
			goto klaar;
		}
		klaar:	// klaar met het selecteren van de ingelezen tekst
		;
	}


}
