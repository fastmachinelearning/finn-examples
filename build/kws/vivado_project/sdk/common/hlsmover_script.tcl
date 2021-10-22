setws .
if { $::argc == 1 } {
    set accname [lindex $::argv 0]
    createhw -name $accname\_hlsmover_platform -hwspec hdf/$accname\_hlsmover_design_wrapper.hdf
    createapp -name $accname\_hlsmover_standalone -app {Hello World} -proc ps7_cortexa9_0 -hwproject $accname\_hlsmover_platform -os standalone
}
