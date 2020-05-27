#!/usr/bin/env  python3
# -*- coding: utf-8 -*-  
# source  /space/group/nrc_ict/project/pkgs/ubuntu18/gcc-7.3.0/miniconda3/activate
# conda activate HuggingFace-2.6

# @file HuggingFace_wrapper.py
# @brief Wrapper to extract features using HuggingFace API.
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

from transformers import AutoConfig
from transformers import AutoModelWithLMHead
from transformers import AutoTokenizer

from torch.nn import CrossEntropyLoss

class Contextual_t:
    def __init__(self, modelname_str, layer_str):
        self.device = "cuda" if torch.cuda.is_available() else "cpu"
        self.tokenizer = AutoTokenizer.from_pretrained(modelname_str)
        #NOTE: we use `output_hidden_states=True` to get the intermediate layers' output.
        config = AutoConfig.from_pretrained(modelname_str, output_hidden_states=True)
        self.model = AutoModelWithLMHead.from_pretrained(modelname_str, config=config).to(self.device)
        self.layer = int(layer_str)
        #self.max_seq = config.to_dict()["max_position_embeddings"]

        # extract features (left-to-right LM scores at the last layer
        # and the intermediate states at any given layer) from loaded model for one sentence at a time
    def get_features(self, sentence):
        # Encode text
        # Add special tokens takes care of adding [CLS], [SEP], <s>... tokens in the right way for each model.
        tids = self.tokenizer.encode(sentence, add_special_tokens=True, max_length=self.tokenizer.max_len)
        input_ids = torch.tensor([tids]).to(self.device)
        
        with torch.no_grad():
            # Calling model(), i.e. model.forward(), returns the last layer prediction scores and all hidden states
            pred_scores, hidden_states = self.model(input_ids)
            # hidden states include the input embeddings and all intermediate states
            embeddings, *intermediate_states = hidden_states

            # compute lm_loss
            # (copy from BERTModel because some ModelwithLMHead (e.g. RoBERTa, XLM) do not implement left-to-right LM score)
            pred_scores = pred_scores[:, :-1, :].contiguous()
            lm_labels = input_ids[:,1:].contiguous()
            loss_fct=CrossEntropyLoss()
            lm_loss = loss_fct(pred_scores.view(-1, self.model.config.vocab_size), lm_labels.view(-1))

            # get back the subword units
            tokens=self.tokenizer.convert_ids_to_tokens(input_ids[0])
        
        return lm_loss.tolist(), tokens[1:-1], intermediate_states[self.layer][0].tolist()[1:-1]

if __name__ == '__main__':
    print("Loading", sys.argv[1], "...")
    contextual_model = Contextual_t(sys.argv[1], sys.argv[2])
    print ("Done")
    
    lm_scores, tokens, embs = contextual_model.get_features('Eine republikanische Strategie, um der Wiederwahl von Obama entgegenzutreten')
    print(lm_scores)
    print(tokens)
    #print(embs)

    lm_scores, tokens, embs = contextual_model.get_features('A Republican strategy to counter the re-election of Obama')
    print(lm_scores)
    print(tokens)
    #print(embs)
    
    lm_scores, tokens, embs = contextual_model.get_features('A Republican strategy to confront the re-election of Obama')
    print(lm_scores)
    print(tokens)
    #print(embs)
