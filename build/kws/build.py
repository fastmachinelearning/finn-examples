# Copyright (c) 2021, Xilinx
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of FINN nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import finn.builder.build_dataflow as build
import finn.builder.build_dataflow_config as build_cfg

from finn.core.modelwrapper import ModelWrapper
from finn.builder.build_dataflow_config import DataflowBuildConfig
from finn.transformation.insert_topk import InsertTopK
from finn.builder.build_dataflow_steps import build_dataflow_step_lookup
import time
import finn.core.onnx_exec as oxe
import numpy as np
import datetime
from glob import glob


# Inject the preprocessing step into FINN to enable json serialization later on
def step_preprocess(model: ModelWrapper, cfg: DataflowBuildConfig):
    model = model.transform(InsertTopK(k=1))
    return model


build_dataflow_step_lookup["step_preprocess_InsertTopK"] = step_preprocess

estimate_steps = ["step_preprocess_InsertTopK"] + build_cfg.estimate_only_dataflow_steps
estimate_outputs = [build_cfg.DataflowOutputType.ESTIMATE_REPORTS]
build_steps = ["step_preprocess_InsertTopK"] + build_cfg.default_build_dataflow_steps
build_outputs = [
    build_cfg.DataflowOutputType.ESTIMATE_REPORTS,
    build_cfg.DataflowOutputType.STITCHED_IP,
    build_cfg.DataflowOutputType.PYNQ_DRIVER,
    build_cfg.DataflowOutputType.BITFILE,
    build_cfg.DataflowOutputType.DEPLOYMENT_PACKAGE,
]
verification_steps = [
    build_cfg.VerificationStepType.QONNX_TO_FINN_PYTHON,
    build_cfg.VerificationStepType.TIDY_UP_PYTHON,
    build_cfg.VerificationStepType.STREAMLINED_PYTHON,
    build_cfg.VerificationStepType.FOLDED_HLS_CPPSIM,
    # build_cfg.VerificationStepType.STITCHED_IP_RTLSIM,     # Fails with timeout
]

model_name = (
    "new_ONNX_exports/MLP_W3A3_scale_init-0.1_no_per_channel_"
    "scaling_at_output_usigned_non-narrow_relu_act_QONNX"
)
model_file = model_name + ".onnx"

# Change the ONNX opset from version 9 to 11, which adds support for the TopK node
from finn.core.modelwrapper import ModelWrapper
model = ModelWrapper(model_file)
model.model.opset_import[0].version = 11
model_file = model_file.replace(".onnx", "_opset-11.onnx")
model.save(model_file)

# platform_name = "Ultra96"
# platform_name = "Pynq-Z1"
platform_name = "ZCU104"
output_dir = f"{time.time():.2f}_output_{model_name.replace('/','_')}_{platform_name}"

# Configure build
cfg = build_cfg.DataflowBuildConfig(
    # steps=estimate_steps, generate_outputs=estimate_outputs,
    verify_steps=verification_steps,
    steps=build_steps,
    generate_outputs=build_outputs,
    output_dir=output_dir,
    target_fps=200000,
    synth_clk_period_ns=10.0,
    board=platform_name,
    shell_flow_type=build_cfg.ShellFlowType.VIVADO_ZYNQ,
    save_intermediate_models=True,
    stitched_ip_gen_dcp=True,
    verify_save_full_context=True,
)
# Build the model
build.build_dataflow_cfg(model_file, cfg)

# Save Build config
config_json_path = f"{output_dir}/DataflowBuildConfig.json"
with open(config_json_path, "w") as f:
    f.write(cfg.to_json())
print(f"Saved DataflowBuildConfig to: {config_json_path}")

# Export quantized inputs
print("Quantizing validation dataset.")
parent_model = ModelWrapper(output_dir + "/intermediate_models/dataflow_parent.onnx")
input_shape = (1, 1, 10, 49)
last_node = parent_model.graph.node[-2]

for f_name in glob("*.npz"):
    print(f"Processing file: {f_name}")

    with open(f_name, "rb") as f:
        np_f = np.load(f)
        data_arr = np_f["data_arr"]
        label_arr = np_f["label_arr"]

    pre_processed_inputs = []
    start_time = time.time()
    for i in range(len(data_arr)):
        input_tensor_finn = data_arr[i].reshape(input_shape)

        # Execute with FINN-ONNX
        input_dict = {parent_model.graph.input[0].name: input_tensor_finn}
        output_dict = oxe.execute_onnx(
            parent_model,
            input_dict,
            True,
            end_node=last_node,
        )
        finn_output = output_dict[last_node.output[0]]
        pre_processed_inputs.append(finn_output)

        diff_time = time.time() - start_time
        time_per_sample = diff_time / (i + 1)
        time_left = (len(data_arr) - (i + 1)) * time_per_sample
        time_left = datetime.timedelta(seconds=time_left)
        print(
            f"Processed: {100*(i+1)/len(data_arr):.1f} [%], "
            f"time left: {str(time_left)}",
            end="\r",
        )
    print()

    # Make compatible with FINN driver
    pre_processed_inputs = np.asarray(pre_processed_inputs)
    pre_processed_inputs = np.squeeze(pre_processed_inputs)
    pre_processed_inputs = pre_processed_inputs.astype(np.int8)

    # Save data
    export_path = output_dir + "/" + f_name.replace(".npz", "_{}_len_{}.npy")
    print(f"Saving data to: {export_path}")
    np.save(
        export_path.format("inputs", len(pre_processed_inputs)), pre_processed_inputs
    )
    np.save(export_path.format("outputs", len(label_arr)), label_arr)
