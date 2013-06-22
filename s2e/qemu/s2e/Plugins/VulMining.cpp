/*
 * S2E Selective Symbolic Execution Framework
 *
 * Copyright (c) 2010, Dependable Systems Laboratory, EPFL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Dependable Systems Laboratory, EPFL nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE DEPENDABLE SYSTEMS LABORATORY, EPFL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Currently maintained by:
 *    wzy <wuzhiyong0127@gmail.com>
 *
 * All contributors are listed in S2E-AUTHORS file.
 *
 */

/**
This plugin is used to implement vulnerability mining. It includes three parts:
1) the start point to make input symbolic, which also can be understood as taint source;
2) assert part, put some rules here to finding the bugs of interesting points, like vulnerable functions and loops;
3) termination points where disable forking or terminate;
 */

extern "C" {
#include "config.h"
#include "qemu-common.h"
#include "disas.h"
}


#include <s2e/S2E.h>
#include <s2e/Utils.h>
#include <s2e/ConfigFile.h>
#include <s2e/Plugins/Opcodes.h>
#include "VulMining.h"

#include <s2e/S2EExecutionState.h>
#include <s2e/S2EExecutor.h>
#include <klee/Solver.h>

#include "../../monitor.h"
//#include "../../disas.h"

#include <sstream>
#include <fstream>

using namespace std;

using namespace s2e;
using namespace s2e::plugins;

#define MAX_FUNCNAME 20
#define ADDRESS_SIZE 8

S2E_DEFINE_PLUGIN(VulMining, "Plugin for mining vul" , "VulMining" , "CodeSelector" , "ModuleExecutionDetector");



VulMining::~VulMining()
{

}


void VulMining::initialize()
{

	m_CodeSelector = (CodeSelector*)s2e()->getPlugin("CodeSelector");
	m_executionDetector = (ModuleExecutionDetector*)s2e()->getPlugin("ModuleExecutionDetector");

   	m_onTranslateInstruction = s2e()->getCorePlugin()->onTranslateInstructionStart.connect(
        	sigc::mem_fun(*this, &VulMining::onTranslateInstructionStart));

}
//
void VulMining::onTranslateInstructionStart(ExecutionSignal *signal,
                                                   S2EExecutionState *state,
                                                   TranslationBlock *tb,
                                                   uint64_t pc)
{
	if (!m_CodeSelector->isPidToTrack(state)) {
		return;
	}

    const ModuleDescriptor *currentModule =
    		m_executionDetector->getCurrentDescriptor(state);

    if (!currentModule) {
        // Outside of any module, do not need
        // to instrument tb exits.
        return;
    }

	s2e()->getMessagesStream() << "onTranslateInstructionStart pc: " << hexval(pc) << "\n";

	if (pc == 0x080483bc) {
		s2e()->getMessagesStream() << "hit memcpy! " << hexval(pc) << "\n";
		disasPC(pc);
		signal->connect(sigc::mem_fun(*this, &VulMining::assertMemcpy));
	}

/*
	if ((currentModule->Name == "libc-2.11.1.so") && (pc == (currentModule->LoadBase + 0x00073881) || pc == (currentModule->LoadBase + 0x00073887))) {
		s2e()->getMessagesStream() << "hit pc! " << hexval(pc) << "\n";
		disasPC(pc);
	}
*/

}

