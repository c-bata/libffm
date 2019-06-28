#pragma GCC diagnostic ignored "-Wunused-result"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ffm.h"

#if defined USEOMP
#include <omp.h>
#endif

using namespace std;
using namespace ffm;

string train_help() {
  return string(
      "usage: ffm-train [options] training_set_file [model_file]\n"
      "\n"
      "options:\n"
      "-l <lambda>: set regularization parameter (default 0.00002)\n"
      "-k <factor>: set number of latent factors (default 4)\n"
      "-t <iteration>: set number of iterations (default 15)\n"
      "-r <eta>: set learning rate (default 0.2)\n"
      "-s <nr_threads>: set number of threads (default 1)\n"
      "-p <path>: set path to the validation set\n"
      "-f <path>: set path for production model file\n"
      "-m <prefix>: set key prefix for production model\n"
      "-W <path>: set path for importance weights file\n"
      "--quiet: quiet mode (no output)\n"
      "--old-style-model: generate old style model file\n"
      "--no-norm: disable instance-wise normalization\n"
      "--auto-stop: stop at the iteration that achieves the best validation "
      "loss (must be used with -p)\n");
}

struct Option {
  string tr_path;
  string va_path;
  string model_path;
  string model_weights_path;
  string importance_weights_path;
  string key_prefix;
  ffm_parameter param;
  bool old_style_model = false;
  bool quiet = false;
  ffm_int nr_threads = 1;
};

string basename(string path) {
  const char *ptr = strrchr(&*path.begin(), '/');
  if (!ptr)
    ptr = path.c_str();
  else
    ptr++;
  return string(ptr);
}

Option parse_option(int argc, char **argv) {
  vector<string> args;
  for (int i = 0; i < argc; i++)
    args.push_back(string(argv[i]));

  if (argc == 1)
    throw invalid_argument(train_help());

  Option opt;

  ffm_int i = 1;
  for (; i < argc; i++) {
    if (args[i].compare("-t") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify number of iterations after -t");
      i++;
      opt.param.nr_iters = atoi(args[i].c_str());
      if (opt.param.nr_iters <= 0)
        throw invalid_argument(
            "number of iterations should be greater than zero");
    } else if (args[i].compare("-k") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify number of factors after -k");
      i++;
      opt.param.k = atoi(args[i].c_str());
      if (opt.param.k <= 0)
        throw invalid_argument("number of factors should be greater than zero");
    } else if (args[i].compare("-r") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify eta after -r");
      i++;
      opt.param.eta = atof(args[i].c_str());
      if (opt.param.eta <= 0)
        throw invalid_argument("learning rate should be greater than zero");
    } else if (args[i].compare("-l") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify lambda after -l");
      i++;
      opt.param.lambda = atof(args[i].c_str());
      if (opt.param.lambda < 0)
        throw invalid_argument(
            "regularization cost should not be smaller than zero");
    } else if (args[i].compare("-s") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify number of threads after -s");
      i++;
      opt.nr_threads = atoi(args[i].c_str());
      if (opt.nr_threads <= 0)
        throw invalid_argument("number of threads should be greater than zero");
    } else if (args[i].compare("-p") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify path after -p");
      i++;
      opt.va_path = args[i];
    } else if (args[i].compare("-m") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify model key prefix after -m");
      i++;
      opt.key_prefix = args[i];
    } else if (args[i].compare("-f") == 0) {
      if (i == argc - 1)
        throw invalid_argument(
            "need to specify production model file path after -f");
      i++;
      opt.model_weights_path = args[i];
    } else if (args[i].compare("-W") == 0) {
      if (i == argc - 1)
        throw invalid_argument("need to specify weights file path after -W");
      i++;
      opt.importance_weights_path = args[i];
    } else if (args[i].compare("--no-norm") == 0) {
      opt.param.normalization = false;
    } else if (args[i].compare("--quiet") == 0) {
      opt.quiet = true;
    } else if (args[i].compare("--old-style-model") == 0) {
      opt.old_style_model = true;
    } else if (args[i].compare("--auto-stop") == 0) {
      opt.param.auto_stop = true;
    } else {
      break;
    }
  }

  if (i != argc - 2 && i != argc - 1)
    throw invalid_argument("cannot parse command\n");

  opt.tr_path = args[i];
  i++;

  if (i < argc) {
    opt.model_path = string(args[i]);
  } else if (i == argc) {
    opt.model_path = basename(opt.tr_path) + ".model";
  } else {
    throw invalid_argument("cannot parse argument");
  }

  return opt;
}

int train_on_disk(Option opt) {
  string tr_bin_path = basename(opt.tr_path) + ".bin";
  string va_bin_path =
      opt.va_path.empty() ? "" : basename(opt.va_path) + ".bin";

  ffm_read_problem_to_disk(opt.tr_path, tr_bin_path);
  if (!opt.va_path.empty())
    ffm_read_problem_to_disk(opt.va_path, va_bin_path);

  ffm_model model = ffm_train_on_disk(tr_bin_path.c_str(), va_bin_path.c_str(),
                                      opt.importance_weights_path, opt.param);

  if (!opt.old_style_model)
    ffm_save_model(model, opt.model_path);
  else
    ffm_save_old_style_model(model, opt.model_path);

  if (!opt.model_weights_path.empty())
    ffm_save_model_weights(model, opt.model_weights_path, opt.key_prefix);

  return 0;
}

int main(int argc, char **argv) {
  Option opt;
  try {
    opt = parse_option(argc, argv);
  } catch (invalid_argument &e) {
    cout << e.what() << endl;
    return 1;
  }

  if (opt.quiet)
    cout.setstate(ios_base::badbit);

  if (opt.param.auto_stop && opt.va_path.empty()) {
    cout << "To use auto-stop, you need to assign a validation set" << endl;
    return 1;
  }

#if defined USEOMP
  omp_set_num_threads(opt.nr_threads);
#endif

  train_on_disk(opt);

  return 0;
}
