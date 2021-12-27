setws .
if { $::argc == 1 } {
    set accname [lindex $::argv 0]
    createhw -name $accname\_platform -hwspec hdf/$accname\_design_wrapper.hdf
    createapp -name $accname\_standalone -app {Hello World} -proc microblaze_mcu -hwproject $accname\_platform -os standalone
}