void VulMining::assertMemcpy(S2EExecutionState *state, uint64_t pc) {

	uint64_t value = 0, address = 0, sp = 0;
	uint64_t param1 = 0, param2 = 0, param3 = 0;

	sp = state->getSp();
	address =sp + 0xc;

	state->readMemoryConcrete(sp + 0x4, &param1, sizeof(uint32_t));
    state->readMemoryConcrete(sp + 0x8, &param2, sizeof(uint32_t));
    state->readMemoryConcrete(sp + 0xc, &param3, sizeof(uint32_t));
    state->readMemoryConcrete(address, &value, sizeof(uint32_t));

	s2e()->getMessagesStream() << "assertMemcpy pc " << hexval(pc) << "\n"
							   << "---------sp     : " << hexval(sp) << "\n"
							   << "---------param1 : " << hexval(param1) << "\n"
							   << "---------param2 : " << hexval(param2) << "\n"
							   << "---------param3 : " << hexval(param3) << "\n"
							   << "---------param3 address : " << hexval(address) << "\n"
							   << "---------param3 value   : " << hexval(value) << "\n";
	//added by cdboot 20120614
	uint32_t edi = 0 , ecx = 0;
	state->readCpuRegisterConcrete(CPU_OFFSET(regs[R_EDI]), &edi, sizeof(edi));
	state->readCpuRegisterConcrete(CPU_OFFSET(regs[R_ECX]), &ecx, sizeof(ecx));

	//added by cdboot 20120607
	uint64_t maxSize = 0x20;
	bool onStack = false;
	StackFrameInfo info;
	onStackCheck.emit(state, param1, param3, &info);

	if(info.StackSize == 0x00000000){
		s2e()->getMessagesStream() << "address:" << hexval(param1) << "is not in the stack!" << "\n";
	}else if(info.StackSize == 0xffffffff){
		s2e()->getMessagesStream() << "address:" << hexval(param1) << "is not in a valid stack frame!" << "\n";
	}else{
		maxSize = info.FrameSize;
		onStack = true;
	}

	if(!onStack){
		s2e()->getMessagesStream() << "not in the stack or not in a valid stack frame!" << "\n";
		return;
	}

    klee::ref<klee::Expr> symValue = state->readMemory(address, klee::Expr::Int32);
    if(isa<klee::ConstantExpr>(symValue))
    {
    	s2e()->getMessagesStream() << "param3 is a concrete value" << "\n";
    	return;
    }
	s2e()->getMessagesStream() << "---------param3 symbolic value : " << symValue << "\n";

	klee::ref<klee::Expr> cond = klee::SgtExpr::create(symValue, klee::ConstantExpr::create(/*0x20*/maxSize, symValue.get()->getWidth()));
	s2e()->getMessagesStream() << "---------assert cond : " << cond << "\n";

	bool isTrue;
	if (!(s2e()->getExecutor()->getSolver()->mayBeTrue(klee::Query(state->constraints, cond), isTrue))) {
		s2e()->getMessagesStream() << "failed to assert the condition" << "\n";
		return;
    	}
	if (isTrue) {
		ConcreteInputs inputs;
		ConcreteInputs::iterator it;
		s2e()->getExecutor()->addConstraint_pub(*state, cond);

		s2e()->getExecutor()->getSymbolicSolution(*state, inputs);
		s2e()->getMessagesStream() << "---------memcpy crash detected!" << "\n"
								   << "---------input value : " << "\n";
	    for (it = inputs.begin(); it != inputs.end(); ++it) {
	        const VarValuePair &vp = *it;
   		    s2e()->getMessagesStream() << "---------" << vp.first << " : ";

	        for (unsigned i=0; i<vp.second.size(); ++i) {
				 s2e()->getMessagesStream() << hexval((unsigned char) vp.second[i]) << " ";
        	}
   		    s2e()->getMessagesStream() << "\n";
		}
	}
}

void VulMining::disasPC(uint64_t pc)
{
	char *buf;
	long lSize;
	long result;

	FILE *m_logFile;
	m_logFile = fopen("/home/cdboot/disas.txt", "w+");
	target_disas(m_logFile, pc, 1, 0);
	fclose(m_logFile);


	m_logFile = fopen("/home/cdboot/disas.txt", "r");

	fseek (m_logFile , 0 , SEEK_END);
	lSize = ftell (m_logFile);
	rewind (m_logFile);


	buf = (char*) malloc (sizeof(char)*lSize);
	memset( buf, 0, sizeof(char)*lSize);
    if (buf == NULL)
    	{fputs ("Memory error",stderr); exit (2);}

	result = fread (buf,1,lSize,m_logFile);
	if (result != lSize)
		{fputs ("Reading error",stderr); exit (3);}


	fclose(m_logFile);

	s2e()->getDebugStream() <<"buf: "<<buf<<"\n";

	free(buf);

}
