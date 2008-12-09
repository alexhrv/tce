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
 * @file UnboundRegisterFileState.hh
 *
 * Declaration of UnboundRegisterFileState class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNBOUND_REGISTER_FILE_STATE_HH
#define TTA_UNBOUND_REGISTER_FILE_STATE_HH

#include <vector>

#include "RegisterFileState.hh"

class RegisterState;

/**
 * Holds unlimited number of RegisterState objects.
 */
class UnboundRegisterFileState : public RegisterFileState {
public:
    UnboundRegisterFileState();
    virtual ~UnboundRegisterFileState();

    virtual RegisterState& registerState(int index)
        throw (OutOfRange);

    virtual std::size_t registerCount() const;

    bool isRegisterInUse(std::size_t index) const;

private:
    /// Copying not allowed.
    UnboundRegisterFileState(const UnboundRegisterFileState&);
    /// Assignment not allowed.
    UnboundRegisterFileState& operator=(const UnboundRegisterFileState&);

    /// Contains all the registers of the register file.
    std::vector<RegisterState*> registerStates_;
};

#endif
