void setup()
{
	pinMode(8, OUTPUT);
	digitalWrite(8,HIGH);
	Serial.begin(115200);
}

void loop()
{
	static char const pins[] = {A0, A1, A2, A3, A4, A5};
	static int lastValues[sizeof(pins)];
	bool highPins[sizeof(pins)];
	for(int i = 0; i < sizeof(pins); i++)
	{
		int curVal = analogRead(pins[i]);
#define THRESHOLD 20
		highPins[i] = curVal >= lastValues[i] && (curVal - lastValues[i] > THRESHOLD);
		lastValues[i] = curVal;
		delay(2);
	}

	char outBuf[2];
	outBuf[1] = '\n';
	for(char i = 0; i < sizeof(pins); i++)
	{
		if(highPins[i])
		{
			outBuf[0] = 0x30 + i;
			Serial.write(outBuf, 2);
		}
	}
}
