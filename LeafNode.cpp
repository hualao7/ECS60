#include <iostream>
#include <climits>
#include "LeafNode.h"
#include "InternalNode.h"
#include "QueueAr.h"

using namespace std;


LeafNode::LeafNode(int LSize, InternalNode *p,
  BTreeNode *left, BTreeNode *right) : BTreeNode(LSize, p, left, right)
{
  values = new int[LSize];
}  // LeafNode()

void LeafNode::addToLeft(int value, int last)
{
  leftSibling->insert(values[0]);

  for(int i = 0; i < count - 1; i++)
    values[i] = values[i + 1];

  values[count - 1] = last;
  if(parent)
    parent->resetMinimum(this);
} // LeafNode::ToLeft()

void LeafNode::addToRight(int value, int last)
{
  rightSibling->insert(last);

  if(value == values[0] && parent)
    parent->resetMinimum(this);
} // LeafNode::addToRight()

void LeafNode::addToThis(int value)
{
  int i;

  for(i = count - 1; i >= 0 && values[i] > value; i--)
      values[i + 1] = values[i];

  values[i + 1] = value;
  count++;

  if(value == values[0] && parent)
    parent->resetMinimum(this);
} // LeafNode::addToThis()


void LeafNode::addValue(int value, int &last)
{
  int i;

  if(value > values[count - 1])
    last = value;
  else
  {
    last = values[count - 1];

    for(i = count - 2; i >= 0 && values[i] > value; i--)
      values[i + 1] = values[i];
    // i may end up at -1
    values[i + 1] = value;
  }
} // LeafNode:: addValue()


int LeafNode::getMaximum()const
{
  if(count > 0)  // should always be the case
    return values[count - 1];
  else
    return INT_MAX;
} // getMaximum()


int LeafNode::getMinimum()const
{
  if(count > 0)  // should always be the case
    return values[0];
  else
    return 0;

} // LeafNode::getMinimum()


LeafNode* LeafNode::insert(int value)
{
  int last;

  if(count < leafSize)
  {
    addToThis(value);
    return NULL;
  } // if room for value

  addValue(value, last);

  if(leftSibling && leftSibling->getCount() < leafSize)
  {
    addToLeft(value, last);
    return NULL;
  }
  else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < leafSize)
    {
      addToRight(value, last);
      return NULL;
    }
    else // both siblings full or non-existent
      return split(value, last);
}  // LeafNode::insert()

void LeafNode::print(Queue <BTreeNode*> &queue)
{
  cout << "Leaf: ";
  for (int i = 0; i < count; i++)
    cout << values[i] << ' ';
  cout << endl;
} // LeafNode::print()


LeafNode* LeafNode::remove(int value)
{   // To be written by student

  // if no underflow will happen after deletion  直接删除
  if (count > (leafSize + 1)/2 ){
    deleteThis(value);
  	return NULL;
  }

  deleteValue(value);

  // underflow, borrow from left sib if left sib has extra elements 删掉可以往左边借
  if(leftSibling && leftSibling->getCount() > ((leafSize + 1)/ 2)){
  	borrowFromLeft(value);
  	return NULL;
  }

  // underflow, has leftSib, but underflow
  if(leftSibling){
  	mergeWithLeft(value);
  	return NULL;
  }

  // underflow, no leftSib, borrow from right sib if rightSib has extra elements
  if(rightSibling && rightSibling->getCount() > ((leafSize + 1) / 2)){
  borrowFromRight(value);
  return NULL;
  }

  // underflow, no leftSib, rightSib is underflow
  if(rightSibling){
  mergeWithRight(value);
  return NULL;
  }

  // no siblings
  return NULL;  // filler for stub
}  // LeafNode::remove()


void LeafNode::deleteThis(int value){
	if (value == values[0] && parent)
		parent->resetMinimum(this);//换key

	for (int i = 0; i < count - 1; i ++){
		if(values[i] == value)
			values[i] = values[i + 1];
	}
	count --;
}

void LeafNode::deleteValue(int value){//为什么又删一遍？
	for (int i = 0; i < count - 1; i ++){
		if(values[i] == value)
			values[i] = values[i + 1];
	}
	count --;
}
//左右富余情况下 插到少的位置去 换key
void LeafNode::borrowFromLeft(int value){
	int leftMax = leftSibling->getMaximum();
	leftSibling->remove(leftMax);
	insert(leftMax);
	count ++;
	parent->resetMinimum(this);
}

void LeafNode::borrowFromRight(int value){
	int rightMin = rightSibling->getMinimum();
	rightSibling->remove(rightMin);
	insert(rightMin);
	count ++;
}
//左右都不富裕 把爸爸拉下来
void LeafNode::mergeWithLeft(int value){
	for (int i = 0; i < count - 1; i ++){
		leftSibling->insert(values[i]);
	}

	for (int i = 0; i < count - 1; i ++){
		remove(values[i]);
	}

	if(rightSibling){
	leftSibling->setRightSibling(getRightSibling());
	rightSibling->setLeftSibling(getLeftSibling());
	}
}

void LeafNode::mergeWithRight(int value){
	for (int i = 0; i < count - 1; i ++){
		rightSibling->insert(values[i]);
	}

	for (int i = 0; i < count - 1; i ++){
		remove(values[i]);
	}

		// reset rightSibling's parent's minimum
// 	rightSibling.parent->resetMinimum(rightSibling);

	if(leftSibling){
	leftSibling->setRightSibling(getRightSibling());
	rightSibling->setLeftSibling(getLeftSibling());
	}


}


LeafNode* LeafNode::split(int value, int last)
{
  LeafNode *ptr = new LeafNode(leafSize, parent, this, rightSibling);


  if(rightSibling)
    rightSibling->setLeftSibling(ptr);

  rightSibling = ptr;

  for(int i = (leafSize + 1) / 2; i < leafSize; i++)
    ptr->values[ptr->count++] = values[i];

  ptr->values[ptr->count++] = last;
  count = (leafSize + 1) / 2;

  if(value == values[0] && parent)
    parent->resetMinimum(this);
  return ptr;
} // LeafNode::split()

