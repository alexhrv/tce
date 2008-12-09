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
 * @file ProgramDependenceNode.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_PROGRAM_DEPENDENCE_NODE_HH
#define PROGRAM_DEPENDENCE_NODE_HH

#include "MoveNode.hh"
#include "ControlDependenceNode.hh"
/**
*/
class ProgramDependenceNode : public GraphNode {
public:
    ProgramDependenceNode(ControlDependenceNode& cdgNode, int nodeID);
    ProgramDependenceNode(
        MoveNode& mNode, 
        int nodeID, 
        bool predicate = false);
    virtual ~ProgramDependenceNode();
   
    bool isRegionNode() const;
    bool isPredicateMoveNode() const;
    bool isMoveNode() const;
    MoveNode& moveNode();
    const MoveNode& moveNode() const;
    std::string dotString() const;
    std::string toString() const;
    
protected:
    friend class ProgramDependenceGraph;
    void setPredicateMoveNode();
    
private:
    MoveNode* mNode_;
    ControlDependenceNode* cdgNode_;
    bool region_;
    bool predicate_;
};

#endif
