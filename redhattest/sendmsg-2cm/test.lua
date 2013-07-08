s2e = {
    kleeArgs = {
        --"--ignore-always-concrete", "--simplify-sym-indices", "--max-sym-array-size=4",
--        "--use-batching-search=true", "--batch-time=1.0",
--          "--print-mode-switch",
        "--use-dfs-search",
--	"--use-interleaved-query-cost-NURS",
--        "--use-random-path",
        --"--use-non-uniform-random-search",
--	"--simplify-sym-indices",
        --"--use-independent-solver=false",
--        "--use-cex-cache=true",
--        "--use-cache=true",
--        "--use-fast-cex-solver=true",
--        "--max-stp-time=30",
--        "--use-expr-simplifier=true",
--	"--debug-expr-simplifier=true",
--        "--print-expr-simplifier=false",
--        "--use-query-log", 
--        "--use-query-pc-log", 
--	"--use-stp-query-pc-log" ,
--	"--max-memory=0",
--	"--max-memory-inhibit=true",
--	"--output-stats=true",
--	"--output-istats=false", --this is broken
--       "--fork-on-symbolic-address",
--        "--max-forks-on-concretize=32",
        --"--show-repeated-warnings=true",
        --"--all-external-warnings=true",
--        "--flush-tbs-on-state-switch=false",
--        "--keep-llvm-functions=true",
--        "--state-shared-memory=true",
--        "--s2e-persistent-disk-writes=true",
--"--s2e-enable-symbolic-hardware=false"
    }
}


plugins = {
    "BaseInstructions",
    "Example",
--    "TestCaseGenerator",
    "MemoryManager",
--    "WindowsMonitor", 
--    "ModuleExecutionDetector",	
    "RawMonitor",
    "FunctionMonitor",
--    "LibraryCallMonitor",
--    "ExecutionTracer",
--    "ModuleTracer",
--    "SymbolicHardware",

--     "GenericDataSelector",
--      "CodeSelector",
--      "StateManager",
--       "Annotation",
	"HostFiles",
}

pluginsConfig = {}

pluginsConfig.HostFiles = {
	baseDir = "/home/xqx/git/s2e1.1/redhattest/guest_program/sendmsg-2cm",  
}

pluginsConfig.RawMonitor = {
    kernelStart = 0xC0000000,
    ip_options_get = {
	delay = false,
	name = "ip_options_get",
--	start = 0xc02620b0,   --ip_options_get
--	start = 0xc0240440,
	start = 0xc028a480,  --sys_socket
	size = 4,
	nativebase = 0x8048000,
	kernelmode = true
    }

}
pluginsConfig.WindowsMonitor = {
    version="XPSP3",
    userMode=true,
    kernelMode=true,
    checked=false,
    monitorModuleLoad=true,
    monitorModuleUnload=true,
    monitorProcessUnload=true,
    modules={
		kernel32_dll={
			name="kernel32.dll",
			size=1000,
        },
    },
}

pluginsConfig.ModuleExecutionDetector = {
	trackAllModules=false,
--	configureAllModules=false,    
	calc_exe_1 = {
        moduleName = "calc.exe",
        kernelMode = false,
    },
   tests2e_exe = {
        moduleName = "tests2e.exe",
        kernelMode = false,
    },
        
--	kernel32_dll_2 = {
--        moduleName = "kernel32.dll",
--        kernelMode = false,
--    },
    --ndis_sys_1 = {
    --    moduleName = "ndis.sys",
    --    kernelMode = true,
    --},
}


pluginsConfig.GenericDataSelector = {
   tests2e = {
	   moduleId = "tests2e_exe",
	   rule = "injectmain",
	   pc = 0x401000,  --for vc
      -- pc = 0x40140D,  -- for gcc
	   concrete = false,
	   value = 0,
      register = 2,
	   makeParamsSymbolic = true,
	   makeParamCountSymbolic = false,
    },
}

pluginsConfig.CodeSelector = {
    modules = {"tests2e_exe"},                              
}


pluginsConfig.StateManager = {
    timeout = 200 --Kill all states except one after x seconds
--    timeout=!timeout!
}


function killstates_oncall(state, plg)
          plg:setKill(true);
end

pluginsConfig.Annotation =
{
	tests2e_exe_a1 = {
        active=true,
        module="tests2e_exe",
        address=0x40108F,
        instructionAnnotation="killstates_oncall",
    },
}


--Beware! The fake id must not collide with any existing QEMU device.
pluginsConfig.SymbolicHardware = {
     pcntpci5f = {
        id="pcnetf",
        type="pci",
        vid=0x1022,
        pid=0x2000,
        classCode=2,
        revisionId=0x7,
        interruptPin=1,
        resources={
            r0 = { isIo=true, size=0x20, isPrefetchatchable=false },
	    r1 = { isIo=false, size=0x20, isPrefetchable=false	 }
        }
    },
}




