# YiSi, semantic machine translation evaluation metric for evaluating languages with different level of available resources
## Introduction
YiSi[1] is a family semantic machine translation (MT) evaluation metrics with 
flexible architecture for evaluating MT output in languages of different 
resource level. Inspired by MEANT 2.0 (Lo, 2017), YiSi-1 measures the similarity 
between the human reference and machine translations by aggregating the weighted
distributional lexical semantic similarity, and optionally shallow semantic 
structures. YiSi-0 is a degenerate resource-free version using the longest 
common character substring accuracy to replace distributional semantics for 
evaluating lexical similarity between the human reference and MT output.

YiSi-1 achieves the highest average correlation with human direct assessment 
(DA) judgment across all language pairs at system-level and the highest median 
correlation with DA relative ranking across all language pairs at segment-level 
in WMT2018 metrics task (Ma et al., 2018).

YiSi-0 is readily available for evaluating all languages while YiSi-1 requires a 
monolingual corpus in the output language to train the distributional lexical 
semantics model. YiSi-1_srl is designed for resource-rich languages that are 
equipped with automatic semantic role labeler. 

[1] YiSi is the romanization of the Cantonese word "意思/meaning".

## Installation
