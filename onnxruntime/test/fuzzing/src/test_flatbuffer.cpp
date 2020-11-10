// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// if we can't load an ORT format model we can't really test anything
#if defined(ENABLE_ORT_FORMAT_LOAD)
#include "OnnxPrediction.h"
#include <filesystem>

int main(int argc, char* argv[])
{
  (void) argc;
  (void) argv;
  Ort::Env env;
  std::wstring modelFile = L"testdata/mnist.onnx";
  std::wstring ortModelFile = L"testdata/mnist.onnx.ort";
  Ort::SessionOptions so;
  so.SetGraphOptimizationLevel(ORT_DISABLE_ALL);
  so.SetOptimizedModelFilePath(ortModelFile.c_str());
  so.AddConfigEntry(kOrtSessionOptionsConfigSaveModelFormat, "ORT");
  Ort::Session session(env, modelFile.c_str(), so);
  std::vector<char> modelData;
  size_t numBytes = std::filesystem::file_size(ortModelFile);
  modelData.resize(numBytes);
  std::ifstream bytes_stream(ortModelFile, std::ifstream::in | std::ifstream::binary);
  bytes_stream.read(modelData.data(), numBytes);
  bytes_stream.close();
  size_t num_ort_exception{0};
  size_t num_std_exception{0};
  size_t num_unknown_exception{0};
  size_t num_successful_runs{0};

  for (int i=0; i < numBytes; i++) {
    char tmp = modelData[i];
    int j = (numBytes-i-1) ?  (i+1) : 0; // j = (i+1) % (numBytes -1)
    modelData[i] ^= modelData[j];
    OnnxPrediction predict(modelData);
    modelData[i] = tmp;
    try 
    {
      Logger::testLog<< "Starting Test" << Logger::endl;
      predict.SetupInput(GenerateDataForInputTypeTensor, 0);
      predict.RunInference();
      predict.PrintOutputValues();
      num_successful_runs++;
      Logger::testLog<< "Completed Test iteration: " << i << Logger::endl;
    }
    catch(const Ort::Exception& ortException)
    {
      num_ort_exception++;
      Logger::testLog<< L"onnx runtime exception: " << ortException.what() << Logger::endl;
      Logger::testLog<< "Failed Test iteration: " << i << Logger::endl;
    }
    catch(const std::exception& e)
    {
      num_std_exception++;
      Logger::testLog<< L"standard exception: " << e.what() << Logger::endl;
      Logger::testLog<< "Failed Test iteration: " << i << Logger::endl;
    }
    catch(...)
    {
      num_unknown_exception++;
      Logger::testLog<< L"unknown exception: " << Logger::endl;
      Logger::testLog<< "Failed Test iteration: " << i << Logger::endl;
      throw;
    }
  }
}
#endif