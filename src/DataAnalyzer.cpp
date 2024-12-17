// Ajout des nouvelles implémentations
void DataAnalyzer::initializeMLModel() {
    model = tflite::FlatBufferModel::BuildFromFile("models/analyzer_model.tflite");
    if (!model) {
        throw std::runtime_error("Failed to load ML model");
    }

    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    builder(&interpreter);

    if (!interpreter) {
        throw std::runtime_error("Failed to initialize interpreter");
    }

    interpreter->AllocateTensors();
}

AnalysisResult DataAnalyzer::analyzeProfileImage(const std::string& image_path) {
    AnalysisResult result;
    try {
        cv::Mat image = cv::imread(image_path);
        if (image.empty()) {
            result.success = false;
            result.message = "Failed to load image";
            return result;
        }

        // Prétraitement de l'image
        cv::Mat processed;
        cv::resize(image, processed, cv::Size(224, 224));
        cv::cvtColor(processed, processed, cv::COLOR_BGR2RGB);

        // Normalisation
        processed.convertTo(processed, CV_32F, 1.0/255);

        // Analyse avec TensorFlow Lite
        float* input = interpreter->typed_input_tensor<float>(0);
        memcpy(input, processed.data, processed.total() * processed.elemSize());

        interpreter->Invoke();

        float* output = interpreter->typed_output_tensor<float>(0);
        for (int i = 0; i < 5; i++) {
            result.predictions.push_back({"class_" + std::to_string(i), output[i]});
        }

        result.success = true;
        result.confidence = *std::max_element(output, output + 5);
    } catch (const std::exception& e) {
        result.success = false;
        result.message = e.what();
    }
    return result;
}

std::vector<float> DataAnalyzer::extractFeatures(const UserProfile& profile) {
    std::vector<float> features;
    
    // Caractéristiques basées sur l'activité
    features.push_back(profile.connected_services.size());
    features.push_back(profile.known_aliases.size());
    
    // Analyse des patterns d'activité
    double activity_entropy = 0.0;
    for (const auto& [_, pattern] : profile.activity_patterns) {
        activity_entropy -= pattern * log2(pattern);
    }
    features.push_back(activity_entropy);
    
    // Score de confiance
    features.push_back(profile.trust_score);
    
    return features;
}

double DataAnalyzer::calculateProfileEntropy(const UserProfile& profile) {
    std::map<std::string, int> feature_counts;
    double total_features = 0;
    
    // Compte des caractéristiques
    for (const auto& alias : profile.known_aliases) {
        feature_counts[alias]++;
        total_features++;
    }
    
    for (const auto& service : profile.connected_services) {
        feature_counts[service]++;
        total_features++;
    }
    
    // Calcul de l'entropie
    double entropy = 0.0;
    for (const auto& [_, count] : feature_counts) {
        double p = count / total_features;
        entropy -= p * log2(p);
    }
    
    return entropy;
}

std::vector<std::pair<std::string, double>> DataAnalyzer::predictFutureActivity(const UserProfile& profile) {
    std::vector<std::pair<std::string, double>> predictions;
    
    // Analyse des patterns temporels
    auto patterns = analyzeActivityPatterns(profile.game_profiles);
    
    // Utilisation du modèle ML pour les prédictions
    std::vector<float> features = extractFeatures(profile);
    
    float* input = interpreter->typed_input_tensor<float>(0);
    memcpy(input, features.data(), features.size() * sizeof(float));
    
    interpreter->Invoke();
    
    float* output = interpreter->typed_output_tensor<float>(0);
    for (int i = 0; i < 3; i++) {
        predictions.push_back({"activity_" + std::to_string(i), output[i]});
    }
    
    return predictions;
}

void DataAnalyzer::generateNetworkGraph(const std::vector<UserProfile>& profiles, const std::string& output_path) {
    // Création d'un graphe de relations
    std::ofstream dot_file(output_path + ".dot");
    dot_file << "digraph ProfileNetwork {\n";
    
    // Nœuds
    for (const auto& profile : profiles) {
        dot_file << "  \"" << profile.username << "\" [label=\"" 
                << profile.username << "\\n"
                << "Trust: " << profile.trust_score << "\"];\n";
    }
    
    // Connexions
    for (size_t i = 0; i < profiles.size(); i++) {
        for (size_t j = i + 1; j < profiles.size(); j++) {
            double similarity = calculateProfileSimilarity(profiles[i], profiles[j]);
            if (similarity > 0.5) {
                dot_file << "  \"" << profiles[i].username << "\" -> \""
                        << profiles[j].username << "\" [weight=" 
                        << similarity << "];\n";
            }
        }
    }
    
    dot_file << "}\n";
    dot_file.close();
    
    // Génération de l'image avec GraphViz
    std::string cmd = "dot -Tpng " + output_path + ".dot -o " + output_path + ".png";
    system(cmd.c_str());
}

void DataAnalyzer::exportAnalysisReport(const UserProfile& profile, const std::string& format) {
    std::string filename = "reports/" + profile.username + "_analysis." + format;
    
    if (format == "json") {
        Json::Value report;
        report["username"] = profile.username;
        report["trust_score"] = profile.trust_score;
        report["entropy"] = calculateProfileEntropy(profile);
        
        Json::Value patterns(Json::arrayValue);
        for (const auto& [activity, probability] : profile.activity_patterns) {
            Json::Value pattern;
            pattern["activity"] = activity;
            pattern["probability"] = probability;
            patterns.append(pattern);
        }
        report["activity_patterns"] = patterns;
        
        std::ofstream file(filename);
        Json::StyledWriter writer;
        file << writer.write(report);
    }
    else if (format == "html") {
        // Génération d'un rapport HTML interactif avec graphiques
        std::ofstream file(filename);
        file << "<html><head>\n";
        file << "<script src='https://cdn.plot.ly/plotly-latest.min.js'></script>\n";
        file << "</head><body>\n";
        file << "<h1>Analysis Report for " << profile.username << "</h1>\n";
        // ... Ajout de graphiques et visualisations ...
        file << "</body></html>\n";
    }
}