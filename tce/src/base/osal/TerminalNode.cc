/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file TerminalNode.cc
 *
 * Implementation of TerminalNode class.
 *
 * @author Mikael Lepist� 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include "TerminalNode.hh"
#include "Conversion.hh"

/**
 * Constructor.
 */
TerminalNode::TerminalNode(int operandIndex) :
    OperationDAGNode(), operandIndex_(operandIndex) {}

/**
 * Copoy constructor.
 */
TerminalNode::TerminalNode(const TerminalNode& other) :
    OperationDAGNode(), operandIndex_(other.operandIndex()) {}

/**
 * Clones node. Needed for dynamic binding, when copying 
 * instances through the base class.
 *
 * @return Dynamically allocated copy of the node. 
 */
GraphNode* 
TerminalNode::clone() const {
    return new TerminalNode(*this);
}

int 
TerminalNode::operandIndex() const {
    return operandIndex_;
}

std::string 
TerminalNode::toString() const {
    return "IO(" + Conversion::toString(operandIndex_) + ")";
}
