if { $::argc == 1 } {
    set base_dir [lindex $::argv 0]
    set proj finn_zynq_link
    set accname finn_cnv_w1a1
    # Export the bitstream and the hardware for the SDK
    puts "INFO: Export hardware..."
    file copy -force $base_dir/$proj.runs/impl_1/top_wrapper.sysdef ../sdk/hdf/$accname\_hlsmover_design_wrapper.hdf
}
exit
