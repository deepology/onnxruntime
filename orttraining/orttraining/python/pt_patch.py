import torch

from torch.onnx import symbolic_opset10
from torch.onnx.symbolic_helper import parse_args

@parse_args('v', 'v', 'v', 'v', 'i', 'none')
def nll_loss(g, self, target, weight=None, reduction='mean', ignore_index=-100):
    if not weight and not ignore_index:
        return g.op("nll_loss", self, target)
    elif ignore_index:
        ignore_index_ = g.op("Constant", value_t=torch.tensor(ignore_index, dtype=torch.int64))
        eq_ = g.op("Equal", target, ignore_index_)
        not_eq_ = g.op("Not", eq_)
        weight_ = g.op("Cast", not_eq_, to_i=1)      # FLOAT = 1;   // float
        not_eq_int64_ = g.op("Cast", not_eq_, to_i=7)   #INT64 = 7;   // int64_t
        target_ = g.op("Mul", target, not_eq_int64_)
        # if weight:
        #     weight_ = g.op("Mul", weight_, weight)
        return g.op("nll_loss", self, target_, weight_)

symbolic_opset10.nll_loss = nll_loss

@parse_args('v', 'v', 'v', 'i', 'none')
def kl_div(g, self, target, reduction='mean', log_target=False):
    return g.op("kl_div", self, target) # , reduction=reduction, log_target=log_target)

symbolic_opset10.kl_div = kl_div

@parse_args('v', 'v', 'v', 'v', 'v', 'i', 'none')
def cosine_embedding_loss(g, self, input1, input2, target, margin=0, reduction='mean'):
    return g.op("cosine_embedding_loss", self, input1, input2, target)

symbolic_opset10.cosine_embedding_loss = cosine_embedding_loss
