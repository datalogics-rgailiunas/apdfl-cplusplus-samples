build_run_all.bat builds and runs each DataLogics APDFL sample, and outputs the results.
By default, this occurs with the Debug/x64 configuration, but the desired configuration can be specified with the arguments below.

USAGE:

    build_run_all.bat [arguments]...

Arguments are case-insensitive.
   ARGUMENT      EFFECT
   -rel          Build for Release configuration instead of Debug configuration.
   -32           Build with the 32-bit libraries instead of the 64-bit libraries.
                     (The libraries must be in the correct relative paths to the samples.)
   -noRun        Don't run the samples, just build them.
   -noAD         Don't process the Adobe samples.
   -noDL         Don't process the Datalogics samples.