#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>

class SocialMediaAPI;

class DataAnalyzer {
public:
    struct UserProfile {
        std::string username;
        std::vector<std::string> known_aliases;
        std::vector<std::string> connected_services;
        std::map<std::string, std::string> metadata;
        std::vector<std::string> game_profiles;
        std::vector<cv::Mat> profile_images;
        double trust_score;
        std::map<std::string, double> activity_patterns;
    };

    struct AnalysisResult {
        bool success;
        std::string message;
        double confidence;
        std::vector<std::pair<std::string, double>> predictions;
        std::string error_message;
    };

    DataAnalyzer();
    ~DataAnalyzer();

    // Méthodes principales
    AnalysisResult analyzeProfile(const std::string& username);
    AnalysisResult analyzeImage(const cv::Mat& image);
    AnalysisResult analyzeText(const std::string& text);
    
    // Gestion des ressources
    void cleanupResources();
    void releaseMemory();
    bool initializeModels();
    
    // Gestion des erreurs
    std::string getLastError() const;
    bool isInitialized() const;
    void setLastError(const std::string& error);

private:
    std::unique_ptr<SocialMediaAPI> social_media_api;
    std::unique_ptr<tflite::Interpreter> interpreter;
    std::unique_ptr<tflite::FlatBufferModel> model;
    
    std::vector<cv::Mat> cached_images;
    std::string last_error;
    bool initialized;
    
    // Méthodes privées de nettoyage
    void clearImageCache();
    void unloadModels();
    void cleanupAPI();
};