MODULE = finn_kws_mlp_w3a3
PROJECT_AXIDMA = $(MODULE)_project
PROJECT_DIR_AXIDMA = $(PROJECT_AXIDMA)

help:
	@echo "INFO: make <TAB> for targets"
.PHONY: help

--wrapper:
	sed 's/TEMPLATE/$(MODULE)/g' verilog/TEMPLATE_design_wrapper.v > $(MODULE)_design_wrapper.v
.PHONY: --wrapper

sys-gui: --wrapper
	vivado -source tcl/axidma_script.tcl -mode gui -tclargs $(MODULE) 0 output_axidma_QONNX_model_Arty-A7
.PHONY: sys-gui

sys-gui-debug: --wrapper
	vivado -source tcl/axidma_script.tcl -mode gui -tclargs $(MODULE) 1 output_axidma_QONNX_model_Arty-A7
.PHONY: sys-gui-debug

sys: --wrapper
	vivado -source tcl/axidma_script.tcl -mode batch -tclargs $(MODULE) 0 output_axidma_QONNX_model_Arty-A7
.PHONY: sys

gui:
	vivado $(PROJECT_DIR_AXIDMA)/$(PROJECT_AXIDMA).xpr
.PHONY: gui

clean:
	rm -rf *.log *.txt *.jou *.str *_project NA .Xil $(MODULE)_design_wrapper.v
.PHONY: clean
