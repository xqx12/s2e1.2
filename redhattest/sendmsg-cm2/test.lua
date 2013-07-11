s2e = {
	kleeArgs = {
		"--use-dfs-search=true",
--		"--print-mode-switch=true",
		"--verbose-state-switching=true",
		"--verbose-state-deletion=true",
		"--verbose-fork-info=true",
		"--print-forking-status=true",
		--"--use-random-path=true"
	}
}

plugins = {
	"BaseInstructions",
	--"RawMonitor",
	--"ModuleExecutionDetector",
	--"FunctionMonitor",
	--"MemoryManager",
	"ExecutionTracer",
	--"CallTracer",
	"HostFiles",
}

pluginsConfig = {}

pluginsConfig.RawMonitor = {
	kernelStart  = 0xc0000000,
	__kmalloc = {
		delay = false,
		name = "__kmalloc",
		start = 0xc02620b0,
		size = 0x10,
		nativebase = 0x80480000,
		kernelmode = true
	}
}

pluginsConfig.MemoryManager = {
	pc___kmalloc = 0xc013aa30,
}

pluginsConfig.HostFiles = {
	--baseDirs = {"/home/xqx/xqx/guest_program/socket/sendmsg-cm2/"}
	baseDirs = {"/tmp/"}
}


