/*
 * btree.cpp
 *
 *  Created on: Nov 2, 2009
 *      Author: yogesh
 */
#include "btree.h"
#include "str.h"

template<class T>
void swap(T &a, T &b);

tree* getTree(int argc, char *argv[])
{
	string fileName;
	if (argc > 1)
		fileName = argv[1];
	else
		fileName = DEFAULT_BTREE_FILE;

	tree *treeInstance = new btree(fileName);
	return treeInstance;
}

btree::btree(string &file) :
	fileName(file), searchFile(NULL), rootOffset(0)
{
	filereader fileReader;
	fileReader.open(fileName, 'w');
	fileReader.close();
}

btree::~btree()
{
	if (searchFile)
	{
		searchFile->close();
		delete searchFile;
	}
}

void btree::insert(int key)
{
	filereader treeFile;
	treeFile.open(fileName, 'x');

	btree_node node;
	unsigned int read = treeFile.read_raw((char *) &node, sizeof(btree_node));

	if (read < sizeof(btree_node))
	{
		// empty index file
		btree_node root;
		root.key[root.n++] = key;
		treeFile.clear();
		treeFile.write_raw((char *) &root, sizeof(btree_node));
	}
	else
	{

		long rightChildOffset = INVALID_OFFSET;
		long leftChildOffset = INVALID_OFFSET;
		int splitKey;

		if (insert(key, rootOffset, treeFile, leftChildOffset,
				rightChildOffset, splitKey))
		{
			// new rootNode
			btree_node newRootNode;
			newRootNode.n = 1;
			newRootNode.key[0] = splitKey;
			newRootNode.child[0] = leftChildOffset;
			newRootNode.child[1] = rightChildOffset;

			treeFile.seek(0, END);
			rootOffset = treeFile.offset();
			treeFile.write_raw((char *) &newRootNode, sizeof(btree_node));

		}
	}
	treeFile.close();
}

bool btree::insert(int key, long offset, filereader& treeFile,
		long &leftChildOffset, long &rightChildOffset, int &splitKey)
{
	treeFile.seek(offset, BEGIN);
	btree_node node;
	treeFile.read_raw((char *) &node, sizeof(btree_node));
	treeFile.seek(treeFile.offset() - sizeof(btree_node), BEGIN);

	if (isLeafNode(node))
	{
		return insertInNode(key, node, treeFile, leftChildOffset,
				rightChildOffset, splitKey);
	}
	else
	{
		// find the appropriate child node
		long childOffset = INVALID_OFFSET;
		if (key < node.key[0])
		{
			childOffset = node.child[0];
		}
		else if (key > node.key[node.n - 1])
		{
			childOffset = node.child[node.n];
		}
		else
		{
			for (int i = 0; i < node.n - 1; i++)
			{
				if (key > node.key[i] && key < node.key[i + 1])
				{
					childOffset = node.child[i + 1];
					break;
				}
			}
		}
		if (childOffset != INVALID_OFFSET)
		{
			if (insert(key, childOffset, treeFile, leftChildOffset,
					rightChildOffset, splitKey))
			{
				treeFile.seek(offset, BEGIN);
				return insertInNode(splitKey, node, treeFile, leftChildOffset,
						rightChildOffset, splitKey);
			}
			else
			{
				treeFile.seek(offset, BEGIN);
				return false;
			}
		}
	}
	return false;
}

bool btree::insertInNode(int key, btree_node &node, filereader& treeFile,
		long &leftChildOffset, long &rightChildOffset, int &splitKey)
{
	if (node.n == KEYS_NO)
	{
		// insert and split
		insertAndSplit(key, node, treeFile, leftChildOffset, rightChildOffset,
				splitKey);
		return true;
	}
	else
	{
		// insert in proper place
		insertInNodeWithSpace(key, node, treeFile, leftChildOffset,
				rightChildOffset);
		return false;
	}
}

void btree::insertAndSplit(int key, btree_node &node, filereader& treeFile,
		long &leftChildOffset, long &rightChildOffset, int &splitKey)
{
	long keys[KEYS_NO + 1];
	long children[KEYS_NO + 2];

	// create a node of size KEYS_NO+1
	int i = 0;
	while (node.key[i] < key && i < node.n)
	{
		keys[i] = node.key[i];
		children[i] = node.child[i];
		i++;
	}
	keys[i] = key;
	children[i] = leftChildOffset;
	children[i + 1] = rightChildOffset;

	while (i < KEYS_NO)
	{
		keys[i + 1] = node.key[i];
		children[i + 2] = node.child[i + 1];
		i++;
	}

	// find middle element that will be the split key
	int middle = (KEYS_NO + 1) / 2;
	splitKey = keys[middle];

	// create right child
	btree_node leftChild;
	int j;
	for (j = 0; j < middle; j++)
	{
		leftChild.key[j] = keys[j];
		leftChild.child[j] = children[j];
		leftChild.n++;
	}
	leftChild.child[j] = children[j];

	// create right child
	btree_node rightChild;
	for (j = middle + 1; j < KEYS_NO + 1; j++)
	{
		rightChild.key[rightChild.n] = keys[j];
		rightChild.child[rightChild.n] = children[j];
		rightChild.n++;
	}
	rightChild.child[rightChild.n] = children[j];

	leftChildOffset = treeFile.offset();

	//write the left child
	treeFile.write_raw((char *) &leftChild, sizeof(btree_node));

	//write the newly created right child
	treeFile.seek(0, END);
	rightChildOffset = treeFile.offset();
	treeFile.write_raw((char *) &rightChild, sizeof(btree_node));

	// restore the position of the file
	treeFile.seek(leftChildOffset, BEGIN);
}

