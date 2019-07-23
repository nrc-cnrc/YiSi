#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# @file BERT_wrapper.py
# @brief Wrapper to extract BERT word embeddings.
#
# @author Darlene Stewart
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2019, Sa Majeste la Reine du Chef du Canada /
# Copyright 2019, Her Majesty in Right of Canada

"""
This wrapper:
   - calls BERT's extract_features.py script
   - massages the JSON output into the token text, unit text, BERT embeddings
     expected by YiSi.
"""

import sys
import os.path
import time
from argparse import ArgumentParser
import tempfile
import json
import subprocess
import resource


class Timer(object):
   """Timing context manager"""
   print_timing = True
   def __init__(self, description):
      self.description = description
   def __enter__(self):
      self.start = time.time()
   def __exit__(self, type, value, traceback):
      self.end = time.time()
      self.time = self.end - self.start
      if self.print_timing:
         print("*** Timer:", self.description, "took", format(self.time, '.3f'),
               "seconds.", file=sys.stderr)


def fatal_error(*args, **kwargs):
   """Print a fatal error message to stderr and exit with code 1."""
   print("Fatal error:", *args, file=sys.stderr, **kwargs)
   sys.exit(1)


try:
   import bert
except ImportError as e:
   try:
      bert_path = os.environ['BERT_HOME']
   except KeyError as e:
      fatal_error("BERT not installed in site-packages.", 
                  "BERT_HOME not defined in the environment.",
                  "Unable to locate BERT.")
   sys.path.insert(0, bert_path)
   sys.path.insert(0, os.path.dirname(bert_path))
   import bert

with Timer("importing bert.extract_features"):
   import bert.extract_features

import tensorflow as tf
from tensorflow.python.platform import flags

tf.contrib._warning = ""   # Silence the v1 contrib warning on sys.stdout.


def warn(*args, **kwargs):
   """Print an warning message to stderr."""
   print("Warning:", *args, file=sys.stderr, **kwargs)
   return

def info(*args, **kwargs):
   """Print information output to stderr."""
   print(*args, file=sys.stderr, **kwargs)

verbose_flag = False
debug_flag = False

def debug(*args, **kwargs):
   """Print debug output to stderr if debug_flag (-d) is set."""
   if debug_flag:
      print("Debug:", *args, file=sys.stderr, **kwargs)

def verbose(*args, **kwargs):
   """Print verbose output to stderr if verbose_flag (-v) or debug_flag (-d) is set."""
   if verbose_flag or debug_flag:
      print(*args, file=sys.stderr, **kwargs)

def print_memory_usage(label=""):
   usage = resource.getrusage(resource.RUSAGE_SELF)
   debug(label, "RSS:", usage.ru_maxrss / 1000., "MB")


default_bert_model = "multi_cased_L-12_H-768_A-12"
default_layers = "-4"
default_max_seq_length = 512
default_batch_size = 8
default_do_lower_case = False

class BERT_model:
   def __init__(self, bert_model_name=default_bert_model,
                layers=default_layers, max_seq_length=default_max_seq_length,
                batch_size=default_batch_size, do_lower_case=default_do_lower_case):
      """Initialize parameters for the BERT model to be used.
      
      See the BERT extract_features.py help for more details for all but the first argument.
      
      bert_model_name: BERT model (directory) name or full path to BERT model directory
      layers: string identifying the layers to use (as a comma separated list)
      max_seq_length: maximum input sequence length
      batch_size: batch size for predictions
      do_lower_case: whether to lower case the input (set to True for uncased
                     models, or to False for cased models)
      """
      self.bert_model_name = bert_model_name
      if os.path.dirname(bert_model_name) != '':
         self.bert_model_dir = bert_model_name
      else:
         self.bert_model_dir = os.path.join(os.environ['BERT_MODEL_HOME'], bert_model_name)
      if not os.path.isdir(os.path.realpath(self.bert_model_dir)):
         fatal_error("BERT model directory doesn't exist:", self.bert_model_dir)
      self.layers = layers
      self.max_seq_length = max_seq_length
      self.batch_size = batch_size
      self.do_lower_case = do_lower_case

   def extract_features_2_yisi(self, input_file, return_json=False):
      """Call BERT extract_features.py script to extract BERT word embeddings,
      and massage the JSON  output into YiSi format.

      input_file: path to the input text file.
      return_json: whether to return the BERT output JSON too.
      returns: per-sentence list of 3-tuples (units list, toks list, embeddings list);
         optionally, return the BERT output JSON list too, in which case,
         return a 2-tuple: (sents, bert_json)
      """
      tmp_output_f = tempfile.NamedTemporaryFile(prefix="BERT_wrapper_", suffix=".json", delete=False)
      tmp_output_file = tmp_output_f.name
      tmp_output_f.close()

      argv = [
              "--input_file=" + input_file,
              "--output_file=" + tmp_output_file,
              "--vocab_file=" + os.path.join(self.bert_model_dir, "vocab.txt"),
              "--bert_config_file=" + os.path.join(self.bert_model_dir, "bert_config.json"),
              "--init_checkpoint=" + os.path.join(self.bert_model_dir, "bert_model.ckpt"),
              "--layers=" + self.layers,
              "--max_seq_length=" + str(self.max_seq_length),
              "--batch_size=" + str(self.batch_size),
              "--do_lower_case=" + str(self.do_lower_case)
             ]
      argv = flags.FLAGS(["bert.extract_features"]+argv, known_only=True)

      bert.extract_features.main(argv)

      sents = []
      bert_json = []
      with open(tmp_output_file, "r") as out_f:
         for line in out_f:
            line = line.rstrip('\n')
            sents.append(self.bert_json_2_yisi(line))
            if return_json:
               bert_json.append(line)

      os.unlink(tmp_output_file)

      return (sents, bert_json) if return_json else sents

   def bert_json_2_yisi(self, bert_json):
      """Convert JSON returned by bert.extract_features to YiSi format.

      bert_json: BERT output JSON string (single sentence)
                 or list of BERT output JSON strings (multiple sentences)
      returns: 3-tuple (units list, toks list, embeddings list) for a single sentence
               or per-sentence list of 3-tuples for multiple sentences
      """
      bert_json_list = bert_json if isinstance(bert_json, list) else [bert_json]
      sents = []

      for line in bert_json_list:
         data = json.loads(line)
         units = []
         toks = []
         embeddings = []
         tid = -1
         tok = None
         for feature in data["features"][1:-1]:
            unit = feature["token"]
            units.append(unit)
            if unit.startswith('##'):
               tok += unit[2:]
            else:
               if tok != None:
                  toks.append(tok)
               tid += 1
               tok = unit if unit != "[UNK]" else "UNK"
            unit_emb = feature["layers"][0]["values"]
            embeddings.append((tid, unit_emb))
         if tok != None:
            toks.append(tok)
         sents.append((units, toks, embeddings))

      return sents if isinstance(bert_json, list) else sents[0]


