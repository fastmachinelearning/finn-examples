# The KWS examplee

The KWS example includes an MLP for the Google SpeechCommandsV2 dataset.

## Build stitched-IP for MLPerf tiny and bitfile for PYNQ

The build is currently configured for the ARTY-A7 board and a throughput of 200k FPS at a clock frequency of 100 MHz.

1. Download the pretrained MLP ONNX models and pre-processed validation data using the `get-kws-data-model.sh` script.

2. Launch the build as follows:
```shell
# update this according to where you cloned this repo:
FINN_EXAMPLES=/path/to/finn-examples
# cd into finn submodule
cd $FINN_EXAMPLES/build/finn
# launch the build on the bnn-pynq folder
bash run-docker.sh build_custom /path/to/finn-examples/build/kws-arty7
```

3. The generated outputs will be under `kws-arty7/<timestamp>_output_<onnx_file_name>_<platform>`. 
You can find a description of the generated files [here](https://finn-dev.readthedocs.io/en/latest/command_line.html#simple-dataflow-build-mode).
The folder will additionally include the quantized inputs for verification (`all_validation_KWS_data_inputs_len_10102.npy`) and the expected outputs (`all_validation_KWS_data_outputs_len_10102.npy`).
When running the network on hardware the validation should achieve an accuracy of 89.78 % with 9070 of the 10102 samples being classified correctly.  

## Creating the bare metal application for MLPerf tiny
The following steps are adapted from original instructions by Giuseppe:

1. Make sure the shell has access to the Vivado 2019.1 tools and SDK.
2. Run the Vivado project script: 

```shell
cd $FINN_EXAMPLES/build/kws-arty7/vivado_project/sys
make sys
```

3. The Vivado script essentially opens the FINN project and export the HDF file for the SDK in the directory `$FINN_EXAMPLES/build/kws/vivado_project/sdk/hdf/`. Note that when we use HLS datamovers we don't have to create our own Vivado project (as with the AXI DMA IP).
4. Finally, let's create the SDK project + running it:
```shell
cd $FINN_EXAMPLES/build/kws-arty7/vivado_project/sdk
make sdk gui
```
5. This will pop up the SDK, close the "welcome" tab and you should have the baremetal app.

