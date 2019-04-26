/*
	Andrew Barlow, Addesh Bhagwandeen, Jerry Ju, and Mohammed Moustafa
	Card dealing robot final code
	MTE 100 and GENE 121
	Due: Friday, November 23rd, 2018
*/

void resetPosition()
{
	motor[motorB] = -25;
	while(nMotorEncoder[motorB] > 0)
	{}
	motor[motorB] = 25;
	while(nMotorEncoder[motorB] < 0)
	{}
	motor[motorB] = 0;
	motor[motorC] = -25;
	while(nMotorEncoder[motorC] > 0)
	{}
	motor[motorC] = 0;
}

bool checkForCard()
{
	if(SensorValue[S4] == 0)
	{
		wait10Msec(5);
		if(SensorValue[S4] == 0)
			return true;
	}
	return false;
}

int GameSelect()
{
	displayTextLine(2, "Game Select");
	displayTextLine(4, "left for Standard Deal");
	displayTextLine(6, "right for twenty-one");
	displayTextLine(8, "up for party mode");
	displayTextLine(10, "down to quit");
	wait10Msec(25);

	while(!getButtonPress(buttonAny))
	{}

	if(getButtonPress(buttonLeft))
		return 0;
	else if(getButtonPress(buttonRight))
		return 1;
	else if(getButtonPress(buttonUp))
		return 2;
	else if(getButtonPress(buttonDown))
		return 100;
	return -1;
}

void deckPrompt()
{
	eraseDisplay();
	displayTextLine(2, "Place the deck into the robot!");
	while(checkForCard() == true)
	{}
}

void dealCard()
{
   nMotorEncoder[motorD] = 0;
   motor[motorD] = -100; //pushing card out
   while(nMotorEncoder[motorD] > -210)
   {}

   nMotorEncoder[motorD] = 0;
   motor[motorD] = 100; //pulling second card back in
   while(nMotorEncoder[motorD] < 100)
   {}

   motor[motorD] = 0;
   wait10Msec(20);
}

void rotate(int numPeople, int cardsPerPerson, int* peopleLocation)
{
   resetGyro(S1);
   int direction = 1;  //i.e. if its going CW or CCW

   //for one rotation
   for(int cards = 0; cards < cardsPerPerson; cards++)
   {
      if(direction == 1 && checkForCard() == true) //going clockwise
      {
         for(int person = 0; person < numPeople; person++)
         {
            motor[motorB] = 25;
            while(getGyroDegrees(S1) < peopleLocation[person])
            {}
            motor[motorB] = 0;

            dealCard();
         }
      }
      else if(direction == -1 && checkForCard() == true) //for going CCW
      {
         for(int tempNumPeople = (numPeople - 1); tempNumPeople >= 0; tempNumPeople--)
         {
            motor[motorB] = -25;
            while(getGyroDegrees(S1) > peopleLocation[tempNumPeople])
            {}
            motor[motorB] = 0;

            dealCard();
         }
      }
      else if(checkForCard() == false)
      {
      	deckPrompt();
      }
      direction *= -1; //switch direction from CCW to CW
   }
}


void sort(int * locations, int length)
{
  int index = 0, count = 0, temp = 0;

  for(int step = 0; step < length; step++)
	{
		if(locations[step] == 0)
			locations[step] = 1000;
	}
  for (index = 1; index < length; index++)
  {
    count = index;
    while (count > 0 && locations[count - 1] > locations[count])
      {
      	temp = locations[count];
        locations[count] = locations[count - 1];
        locations[count - 1] = temp;
        count--;
      }
  }
}

void getPlayerLocations(int * locations, int & numPlayers)
{
	numPlayers = 0;
	nMotorEncoder[motorC] = 0;
	int rotationDegrees = 0, direction = 1;

	displayString(2, "Hit the left button when the");
	displayString(4, "pointer is pointing at you.");
	displayString(6, "When all of the players are");
	displayString(8, "done, hit the right button.");

	while (SensorValue[S2] == 0)
	{
		if(direction == 1)
		{
			motor[motorC] = 10 * direction;

			wait1Msec(50);
			while(nMotorEncoder[motorC] < 180 && SensorValue[S2] == 0)
			{
				if (SensorValue[S3] == 0)
				{}

				else
				{
					motor[motorC] = 0;

					while(SensorValue[S3] == 1)
					{}
					rotationDegrees = nMotorEncoder[motorC] %360;
					locations[numPlayers] = rotationDegrees;
					numPlayers++;
					wait1Msec(400);
				}

				motor[motorC] = 10;
			}
		}
    else if(direction == -1)
    {
    	motor[motorC] = 10 * direction;

      wait1Msec(50);
      while(nMotorEncoder[motorC] > 0 && SensorValue[S2] == 0)
      {
      	if (SensorValue[S3] == 0)
        {}
      	else
        {
        	motor[motorC] = 0;
          while(SensorValue[S3] == 1)
          {}
          rotationDegrees = nMotorEncoder[motorC]%360;
          locations[numPlayers] = rotationDegrees;
          numPlayers++;
          wait1Msec(400);
      	}
      	motor[motorC] = -10;
      }
     }

     direction *= -1;
  }

  sort(locations, 10);
	motor[motorC] = 0;
}

