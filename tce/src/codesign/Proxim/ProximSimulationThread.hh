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
 * @file ProximSimulationThread.hh
 *
 * Declaration of ProximSimulationThread class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SIMULATION_THREAD_HH
#define TTA_PROXIM_SIMULATION_THREAD_HH


#include <wx/wx.h>
#include "ProximMainFrame.hh"
#include "ProximLineReader.hh"
#include "Listener.hh"

class SimulatorInterpreter;
class SimulatorInterpreterContext;
class TracedSimulatorFrontend;
class ProximRuntimeErrorHandler;

/**
 * ProximSimulationThread class wraps the simulation backend in a separate
 * worker thread.
 *
 * The worker thread utilizes ProximLinereader to read commands from the user.
 * Simulator events and input requests are passed to the GUI thread as custom
 * wxEvents (see SimulatorEvent class).
 */
class ProximSimulationThread : public wxThread, public Listener {
public:
    ProximSimulationThread();
    virtual ~ProximSimulationThread();
    virtual ExitCode Entry();
    virtual void handleEvent(int event);
    ProximLineReader& lineReader();
    TracedSimulatorFrontend* frontend();
    void initialize(ProximMainFrame* gui);
    SimulatorInterpreter* interpreter();

    void requestStop();
    void killSimulation();
    void finishSimulation();
    bool isBusy();

private:
    /// SimulatorInterpreterContext. ?
    SimulatorInterpreterContext* interpreterContext_;
    /// SimulatorInterpreter where the user input is passed to.
    SimulatorInterpreter* interpreter_;
    /// Simulator backend.
    TracedSimulatorFrontend* simulation_;
    /// Linereader used for reading the user input.
    ProximLineReader* lineReader_;
    /// Proxim main frame where the simulator events are passed to.
    ProximMainFrame* gui_;
    /// Runtime error handler.
    ProximRuntimeErrorHandler* runtimeErrorHandler_;

};
#endif
