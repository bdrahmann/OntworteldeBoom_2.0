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
	

}	// einde LeesWaterSensor

void LeesSpanningssensor() {	//de routine om de spanningssensor te lezen
	emon1.calcVI(20, 2000);         // Calculate all. No.of half wavelengths (crossings), time-out
	float Vrms = (emon1.Vrms - 3);
	if (Vrms < 0) { Vrms = 0.0; }
	netspanning = long(Vrms);
	PrintProc('q', String(netspanning));
	if (netspanning <5)		// er is geen netspanning
		if (netspanning_weg) {}	// dan niks doen, hij was al weg
		else		// spanning was nog niet weg
		{
			netspanning_weg = true;
			StuurBericht("17"); // stuur SMS
		}
	else
	{
		if (!netspanning_weg) {}	// doe niks, netspanning was er al
		else      // spanning was weg en is nu terug
		{
			netspanning_weg = false;
			StuurBericht("18"); // stuur SMS
		}
	}
	if (netspanning_weg) PrintProc('r', "1");
	else PrintProc('r', "0");
	
	
}
