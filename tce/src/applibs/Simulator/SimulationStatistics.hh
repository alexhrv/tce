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
 * @file SimulationStatistics.hh
 *
 * Declaration of SimulationStatistics class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_STATISTICS_HH
#define TTA_SIMULATION_STATISTICS_HH

#include <vector>

namespace TTAProgram {
    class Program;
}

class SimulationStatisticsCalculator;
class InstructionMemory;

/**
 * Calculates simulation statistics using user-given calculation classes.
 */
class SimulationStatistics {
public:
    SimulationStatistics(
        const TTAProgram::Program& programData, 
        const InstructionMemory& executionCounts);
    virtual ~SimulationStatistics();

    void addStatistics(SimulationStatisticsCalculator& statisticsType);
    void calculate();

private:
    /// All registered statistics types are stored in this container.
    std::vector<SimulationStatisticsCalculator*> statisticsTypes_;
    /// The program used in calculating the statistics.
    const TTAProgram::Program& program_;
    /// The execution counts of instructions and moves in the program.
    const InstructionMemory& executionCounts_;
};
#endif

