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
 * @file DataDependenceEdge.hh
 *
 * Declaration of data dependence edge class
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_DEPENDENCE_EDGE_HH
#define TTA_DATA_DEPENDENCE_EDGE_HH

#include <string>

#include "GraphEdge.hh"

class DataDependenceEdge : public GraphEdge {
public:
    enum DependenceType {
        DEP_UNKNOWN = 0,
        DEP_RAW     = 1,
        DEP_WAR     = 2,
        DEP_WAW     = 3};

    enum EdgeReason {
        EDGE_REGISTER,
        EDGE_MEMORY,
        EDGE_FUSTATE,
        EDGE_OPERATION,
        EDGE_RA};

    DataDependenceEdge(
        EdgeReason edgereason, DependenceType deptype,
        bool guard = false, bool certainAlias = false, 
        bool tailPseudo = false, bool headPseudo = false );
    virtual ~DataDependenceEdge() {}

    std::string toString() const;
    DependenceType dependenceType() {
        return dependenceType_;
    }
    EdgeReason edgeReason() {
        return edgeReason_;
    };
    bool guardUse() {
        return guard_;
    }
    bool certainAlias() {
        return certainAlias_;
    }
    bool tailPseudo() {
        return tailPseudo_;
    }
    bool headPseudo() {
        return headPseudo_;
    }

    bool operator ==(const DataDependenceEdge& other) const;
private:
    std::string depTypeSt() const;
    std::string edgeReasonSt() const;
    std::string guardSt() const;
    std::string pseudoSt() const;

    DependenceType dependenceType_;
    EdgeReason edgeReason_;
    bool guard_;
    bool certainAlias_;
    bool tailPseudo_;
    bool headPseudo_;
};

#endif
