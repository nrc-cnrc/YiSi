#!/usr/bin/env  python3
# -*- coding: utf-8 -*-  
# source  /space/group/nrc_ict/project/pkgs/ubuntu18/gcc-7.3.0/miniconda3/activate
# conda activate HuggingFace-2.6

# @file HF_tokenization.py
# @brief Unit tokenization using HuggingFace API.
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2019, Sa Majeste la Reine du Chef du Canada /
# Copyright 2019, Her Majesty in Right of Canada


import sys
import torch

from transformers import AutoTokenizer

class Tokenizer_t:
    def __init__(self, modelname_str):
        self.device = "cuda" if torch.cuda.is_available() else "cpu"
        self.tokenizer = AutoTokenizer.from_pretrained(modelname_str)

        # extract features (left-to-right LM scores at the last layer
        # and the intermediate states at any given layer) from loaded model for one sentence at a time
    def get_units(self, sentence):
        # Encode text
        # Add special tokens takes care of adding [CLS], [SEP], <s>... tokens in the right way for each model.
        tids = self.tokenizer.encode(sentence, add_special_tokens=True)
        input_ids = torch.tensor([tids]).to(self.device)
        
        with torch.no_grad():
            # get back the subword units
            tokens=self.tokenizer.convert_ids_to_tokens(input_ids[0])
        
        return tokens[1:-1]

if __name__ == '__main__':
    tokenizer = Tokenizer_t(sys.argv[1])
    
    for line in sys.__stdin__:
        units = tokenizer.get_units(line)
        clean_units = [u.replace(sys.argv[2], "") for u in units]
        print((" ".join(clean_units)).replace("  "," "))
