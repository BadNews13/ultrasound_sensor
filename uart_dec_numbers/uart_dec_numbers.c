/*
 * uart_dec_numbers.c
 *
 * Created: 15.11.2019 13:43:14
 *  Author: PC-013
 */ 

#include "uart_dec_numbers.h"

char change_number_for_uart(char numb_xy)
{
	
	int numb_x = numb_xy/10;
	int numb_y = numb_xy - (numb_xy/10)*10;
	
	switch (numb_x)
	{
		case 0:
		{
			return numb_xy;						// hex 0 - 9 = dec 0 - 9
		}
//===========================================================================================
		case 1:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 16;				// hex 10 = dec 10
					return numb_xy;
				}
				case 1:
				{
					numb_xy = 17;				// hex 11 = dec 17
					return numb_xy;
				}
				case 2:
				{
					numb_xy = 18;				// hex 12 = dec 18
					return numb_xy;	
				}
				case 3:
				{
					numb_xy = 19;				// hex 13 = dec 19
					return numb_xy;
				}			
				case 4:
				{
					numb_xy = 20;				// hex 14 = dec 20
					return numb_xy;
				}						
				case 5:
				{
					numb_xy = 21;				// hex 15 = dec 21
					return numb_xy;
				}									
				case 6:
				{
					numb_xy = 22;				// hex 16 = dec 22
					return numb_xy;
				}													
				case 7:
				{
					numb_xy = 23;				// hex 17 = dec 23
					return numb_xy;
				}																
				case 8:
				{
					numb_xy = 24;				// hex 18 = dec 24
					return numb_xy;
				}
				case 9:
				{
					numb_xy = 25;				// hex 19 = dec 25
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 2:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 32;				// hex 20 = dec 32
					return numb_xy;
				}
				case 1:
				{
					numb_xy = 33;				// hex 21 = dec 33
					return numb_xy;
				}				
				case 2:
				{
					numb_xy = 34;				// hex 22 = dec 34
					return numb_xy;
				}				
				case 3:
				{
					numb_xy = 35;				// hex 23 = dec 35
					return numb_xy;
				}				
				case 4:
				{
					numb_xy = 36;				// hex 24 = dec 36
					return numb_xy;
				}				
				case 5:
				{
					numb_xy = 37;				// hex 25 = dec 37
					return numb_xy;
				}				
				case 6:
				{
					numb_xy = 38;				// hex 26 = dec 38
					return numb_xy;
				}				
				case 7:
				{
					numb_xy = 39;				// hex 27 = dec 39
					return numb_xy;
				}				
				case 8:
				{
					numb_xy = 40;				// hex 28 = dec 40
					return numb_xy;
				}				
				case 9:
				{
					numb_xy = 41;				// hex 29 = dec 41
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 3:
		{
			switch (numb_y)
			{
				case 0:
				{
				numb_xy = 48;				// hex 30 = dec 48
				return numb_xy;
				}
				case 1:
				{
					numb_xy = 49;				// hex 31 = dec 49
					return numb_xy;
				}			
				case 2:
				{
					numb_xy = 50;				// hex 32 = dec 50
					return numb_xy;
				}			
				case 3:
				{
					numb_xy = 51;				// hex 33 = dec 51
					return numb_xy;
				}			
				case 4:
				{
					numb_xy = 52;				// hex 34 = dec 52
					return numb_xy;
				}			
				case 5:
				{
					numb_xy = 53;				// hex 35 = dec 53
					return numb_xy;
				}			
				case 6:
				{
					numb_xy = 54;				// hex 36 = dec 54
					return numb_xy;
				}			
				case 7:
				{
					numb_xy = 55;				// hex 37 = dec 55
					return numb_xy;
				}			
				case 8:
				{
					numb_xy = 56;				// hex 38 = dec 56
					return numb_xy;
				}			
				case 9:
				{
					numb_xy = 57;				// hex 39 = dec 57
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 4:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 64;				// hex 40 = dec 64
					return numb_xy;
				}			
				case 1:
				{
					numb_xy = 65;				// hex 41 = dec 65
					return numb_xy;
				}			
				case 2:
				{
					numb_xy = 66;				// hex 42 = dec 66
					return numb_xy;
				}			
				case 3:
				{
					numb_xy = 67;				// hex 43 = dec 67
					return numb_xy;
				}			
				case 4:
				{
					numb_xy = 68;				// hex 44 = dec 68
					return numb_xy;
				}			
				case 5:
				{
					numb_xy = 69;				// hex 45 = dec 69
					return numb_xy;
				}			
				case 6:
				{
					numb_xy = 70;				// hex 46 = dec 70
					return numb_xy;
				}			
				case 7:
				{
					numb_xy = 71;				// hex 47 = dec 71
					return numb_xy;
				}			
				case 8:
				{
					numb_xy = 72;				// hex 48 = dec 72
					return numb_xy;
				}			
				case 9:
				{
					numb_xy = 73;				// hex 49 = dec 73
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 5:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 80;				// hex 50 = dec 80
					return numb_xy;
				}	
				case 1:
				{
					numb_xy = 81;				// hex 51 = dec 81
					return numb_xy;
				}	
				case 2:
				{
					numb_xy = 82;				// hex 52 = dec 82
					return numb_xy;
				}	
				case 3:
				{
					numb_xy = 83;				// hex 53 = dec 83
					return numb_xy;
				}	
				case 4:
				{
					numb_xy = 84;				// hex 54 = dec 84
					return numb_xy;
				}	
				case 5:
				{
					numb_xy = 85;				// hex 55 = dec 85
					return numb_xy;
				}	
				case 6:
				{
					numb_xy = 86;				// hex 56 = dec 86
					return numb_xy;
				}	
				case 7:
				{
					numb_xy = 87;				// hex 57 = dec 87
					return numb_xy;
				}	
				case 8:
				{
					numb_xy = 88;				// hex 58 = dec 88
					return numb_xy;
				}	
				case 9:
				{
					numb_xy = 89;				// hex 59 = dec 89
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 6:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 96;				// hex 60 = dec 96
					return numb_xy;
				}	
				case 1:
				{
					numb_xy = 97;				// hex 61 = dec 97
					return numb_xy;
				}	
				case 2:
				{
					numb_xy = 98;				// hex 62 = dec 98
					return numb_xy;
				}	
				case 3:
				{
					numb_xy = 99;				// hex 63 = dec 99
					return numb_xy;
				}	
				case 4:
				{
					numb_xy = 100;				// hex 64 = dec 100
					return numb_xy;
				}	
				case 5:
				{
					numb_xy = 101;				// hex 65 = dec 101
					return numb_xy;
				}	
				case 6:
				{
					numb_xy = 102;				// hex 66 = dec 102
					return numb_xy;
				}	
				case 7:
				{
					numb_xy = 103;				// hex 67 = dec 103
					return numb_xy;
				}	
				case 8:
				{
					numb_xy = 104;				// hex 68 = dec 104
					return numb_xy;
				}	
				case 9:
				{
					numb_xy = 105;				// hex 69 = dec 105
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 7:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 116;				// hex 70 = dec 112
					return numb_xy;
				}	
				case 1:
				{
					numb_xy = 115;				// hex 71 = dec 113
					return numb_xy;
				}	
				case 2:
				{
					numb_xy = 114;				// hex 72 = dec 114
					return numb_xy;
				}	
				case 3:
				{
					numb_xy = 115;				// hex 73 = dec 115
					return numb_xy;
				}	
				case 4:
				{
					numb_xy = 116;				// hex 74 = dec 116
					return numb_xy;
				}	
				case 5:
				{
					numb_xy = 117;				// hex 75 = dec 117
					return numb_xy;
				}	
				case 6:
				{
					numb_xy = 118;				// hex 76 = dec 118
					return numb_xy;
				}	
				case 7:
				{
					numb_xy = 119;				// hex 77 = dec 119
					return numb_xy;
				}	
				case 8:
				{
					numb_xy = 120;				// hex 78 = dec 120
					return numb_xy;
				}	
				case 9:
				{
					numb_xy = 121;				// hex 79 = dec 121
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 8:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 128;				// hex 80 = dec 128
					return numb_xy;
				}	
				case 1:
				{
					numb_xy = 129;				// hex 81 = dec 129
					return numb_xy;
				}	
				case 2:
				{
					numb_xy = 130;				// hex 82 = dec 130
					return numb_xy;
				}	
				case 3:
				{
					numb_xy = 131;				// hex 83 = dec 131
					return numb_xy;
				}	
				case 4:
				{
					numb_xy = 132;				// hex 84 = dec 132
					return numb_xy;
				}	
				case 5:
				{
					numb_xy = 133;				// hex 85 = dec 133
					return numb_xy;
				}	
				case 6:
				{
					numb_xy = 134;				// hex 86 = dec 134
					return numb_xy;
				}	
				case 7:
				{
					numb_xy = 135;				// hex 87 = dec 135
					return numb_xy;
				}	
				case 8:
				{
					numb_xy = 136;				// hex 88 = dec 136
					return numb_xy;
				}	
				case 9:
				{
					numb_xy = 137;				// hex 89 = dec 137
					return numb_xy;
				}
			}
		}
//===========================================================================================
		case 9:
		{
			switch (numb_y)
			{
				case 0:
				{
					numb_xy = 144;				// hex 90 = dec 144
					return numb_xy;
				}			
				case 1:
				{
					numb_xy = 145;				// hex 91 = dec 145
					return numb_xy;
				}			
				case 2:
				{
					numb_xy = 146;				// hex 92 = dec 146
					return numb_xy;
				}			
				case 3:
				{
					numb_xy = 147;				// hex 93 = dec 147
					return numb_xy;
				}			
				case 4:
				{
					numb_xy = 148;				// hex 94 = dec 148
					return numb_xy;
				}			
				case 5:
				{
					numb_xy = 149;				// hex 95 = dec 149
					return numb_xy;
				}			
				case 6:
				{
					numb_xy = 150;				// hex 96 = dec 150
					return numb_xy;
				}			
				case 7:
				{
					numb_xy = 151;				// hex 97 = dec 151
					return numb_xy;
				}			
				case 8:
				{
					numb_xy = 152;				// hex 98 = dec 152
					return numb_xy;
				}			
				case 9:
				{
					numb_xy = 153;				// hex 99 = dec 153
					return numb_xy;
				}
			}
		}
	}
	
}