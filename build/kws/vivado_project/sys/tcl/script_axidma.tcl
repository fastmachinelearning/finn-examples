# Script
set module ""
set debug 0
set stitched_ip_dir ""

if { $argc == 3 } {
    set module [lindex $argv 0]
    set debug [lindex $argv 1]
    set stitched_ip_dir [lindex $argv 2]
} else {
    puts "Error!"
}

# Set accelerator
set top_module $module

# Set directory
set proj_dir "./$module\_project"

# Set project
set proj "$module\_project"

# Set design name
set design_name $module\_design

# Create project
create_project $proj $proj_dir -part xc7z020clg400-1

# Set project properties
set_property board_part www.digilentinc.com:pynq-z1:part0:1.0 [current_project]

# Set IP repository paths
set_property ip_repo_paths $proj_dir/../../../$stitched_ip_dir/stitched_ip/ip [current_project]
update_ip_catalog -rebuild

# Create the design block
create_bd_design $design_name

# Create instance: accelerator acc and set properties
create_bd_cell -type ip -vlnv xilinx_finn:finn:finn_design:1.0 finn_design

# Add data width converter for AXI-stream
create_bd_cell -type ip -vlnv xilinx.com:ip:axis_dwidth_converter:1.1 finn_design_axis_dwidth_converter
set_property -dict [list CONFIG.S_TDATA_NUM_BYTES.VALUE_SRC USER] [get_bd_cells finn_design_axis_dwidth_converter]
set_property -dict [list CONFIG.M_TDATA_NUM_BYTES {10}] [get_bd_cells finn_design_axis_dwidth_converter]

# Connect dwidth converter to FINN IP(s)
connect_bd_intf_net [get_bd_intf_pins finn_design_axis_dwidth_converter/M_AXIS] [get_bd_intf_pins finn_design/s_axis_0]

# Create instance: processing_system_0, and set properties
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 zynq_ps

# Create instance: AXI DMA and set properties
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma
set_property -dict [list CONFIG.c_include_sg {0} CONFIG.c_sg_include_stscntrl_strm {0} CONFIG.c_m_axis_mm2s_tdata_width {8}] [get_bd_cells axi_dma]
set_property -dict [list CONFIG.c_include_mm2s_dre {1} CONFIG.c_include_s2mm_dre {1}] [get_bd_cells axi_dma]

# Set some properties for processing_system_0 after running design automation
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config { \
    make_external "FIXED_IO, DDR" \
    apply_board_preset "1" \
    Master "Disable" \
    Slave "Disable"} [get_bd_cells zynq_ps]
set_property -dict [ list CONFIG.PCW_USE_M_AXI_GP0 {1} ] [get_bd_cells zynq_ps]
set_property -dict [ list CONFIG.PCW_USE_S_AXI_GP0 {1} ] [get_bd_cells zynq_ps]

##set_property -dict [ list CONFIG.PCW_QSPI_PERIPHERAL_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_SD0_PERIPHERAL_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_SD1_PERIPHERAL_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_ENET0_PERIPHERAL_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_UART1_PERIPHERAL_ENABLE {1} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_I2C0_PERIPHERAL_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_UART0_PERIPHERAL_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_USB0_PERIPHERAL_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_GPIO_MIO_GPIO_ENABLE {0} ] [get_bd_cells zynq_ps]
##set_property -dict [ list CONFIG.PCW_GPIO_EMIO_GPIO_ENABLE {0} ] [get_bd_cells zynq_ps]

# Set clock frequency of programmable logic to match the HLS core frequency
set_property -dict [ list CONFIG.PSU__CRL_APB__PL0_REF_CTRL__FREQMHZ {100} ] [get_bd_cells zynq_ps]

# Interconnect IPs
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} \
    Master {/axi_dma/M_AXI_MM2S} \
    Slave {/zynq_ps/S_AXI_GP0} \
    intc_ip {Auto} master_apm {0} } [get_bd_intf_pins zynq_ps/S_AXI_GP0]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} \
    Master {/zynq_ps/M_AXI_GP0} \
    Slave {/axi_dma/S_AXI_LITE} \
    intc_ip {New AXI Interconnect} master_apm {0} } [get_bd_intf_pins axi_dma/S_AXI_LITE]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {Auto} \
    Clk_slave {/zynq_ps/FCLK_CLK0 (100 MHz)} \
    Clk_xbar {/zynq_ps/FCLK_CLK0 (100 MHz)} \
    Master {/axi_dma/M_AXI_S2MM} \
    Slave {/zynq_ps/S_AXI_GP0} \
    intc_ip {/axi_smc} master_apm {0} } [get_bd_intf_pins axi_dma/M_AXI_S2MM]

