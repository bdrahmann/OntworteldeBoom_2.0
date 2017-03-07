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
		
		PompStatus = 1;
		break;
	case 1:	// beginstand
		/*digitalWrite(Pig, LOW);	// zet de PompInGebruik uit
		PS = "In DuoPompRegeling, Status 1 is pomp " + String(Pig - 8) + " uitgezet."; Serial.println(PS);
		if (sw_laagwater == false)
			PompStatus = 2;
			*/
		break;
	case 2:	// zet pomp 2 uit
		/*digitalWrite(Pig, HIGH);
		PS = "In DuoPompRegeling, Status 2 is pomp " + String(Pig - 8) + " aangezet."; Serial.println(PS);
		// start de tijd om de boom de tijd te geven nat te worden
		DroogtijdLL = millis();		// start de tijd om nat te worden
		PompStatus = 3;
		if (sw_laagwater == true) {
			PompStatus = 1; // een status terug
		}*/
		break;
	case 3:	// set pomp 2 aan
		
		break;
	case 4:	// tijd om sensor nat te laten worden loopt
		/*if (sw_laagwater == true) PompStatus = 1; // zet de Pig weer uit
		if (Droog == true) PompStatus = 5;		 // zet de volgende pomp aan
		*/
		break;
	case 5:	// pomp2-aan tijd loopt
		/*Sendkode30((millis() - DroogtijdLL), Droogtijd);	// stuur de status
		if ((millis() - DroogtijdLL) > Droogtijd) PompStatus = 4;
		if (sw_laagwater == true) {
			PompStatus = 1; // een status terug
			Sendkode30(Droogtijd, Droogtijd);	// stuur de status 100% om de progressbar uit te zetten
		}
		
		PompStatus = 6;
		*/
		break;
	case 6:	// pomp2-uit tijd loopt
		/*if (Pig == Pomp1) {
			Pig = Pomp2;
			PompStatus = 7;
		}
		else {
			StuurBericht("11");	// stuur SMS dat pomp2 uitgezet is.
			PompStatus = 8;
		}*/
		break;
	case 7:	// pomp 2 uit
		/*StuurBericht("10");	// stuur SMS dat pomp1 uitgezet is
		PompStatus = 2;*/
		break;
	case 8:	// stuur SMS 2
			/**/
			digitalWrite(Pomp2, HIGH);	// en zet voor alle zekerheid pomp2 maar aan.
			*/
		break;
	case 9: // sluit af
	default: // foutsituatie
			 // TODO geef foutboodschap
		break;
	}	// einde switch case

	String tydelijk = "Pompstatus = " + String(PompStatus) + " en pomp in gebruik = " + String(Pig - 8);
	Serial.println(tydelijk);



}	// einde Duopompregeling
