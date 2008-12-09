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
 * @file TerminalFUPort.hh
 *
 * Declaration of TerminalFUPort class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_FU_PORT_HH
#define TTA_TERMINAL_FU_PORT_HH

#include "Exception.hh"
#include "Terminal.hh"

namespace TTAMachine {
    class HWOperation;
    class BaseFUPort;
}

namespace TTAProgram {

/**
 * Represents an input or output port of a function unit and (when
 * applicable) the operation code written into it.
 *
 * Notice that, in principle, operation codes can be written into FU output 
 * ports.
 */
class TerminalFUPort : public Terminal {
public:
    TerminalFUPort(const TTAMachine::BaseFUPort& port)
        throw (IllegalParameters);
    TerminalFUPort(TTAMachine::HWOperation& operation, int opIndex)
        throw (IllegalParameters);
    virtual ~TerminalFUPort();

    virtual bool isOpcodeSetting() const 
        throw (WrongSubclass);

    virtual bool isTriggering() const 
        throw (WrongSubclass);

    virtual bool isFUPort() const;
    virtual bool isRA() const;

    virtual const TTAMachine::FunctionUnit& functionUnit() const
        throw (WrongSubclass);
    virtual std::string HWOperationName() const;
    virtual Operation& operation() const 
        throw (WrongSubclass, InvalidData);
    virtual Operation& hintOperation() const
        throw (WrongSubclass, InvalidData);
    virtual int operationIndex() const 
        throw (WrongSubclass, InvalidData);
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
    
    virtual TTAMachine::HWOperation* hwOperation() const;

private:
    // copy constructor used internally by copy();
    TerminalFUPort(const TerminalFUPort& tfup);

    /// Assignment not allowed.
    TerminalFUPort& operator=(const TerminalFUPort&);
    int findNewOperationIndex() const;

    /// Port of the unit.
    const TTAMachine::BaseFUPort& port_;
    /// Operation code transported to the port.
    TTAMachine::HWOperation* operation_;
    /// The OSAL operation.
    Operation* opcode_;
    /// Operation index.
    int opIndex_;
};

}

#endif
