/*
 * OSINT Tool - API References and Documentation
 * 
 * Required APIs and Registration Links:
 * 
 * 1. VirusTotal API
 *    - Registration: https://www.virustotal.com/gui/join-us
 *    - Documentation: https://developers.virustotal.com/reference
 *    - Use: Malware detection and file/URL analysis
 * 
 * 2. Shodan API
 *    - Registration: https://account.shodan.io/register
 *    - Documentation: https://developer.shodan.io/api
 *    - Use: Internet-connected device search and vulnerability scanning
 * 
 * 3. Twitter API (X)
 *    - Registration: https://developer.twitter.com/en/apply-for-access
 *    - Documentation: https://developer.twitter.com/en/docs/twitter-api
 *    - Use: Social media analysis and user information gathering
 * 
 * 4. GitHub API
 *    - Registration: https://github.com/settings/tokens
 *    - Documentation: https://docs.github.com/en/rest
 *    - Use: Code repository and user activity analysis
 * 
 * 5. Exploit-DB API
 *    - Access: https://www.exploit-db.com/api
 *    - Documentation: https://www.exploit-db.com/docs
 *    - Use: Vulnerability and exploit research
 * 
 * 6. Censys API
 *    - Registration: https://search.censys.io/account/api
 *    - Documentation: https://search.censys.io/api
 *    - Use: Internet security and certificate analysis
 * 
 * Additional Useful APIs:
 * - HaveIBeenPwned: https://haveibeenpwned.com/API/v3
 * - SecurityTrails: https://securitytrails.com/corp/api
 * - Maltego: https://www.maltego.com/transform-hub/
 * - AbuseIPDB: https://www.abuseipdb.com/api
 * 
 * Note: Most APIs require authentication keys. Store your API keys securely
 * and never commit them to version control.
 */


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "core/core.h"
#include "Tool/tool.h"

std::vector<std::string> splitCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

int main(int argc, char *argv[]) {
    core::init();
    
    std::cout << "=== OSINT Analysis Tool ===\n";
    std::cout << "Type 'help' for available commands or 'exit' to quit\n";

    std::string input;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        
        if (input.empty()) continue;
        
        auto tokens = splitCommand(input);
        if (tokens.empty()) continue;
        
        std::string command = tokens[0];
        if (command == "exit") break;
        
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        tool::processCommand(command, args);
    }
    
    return 0;
}