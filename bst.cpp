/*
 * bst.cpp
 *
 *  Created on: Nov 2, 2009
 *      Author: yogesh
 */

#include <queue>
#include "bst.h"
#include "filereader.h"
#include "str.h"

tree* getTree(int argc, char *argv[])
{
	string fileName;
	if (argc > 1)
		fileName = argv[1];
	else
		fileName = DEFAULT_BST_FILE;

	tree *treeInstance = new bst(fileName);

	return treeInstance;
}

bst::bst(string &file) :
	fileName(file)
{
	filereader fileReader;
	fileReader.open(fileName, 'w');
	fileReader.close();
}

bst::~bst()
{
}

void bst::insert(int key)
{
	filereader fileReader;
	fileReader.open(fileName, 'x');
	bst_node parentNode;
	long parentOffset = findParent(key, &parentNode);
	if (parentOffset != INVALID_OFFSET)
	{
		bst_node newNode(key);
		fileReader.seek(0, END);
		long childOffset = fileReader.offset();
		fileReader.write_raw((char *) &newNode, sizeof(bst_node));

		if (key < parentNode.key)
		{
			parentNode.l = childOffset;
		}
		else if (key > parentNode.key)
		{
			parentNode.r = childOffset;
		}

		fileReader.seek(parentOffset, BEGIN);
		fileReader.write_raw((char *) &parentNode, sizeof(bst_node));
	}
	else
	{
		bst_node newNode(key);
		fileReader.write_raw((char *) &newNode, sizeof(bst_node));
	}
	fileReader.close();
}

bool bst::find(int key)
{
	filereader fileReader;
	fileReader.open(fileName, 'r');
	bst_node node;
	while (!fileReader.eof())
	{
		unsigned int read = fileReader.read_raw((char *) &node,
				sizeof(bst_node));
		if (read < sizeof(bst_node))
			break;

		if (key == node.key)
		{
			return true;
		}
		if (key < node.key)
		{
			if (node.l != INVALID_OFFSET)
				fileReader.seek(node.l, BEGIN);
			else
				break;
		}
		else if (key > node.key)
		{
			if (node.r != INVALID_OFFSET)
				fileReader.seek(node.r, BEGIN);
			else
				break;
		}
	}


	fileReader.close();
	return false;
}

long bst::findParent(int key, bst_node *parent)
{
	filereader fileReader;
	fileReader.open(fileName, 'r');
	bool found = false;
	bst_node node;
	while (!fileReader.eof() && !found)
	{
		unsigned int read = fileReader.read_raw((char *) &node,
				sizeof(bst_node));
		if (read < sizeof(bst_node))
			break;

		if (key < node.key)
		{
			if (node.l != INVALID_OFFSET)
			{
				fileReader.seek(node.l, BEGIN);
			}
			else
			{
				found = true;
			}
		}
		else if (key > node.key)
		{
			if (node.r != INVALID_OFFSET)
			{
				fileReader.seek(node.r, BEGIN);
			}
			else
			{
				found = true;
			}
		}
	}

	if (found)
	{
		*parent = node;
		fileReader.close();
		return (fileReader.offset() - sizeof(bst_node));
	}
	else
	{
		fileReader.close();
		return INVALID_OFFSET;
	}
}

void bst::print()
{
	filereader fileReader;
	fileReader.open(fileName, 'r');

	print_node pNode;
	pNode.level = 1;
	pNode.offset = 0;
	// read root node
	unsigned int read = fileReader.read_raw((char *) &(pNode.bst),
			sizeof(bst_node));
	if (read < sizeof(bst_node))
		return;

	std::queue<print_node> printQueue;
	printQueue.push(pNode);
	int curLevel = 0;

	while (printQueue.size() > 0)
	{
		pNode = printQueue.front();
		printQueue.pop();

		if (pNode.level != curLevel)
		{
			curLevel = pNode.level;
			printf("\n%d: ", curLevel);
		}

		printf("%d/%ld ", pNode.bst.key, pNode.offset);

		addToQueue(pNode.bst.l, pNode, printQueue, fileReader);
		addToQueue(pNode.bst.r, pNode, printQueue, fileReader);
	}

	fileReader.close();
}

void bst::addToQueue(long offSet, print_node &pNode,
		std::queue<print_node> &printQueue, filereader &fileReader)
{
	if (offSet != INVALID_OFFSET)
	{
		print_node cNode;
		cNode.level = pNode.level + 1;
		cNode.offset = offSet;

		fileReader.seek(offSet , BEGIN);
		unsigned int read = fileReader.read_raw((char *) &(cNode.bst), sizeof(bst_node));
		if (read == sizeof(bst_node))
		{
			printQueue.push(cNode);
		}
	}
}
