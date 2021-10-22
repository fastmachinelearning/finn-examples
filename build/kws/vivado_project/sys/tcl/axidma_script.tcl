# Script

# Set accelerator
set accname finn_cnv_w1a1

# Set directory
set proj_dir "./$accname\_project"

# Set project
set proj "$accname\_project"

# Set board
set board "pynqz1"

# Create project
create_project $proj $proj_dir -part xc7z020clg400-1

# Set project properties
set_property board_part www.digilentinc.com:pynq-z1:part0:1.0 [current_project]

# Set IP repository paths

#
# ATTENTION: You have to copy the IP repo paths from
# ../../output_axidma_cnv-w1a1_Pynq-Z1/stitched_ip/make_project.tcl
#
set_property ip_repo_paths [list \
    ../../../finn/finn-rtllib/memstream \
    ../../output_axidma_cnv-w1a1_Pynq-Z1/stitched_ip \
    /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_Thresholding_Batch_0_gml_6lg5/project_Thresholding_Batch_0/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_0_878_ux32/project_StreamingDataWidthConverter_Batch_0/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_2_qwcda52_/project_StreamingFIFO_2/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_ConvolutionInputGenerator_0_23qo9_7x/project_ConvolutionInputGenerator_0/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_3_vs33_njw/project_StreamingFIFO_3/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_0_3u2h1bm3/project_StreamingFCLayer_Batch_0/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_1_gb4uf6jy/project_StreamingDataWidthConverter_Batch_1/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_5_btojmyoz/project_StreamingFIFO_5/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_ConvolutionInputGenerator_1_x64m9_zn/project_ConvolutionInputGenerator_1/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_6_q8egvj1_/project_StreamingFIFO_6/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_1_tm8o4pgy/project_StreamingFCLayer_Batch_1/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_2_dgcglqs6/project_StreamingDataWidthConverter_Batch_2/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingMaxPool_Batch_0_p2ol669h/project_StreamingMaxPool_Batch_0/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_9_75y4or48/project_StreamingFIFO_9/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_3_pveg1skn/project_StreamingDataWidthConverter_Batch_3/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_10_sa9q_v3y/project_StreamingFIFO_10/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_ConvolutionInputGenerator_2_srncq1y9/project_ConvolutionInputGenerator_2/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_11_o_efok0t/project_StreamingFIFO_11/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_2_jq193ybx/project_StreamingFCLayer_Batch_2/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_4_f57iu__q/project_StreamingDataWidthConverter_Batch_4/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_13_zq8bedps/project_StreamingFIFO_13/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_ConvolutionInputGenerator_3_3y5msets/project_ConvolutionInputGenerator_3/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_14_nhkknibk/project_StreamingFIFO_14/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_3_ib25t_cb/project_StreamingFCLayer_Batch_3/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_5_9rbum0me/project_StreamingDataWidthConverter_Batch_5/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingMaxPool_Batch_1_6xjairga/project_StreamingMaxPool_Batch_1/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_6_q8prl01p/project_StreamingDataWidthConverter_Batch_6/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_ConvolutionInputGenerator_4_ny1xdcyz/project_ConvolutionInputGenerator_4/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_19_opih64_8/project_StreamingFIFO_19/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_4__w5xr300/project_StreamingFCLayer_Batch_4/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_7_6osel33h/project_StreamingDataWidthConverter_Batch_7/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_ConvolutionInputGenerator_5_9tkhmmwm/project_ConvolutionInputGenerator_5/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_5_7r7nc866/project_StreamingFCLayer_Batch_5/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_8_4obqresb/project_StreamingDataWidthConverter_Batch_8/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_24_x7qw54k5/project_StreamingFIFO_24/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_6_3k7rq6yb/project_StreamingFCLayer_Batch_6/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_9_eqgatddl/project_StreamingDataWidthConverter_Batch_9/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFIFO_26_yq7xx2_4/project_StreamingFIFO_26/sol1/impl/verilog /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_7_13ha46yx/project_StreamingFCLayer_Batch_7/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingFCLayer_Batch_8_jfbygni1/project_StreamingFCLayer_Batch_8/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_StreamingDataWidthConverter_Batch_10_f43jb915/project_StreamingDataWidthConverter_Batch_10/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_LabelSelect_Batch_0_586oc_sj/project_LabelSelect_Batch_0/sol1/impl/ip /tmp/finn_axidma_dev_giuseppe/code_gen_ipgen_TLastMarker_0_7o6xstaw/project_TLastMarker_0/sol1/impl/ip \
    ] [current_project]

update_ip_catalog -rebuild

# Create the design block
set design_name $accname\_design
create_bd_design $design_name

# Name of the processing system (PS)
set ps "xilinx.com:ip:processing_system7:5.5"

# Create instance: accelerator acc and set properties
set acc_0 [ create_bd_cell -type ip -vlnv xilinx_finn:finn:finn_design:1.0 $accname]

# Create instance: processing_system_0, and set properties
set zynq_ps [ create_bd_cell -type ip -vlnv $ps zynq_ps ]

# Create instance: AXI DMA and set properties
set axi_dma [create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma]
set_property -dict [ list \
    CONFIG.c_include_sg {0} \
    CONFIG.c_sg_include_stscntrl_strm {0} \
    CONFIG.c_m_axis_mm2s_tdata_width {8} \
] $axi_dma
set_property -dict [list CONFIG.c_include_mm2s_dre {1} CONFIG.c_include_s2mm_dre {1}] $axi_dma


