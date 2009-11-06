/*
 * Treee.h
 *
 *  Created on: Nov 2, 2009
 *      Author: yogesh
 */

#ifndef TREE_H_
#define TREE_H_

#define INVALID_OFFSET -1
#define INVALID_KEY -1

class tree
{
public:
	virtual void insert(int key)=0;
	virtual bool find(int key) = 0;
	virtual void print() = 0;
	virtual ~tree() {};
};

#endif /* TREEE_H_ */
