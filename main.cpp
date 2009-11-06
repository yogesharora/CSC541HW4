/*
 * main.cpp
 *
 *  Created on: Nov 2, 2009
 *      Author: yogesh
 */

#include "tree.h"
#include "filereader.h"
#include "str.h"
#include "btree.h"
#include "bst.h"
#include <ctime>
#include <sys/time.h>

#define COMMAND_SEPARATOR " "

tree* getTree(int argc, char* argv[]);

int main(int argc, char *argv[])
{
	filereader consoleReader;
	consoleReader.open('r');

	tree *treeInstance = getTree(argc, argv);

	float totalTime = 0;
	float finds = 0;
	while (!consoleReader.eof())
	{
		string buff;
		consoleReader.getline(buff, 0);
		string commands[2];
		buff.token(commands, 2, COMMAND_SEPARATOR);
		string command = commands[0];
		string data = commands[1];
		if (command == "add")
		{
			treeInstance->insert(atoi(data));
		}
		else if (command == "find")
		{
			struct timeval beg; // Start time
			struct timeval end; // Finish time
			int key = atoi(data);
			gettimeofday(&beg, NULL);
			bool found = treeInstance->find(key);
			gettimeofday(&end, NULL);
			if(found)
				printf("Record %d exists.\n", key);
			else
				printf("Record %d does not exist.\n", key);

			float timeTaken = ((float) end.tv_sec + (float) (end.tv_usec)
					/ 1000000) - ((float) beg.tv_sec + (float) (beg.tv_usec)
					/ 1000000);

			totalTime += timeTaken;
			finds++;
		}
		else if (command == "print")
		{
			treeInstance->print();
			printf("\n\nSum: %f", totalTime);
			printf("\nAvg: %f", (totalTime / finds));
		}
		else if (command == "end")
		{
			break;
		}
	}
	consoleReader.close();
	delete treeInstance;
}
