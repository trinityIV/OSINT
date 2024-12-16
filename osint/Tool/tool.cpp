#include "tool.h"
#include <iostream>
#include <iomanip>

namespace tool {
    std::unique_ptr<core::OsintModule> OsintTool::createOsintModule() {
        return std::make_unique<core::OsintModuleImpl>();
    }

    void OsintTool::displayDomainInfo(const core::DomainInfo& info) {
        std::cout << "\n=== Domain Information ===\n";
        std::cout << "Domain: " << info.domain << "\n";
        std::cout << "IP: " << info.ip << "\n";
        if (!info.registrar.empty())
            std::cout << "Registrar: " << info.registrar << "\n";
        if (!info.creation_date.empty())
            std::cout << "Creation Date: " << info.creation_date << "\n";
        if (!info.name_servers.empty()) {
            std::cout << "Name Servers:\n";
            for (const auto& ns : info.name_servers)
                std::cout << "  - " << ns << "\n";
        }
    }

    void OsintTool::searchAndDisplayEmails(const std::string& domain) {
        auto osint = createOsintModule();
        auto emails = osint->findEmails(domain);
        std::cout << "\n=== Found Emails ===\n";
        for (const auto& email : emails)
            std::cout << email << "\n";
    }

    void OsintTool::searchAndDisplayUsernames(const std::string& username) {
        auto osint = createOsintModule();
        auto results = osint->searchUsername(username);
        std::cout << "\n=== Username Search Results ===\n";
        for (const auto& result : results)
            std::cout << result << "\n";
    }

    std::unique_ptr<core::PenetrationTestModule> PenetrationTool::createPenTestModule() {
        return std::make_unique<core::PenetrationTestModuleImpl>();
    }

    void PenetrationTool::displayPortScan(const std::string& host) {
        auto pentest = createPenTestModule();
        auto ports = pentest->scanPorts(host);
        std::cout << "\n=== Port Scan Results ===\n";
        for (int port : ports)
            std::cout << "Port " << port << " is open\n";
    }

    void PenetrationTool::displaySecurityHeaders(const std::string& url) {
        auto pentest = createPenTestModule();
        auto headers = pentest->checkSecurityHeaders(url);
        std::cout << "\n=== Security Headers ===\n";
        if (!headers.xss_protection.empty())
            std::cout << "X-XSS-Protection: " << headers.xss_protection << "\n";
        if (!headers.content_security_policy.empty())
            std::cout << "Content-Security-Policy: " << headers.content_security_policy << "\n";
        if (!headers.x_frame_options.empty())
            std::cout << "X-Frame-Options: " << headers.x_frame_options << "\n";
        if (!headers.hsts.empty())
            std::cout << "Strict-Transport-Security: " << headers.hsts << "\n";
    }

    std::unique_ptr<core::SiteAnalyzer> AnalysisTool::createSiteAnalyzer() {
        return std::make_unique<core::SiteAnalyzerImpl>();
    }

    void AnalysisTool::analyzeSite(const std::string& url) {
        auto analyzer = createSiteAnalyzer();
        auto results = analyzer->analyzeContent(url);
        std::cout << "\n=== Site Analysis Results ===\n";
        for (const auto& result : results)
            std::cout << result << "\n";
    }

    void showHelp() {
        std::cout << "\nOSINT Tool Commands:\n"
                  << "  domain <domain>     - Get domain information\n"
                  << "  email <domain>      - Search for email addresses\n"
                  << "  user <username>     - Search for username across platforms\n"
                  << "  scan <host>         - Perform port scan\n"
                  << "  headers <url>       - Check security headers\n"
                  << "  analyze <url>       - Analyze website content\n"
                  << "  help               - Show this help message\n"
                  << "  exit               - Exit the program\n";
    }

    void processCommand(const std::string& command, const std::vector<std::string>& args) {
        if (command == "domain" && !args.empty()) {
            auto osint = OsintTool::createOsintModule();
            auto info = osint->getDomainInfo(args[0]);
            OsintTool::displayDomainInfo(info);
        }
        else if (command == "email" && !args.empty()) {
            OsintTool::searchAndDisplayEmails(args[0]);
        }
        else if (command == "user" && !args.empty()) {
            OsintTool::searchAndDisplayUsernames(args[0]);
        }
        else if (command == "scan" && !args.empty()) {
            PenetrationTool::displayPortScan(args[0]);
        }
        else if (command == "headers" && !args.empty()) {
            PenetrationTool::displaySecurityHeaders(args[0]);
        }
        else if (command == "analyze" && !args.empty()) {
            AnalysisTool::analyzeSite(args[0]);
        }
        else if (command == "help") {
            showHelp();
        }
        else {
            std::cout << "Invalid command. Type 'help' for available commands.\n";
        }
    }

      std::unique_ptr<core::MetadataAnalyzer> MetadataTool::createMetadataAnalyzer() {
        return std::make_unique<core::MetadataAnalyzerImpl>();
    }

    void MetadataTool::analyzeAndDisplayFile(const std::string& filepath) {
        auto analyzer = createMetadataAnalyzer();
        auto metadata = analyzer->analyzeFile(filepath);
        
        std::cout << "\n=== File Metadata Analysis ===\n";
        std::cout << "Filename: " << metadata.filename << "\n";
        std::cout << "Size: " << metadata.file_size << " bytes\n";
        std::cout << "Created: " << metadata.creation_date << "\n";
        if (!metadata.creator.empty())
            std::cout << "Creator: " << metadata.creator << "\n";
        if (!metadata.software.empty())
            std::cout << "Software: " << metadata.software << "\n";
    }

