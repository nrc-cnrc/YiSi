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
from torch.nn import CrossEntropyLoss
from transformers import (
    AutoConfig,
    AutoModel,
    AutoModelForCausalLM,
    AutoModelForMaskedLM,
    AutoModelForSeq2SeqLM,
    AutoTokenizer,
)

# TODO: to be deprecated after we move to use mlm_scoring
clm = ["openai-gpt", "gpt2", "ctrl", "transfo-xl", "xlnet", "reformer"]
mlm = [
    "bert",
    "distilbert",
    "longformer",
    "roberta",
    "bert",
    "flaubert",
    "xlm",
    "xlm-roberta",
    "electra",
    "camembert",
    "albert",
    "mobilebert",
    "reformer",
]
s2s = ["t5", "bart", "marian", "encoder-decoder", "mbart"]

# where the [CLS] token located
head = ["bert", "xlm-roberta", "roberta", "albert", "bart"]
tail = ["xlnet"]


class Contextual_t:

    def __init__(self, modelname_str, layer_str, projection_str=None, proj_type="clp"):
        self.device = "cuda" if torch.cuda.is_available() else "cpu"

        if (modelname_str[0:8] == "nrc-xlm-"):

            tgt=modelname_str[10:12]
            self.srctokenizer = AutoTokenizer.from_pretrained(
                modelname_str[13:],
                merges_file=modelname_str[13:] + "/merges." + src + ".txt",
                do_lowercase_and_remove_accent=False,
            )
            self.tgttokenizer = AutoTokenizer.from_pretrained(
                modelname_str[13:],
                merges_file=modelname_str[13:] + "/merges." + tgt + ".txt",
                do_lowercase_and_remove_accent=False,
            )

        elif (modelname_str[0:10] == "nrc-labse-"):
            self.srctokenizer = AutoTokenizer.from_pretrained("sentence-transformers/LaBSE")

        elif modelname_str[0:4] == "nrc-":
            self.srctokenizer = AutoTokenizer.from_pretrained(modelname_str[4:])
            self.tgttokenizer = self.srctokenizer
        else:
            self.srctokenizer = AutoTokenizer.from_pretrained(modelname_str)
            self.tgttokenizer = self.srctokenizer
        self.proj_type = proj_type
        self.projection = None

        if not (projection_str is None or projection_str == ""):
            self.projection = torch.load(projection_str, map_location="cpu").to(
                self.device
            )
        # NOTE: we use `output_hidden_states=True` to get the intermediate layers' output.
        if (modelname_str[0:8] == "nrc-xlm-"):
            config = AutoConfig.from_pretrained(
                modelname_str[13:] + "/config.json", output_hidden_states=True
            )
        elif (modelname_str[0:14] == "nrc-labse-avg-"):
            config = AutoConfig.from_pretrained(
                modelname_str[14:]+"/config.json", output_hidden_states=True
            )
        elif (modelname_str[0:10] == "nrc-labse-"):
            config = AutoConfig.from_pretrained(
                modelname_str[10:]+"/config.json", output_hidden_states=True
            )            
        elif modelname_str[0:4] == "nrc-":
            config = AutoConfig.from_pretrained(
                modelname_str[4:] + "/config.json", output_hidden_states=True
            )
        else:
            config = AutoConfig.from_pretrained(
                modelname_str, output_hidden_states=True
            )
        cd = config.to_dict()

        self.lmtype = None
        self.model = None
        self.avg=False
        tf = False
        if modelname_str[0:8] == "nrc-xlm-":
            modelname_str = modelname_str[13:] + "/pytorch_model.bin"
        elif (modelname_str[0:14] == "nrc-labse-avg-"):
            self.avg = True
            modelname_str=modelname_str[14:]
        elif (modelname_str[0:10] == "nrc-labse-"):
            modelname_str=modelname_str[10:]
        elif modelname_str[0:4] == "nrc-":
            modelname_str = modelname_str[4:] + "/pytorch_model.bin"
            tf=True

        # TODO: to be deprecated, using AutoModel instead
        if cd["model_type"] in clm:
            print("as clm")
            self.model = AutoModelForCausalLM.from_pretrained(
                modelname_str, config=config
            ).to(self.device)
        elif cd["model_type"] in mlm:
            print("as mlm")
            self.model = AutoModelForMaskedLM.from_pretrained(
                modelname_str, config=config
            ).to(self.device)
        elif cd["model_type"] in s2s:
            print("as seq2seq clm")
            self.model = AutoModelForSeq2SeqLM.from_pretrained(
                modelname_str, config=config
            ).to(self.device)
        else:
            print(
                "WARNING:",
                cd["model_type"],
                "is not any kind of LM, application may not be supported.",
            )
            self.model = AutoModel.from_pretrained(modelname_str, config=config).to(
                self.device
            )
        # /TODO

        if cd["model_type"] in head:
            self.lmtype = "head"
        elif cd["model_type"] in tail:
            self.lmtype = "tail"

        self.layer = int(layer_str)
        # print (self.layer)
        # self.max_seq = config.to_dict()["max_position_embeddings"]

    # extract features (left-to-right LM scores at the last layer
    # and the intermediate states at any given layer) from loaded model for one sentence at a time
    def get_features(self, sentence):
        # Encode text
        # Add special tokens takes care of adding [CLS], [SEP], <s>... tokens in the right way for each model.

        tids = self.tgttokenizer.encode(
            sentence,
            add_special_tokens=True,
            # max_length=self.tgttokenizer.max_len,
            truncation=True,
        )

        input_ids = torch.tensor([tids]).to(self.device)
        tokens = self.tgttokenizer.convert_ids_to_tokens(input_ids[0])
        tokens = [t.replace("</w>", "##") for t in tokens]

        return self.get_feats(input_ids, tokens)

    def get_feats(self, input_ids, tokens):

        with torch.no_grad():
            # Calling model(), i.e. model.forward(), returns the last layer prediction scores and all hidden states
            states = self.model(input_ids)
            print(input_ids)
            pred_scores = states[0]
            hidden_states = states[1]
            # hidden states include the input embeddings and all intermediate states
            embeddings, *intermediate_states = hidden_states
            # get back the subword units

            # TODO: compute mlm_scoring
            if self.lmtype is not None:
                # compute lm_loss
                # (copy from BERTModel because some ModelwithLMHead (e.g. RoBERTa, XLM) do not implement left-to-right LM score)
                lm_labels = input_ids[:, 1:].contiguous()
                pred_scores = pred_scores[:, :-1, :].contiguous()
                loss_fct = CrossEntropyLoss()
                lm_loss = loss_fct(
                    pred_scores.view(-1, self.model.config.vocab_size),
                    lm_labels.view(-1),
                )
            # /TODO

        if self.lmtype is None:
            return 0.0, tokens[1:-1], intermediate_states[self.layer][0].tolist()[1:-1]
        elif self.lmtype == "tail":
            return (
                lm_loss.tolist(),
                tokens[0:-2],
                intermediate_states[self.layer][0].tolist()[0:-2],
            )
        else:
            if self.avg:
                layer1 = intermediate_states[self.layer-1][0].tolist()[1:-1]
                layer2 = intermediate_states[self.layer][0].tolist()[1:-1]
                layer3 = intermediate_states[self.layer+1][0].tolist()[1:-1]
                result = []
                for i in range(len(layer1)):
                    t = []
                    for x,y,z in zip(layer1[i], layer2[i], layer3[i]):
                        t.append((x+y+z)/3.0)
                    result.append(t)
                return lm_loss.tolist(), tokens[1:-1], result
            else:
                return (
                    lm_loss.tolist(),
                    tokens[1:-1],
                    intermediate_states[self.layer][0].tolist()[1:-1],
                )

    def get_proj_features(self, sentence):
        tids = self.srctokenizer.encode(
            sentence,
            add_special_tokens=True,
            # max_length=self.srctokenizer.max_len,
            truncation=True,
        )

        input_ids = torch.tensor([tids]).to(self.device)
        tokens = self.tgttokenizer.convert_ids_to_tokens(input_ids[0])
        tokens = [t.replace("</w>", "##") for t in tokens]

        lm_scores, tokens, embs = self.get_feats(input_ids, tokens)

        if (self.projection is not None) and len(embs) > 0:
            if self.proj_type == "clp":
                return (
                    lm_scores,
                    tokens,
                    torch.mm(
                        torch.tensor(embs).to(self.device), self.projection
                    ).tolist(),
                )
            else:
                cos = torch.nn.CosineSimilarity(dim=1, eps=1e-6)
                embt = torch.tensor(embs).to(self.device)
                embtn = torch.nn.functional.normalize(embt, p=2, dim=1)
                result = embtn - cos(embtn, (self.projection).unsqueeze(0)).unsqueeze(
                    1
                ) * (self.projection).unsqueeze(0)
                return lm_scores, tokens, result.tolist()
        else:
            return lm_scores, tokens, embs


if __name__ == "__main__":
    configs = sys.argv[1].split(":")
    print("Loading", configs[0], "...")
    p = ""
    if len(configs) > 2:
        p = configs[2]
    t = ""
    if len(configs) == 4:
        t = configs[3]
    contextual_model = Contextual_t(configs[0], configs[1], p, t)
    print("Done")

    lm_scores, tokens, embs = contextual_model.get_features(
        "Zhang Guangjun was appointed as the Vice Governor of Guangdong Province ."
    )
    print(lm_scores)
    print(
        "Zhang G ##uang ##jun was appointed as the Vice Governor of Guangdong Province ."
    )
    print(" ".join(tokens))
    print("-0.037141 0.890617 -1.032779 0.15961 -0.295036 -0.291029 -0.269546")
    # print(embs)
    print(embs[0][0:7])
