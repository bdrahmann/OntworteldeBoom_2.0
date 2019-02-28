/* Ontwortelde Boom
**
** versie 2.0
**
** 20180513
**
**  Hier wordt vastgesteld wat de stand is van de vlotter die moet signaleren of er voldoende water in de bak staat.
**	Als er niet voldoende water in de bak staat, moet de pomp worden uitgeschakeld.
**	digitalRead(Vlotterlaag) == HIGH als de vlotter  te weinig water in de bak meet.
**	sw_laagwater wordt dan gezet.
**	sw_laagwater = true betekent: ja, er staat te weinig water in de bak
**
**	Als de vorige (sw_laagwater) stand wijzigt (digitalRead), moet dmv een timer de "dender" van de meting eruit worden gehaald.
*/

void LeesLaagwater() {
	//TODO printopdrachten sturen
	vlotter = digitalRead(VlotterLaag);
	if (sw_laagwater == true) {	// er was te weinig water in de bak
		if (vlotter == HIGH) {	// en er is nog steeds te weinig water
			looptijdLL = 0;	// zet de denderlooptijd weer op 0
			//vlotter = true;
		}
		else {	// de vlotter geeft voldoende water aan
			if (looptijdLL == 0) {	// als de tijd nog niet loopt, zet de tijd
				looptijdLL = millis();	// zet de tijd	
			}
			else {	// de tijd loopt al
				lopende_vlottertijd = millis() - looptijdLL;
				if (lopende_vlottertijd > laagwater_delay) {	// als de tijd verstreken is
					sw_laagwater = false;	// er is definitief vastgesteld dat er voldoende water in de bak zit
				}
				else
				{
					// do nothing
				}
			}
		}
	}
	else {	// sw_laagwater = false. Er was dus voldoende water
		if (vlotter == LOW) {	// en er is nog steeds voldoende water
			looptijdLL = 0;	// zet de denderlooptijd weer op 0
			//vlotter = false;
		}
		else {	// de vlotter geeft onvoldoende water aan
			if (looptijdLL == 0) {	// als de tijd nog niet loopt, zet de tijd
				looptijdLL = millis();	// zet de tijd
			}
			else {	// de tijd loopt al
				lopende_vlottertijd = millis() - looptijdLL;
				if (lopende_vlottertijd > laagwater_delay) {	// als de tijd verstreken is
					sw_laagwater = true;	// er is definitief vastgesteld dat er onvoldoende water in de bak zit
					
				}
				else
				{
					// do nothing
				}
			}
		}
	}
	if (vlotter) PrintProc('b', "LAAG"); else PrintProc('b', "HOOG");
	PrintProc('c', String(laagwater_delay));
	PrintProc('l', String(lopende_vlottertijd));
	if (sw_laagwater) {
		PrintProc('h', "LAAG");
		StuurSMS();	// zend een SMS 
	}
	else PrintProc('h', "HOOG");
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
	

}