template<class T>
void swap(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}

void btree::insertInNodeWithSpace(int key, btree_node &node,
		filereader& treeFile, long leftChildOffset, long rightChildOffset)
{
	// TODO duplicate key check
	node.key[node.n++] = key;
	node.child[node.n] = rightChildOffset;

	for (int i = node.n - 1; i > 0; i--)
	{
		if (node.key[i] < node.key[i - 1])
		{
			swap(node.key[i], node.key[i - 1]);
			swap(node.child[i + 1], node.child[i]);
		}
	}

	treeFile.seek(treeFile.offset(), BEGIN);
	treeFile.write_raw((char *) &node, sizeof(btree_node));
	// restore the tree file
	treeFile.seek(treeFile.offset() - sizeof(btree_node), BEGIN);
}

bool btree::isLeafNode(btree_node &node)
{
	for (int i = 0; i < KEYS_NO + 1; i++)
	{
		if (node.child[i] != INVALID_OFFSET)
			return false;
	}

	return true;
}

bool btree::find(int key)
{
	if (searchFile == NULL)
	{
		searchFile = new filereader();
		searchFile->open(fileName, 'r');
	}

	btree_node node;
	searchFile->seek(rootOffset, BEGIN);
	while (!searchFile->eof())
	{
		unsigned int read = searchFile->read_raw((char *) &node,
				sizeof(btree_node));

		if (read < sizeof(btree))
			break;

		int childOffset = INVALID_OFFSET;
		if (key < node.key[0])
		{
			childOffset = node.child[0];
		}
		else if (key > node.key[node.n - 1])
		{
			childOffset = node.child[node.n];
		}
		else
		{

			int beg = 0;
			int end = node.n;

			while (beg <= end)
			{
				int mid = (beg + end) / 2;
				if (beg == end && key != node.key[mid])
				{
					break;
				}
				if (key == node.key[mid])
				{
					return true;
				}
				else if (key > node.key[mid])
				{
					beg = mid + 1;
				}
				else if (key < node.key[mid])
				{
					end = mid - 1;
				}
			}

			if (node.key[beg] < key && node.key[beg + 1] > key)
				childOffset = node.child[beg + 1];
			else if (node.key[beg - 1] < key && node.key[beg] > key)
				childOffset = node.child[beg];
		}

		if (childOffset == INVALID_OFFSET)
			break;

		searchFile->seek(childOffset, BEGIN);
	}

	return false;
}

void btree::print()
{
	if (searchFile == NULL)
	{
		searchFile = new filereader();
		searchFile->open(fileName, 'r');
	}

	btree_node node;
	unsigned int read =
			searchFile->read_raw((char *) &node, sizeof(btree_node));

	if (read == sizeof(btree_node))
	{
		print_node pNode;

		searchFile->seek(rootOffset, BEGIN);
		read = searchFile->read_raw((char *) &pNode.btree, sizeof(btree_node));
		if (read < sizeof(btree_node))
			return;

		pNode.level = 1;
		pNode.offset = rootOffset;

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

			for (int i = 0; i < pNode.btree.n; i++)
			{
				printf("%d", pNode.btree.key[i]);
				if (i != pNode.btree.n - 1)
					printf(",");

				addToQueue(pNode.btree.child[i], pNode, printQueue, *searchFile);
			}
			addToQueue(pNode.btree.child[pNode.btree.n], pNode, printQueue,
					*searchFile);
			printf("/%ld ", pNode.offset);
		}
	}
}

void btree::addToQueue(long offSet, print_node &pNode,
		std::queue<print_node> &printQueue, filereader &treeFile)
{
	if (offSet != INVALID_OFFSET)
	{
		print_node cNode;
		cNode.level = pNode.level + 1;
		cNode.offset = offSet;

		treeFile.seek(offSet, BEGIN);
		unsigned int read = treeFile.read_raw((char *) &(cNode.btree),
				sizeof(btree_node));
		if (read == sizeof(btree_node))
		{
			printQueue.push(cNode);
		}
	}
}
