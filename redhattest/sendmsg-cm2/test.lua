s2e = {
	kleeArgs = {
		"--use-dfs-search=true",
--		"--print-mode-switch=true",
--		"--verbose-state-switching=true",
--		"--verbose-state-deletion=true",
		"--verbose-fork-info=true",
		"--print-forking-status=true",
--		"--print-llvm-instructions=true",
		--"--use-random-path=true"
	}
}

plugins = {
	"BaseInstructions",
	"RawMonitor",
	"ModuleExecutionDetector",
	"FunctionMonitor",
	"MemoryManager",
	"ExecutionTracer",
	"ModuleTracer",
	"TranslationBlockTracer",
	"TestCaseGenerator",
	--"CallTracer",
	"HostFiles",
}

pluginsConfig = {}

pluginsConfig.RawMonitor = {
	kernelStart  = 0xc0000000,
	__kmalloc = {
		delay = false,
		name = "__kmalloc",
		--start = 0xc02620b0,
		start = 0xc0240440,
		size = 0x300000,
		--nativebase = 0xc0100000,
		nativebase = 0xc0240440,
		kernelmode = true
	}
}

pluginsConfig.ModuleExecutionDetector = {
    __kmalloc = {
        moduleName = "__kmalloc",
        kernelMode = true,
    },

}

pluginsConfig.MemoryManager = {
	pc___kmalloc = 0xc013aa30,
}

pluginsConfig.HostFiles = {
	baseDirs = {"/home/xqx/xqx/guest_program/sendmsg-cm2/"}
	--baseDirs = {"/tmp/"}
}