def get_args():
   """Command line argument processing."""

   global verbose_flag, debug_flag

   usage="BERT_wrapper [options] input_file [output_file_prefix]"
   help="""
   Wrapper to extract BERT word embeddings, which: calls BERT's
   extract_features.py script; and then massages the JSON output into the token
   text, unit text, BERT embeddings expected by YiSi.
   """

   parser = ArgumentParser(usage=usage, description=help)

#    parser.add_argument("-h", "-help", "--help", action=HelpAction)
   parser.add_argument("-v", "--verbose", action='store_true', default=False,
                       help="print verbose output to stderr [%(default)s]")
   parser.add_argument("-d", "--debug", action='store_true', default=False,
                       help="print debug output to stderr [%(default)s]")

   parser.add_argument("-m", "--bert_model", type=str, default=default_bert_model,
                       help="BERT model name/path [%(default)s]")
   parser.add_argument("-l", "--layers", type=str, default=default_layers,
                       help="comma separated list of the model layers to use [%(default)s]")
   parser.add_argument("-seq", "--max_seq_length", type=int, default=default_max_seq_length,
                       help="maximum input sequence length [%(default)s]")
   parser.add_argument("-bs", "--batch_size", type=int, default=default_batch_size,
                       help="batch size for predictions [%(default)s]")
   parser.add_argument("-lc", "--do_lower_case", action='store_true', default=default_do_lower_case,
                       help="whether to lower case the input (set to True for uncased " 
                            "models, or to False for cased models) [%(default)s]")
   parser.add_argument("--json", dest="write_json", action='store_true', default=False,
                       help="write the BERT json file too [%(default)s]")

   # The following use the portage_utils version of open to open files.
   parser.add_argument("input_file", nargs='?', type=str,
                       help="input file. Use '-' for stdin.")
   parser.add_argument("output_file_prefix", nargs='?', type=str, default=None,
                       help="output file prefix [input_file]")

   cmd_args = parser.parse_args()

   if cmd_args.verbose:
      verbose_flag = True
   if cmd_args.debug:
      debug_flag = True
   # Print timings only when -d or -v set.
   Timer.print_timing = verbose_flag or debug_flag

   if cmd_args.output_file_prefix is None:
      if cmd_args.input_file == '-':
         fatal_error("output_file_prefix is required when input_file is stdin ('-').")
      else:
         cmd_args.output_file_prefix = cmd_args.input_file

   return cmd_args


def main():
   cmd_args = get_args()
    
   print_memory_usage("start")

   bert_model = BERT_model(cmd_args.bert_model, cmd_args.layers,
                           cmd_args.max_seq_length, cmd_args.batch_size,
                           cmd_args.do_lower_case)

   with Timer("  BERT_model.extract_features_2_yisi"):
      results = bert_model.extract_features_2_yisi(cmd_args.input_file, cmd_args.write_json)
      if cmd_args.write_json:
         yisi_out, bert_json = results
      else:
         yisi_out = results

   with Timer("  writing files"):
      if cmd_args.write_json:
         with open(cmd_args.output_file_prefix+".bert.json", "w") as json_f:
            for line in bert_json:
               print(line, file=json_f)

      with open(cmd_args.output_file_prefix+".bert.unit", "w") as unit_f, \
           open(cmd_args.output_file_prefix+".bert.tok", "w") as tok_f, \
           open(cmd_args.output_file_prefix+".bert.emb", "w") as emb_f:
         for units, toks, embeddings in yisi_out:
            print(*units, file=unit_f)
            print(*toks, file=tok_f)
            for uid, embedding in enumerate(embeddings):
               tid, unit_emb = embedding
               print(uid, tid, ' '.join(str(n) for n in unit_emb), sep='\t', file=emb_f)
            print(file=emb_f)

   print_memory_usage("end")


if __name__ == '__main__':
   with Timer("main"):
      main()
