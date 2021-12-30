ACCNAME=finn_kws_mlp_w3a3

help:
	@echo "INFO: make <TAB> to show targets"
.PHONY: help

--setup-axidma:
	xsct script_axidma.tcl $(ACCNAME)
.PHONY: --setup-axidma

sdk-axidma: --setup-axidma
	rm -f $(ACCNAME)_axidma_standalone/src/helloworld.c
	cd  $(ACCNAME)_axidma_standalone/src && ln -s ../../common/axidma_main.c main.c && ln -s ../../common/src.h && ln -s ../../common/dst.h
.PHONY: sdk-axidma

--setup-hlsmover:
	xsct script_hlsmover.tcl $(ACCNAME)
.PHONY: --setup-hlsmover

sdk-hlsmover: --setup-hlsmover
	rm -f $(ACCNAME)_hlsmover_standalone/src/helloworld.c
	cd  $(ACCNAME)_hlsmover_standalone/src && ln -s ../../common/hlsmover_main.c main.c && ln -s ../../common/src.h && ln -s ../../common/dst.h
.PHONY: sdk-hlsmover

gui:
	xsdk --workspace .
.PHONY: gui

#SAMPLE_COUNT=64
##SAMPLE_COUNT=166000

#data:
#	make -C ../../utils/dat2header/sim
#	../../utils/dat2header/sim/dat2header ../../hls/$(PROJECT_HLS)/tb_data/tb_input_features.dat $(PROJECT)/src/src.h src $(SAMPLE_COUNT)
#	../../utils/dat2header/sim/dat2header ../../hls/$(PROJECT_HLS)/tb_data/csim_results.log $(PROJECT)/src/dst.h dst $(SAMPLE_COUNT)
#.PHONY: data

clean:
	rm -rf $(ACCNAME)*_bsp
	rm -rf $(ACCNAME)*_platform
	rm -rf $(PROJECT)*_standalone
	rm -rf RemoteSystemsTempFiles
	rm -rf SDK.log
	rm -rf webtalk
	rm -rf .sdk
	rm -rf .Xil
	rm -rf .metadata
	rm -rf hdf/*.c hdf/drivers hdf/*.h hdf/*.bit hdf/*.html hdf/*.tcl
.PHONY: clean

ultraclean: clean
	rm hdf/*.hdf
.PHONY: ultraclean
