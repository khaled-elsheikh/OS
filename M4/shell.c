int main()
{
	while(1)
	{
		char* input;
		char buffer[13312];
		interrupt(0x21, 0, "xOS>\0", 0, 0);
		interrupt(0x21, 1, input, 0, 0);
		if (input[0] == 'v' && input[1] == 'i' && input[2] == 'e' && input[3] == 'w')
		{
			input += 5;
			interrupt(0x21, 3, input, buffer, 0);
			interrupt(0x21, 0, buffer, 0, 0);
			interrupt(0x21, 5, 0, 0, 0);
		}
		else
		{
			if(input[0] == 'e' && input[1] == 'x' && input[2] == 'e' && input[3] == 'c' && input[4] == 'u' && input[5] == 't' && input[6] == 'e')
			{
				input += 8;
				interrupt(0x21, 4, input, 0x2000, 0);
			}
			else
			{
				if(input[0] == 'd' && input[1] == 'e' && input[2] == 'l' && input[3] == 'e' && input[4] == 't' && input[5] == 'e'){
					input += 7;
					interrupt(0x21, 7, input, 0, 0);

				}else{
					if(input[0] == 'c' && input[1] == 'o' && input[2] == 'p' && input[3] == 'y'){
						input += 5;
					}else{
						interrupt(0x21, 0, "Bad Command\n\0", 0, 0);
					}
				}
			}
		}
	}
}