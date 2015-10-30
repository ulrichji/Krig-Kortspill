#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

void shuffleDeck(char *deck)
{
	int i;
	for(i=0;i<52;i++)
	{
		int newPos = rand() % 52;
		char temp = deck[i];
		deck[i] = deck[newPos];
		deck[newPos] = temp;
	}
}

int getNumberOfRounds()
{
	char deck[52];
	int i = 0;
	int numberOfRounds = 0;
	
	for(i=0;i<13;i++)
	{
		deck[i*4] = i;
		deck[i*4 + 1] = i;
		deck[i*4 + 2] = i;
		deck[i*4 + 3] = i;
	}
	
	shuffleDeck(deck);
	
	char p1 [52];
	char p2 [52];
	
	int p1Counter = 0;
	int p2Counter = 0;
	
	for(i=0;i<26;i++)
	{
		p1[i] = deck[i];
		p2[i] = deck[i + 26];
	}
	p1Counter = 26;
	p2Counter = 26;
	
	int p1Index = 0;
	int p2Index = 0;
	
	while(p1Index != p1Counter && p2Index != p2Counter)
	{
		numberOfRounds++;
		if(p1[p1Index] > p2[p2Index])
		{
			p1[p1Counter++] = p1[p1Index++];
			if(p1Counter >= 52)
				p1Counter = 0;
			if(p1Index >= 52)
				p1Index = 0;
				
			p1[p1Counter++] = p2[p2Index++];
			if(p1Counter >= 52)
				p1Counter = 0;
			if(p2Index >= 52)
				p2Index = 0;
		}
		else if(p1[p1Index] < p2[p2Index])
		{
			p2[p2Counter++] = p2[p2Index++];
			if(p2Counter >= 52)
				p2Counter = 0;
			if(p2Index >= 52)
				p2Index = 0;
			
			p2[p2Counter++] = p1[p1Index++];
			if(p2Counter >= 52)
				p2Counter = 0;
			if(p1Index >= 52)
				p1Index = 0;
		}
		else
		{
			int p1WarIndex = p1Index;
			int p2WarIndex = p2Index;
			int equalCount = 0;
			
			if(p1Index > p1Counter)
				p1Counter = 52 + p1Counter;
			if(p2Index > p2Counter)
				p2Counter = 52 + p2Counter;	
			do
			{
				p1WarIndex += 4;
				p2WarIndex += 4;
					
				if(p1WarIndex >= p1Counter)
					break;
				if(p2WarIndex >= p2Counter)
					break;
				equalCount++;
			}while(p1[p1WarIndex % 52] == p2[p2WarIndex % 52]);
			
			//The war was ended
			if(p1WarIndex < p1Counter && p2WarIndex < p2Counter)
			{
				p1Counter = p1Counter % 52;
				p2Counter = p2Counter % 52;
				if(p1[p1WarIndex % 52] > p2[p2WarIndex % 52])
				{
					while(p1Index < p1WarIndex)
					{
						p1[p1Counter++] = p1[(p1Index++) % 52];
						if(p1Counter >= 52)
							p1Counter = 0;
							
						p1[p1Counter++] = p2[(p2Index++) % 52];
						if(p1Counter >= 52)
							p1Counter = 0;
					}
				}
				else
				{
					while(p2Index < p2WarIndex)
					{
						p2[p2Counter++] = p2[(p2Index++) % 52];
						if(p2Counter >= 52)
							p2Counter = 0;
							
						p2[p2Counter++] = p1[(p1Index++) % 52];
						if(p2Counter >= 52)
							p2Counter = 0;
					}
				}
				p1Index = p1Index % 52;
				p2Index = p2Index % 52;
			}
			else
				break;
		}
	}
	
	return numberOfRounds;
}

int main(int argc, char *argv[])
{
	int i=0;
	int longestRound = 0;
	int shortestRound = 10000;
	int sum = 0;
	int numberOfRounds = 10000000;
	int count = 0;
	
	srand(time(NULL));
	#pragma omp parallel for
	for(i=0;i<numberOfRounds;i++)
	{
		int roundCount = getNumberOfRounds();
		if(i % (numberOfRounds / 100) == 0)
		{
			printf("Calculated %d%%. Round %d was %d rounds long\n",count,i,roundCount);
			count++;
		}
		
		if(roundCount > longestRound)
			longestRound = roundCount;
		if(roundCount < shortestRound)
			shortestRound = roundCount;
		
		#pragma omp atomic
		sum += roundCount;
	}
	
	double average = (double)sum / (double)numberOfRounds;
	
	printf("The longest round was: %d\nThe shortest round was: %d\n, and the average round was %.2f",longestRound,shortestRound,average);
	
	return 0;
}
