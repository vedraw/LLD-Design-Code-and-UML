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

enum class PlatformType { ANDROID, IOS };
enum class UpdateType { MANDATORY, OPTIONAL };

class Version{
public:
    string versionNumber;
    //PlatformType platformType; assumption Version never travels without its associated platformtype
    UpdateType updatetype;
    string releaseNotes;
    string minSupportedVersion;
    bool disabled;

    Version() = default;
    Version(string versionNumber,
        UpdateType updatetype, string releaseNotes, 
        string minSupportedVersion){
            this->versionNumber = versionNumber;
            //this->platformType=platformType;
            this->updatetype = updatetype;
            this->releaseNotes = releaseNotes;
            this->minSupportedVersion = minSupportedVersion;
            this->disabled = false;
        }
};

//CRUD on the Version
class VersionManager{
public:
    map<PlatformType, vector<Version>> PlatformVersionsMap;

    void updateVersion(PlatformType platform, Version version){
        PlatformVersionsMap[platform].push_back(version);
    }

    void disableVersion(PlatformType platform, Version version){
        for(auto v:PlatformVersionsMap[platform]){
            if(v.versionNumber==version.versionNumber){
                v.disabled = true;
                break;
            }
        }
    }

    void enableVersion(PlatformType platform, Version version){};

    Version getLatestVersion(PlatformType platform){
        return PlatformVersionsMap[platform].back();
    }
    vector<Version> getAllVersions(PlatformType platform){
        return PlatformVersionsMap[platform];
    }
};

struct VersionCheckResult {
    bool upToDate;
    bool mandatory;
    Version latestVersion;
};

class VersionComparator {
public:
    static int compare(const std::string& v1, const std::string& v2) {
        auto split = [](const std::string& str) {
            std::vector<int> parts;
            std::stringstream ss(str);
            std::string token;
            while (std::getline(ss, token, '.'))
                parts.push_back(std::stoi(token));
            return parts;
        };

        auto p1 = split(v1), p2 = split(v2);
        for (size_t i = 0; i < std::max(p1.size(), p2.size()); ++i) {
            int a = (i < p1.size()) ? p1[i] : 0;
            int b = (i < p2.size()) ? p2[i] : 0;
            if (a != b) return (a > b) ? 1 : -1;
        }
        return 0;
    }
};

class VersionCheckService{
public:
    VersionManager* versionManager; //source of truth for version check service

    VersionCheckService(VersionManager* versionManager){
        this->versionManager = versionManager;
    }

    VersionCheckResult checkVersion(PlatformType platform, string versionNumber){
        auto latestVersion=versionManager->getLatestVersion(platform);
        if(versionNumber==latestVersion.versionNumber){ //version matched
            return {true, latestVersion.updatetype==UpdateType::MANDATORY, latestVersion};
        }else{
            return {false, latestVersion.updatetype==UpdateType::MANDATORY, latestVersion};
        }

        //version not matched
        VersionCheckResult result{true, false, latestVersion}; //random initialization
        int cmp = VersionComparator::compare(versionNumber, latestVersion.versionNumber);
        if (cmp < 0) { // current < latest
            result.upToDate = false;
            result.mandatory = (latestVersion.updatetype == UpdateType::MANDATORY) ||
                               (VersionComparator::compare(versionNumber, latestVersion.minSupportedVersion) < 0);
        }
        if (latestVersion.disabled) {
            result.upToDate = false;
            result.mandatory = true;
        }
        return result;
    }
};

int main(){
    VersionManager vm;
    Version v=Version("1.1.0", UpdateType::MANDATORY, "ReleaseNote1", "1.1.0");
    vm.updateVersion(PlatformType::ANDROID, v);

    VersionCheckService vcs(&vm);
    auto res = vcs.checkVersion(PlatformType::ANDROID, "1.0.0");

    std::cout << "Up to date? " << (res.upToDate ? "Yes" : "No") << "\n";
    std::cout << "Mandatory update? " << (res.mandatory ? "Yes" : "No") << "\n";
    std::cout << "Latest version: " << res.latestVersion.versionNumber << "\n";

    auto allVersions = vm.getAllVersions(PlatformType::ANDROID);
    for(auto v:allVersions){
        std::cout << "All Versions: " << v.versionNumber;
        cout << (static_cast<int>(v.updatetype) == 0 ? " MANDATORY" : " OPTIONAL") << endl;
    }
    return 0;
}