# Set some properties for processing_system_0 after running design automation
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 \
     -config { make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  $zynq_ps
set_property -dict [ list CONFIG.PCW_USE_M_AXI_GP0 {1} ] $zynq_ps
set_property -dict [ list CONFIG.PCW_USE_S_AXI_GP0 {1} ] $zynq_ps

#set_property -dict [ list CONFIG.PCW_QSPI_PERIPHERAL_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_SD0_PERIPHERAL_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_SD1_PERIPHERAL_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_ENET0_PERIPHERAL_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_UART1_PERIPHERAL_ENABLE {1} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_I2C0_PERIPHERAL_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_UART0_PERIPHERAL_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_USB0_PERIPHERAL_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_GPIO_MIO_GPIO_ENABLE {0} ] $zynq_ps
#set_property -dict [ list CONFIG.PCW_GPIO_EMIO_GPIO_ENABLE {0} ] $zynq_ps

# Set clock frequency of programmable logic to match the HLS core frequency
set_property -dict [ list CONFIG.PSU__CRL_APB__PL0_REF_CTRL__FREQMHZ {100} ] $zynq_ps

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
    intc_ip {New AXI Interconnect} master_apm {0} } [get_bd_intf_pins $axi_dma/S_AXI_LITE]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { \
    Clk_master {Auto} \
    Clk_slave {/zynq_ps/FCLK_CLK0 (100 MHz)} \
    Clk_xbar {/zynq_ps/FCLK_CLK0 (100 MHz)} \
    Master {/axi_dma/M_AXI_S2MM} \
    Slave {/zynq_ps/S_AXI_GP0} \
    intc_ip {/axi_smc} master_apm {0} } [get_bd_intf_pins $axi_dma/M_AXI_S2MM]

#
connect_bd_intf_net [get_bd_intf_pins $accname/m_axis_0] [get_bd_intf_pins $axi_dma/S_AXIS_S2MM]
connect_bd_intf_net [get_bd_intf_pins $axi_dma/M_AXIS_MM2S] [get_bd_intf_pins $accname/s_axis_0]

#
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/zynq_ps/FCLK_CLK0 (100 MHz)" }  [get_bd_pins $accname/ap_clk]

# Connection of the interrupt signals
set_property -dict [list CONFIG.PCW_USE_FABRIC_INTERRUPT {1} CONFIG.PCW_IRQ_F2P_INTR {1}] [get_bd_cells zynq_ps]
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0
connect_bd_net [get_bd_pins $axi_dma/mm2s_introut] [get_bd_pins xlconcat_0/In0]
connect_bd_net [get_bd_pins $axi_dma/s2mm_introut] [get_bd_pins xlconcat_0/In1]
connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins zynq_ps/IRQ_F2P]
set_property name interrupt_bundle [get_bd_cells xlconcat_0]

# Assign the address in memory for the accelerator execution
assign_bd_address
delete_bd_objs [get_bd_addr_segs $axi_dma/Data_MM2S/SEG_zynq_ps_GP0_QSPI_LINEAR]
delete_bd_objs [get_bd_addr_segs -excluded $axi_dma/Data_MM2S/SEG_zynq_ps_GP0_IOP]
delete_bd_objs [get_bd_addr_segs -excluded $axi_dma/Data_MM2S/SEG_zynq_ps_GP0_M_AXI_GP0]
delete_bd_objs [get_bd_addr_segs $axi_dma/Data_S2MM/SEG_zynq_ps_GP0_QSPI_LINEAR]
delete_bd_objs [get_bd_addr_segs -excluded $axi_dma/Data_S2MM/SEG_zynq_ps_GP0_IOP]
delete_bd_objs [get_bd_addr_segs -excluded $axi_dma/Data_S2MM/SEG_zynq_ps_GP0_M_AXI_GP0]

# ILA
set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {zynq_ps_axi_periph_M00_AXI}]
set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {finn_cnv_w1a1_m_axis_0}]
set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {axi_dma_M_AXIS_MM2S}]
set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {axi_dma_M_AXI_S2MM}]
set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets {axi_dma_M_AXI_MM2S}]
apply_bd_automation -rule xilinx.com:bd_rule:debug -dict [list \
    [get_bd_intf_nets finn_cnv_w1a1_m_axis_0] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    [get_bd_intf_nets axi_dma_M_AXIS_MM2S] {AXIS_SIGNALS "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    [get_bd_intf_nets axi_dma_M_AXI_S2MM] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    [get_bd_intf_nets axi_dma_M_AXI_MM2S] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
    [get_bd_intf_nets zynq_ps_axi_periph_M00_AXI] {AXI_R_ADDRESS "Data and Trigger" AXI_R_DATA "Data and Trigger" AXI_W_ADDRESS "Data and Trigger" AXI_W_DATA "Data and Trigger" AXI_W_RESPONSE "Data and Trigger" CLK_SRC "/zynq_ps/FCLK_CLK0" SYSTEM_ILA "Auto" APC_EN "0" } \
]
set_property name system_ila_finn [get_bd_cells system_ila_0]
set_property -dict [list CONFIG.C_BRAM_CNT {18.5} CONFIG.C_DATA_DEPTH {1024}] [get_bd_cells system_ila_finn]

# Validate the design block we created
validate_bd_design

# Save design
save_bd_design

# Create verilog top
make_wrapper -files [get_files $proj_dir/$proj.srcs/sources_1/bd/$design_name/$design_name.bd] -top
add_files -norecurse $proj_dir/$proj.srcs/sources_1/bd/$design_name/hdl/$design_name\_wrapper.v

# Run the synthesis step
launch_runs synth_1
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
file copy -force $proj_dir/$proj.runs/impl_1/$design_name\_wrapper.sysdef ../sdk/hdf/$accname\_axidma_design_wrapper.hdf
