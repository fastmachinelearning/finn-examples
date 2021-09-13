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
from finn.core.datatype import DataType
from finn.builder.build_dataflow_config import DataflowBuildConfig
from finn.transformation.insert_topk import InsertTopK
from finn.transformation.general import RemoveUnusedTensors, RemoveStaticGraphInputs


def step_preprocess(model: ModelWrapper, cfg: DataflowBuildConfig):
    #model = model.transform(RemoveStaticGraphInputs())
    # remove input quantization (assumed to be done offline)
    #for i in range(3):
    #    model.graph.node.remove(model.graph.node[0])
    
    # wire first matmul directly to input
    #first_matmul = model.get_nodes_by_op_type("MatMul")[0]
    #first_matmul.input[0] = model.graph.input[0].name
    #model.set_tensor_shape(model.graph.input[0].name, (1, 490))
    #model.set_tensor_datatype(model.graph.input[0].name, DataType.INT8)
    #model = model.transform(RemoveUnusedTensors())
    # add top-k node at the output
    model = model.transform(InsertTopK(k=1))
    return model

estimate_steps = [step_preprocess] + build_cfg.estimate_only_dataflow_steps
estimate_outputs = [build_cfg.DataflowOutputType.ESTIMATE_REPORTS]
build_steps = [step_preprocess] + build_cfg.default_build_dataflow_steps
build_outputs = [
    build_cfg.DataflowOutputType.PYNQ_DRIVER,
    build_cfg.DataflowOutputType.BITFILE,
    build_cfg.DataflowOutputType.DEPLOYMENT_PACKAGE
]
model_name = "MLP_W3A3_scale_init-0.1_no_per_channel_scaling_at_output_usigned_non-narrow_relu_act_FINN"
model_file = model_name + ".onnx"

cfg = build_cfg.DataflowBuildConfig(
    #steps=estimate_steps, generate_outputs=estimate_outputs,
    steps=build_steps, generate_outputs=build_outputs,
    output_dir="output_" + model_name,
    target_fps=10000,
    synth_clk_period_ns=5.0,
    board="Pynq-Z1",
    shell_flow_type=build_cfg.ShellFlowType.VIVADO_ZYNQ,
)
build.build_dataflow_cfg(model_file, cfg)