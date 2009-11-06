/*
 * Bst.h
 *
 *  Created on: Nov 2, 2009
 *      Author: yogesh
 */

#ifndef BST_H_
#define BST_H_

#include "str.h"
#include "filereader.h"
#include "tree.h"
#include <queue>

#define DEFAULT_BST_FILE "default.bst"

class bst : public tree
{
	typedef struct bst_node
	{
		int key; // Key value
		long l; // File offset of left child node
		long r; // File offset of right child node

		bst_node(int keyVal)
		{
			key = keyVal;
			l = INVALID_OFFSET;
			r = INVALID_OFFSET;
		}

		bst_node()
		{
			key = INVALID_OFFSET;
			l = INVALID_OFFSET;
			r = INVALID_OFFSET;
		}
	} bst_node;

	typedef struct print_node
	{
		bst_node bst;
		int level;
		long offset;
	};

	string fileName;

	long findParent(int key, bst_node *parent);
	void addToQueue(long offSet, print_node &pNode,
			std::queue<print_node> &printQueue, filereader &fileReader);
public:
	bst(string &fileName);
	void insert(int key);
	bool find(int key);
	void print();
	virtual ~bst();
};

#endif /* BST_H_ */
