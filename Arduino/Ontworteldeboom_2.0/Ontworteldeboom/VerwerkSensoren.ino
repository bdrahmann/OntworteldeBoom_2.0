/*	Hier worden de watersensoren uitgelezen
**	Uitgelezen waarde = 0 wil zeggen: drijfnat
**	Uitgelezen waarde hoog = 1 wil zeggen: droog
**
** versie 2.0
**
** 20170306
*/

void LeesWatersensor() {		// de routine om regensensor RG-11 uit te lezen
	if (digitalRead(waterSensor) == HIGH) {	// dan komt er geen water op de sensor
		Droog = true;
		PrintProc('g', "DROOG");
	}
	else		// er staat water op de sensor
	{
		Droog = false;
		PrintProc('g', "NAT");
	}
	

}	// einde LeesRegenSensor








