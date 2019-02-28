/*	Alleen pomp2 wordt aangestuurd.
*** Versie 2.0
**
** Pomp 2 wordt aangestuurd en gemonitord.
**
** Pomp wordt aangezet door Arduino uitgang Hoog te maken. Het relais valt dan af, de pomp gaat aan.
** Het relais heeft een wisselschakelaar. De pomp moet aan de juiste schakelstand aangesloten worden.
** Dat is: Relais uit: verbinding aan.
** 
**
** versie 2.0
**
** 20180513
*/

void RegelPomp2() {
	switch (PompStatus) {
	case 0: // init
		PompStatus = 1;
		lopende_droogtijd = 0;
		break;
	case 1:	// beginstand
		digitalWrite(Pomp2, LOW);	// pomp2 uit
		PompStatus = 2;
		break;
	case 2:	// normale situatie met pomp2 uit
		if (Droog) {	// als de watersensor droog aangeeft
			PompStatus = 3;
			droogtijdLL = millis();	// start de droogtijdmeting
		}
		break;
	case 3:	// controleer droogtijd
		if (!Droog) {	// als de sensor weer nat is
			droogtijdLL = 0;
			lopende_droogtijd = 0;
			PompStatus = 2;	// ga terug naar status2
		}
		else {	// nog steeds droog
			lopende_droogtijd = millis() - droogtijdLL;
			if (lopende_droogtijd > droogtijd) { 	//droogtijd verlopen?
				droogtijdLL = 0;
				PompStatus = 4;
				StuurBericht("11");
			}
		}
		break;
	case 4:	// set pomp 2 aan
		digitalWrite(Pomp2, HIGH);	// set pomp2 aan
		break;
	case 5:	// pomp2 is met de hand uit gezet
		digitalWrite(Pomp2, LOW);	// set pomp2 uit
		break;
	default: // foutsituatie
		// TODO geef foutboodschap
		break;
	}	// einde switch case
	PrintProc('a', String(digitalRead(Pomp2)));
	PrintProc('d', String(PompStatus));
	PrintProc('e', String(lopende_droogtijd));	// lopende droogtijd
	PrintProc('f', String(droogtijd));		// max droogtijd
	PrintProc('s', String(digitalRead(Pomp1)));
}	// einde RegelPomp2