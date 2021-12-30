MODULE = finn_kws_mlp_w3a3
PROJECT_AXIDMA = $(MODULE)_axidma_project
PROJECT_DIR_AXIDMA = $(MODULE)_axidma_project

PROJECT_DIR_HLSMOVER = $(shell ls -td -- /tmp/finn_dev_$$USER/vivado_zynq*/ | head -n 1)

help:
	@echo "INFO: make <TAB> for targets"
.PHONY: help

sys-axidma-gui:
	vivado -source tcl/script_axidma.tcl -mode gui -tclargs $(MODULE) 0 output_axidma_QONNX_model_Pynq-Z1
.PHONY: sys-axidma-gui

sys-axidma-gui-debug:
	vivado -source tcl/script_axidma.tcl -mode gui -tclargs $(MODULE) 1 output_axidma_QONNX_model_Pynq-Z1
.PHONY: sys-axidma-gui-debug

sys-axidma:
	vivado -source tcl/script_axidma.tcl -mode batch -tclargs $(MODULE) 0 output_axidma_QONNX_model_Pynq-Z1
.PHONY: sys-axidma

gui-axidma:
	vivado $(PROJECT_DIR_AXIDMA)/$(PROJECT_AXIDMA).xpr
.PHONY: gui-axidma

#sys-hlsmover:
#	vivado -mode tcl $(PROJECT_DIR_HLSMOVER)/finn_zynq_link.xpr -source tcl/script_hlsmover.tcl -tclargs $(PROJECT_DIR_HLSMOVER) 
#.PHONY: sys-hlsmover

#gui-hlsmover:
#	vivado $(PROJECT_DIR_HLSMOVER)/finn_zynq_link.xpr
#.PHONY: gui-hlsmover

clean:
	rm -rf *.log *.txt *.jou *.str *_project NA .Xil
.PHONY: clean