#
connect_bd_intf_net [get_bd_intf_pins finn_design/m_axis_0] [get_bd_intf_pins axi_dma/S_AXIS_S2MM]
connect_bd_intf_net [get_bd_intf_pins axi_dma/M_AXIS_MM2S] [get_bd_intf_pins finn_design_axis_dwidth_converter/S_AXIS]

#
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/zynq_ps/FCLK_CLK0 (100 MHz)" }  [get_bd_pins finn_design/ap_clk]

# Connection of the interrupt signals
set_property -dict [list CONFIG.PCW_USE_FABRIC_INTERRUPT {1} CONFIG.PCW_IRQ_F2P_INTR {1}] [get_bd_cells zynq_ps]
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0
connect_bd_net [get_bd_pins axi_dma/mm2s_introut] [get_bd_pins xlconcat_0/In0]
connect_bd_net [get_bd_pins axi_dma/s2mm_introut] [get_bd_pins xlconcat_0/In1]
connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins zynq_ps/IRQ_F2P]
set_property name interrupt_bundle [get_bd_cells xlconcat_0]

# Assign the address in memory for the accelerator execution
assign_bd_address
delete_bd_objs [get_bd_addr_segs axi_dma/Data_MM2S/SEG_zynq_ps_GP0_QSPI_LINEAR]
delete_bd_objs [get_bd_addr_segs -excluded axi_dma/Data_MM2S/SEG_zynq_ps_GP0_IOP]
delete_bd_objs [get_bd_addr_segs -excluded axi_dma/Data_MM2S/SEG_zynq_ps_GP0_M_AXI_GP0]
delete_bd_objs [get_bd_addr_segs axi_dma/Data_S2MM/SEG_zynq_ps_GP0_QSPI_LINEAR]
delete_bd_objs [get_bd_addr_segs -excluded axi_dma/Data_S2MM/SEG_zynq_ps_GP0_IOP]
delete_bd_objs [get_bd_addr_segs -excluded axi_dma/Data_S2MM/SEG_zynq_ps_GP0_M_AXI_GP0]

### ILAs
#set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {zynq_ps_axi_periph_M00_AXI}]
#set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets finn_design\_m_axis_0]
#set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {axi_dma_M_AXIS_MM2S}]
#set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {axi_dma_M_AXI_S2MM}]
#set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {axi_dma_M_AXI_MM2S}]
#apply_bd_automation -rule xilinx.com:bd_rule:debug -dict [list \
#    [get_bd_intf_nets finn_design\_m_axis_0] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
#    [get_bd_intf_nets axi_dma_M_AXIS_MM2S] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
#    [get_bd_intf_nets axi_dma_M_AXI_S2MM] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
#    [get_bd_intf_nets axi_dma_M_AXI_MM2S] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
#    [get_bd_intf_nets zynq_ps_axi_periph_M00_AXI] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
#]
#set_property name system_ila_finn [get_bd_cells system_ila_0]
#set_property -dict [list CONFIG.C_BRAM_CNT {18.5} CONFIG.C_DATA_DEPTH {1024}] [get_bd_cells system_ila_finn]

# Validate the design block we created
validate_bd_design

# Save design
save_bd_design

# Create verilog top
make_wrapper -files [get_files $proj_dir/$proj.srcs/sources_1/bd/$design_name/$design_name.bd] -top
add_files -norecurse $proj_dir/$proj.srcs/sources_1/bd/$design_name/hdl/$design_name\_wrapper.v

if { $debug == 0 } {
    # Run the synthesis step
    launch_runs synth_1 -j 16
    wait_on_run -timeout 360 synth_1

    # Run the implementation step
    puts "INFO: Run bistream generation..."
    launch_runs impl_1 -to_step write_bitstream -jobs 18
    wait_on_run -timeout 360 impl_1

    if {[get_property PROGRESS [get_runs impl_1]] != "100%"} {
        error "ERROR: bitstream generation failed!"
    }

    # Export the bitstream and the hardware for the SDK
    puts "INFO: Export hardware..."
    file copy -force $proj_dir/$proj.runs/impl_1/$design_name\_wrapper.sysdef \
        ../sdk/hdf/$module\_axidma_design_wrapper.hdf
}
