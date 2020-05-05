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

# load model by name
def load_model(modelname):
    tokenizer = AutoTokenizer.from_pretrained(modelname)
    #NOTE: we use `output_hidden_states=True` to get the intermediate layers' output.
    config = AutoConfig.from_pretrained(modelname, output_hidden_states=True)
    model     = AutoModelWithLMHead.from_pretrained(modelname, config=config)
    return [tokenizer, model]

# extract features (left-to-right LM scores at the last layer and the intermediate states at any given layer) from loaded model for one sentence at a time
def extract_features(tokenizer, model, layer, sentence):
    # Encode text
    # Add special tokens takes care of adding [CLS], [SEP], <s>... tokens in the right way for each model.
    input_ids = torch.tensor([tokenizer.encode(sentence, add_special_tokens=True)])

    with torch.no_grad():
        # Calling model(), i.e. model.forward(), returns the last layer prediction scores and all hidden states
        pred_scores, hidden_states = model(input_ids)
        # hidden states include the input embeddings and all intermediate states
        embeddings, *intermediate_states = hidden_states

        # compute lm_loss
        # (copy from BERTModel because some ModelwithLMHead (e.g. RoBERTa, XLM) do not implement left-to-right LM score)
        pred_scores = pred_scores[:, :-1, :].contiguous()
        lm_labels = input_ids[:,1:].contiguous()
        loss_fct=CrossEntropyLoss()
        lm_loss = loss_fct(pred_scores.view(-1, model.config.vocab_size), lm_labels.view(-1))

        # get back the subword units
        tokens=tokenizer.convert_ids_to_tokens(input_ids[0])
        
    return lm_loss, tokens, intermediate_states[layer][0]

if __name__ == '__main__':

    print('Loading...')
    tokenizer, model = load_model(sys.argv[1])
    print('Done')

    lm_scores, tokens, embs = extract_features(tokenizer, model, int(sys.argv[2]), 'Eine republikanische Strategie, um der Wiederwahl von Obama entgegenzutreten')
    print(lm_scores)
    print(tokens)
    print(embs)

    lm_scores, tokens, embs = extract_features(tokenizer, model, int(sys.argv[2]), 'A Republican strategy to counter the re-election of Obama')
    print(lm_scores)
    print(tokens)
    print(embs)
    
    lm_scores, tokens, embs = extract_features(tokenizer, model, int(sys.argv[2]), 'A Republican strategy to confront the re-election of Obama')
    print(lm_scores)
    print(tokens)
    print(embs)
