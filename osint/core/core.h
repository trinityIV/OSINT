#pragma once
#include <string>
#include <vector>
#include <memory>

namespace core {
    void init();

    struct DomainInfo {
        std::string domain;
        std::string ip;
        std::string registrar;
        std::string creation_date;
        std::vector<std::string> name_servers;
    };

    struct SecurityHeaders {
        std::string xss_protection;
        std::string content_security_policy;
        std::string x_frame_options;
        std::string hsts;
    };

    class OsintModule {
    public:
        virtual DomainInfo getDomainInfo(const std::string& domain) = 0;
        virtual std::vector<std::string> findEmails(const std::string& domain) = 0;
        virtual std::vector<std::string> searchUsername(const std::string& username) = 0;
    };

    class PenetrationTestModule {
    public:
        virtual std::vector<int> scanPorts(const std::string& host) = 0;
        virtual SecurityHeaders checkSecurityHeaders(const std::string& url) = 0;
        virtual bool checkSSLConfig(const std::string& domain) = 0;
    };

    class SiteAnalyzer {
    public:
        virtual bool checkMaliciousUrl(const std::string& url) = 0;
        virtual std::vector<std::string> analyzeContent(const std::string& url) = 0;
        virtual bool validateCertificate(const std::string& domain) = 0;
    };

     struct FileMetadata {
        std::string filename;
        std::string creator;
        std::string creation_date;
        std::string last_modified;
        std::string software;
        std::map<std::string, std::string> exif_data;
        size_t file_size;
    };

    struct SocialMediaProfile {
        std::string platform;
        std::string username;
        std::string profile_url;
        std::string avatar_url;
        std::string account_creation;
        bool is_verified;
        int followers_count;
        std::vector<std::string> related_accounts;
    };

    struct Vulnerability {
        std::string cve_id;
        std::string description;
        std::string severity;
        float cvss_score;
        std::string affected_component;
        std::string recommendation;
        std::vector<std::string> references;
    };

    struct ThreatIntel {
        std::string indicator;
        std::string type;  // IP, domain, hash, etc.
        std::string source;
        std::string first_seen;
        std::string last_seen;
        std::vector<std::string> associated_malware;
        std::vector<std::string> tags;
    };

    class MetadataAnalyzer {
    public:
        virtual FileMetadata analyzeFile(const std::string& filepath) = 0;
        virtual std::vector<std::string> extractExifData(const std::string& image_path) = 0;
        virtual std::map<std::string, std::string> analyzeDocument(const std::string& doc_path) = 0;
    };

    class SocialMediaAnalyzer {
    public:
        virtual std::vector<SocialMediaProfile> findProfiles(const std::string& username) = 0;
        virtual std::vector<std::string> getConnections(const std::string& profile_url) = 0;
        virtual std::vector<std::string> getRecentActivity(const std::string& profile_url) = 0;
    };

    class VulnerabilityScanner {
    public:
        virtual std::vector<Vulnerability> scanTarget(const std::string& target) = 0;
        virtual std::vector<std::string> checkExploitDB(const std::string& cve_id) = 0;
        virtual float calculateRiskScore(const std::vector<Vulnerability>& vulns) = 0;
    };

    class ThreatIntelligence {
    public:
        virtual std::vector<ThreatIntel> queryIndicator(const std::string& indicator) = 0;
        virtual bool checkMalwareDatabase(const std::string& hash) = 0;
        virtual std::vector<std::string> getRelatedIndicators(const std::string& indicator) = 0;
    };
}