/*	2 Pompen worden aangestuurd.
** 
** Pomp 1 wordt alleen gemonitord.
** Pomp 2 wordt aangestuurd en gemonitord.
**
** Pomp wordt aangezet door Arduino uitgang Hoog te maken. Het relais valt dan af, de pomp gaat aan.
** Het relais heeft een wisselschakelaar. De pomp moet aan de juiste schakelstand aangesloten worden.
** Dat is: Relais uit: verbinding aan.
** 
** versie 2.0
**
** 20170306
*/

void DuoPompRegeling() {
	String PS;	// tbv serial print
	switch (PompStatus) {
	case 0: // init
		droogtijd = 0;
		PompStatus = 1;
		break;
	case 1:	// beginstand
		PS = "PompStatus = 1"; Serial.println(PS);
		digitalWrite(Pomp2, LOW);	// pomp2 uit
		PompStatus = 2;
		break;
	case 2:	// lees pomp 1
		PS = "PompStatus = 2"; Serial.println(PS);
		if (Pomp1Droog) PompStatus = 3;
		break;
	case 3:	// set pomp 2 uit
		PS = "PompStatus = 3"; Serial.println(PS);
		digitalWrite(Pomp2, LOW);	// pomp2 uit
		if (sw_laagwater == false) PompStatus = 4;	// doorgaan als er voldoende water in de bak zit.
		if (resetStatus) PompStatus = 1;			// na reset begin opnieuw
		break;
	case 4:	// set pomp 2 aan
		PS = "PompStatus = 4"; Serial.println(PS);
		digitalWrite(Pomp2, HIGH);	// set pomp2 aan
		droogtijdLL = millis();		// start de tijd om nat te worden
		PompStatus = 5;
		if (sw_laagwater == true) {
			PompStatus = 3; // een status terug
			Sendkode30(droogtijd, droogtijd);	// stuur de status 100% om de progressbar uit te zetten
		}
		if (resetStatus) PompStatus = 1;		// na reset begin opnieuw
		break;
	case 5:	// pomp2-aan, tijd om sensor nat te laten worden loopt
		PS = "PompStatus = 5"; Serial.println(PS);
		Sendkode30((millis() - droogtijdLL), droogtijd);	// stuur de status
		if ((millis() - droogtijdLL) > droogtijd) {
			PompStatus = 6;
			looptijdaanuit = millis();	// set aantijd 
		}
		if (sw_laagwater == true) {
			PompStatus = 3; // een status terug
			Sendkode30(droogtijd, droogtijd);	// stuur de status 100% om de progressbar uit te zetten
		}
		if (resetStatus) PompStatus = 1;		// na reset begin opnieuw
		break;
	case 6:	// pomp2-aan tijd loopt
		PS = "PompStatus = 6"; Serial.println(PS);
		if (Droog) {
			PompStatus = 8;	// regensensor geeft droog aan
			break;
		}
		// als de aantijd om is:
		if ((millis() - looptijdaanuit) > AAN1_INTERVAL) {
			PompStatus = 7;
			looptijdaanuit = millis();	// set uittijd 
		}
		// zolang de aantijd nog loopt test LW en reset
		if (sw_laagwater == true) PompStatus = 3; // een status terug
		if (resetStatus) PompStatus = 1;		// na reset begin opnieuw	
		break;
	case 7:	//  pomp2-uit tijd loopt
		PS = "PompStatus = 7"; Serial.println(PS);
		digitalWrite(Pomp2, LOW);	// pomp2 uit
		// als de aantijd om is:
		if ((millis() - looptijdaanuit) > UIT1_INTERVAL) {
			PompStatus = 4;
		}
		// zolang de aantijd nog loopt test LW en reset
		if (sw_laagwater == true) PompStatus = 3; // een status terug
		if (resetStatus) PompStatus = 1;		// na reset begin opnieuw	
		break;
	case 8:	// pomp 2 uit
		PS = "PompStatus = 8"; Serial.println(PS);
		digitalWrite(Pomp2, LOW);	// pomp2 uit
		PompStatus = 9;
		break;
	case 9: // stuur SMS 2
		PS = "PompStatus = 9"; Serial.println(PS);
		// TODO sent SMS 2
		PompStatus = 10;
		break;
	case 10:	// sluit af
		PS = "PompStatus = 10"; Serial.println(PS);
		break;
	default: // foutsituatie
		PS = "PompStatus in foutstatus gekomen!!!"; Serial.println(PS);
			 // TODO geef foutboodschap
		break;
	}	// einde switch case

	



}	// einde Duopompregeling

boolean Pomp1Droog() {	// controleert of pomp 1 geen water geeft
	
	if (!Pomp1Aan) {		// staat Pomp1 uit?
		droogtijdLL = 0;	// reset de droogtijd voor de regensensor
		SWPomp1Aan = false;	// set de switch weer uit
		return false;
	}
	else {							// pomp 1 staat aan
		if (!SWPomp1Aan) {			// eerste keer dat pomp1 aan is
			SWPomp1Aan = true;		// set de switch aan
			droogtijdLL = millis();	// start de droogtijdmeting
		}
		if ((millis() - droogtijdLL) < droogtijd) {		// regensensor heeft tijd nodig
			return false;
		}
		else {				// regensensor heeft de tijd gekregen nat te worden
			if (!Droog) {	// niet droog
				return false;
			}
			else {			// regensensor is droog
				// TODO sent SMS1 pomp1 staat droog
				return true;
			}
		}
	}


}	// einde Pomp1Droog
