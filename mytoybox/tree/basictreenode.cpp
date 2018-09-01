/*
 * basictreenode.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: onega
 */

#include "basictreenode.h"

basic_tree_node::~basic_tree_node()
{
    delete left;
    left = nullptr;
    delete right;
    right = nullptr;
}
