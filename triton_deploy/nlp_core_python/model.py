# Triton Python Backend for NLP Core
# Author: theonlychant
import json
import numpy as np
import triton_python_backend_utils as pb_utils

class TritonPythonModel:
    def initialize(self, args):
        pass

    def execute(self, requests):
        responses = []
        for request in requests:
            identity_key = pb_utils.get_input_tensor_by_name(request, "IDENTITY_KEY").as_numpy()[0].decode()
            from_surface = pb_utils.get_input_tensor_by_name(request, "FROM_SURFACE").as_numpy()[0].decode()
            to_surfaces = [x.decode() for x in pb_utils.get_input_tensor_by_name(request, "TO_SURFACES").as_numpy()[0]]
            layer = int(pb_utils.get_input_tensor_by_name(request, "LAYER").as_numpy()[0])
            # Generate proposals (simulate)
            proposals = []
            for to in to_surfaces:
                proposal = {
                    "identity_key": identity_key,
                    "from": from_surface,
                    "to": to,
                    "type": "mutation",
                    "layer": layer
                }
                proposals.append(json.dumps(proposal))
            proposals_np = np.array(proposals, dtype=object)
            out_tensor = pb_utils.Tensor("PROPOSALS", proposals_np)
            responses.append(pb_utils.InferenceResponse(output_tensors=[out_tensor]))
        return responses
