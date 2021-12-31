ACCNAME=finn_kws_mlp_w3a3

help:
	@echo "INFO: make <TAB> to show targets"
.PHONY: help

--setup-axidma:
	xsct script_axidma.tcl $(ACCNAME)
.PHONY: --setup-axidma

sdk-axidma: --setup-axidma
	rm -f $(ACCNAME)_axidma_standalone/src/helloworld.c
	cd  $(ACCNAME)_axidma_standalone/src && ln -s ../../common/main_axidma.c main.c && ln -s ../../common/src.h && ln -s ../../common/dst.h
.PHONY: sdk-axidma

gui:
	xsdk --workspace .
.PHONY: gui

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