void standard(int * locations, int numPlayers)
{
	eraseDisplay();
	displayTextLine(2, "Standard Deal");
	wait10Msec(100);
	rotate(numPlayers,  ceil(52.0 / numPlayers), locations);
}

void twentyOne(int * locations, int numPlayers)
{
	int direction = 1, offset = 0;
	eraseDisplay();
	displayTextLine(2, "Twenty-one");
	displayTextLine(4, "Press up to play another round");
	displayTextLine(6, "Press down to quit");

	wait10Msec(100);
	rotate(numPlayers, 2, locations);

	motor[motorB] = 0;

	while(!getButtonPress(buttonAny) && checkForCard() == true)
	{
	  for(int person = 0; person < numPlayers; person++)
	  {
	  	while(SensorValue(S2) == 0 && SensorValue(S3) == 0  && !getButtonPress(buttonAny))
	  	{}
	  	if(SensorValue(S2) == 1 && checkForCard() == true)
	  	{
	  		dealCard();
			}
			while(SensorValue(S3) == 0 && checkForCard() == true && !getButtonPress(buttonAny))
			{}
			if(SensorValue(S3) == 1 && checkForCard() == true)
			{
				if(direction == 1)
	  		{
	  			offset++;
	  			motor[motorB] = 25 * direction;
		  		while(getGyroDegrees(S1) < locations[person] + (offset * 3) && !getButtonPress(buttonAny))
			  	{}
			  	motor[motorB] = 0;
				}
				else if(direction == -1)
		  	{
		  		offset++;
		  		motor[motorB] = 25 * direction;
			  	while(getGyroDegrees(S1) >= locations[person] + (offset * 3) && !getButtonPress(buttonAny))
				  {}
				  motor[motorB] = 0;
				}
			}
		}
	direction *= -1;
	}
	if(getButtonPress(buttonUp))
		twentyOne(locations, numPlayers);
	else if(getButtonPress(buttonDown))
		resetPosition();
}

void partyMode()
{
	while(checkForCard() == true)
	{
		motor[motorB] = 30;
		while(getGyroDegrees(S1) < 100)
			dealCard();
		motor[motorB] = -30;
		while(getGyroDegrees(S1) > 40)
			dealCard();
	}
}

int playGame()
{
	displayTextLine(2, "Press up to play!");
	displayTextLine(4, "Press down to quit!");

	while(getButtonPress(buttonAny) != 1)
	{}
	if(getButtonPress(buttonUp))
		return 1;
	else if(getButtonPress(buttonDown))
		return 0;
	eraseDisplay();

	return -1;
}

task main()
{
	SensorType[S4] = sensorEV3_Color;
	wait10Msec(5);
	SensorMode[S4] = modeEV3Color_Ambient;
	wait10Msec(5);
	SensorType[S1] = sensorEV3_Gyro;
	wait10Msec(5);
	SensorMode[S1] = modeEV3Gyro_RateAndAngle;
	wait10Msec(5);
	SensorType[S3] = sensorEV3_Touch;
	SensorType[S2] = sensorEV3_Touch;

	playGame();
	while(playGame() == 1)
	{
		int choice = 0, numPlayers = 0;
		int locations[10] = {0,0,0,0,0,0,0,0,0,0}; //fix how to read array size & null cells
		choice = GameSelect();
		getPlayerLocations(locations, numPlayers);

		if(checkForCard() == false)
		{
			eraseDisplay();
			deckPrompt();
		}

		if(choice == 0)
			standard(locations, numPlayers);

		else if(choice == 1)
			twentyOne(locations, numPlayers);

		else if(choice == 2)
			partyMode();

		eraseDisplay();
		resetPosition();
		playGame();
	}
}

//fin.
