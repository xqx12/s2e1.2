s2e = {
  kleeArgs = {
   -- "--use-concolic-execution=true",
   -- "--use-dfs-search=true"
    "--use-random-path=true",
    "--use-batching-search=true", 
    "--batch-time=1.0"
  }
}

plugins = {
  -- Enable S2E custom opcodes
  "BaseInstructions",

  -- Track when the guest loads programs
  "RawMonitor",

  -- Detect when execution enters
  -- the program of interest
  "ModuleExecutionDetector",

  -- Restrict symbolic execution to
  -- the programs of interest
  "CodeSelector",

  "VulMining",

  "StackMonitor",

  "StackChecker",

--  "MemoryChecker",

--  "ExecutionTracer",
}


