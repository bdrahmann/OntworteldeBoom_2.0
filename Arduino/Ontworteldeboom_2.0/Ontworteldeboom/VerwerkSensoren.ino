/*	Hier worden de Raindropsensoren uitgelezen
**	Uitgelezen waarde = 0 wil zeggen: drijfnat
**	Uitgelezen waarde hoog wil zeggen: droog
**
** versie 2.0
**
** 20170306
*/

void LeesRegenSensor() {		// de routine om regensensor RG-11 uit te lezen
	if (digitalRead(RG_sensor) == HIGH) {	// dan komt er geen water op de sensor
		Droog = true;
		Serial.println("RG-11 is: DROOG ");
	}
	else		// er staat water op de sensor
	{
		Droog = false;
		Serial.println("RG-11 is: NAT ");
	}
	

}	// einde LeesRegenSensor