    void MetadataTool::extractAndDisplayExif(const std::string& image_path) {
        auto analyzer = createMetadataAnalyzer();
        auto exif_data = analyzer->extractExifData(image_path);
        
        std::cout << "\n=== EXIF Data ===\n";
        for (const auto& data : exif_data) {
            std::cout << data << "\n";
        }
    }

    std::unique_ptr<core::SocialMediaAnalyzer> SocialMediaTool::createSocialAnalyzer() {
        return std::make_unique<core::SocialMediaAnalyzerImpl>();
    }

    void SocialMediaTool::findAndDisplayProfiles(const std::string& username) {
        auto analyzer = createSocialAnalyzer();
        auto profiles = analyzer->findProfiles(username);
        
        std::cout << "\n=== Social Media Profiles ===\n";
        for (const auto& profile : profiles) {
            std::cout << "Platform: " << profile.platform << "\n";
            std::cout << "Username: " << profile.username << "\n";
            std::cout << "URL: " << profile.profile_url << "\n";
            if (profile.is_verified)
                std::cout << "Verified: Yes\n";
            if (profile.followers_count > 0)
                std::cout << "Followers: " << profile.followers_count << "\n";
            std::cout << "-------------------\n";
        }
    }

    std::unique_ptr<core::VulnerabilityScanner> VulnerabilityTool::createVulnScanner() {
        return std::make_unique<core::VulnerabilityScannerImpl>();
    }

    void VulnerabilityTool::scanAndDisplayVulnerabilities(const std::string& target) {
        auto scanner = createVulnScanner();
        auto vulns = scanner->scanTarget(target);
        
        std::cout << "\n=== Vulnerability Scan Results ===\n";
        for (const auto& vuln : vulns) {
            std::cout << "CVE: " << vuln.cve_id << "\n";
            std::cout << "Severity: " << vuln.severity << "\n";
            std::cout << "CVSS Score: " << vuln.cvss_score << "\n";
            std::cout << "Component: " << vuln.affected_component << "\n";
            if (!vuln.description.empty())
                std::cout << "Description: " << vuln.description << "\n";
            if (!vuln.recommendation.empty())
                std::cout << "Recommendation: " << vuln.recommendation << "\n";
            std::cout << "-------------------\n";
        }
        
        float risk_score = scanner->calculateRiskScore(vulns);
        std::cout << "Overall Risk Score: " << risk_score << "/10\n";
    }

    std::unique_ptr<core::ThreatIntelligence> ThreatIntelTool::createThreatIntel() {
        return std::make_unique<core::ThreatIntelligenceImpl>();
    }

    void ThreatIntelTool::queryAndDisplayIndicator(const std::string& indicator) {
        auto intel = createThreatIntel();
        auto results = intel->queryIndicator(indicator);
        
        std::cout << "\n=== Threat Intelligence Results ===\n";
        for (const auto& result : results) {
            std::cout << "Indicator: " << result.indicator << "\n";
            std::cout << "Type: " << result.type << "\n";
            std::cout << "First Seen: " << result.first_seen << "\n";
            std::cout << "Last Seen: " << result.last_seen << "\n";
            
            if (!result.associated_malware.empty()) {
                std::cout << "Associated Malware:\n";
                for (const auto& malware : result.associated_malware) {
                    std::cout << "  - " << malware << "\n";
                }
            }
            
            if (!result.tags.empty()) {
                std::cout << "Tags:\n";
                for (const auto& tag : result.tags) {
                    std::cout << "  - " << tag << "\n";
                }
            }
            std::cout << "-------------------\n";
        }
    }

    void showHelp() {
        std::cout << "\nOSINT Tool Commands:\n"
                  << "=== Basic Analysis ===\n"
                  << "  domain <domain>     - Get domain information\n"
                  << "  email <domain>      - Search for email addresses\n"
                  << "  user <username>     - Search for username across platforms\n"
                  << "  scan <host>         - Perform port scan\n"
                  << "  headers <url>       - Check security headers\n"
                  << "  analyze <url>       - Analyze website content\n"
                  << "\n=== Advanced Analysis ===\n"
                  << "  metadata <file>     - Analyze file metadata\n"
                  << "  exif <image>        - Extract EXIF data from image\n"
                  << "  social <username>   - Find social media profiles\n"
                  << "  vuln <target>       - Scan for vulnerabilities\n"
                  << "  threat <indicator>  - Query threat intelligence\n"
                  << "  malware <hash>      - Check malware database\n"
                  << "\n=== General ===\n"
                  << "  help               - Show this help message\n"
                  << "  exit               - Exit the program\n";
    }

    void processCommand(const std::string& command, const std::vector<std::string>& args) {
        if (args.empty() && command != "help" && command != "exit") {
            std::cout << "Error: Missing arguments. Type 'help' for usage.\n";
            return;
        }

        if (command == "metadata" && !args.empty()) {
            MetadataTool::analyzeAndDisplayFile(args[0]);
        }
        else if (command == "exif" && !args.empty()) {
            MetadataTool::extractAndDisplayExif(args[0]);
        }
        else if (command == "social" && !args.empty()) {
            SocialMediaTool::findAndDisplayProfiles(args[0]);
        }
        else if (command == "vuln" && !args.empty()) {
            VulnerabilityTool::scanAndDisplayVulnerabilities(args[0]);
        }
        else if (command == "threat" && !args.empty()) {
            ThreatIntelTool::queryAndDisplayIndicator(args[0]);
        }
        else if (command == "malware" && !args.empty()) {
            ThreatIntelTool::checkAndDisplayMalware(args[0]);
        }
}