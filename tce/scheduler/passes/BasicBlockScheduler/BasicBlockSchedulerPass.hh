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
 * @file BasicBlockSchedulerPass.hh
 *
 * Declaration of BasicBlockSchedulerPass class.
 * 
 * Basic block scheduler.
 *
 * @author Pekka J��skel�inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASIC_BLOCK_SCHEDULER_PASS_HH
#define TTA_BASIC_BLOCK_SCHEDULER_PASS_HH

#include <map>
#include <vector>
#include <set>
#include <list>

#include "StartableSchedulerModule.hh"

#include "BasicBlockScheduler.hh"

class BypasserModule;
class DSFillerModule;
/**
 * Implementation of a simple basic block scheduling algorithm.
 */
class BasicBlockSchedulerPass : public StartableSchedulerModule {
public:
    BasicBlockSchedulerPass();
    virtual ~BasicBlockSchedulerPass();

    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }

    void registerHelperModule(HelperSchedulerModule& module) 
        throw (IllegalRegistration);

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;
    
    BypasserModule* bypasserModule_;
    DSFillerModule* fillerModule_;
};

SCHEDULER_PASS(BasicBlockSchedulerPass)

#endif
