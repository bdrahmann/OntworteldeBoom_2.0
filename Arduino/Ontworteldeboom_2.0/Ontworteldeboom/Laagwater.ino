/*	Hier wordt vastgesteld wat de stand is van de vlotter die moet signaleren of er voldoende water in de bak staat.
**	Als er niet voldoende water in de bak staat, moet de pomp worden uitgeschakeld.
**	digitalRead(Vlotterlaag) == HIGH als de vlotter  te weinig water in de bak meet.
**	sw_laagwater wordt dan gezet.
**	sw_laagwater = true betekent: ja, er staat te weinig water in de bak
**
**	Als de vorige (sw_laagwater) stand wijzigt (digitalRead), moet dmv een timer de "dender" van de meting eruit worden gehaald.
*/

void LaagWater() {
	// Serial.print("digitalRead(VlotterLaag) = ");
	// Serial.println(digitalRead(VlotterLaag));

	if (sw_laagwater == true) {	// er was te weinig water in de bak
		if (digitalRead(VlotterLaag) == HIGH) {	// en er is nog steeds te weinig water
			looptijdLL = 0;	// zet de denderlooptijd weer op 0
			Sendkode29(laagwater_delay, laagwater_delay);	// stuur de status "100" om progressbar uit te zetten
															// en doe verder niks
		}
		else {	// de vlotter geeft voldoende water aan
			if (looptijdLL == 0) {	// als de tijd nog niet loopt, zet de tijd
				looptijdLL = millis();	// zet de tijd	
			}
			else {	// de tijd loopt al
				if ((millis() - looptijdLL) > laagwater_delay) {	// als de tijd verstreken is
					sw_laagwater = false;	// er is definitief vastgesteld dat er voldoende water in de bak zit
				}
				else
				{
					// do nothing
				}
			}
			Sendkode29((millis() - looptijdLL), laagwater_delay);	// stuur de status
		}
	}
	else {	// sw_laagwater = false. Er was dus voldoende water
		if (digitalRead(VlotterLaag) == LOW) {	// en er is nog steeds voldoende water
			looptijdLL = 0;	// zet de denderlooptijd weer op 0
			Sendkode29(laagwater_delay, laagwater_delay);	// stuur de status "100" om progressbar uit te zetten
															// en doe verder niks
		}
		else {	// de vlotter geeft onvoldoende water aan
			if (looptijdLL == 0) {	// als de tijd nog niet loopt, zet de tijd
				looptijdLL = millis();	// zet de tijd
			}
			else {	// de tijd loopt al
				if ((millis() - looptijdLL) > laagwater_delay) {	// als de tijd verstreken is
					sw_laagwater = true;	// er is definitief vastgesteld dat er onvoldoende water in de bak zit
					Sendkode29(laagwater_delay, laagwater_delay);	// stuur de status "100" om progressbar uit te zetten
					StuurSMS();	// zend een SMS 
				}
				else
				{
					// do nothing
				}
			}
			Sendkode29((millis() - looptijdLL), laagwater_delay);	// stuur de status
		}
	}
}

void StuurSMS() { // het versturen van een SMS
	if ((millis() - syncTimeLL) > LOG_LL_INTERVAL) {  // de intervaltijd is verstreken
		syncTimeLL = millis();
		LOG_LL_INTERVAL = 1800000; // set nieuwe interval voor SMS 0,5 uur
		StuurBericht("08");
	}  // einde if millis
	else {
		// do nothing
	}
	/*
	Serial.print("In module LaagWater: ");
	Serial.print("sw_laagwater = ");
	Serial.println(sw_laagwater);
	*/

}


