/*
 * btree.h
 *
 *  Created on: Nov 2, 2009
 *      Author: yogesh
 */

#ifndef BTREE_H_
#define BTREE_H_

#include "tree.h"
#include "str.h"
#include "filereader.h"
#include <queue.h>

#define DEFAULT_BTREE_FILE "default.btree"
#define KEYS_NO 32

class btree: public tree
{
	string fileName;
	filereader *searchFile;
	long rootOffset;

	typedef struct btree_node
	{
		int n; // Number of keys in node
		int key[KEYS_NO]; // Key values
		long child[KEYS_NO + 1]; // File offsets of child nodes

		btree_node()
		{
			n = 0;
			int i;
			for (i = 0; i < KEYS_NO; i++)
			{
				child[i] = INVALID_OFFSET;
				key[i] = INVALID_KEY;
			}
			child[i] = INVALID_OFFSET;
		}
	} btree_node;

	typedef struct print_node
	{
		btree_node btree;
		int level;
		long offset;
	};

	bool insert(int key, long offset, filereader& treeFile,
			long &leftChildOffset, long &rightChildOffset, int &splitKey);

	bool insertInNode(int key, btree_node &node, filereader& treeFile,
			long &leftChildOffset, long &rightChildOffset, int &splitKey);

	void insertAndSplit(int key, btree_node &node, filereader& treeFile,
			long &leftChildOffset, long &rightChildOffset, int &splitKey);

	void insertInNodeWithSpace(int key, btree_node &node, filereader& treeFile,
			long leftChildOffset, long rightChildOffset);

	bool isLeafNode(btree_node &node);

	void addToQueue(long offSet, print_node &pNode,
			std::queue<print_node> &printQueue, filereader &fileReader);

public:
	btree(string &fileName);
	virtual ~btree();
	void insert(int key);
	bool find(int key);
	void print();
};

#endif /* BTREE_H_ */
