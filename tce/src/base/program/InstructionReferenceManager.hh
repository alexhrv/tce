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
 * @file InstructionReferenceManager.hh
 *
 * Declaration of InstructionReferenceManager class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_REFERENCE_MANAGER_HH
#define TTA_INSTRUCTION_REFERENCE_MANAGER_HH

#include <map>
#include "Exception.hh"

namespace TTAProgram {

class Instruction;
class InstructionReference;

/**
 * Helps in keeping instructions referenced in POM up-to-date.
 *
 * Instructions are not referenced directly, but through an
 * InstructionReference instance. The actual Instruction instance
 * referred can be changed as needed.
 */
class InstructionReferenceManager {
public:
    InstructionReferenceManager();
    virtual ~InstructionReferenceManager();

    InstructionReference& createReference(Instruction& ins);
    InstructionReference& replace(Instruction& insA, Instruction& insB)
        throw (InstanceNotFound);
    void clearReferences();
    bool hasReference(Instruction& ins) const;
    InstructionReferenceManager* copy() const;

       
private:
    /// Map for instruction references. faster to search than list.
    typedef std::multimap<Instruction*,InstructionReference*> RefMap;

    /// Instruction references to maintain.
    RefMap references_;
public:

    class Iterator {
    public:
        inline Iterator& operator++(); // ++i
        inline InstructionReference& operator*();
        inline const InstructionReference& operator*() const;
        inline InstructionReference* operator->();
        inline const InstructionReference* operator->() const;
        inline bool operator !=(const Iterator& i) const;
        inline Iterator(RefMap::iterator iter);
        inline Iterator(RefMap::iterator& iter);
    private:
        RefMap::iterator iter_;
    };
        
    inline Iterator begin();
    inline Iterator end();
};

#include "InstructionReferenceManager.icc"

}


#endif
