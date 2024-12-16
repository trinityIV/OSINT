#pragma once
#include "../core/core.h"
#include <string>
#include <memory>

namespace tool {
    class OsintTool {
    public:
        static std::unique_ptr<core::OsintModule> createOsintModule();
        static void displayDomainInfo(const core::DomainInfo& info);
        static void searchAndDisplayEmails(const std::string& domain);
        static void searchAndDisplayUsernames(const std::string& username);
    };

    class PenetrationTool {
    public:
        static std::unique_ptr<core::PenetrationTestModule> createPenTestModule();
        static void displayPortScan(const std::string& host);
        static void displaySecurityHeaders(const std::string& url);
        static void displaySSLInfo(const std::string& domain);
    };

    class AnalysisTool {
    public:
        static std::unique_ptr<core::SiteAnalyzer> createSiteAnalyzer();
        static void analyzeSite(const std::string& url);
        static void checkSecurity(const std::string& url);
        static void displayCertificateInfo(const std::string& domain);
    };

    class MetadataTool {
    public:
        static std::unique_ptr<core::MetadataAnalyzer> createMetadataAnalyzer();
        static void analyzeAndDisplayFile(const std::string& filepath);
        static void extractAndDisplayExif(const std::string& image_path);
        static void analyzeAndDisplayDocument(const std::string& doc_path);
    };

    class SocialMediaTool {
    public:
        static std::unique_ptr<core::SocialMediaAnalyzer> createSocialAnalyzer();
        static void findAndDisplayProfiles(const std::string& username);
        static void displayConnections(const std::string& profile_url);
        static void displayRecentActivity(const std::string& profile_url);
    };

    class VulnerabilityTool {
    public:
        static std::unique_ptr<core::VulnerabilityScanner> createVulnScanner();
        static void scanAndDisplayVulnerabilities(const std::string& target);
        static void displayExploits(const std::string& cve_id);
        static void displayRiskAssessment(const std::string& target);
    };

    class ThreatIntelTool {
    public:
        static std::unique_ptr<core::ThreatIntelligence> createThreatIntel();
        static void queryAndDisplayIndicator(const std::string& indicator);
        static void checkAndDisplayMalware(const std::string& hash);
        static void displayRelatedIndicators(const std::string& indicator);
    };


    void initTools();
    void showHelp();
    void processCommand(const std::string& command, const std::vector<std::string>& args);
}