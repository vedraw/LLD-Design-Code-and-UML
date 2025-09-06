#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <set>
#include <map>
#include <optional>
#include <ctime>
#include <unordered_set>
#include <random>
#include <cstdlib>
#include <sstream>
#include <algorithm>
using namespace std;

#include <bits/stdc++.h>
using namespace std;

enum class PlatformType { ANDROID, IOS };
enum class UpdateType { MANDATORY, OPTIONAL };

// -------------------------
// Version Entity
// -------------------------
class Version {
public:
    string versionNumber;
    UpdateType updateType;
    string releaseNotes;
    string minSupportedVersion;
    bool disabled;

    Version() = default;
    Version(string versionNumber,
            UpdateType updateType,
            string releaseNotes,
            string minSupportedVersion)
        : versionNumber(versionNumber),
          updateType(updateType),
          releaseNotes(releaseNotes),
          minSupportedVersion(minSupportedVersion),
          disabled(false) {}
};

// -------------------------
// Version Manager (CRUD)
// -------------------------
class VersionManager {
private:
    map<PlatformType, vector<Version>> platformVersionsMap;

public:
    void addOrUpdateVersion(PlatformType platform, const Version& version) {
        platformVersionsMap[platform].push_back(version);
    }

    void disableVersion(PlatformType platform, const string& versionNumber) {
        for (auto& v : platformVersionsMap[platform]) {
            if (v.versionNumber == versionNumber) {
                v.disabled = true;
                break;
            }
        }
    }

    void enableVersion(PlatformType platform, const string& versionNumber) {
        for (auto& v : platformVersionsMap[platform]) {
            if (v.versionNumber == versionNumber) {
                v.disabled = false;
                break;
            }
        }
    }

    Version getLatestVersion(PlatformType platform) {
        if (platformVersionsMap[platform].empty()) {
            throw runtime_error("No versions available for platform");
        }
        return platformVersionsMap[platform].back();
    }

    vector<Version> getAllVersions(PlatformType platform) {
        return platformVersionsMap[platform];
    }
};

// -------------------------
// Version Check Result DTO
// -------------------------
struct VersionCheckResult {
    bool upToDate;
    bool mandatory;
    Version latestVersion;
    string message;
};

// -------------------------
// Version Comparator Utility
// -------------------------
class VersionComparator {
public:
    static int compare(const string& v1, const string& v2) {
        auto split = [](const string& str) {
            vector<int> parts;
            stringstream ss(str);
            string token;
            while (getline(ss, token, '.')) {
                parts.push_back(stoi(token));
            }
            return parts;
        };

        auto p1 = split(v1), p2 = split(v2);
        for (size_t i = 0; i < max(p1.size(), p2.size()); ++i) {
            int a = (i < p1.size()) ? p1[i] : 0;
            int b = (i < p2.size()) ? p2[i] : 0;
            if (a != b) return (a > b) ? 1 : -1;
        }
        return 0;
    }
};

// -------------------------
// Version Check Service
// -------------------------
class VersionCheckService {
private:
    VersionManager& versionManager;

public:
    VersionCheckService(VersionManager& versionManager)
        : versionManager(versionManager) {}

    VersionCheckResult checkVersion(PlatformType platform, const string& versionNumber) {
        try {
            Version latestVersion = versionManager.getLatestVersion(platform);

            if (latestVersion.disabled) {
                return {false, true, latestVersion, "Latest version is disabled"};
            }

            int cmp = VersionComparator::compare(versionNumber, latestVersion.versionNumber);

            if (cmp == 0) {
                return {true, false, latestVersion, "Version is up to date"};
            } else if (cmp > 0) {
                return {true, false, latestVersion, "You are ahead of latest version"};
            } else {
                bool mandatory = false;
                string message = "Update available";

                if (latestVersion.updateType == UpdateType::MANDATORY) {
                    mandatory = true;
                    message = "Mandatory update required";
                } else {
                    int minCmp = VersionComparator::compare(versionNumber, latestVersion.minSupportedVersion);
                    if (minCmp < 0) {
                        mandatory = true;
                        message = "Mandatory update required - version below minimum supported";
                    }
                }
                return {false, mandatory, latestVersion, message};
            }
        } catch (const exception& e) {
            Version defaultVersion("0.0.0", UpdateType::OPTIONAL, "", "0.0.0");
            return {false, false, defaultVersion, "Error: " + string(e.what())};
        }
    }
};

// -------------------------
// Main Driver (Example)
// -------------------------
int main() {
    VersionManager vm;

    // Add versions
    vm.addOrUpdateVersion(PlatformType::ANDROID, Version("1.0.0", UpdateType::OPTIONAL, "Initial release", "1.0.0"));
    vm.addOrUpdateVersion(PlatformType::ANDROID, Version("1.1.0", UpdateType::MANDATORY, "Security update", "1.0.0"));
    vm.addOrUpdateVersion(PlatformType::ANDROID, Version("1.2.0", UpdateType::OPTIONAL, "Feature update", "1.1.0"));

    VersionCheckService vcs(vm);

    cout << "=== Version Check Tests ===\n";

    // Test 1: Old version
    auto res1 = vcs.checkVersion(PlatformType::ANDROID, "1.0.0");
    cout << "Check 1.0.0 → " << res1.message << " | Mandatory? " << (res1.mandatory ? "Yes" : "No") << "\n";

    // Test 2: Latest version
    auto res2 = vcs.checkVersion(PlatformType::ANDROID, "1.2.0");
    cout << "Check 1.2.0 → " << res2.message << "\n";

    // Test 3: Disable latest and re-check
    vm.disableVersion(PlatformType::ANDROID, "1.2.0");
    auto res3 = vcs.checkVersion(PlatformType::ANDROID, "1.1.0");
    cout << "After disabling 1.2.0 → " << res3.message << " | Mandatory? " << (res3.mandatory ? "Yes" : "No") << "\n";

    // Test 4: Print version history
    cout << "\n=== Version History ===\n";
    for (auto v : vm.getAllVersions(PlatformType::ANDROID)) {
        cout << "Version " << v.versionNumber
             << " | " << (v.updateType == UpdateType::MANDATORY ? "MANDATORY" : "OPTIONAL")
             << " | " << (v.disabled ? "DISABLED" : "ENABLED") << "\n";
    }

    return 0;
}
